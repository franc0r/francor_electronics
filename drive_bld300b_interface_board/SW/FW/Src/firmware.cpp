/**
 * @file firmware.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 * @brief Source of Firmware
 *
 * @version 0.1
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 */

/* Includes ----------------------------------------------------------------------*/
#include "main.h"
#include "firmware.h"
#include <ros.h>
/* -------------------------------------------------------------------------------*/

/* Extern Private Variables ------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;
/* -------------------------------------------------------------------------------*/

/* Private Variables -------------------------------------------------------------*/
francor::Firmware  g_firmware_handle = francor::Firmware(htim1);
/* -------------------------------------------------------------------------------*/

/* Interrupt Functions -----------------------------------------------------------*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  g_firmware_handle.HAL_TIM_IC_CaptureCallback(htim);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  g_firmware_handle.HAL_GPIO_EXTI_Callback(GPIO_Pin);
}

/* -------------------------------------------------------------------------------*/

/* Class Public Functions --------------------------------------------------------*/
/** @addtogroup francor
 * @{
 */
using namespace francor;

Firmware::Firmware(TIM_HandleTypeDef& power_stage_tim) :
    _power_stage_tim(power_stage_tim),
    _power_stage_pwm_fac(0.0f),
    _motor_list({Motorcontroller(htim1, TIM_CHN_1, htim2,
                                 GPIOPin(M1_EN_GPIO_Port, M1_EN_Pin),
                                 GPIOPin(M1_BRK_GPIO_Port, M1_BRK_Pin),
                                 GPIOPin(M1_DIR_GPIO_Port, M1_DIR_Pin),
                                 GPIOPin(M1_HA_GPIO_Port, M1_HA_Pin),
                                 GPIOPin(M1_HB_GPIO_Port, M1_HB_Pin),
                                 GPIOPin(M1_HC_GPIO_Port, M1_HC_Pin)),

                  Motorcontroller(htim1, TIM_CHN_2, htim3,
                                 GPIOPin(M2_EN_GPIO_Port, M2_EN_Pin),
                                 GPIOPin(M2_BRK_GPIO_Port, M2_BRK_Pin),
                                 GPIOPin(M2_DIR_GPIO_Port, M2_DIR_Pin),
                                 GPIOPin(M2_HA_GPIO_Port, M2_HA_Pin),
                                 GPIOPin(M2_HB_GPIO_Port, M2_HB_Pin),
                                 GPIOPin(M2_HC_GPIO_Port, M2_HC_Pin)),

                   Motorcontroller(htim1, TIM_CHN_3, htim4,
                                  GPIOPin(M3_EN_GPIO_Port, M3_EN_Pin),
                                  GPIOPin(M3_BRK_GPIO_Port, M3_BRK_Pin),
                                  GPIOPin(M3_DIR_GPIO_Port, M3_DIR_Pin),
                                  GPIOPin(M3_HA_GPIO_Port, M3_HA_Pin),
                                  GPIOPin(M3_HB_GPIO_Port, M3_HB_Pin),
                                  GPIOPin(M3_HC_GPIO_Port, M3_HC_Pin)),

                  Motorcontroller(htim1, TIM_CHN_4, htim8,
                                 GPIOPin(M4_EN_GPIO_Port, M4_EN_Pin),
                                 GPIOPin(M4_BRK_GPIO_Port, M4_BRK_Pin),
                                 GPIOPin(M4_DIR_GPIO_Port, M4_DIR_Pin),
                                 GPIOPin(M4_HA_GPIO_Port, M4_HA_Pin),
                                 GPIOPin(M4_HB_GPIO_Port, M4_HB_Pin),
                                 GPIOPin(M4_HC_GPIO_Port, M4_HC_Pin))}),
                                 _pwm_list(),
                                 _delta_time(0.0f),
                                 _set_spd_timestamp(0u),
                                 _is_initialized(false)
{

}

void Firmware::init()
{
  /* Start base timer with usec resolution */
  if(HAL_OK != HAL_TIM_Base_Start(&htim5))
  {
    Error_Handler();
  }

  /* initialize power stage */
  if(!initPowerStage(POWER_STAGE_PWM_FREQ_KHz, CPU_CLOCK_MHZ))
  {
    Error_Handler();
  }

  /* initialize motor controllers */
  for(auto& motor : _motor_list)
  {
    if(!motor.init(_power_stage_pwm_fac))
    {
      Error_Handler();
    }

    /* Enable motor with brake */
    motor.enable();
  }

  _is_initialized = true;
}

void Firmware::update()
{
  // Check if class is initialized
  if(!_is_initialized) return;

  /* Update timestamp */
  updateDeltaTime();

  // Check for timeout event
  const uint32_t delta_set_spd = HAL_GetTick() - _set_spd_timestamp;
  if(delta_set_spd > 200)
  {
    for(auto& motor : _motor_list)
    {
      motor.enableBrake();
      motor.setDutyCycle(0.0f);
    }
  }

  for(auto& motor : _motor_list)
  {
    motor.update(_delta_time);
  }
}

void Firmware::setSpeed(const uint8_t drive_id, const float speed)
{
  // Check if class is initialized
  if(!_is_initialized) return;

  if(drive_id >= NUM_DRIVES) return;

  if(0.0f == speed)
  {
    _motor_list[drive_id].enableBrake();
    _motor_list[drive_id].setDutyCycle(0.0f);
  }
  else
  {
    _motor_list[drive_id].disableBrake();
    _motor_list[drive_id].setDutyCycle(speed);
  }

  // Store timestamp
  _set_spd_timestamp = HAL_GetTick();
}

const float Firmware::getSpeet(const uint8_t drive_id)
{
  // Check if class is initialized
  if(!_is_initialized) return 0.0f;

  if(drive_id >= NUM_DRIVES) return 0.0f;

  return _motor_list[drive_id].getCurrentSpeedRPM();
}

void Firmware::HAL_TIM_IC_CaptureCallback(const TIM_HandleTypeDef* htim)
{
  /* Call motor interrupts */
  for(auto& motor : _motor_list)
  {
    motor.HallInterrupt(htim);
  }
}


void Firmware::HAL_GPIO_EXTI_Callback(const uint16_t GPIO_Pin)
{

}

/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/

bool Firmware::initPowerStage(const uint8_t pwm_frequency_kHz,
                              const uint8_t cpu_clock_MHz)
{
  /* Calculate bus clock */
  uint8_t bus_clock_MHz = cpu_clock_MHz;
 /* switch(_power_stage_tim.Init.ClockDivision)
  {
    case TIM_CLOCKDIVISION_DIV2:
    {
      bus_clock_MHz = cpu_clock_MHz / 2u;
    }break;
    case TIM_CLOCKDIVISION_DIV4:
    {
      bus_clock_MHz = cpu_clock_MHz / 4u;
    }break;
  }*/

  /* Calculate prescaler and period of PWM timer */
  uint32_t period = 0u;
  uint32_t prescaler = 0u;
  for(;;)
  {
    /* Check if the numeric limit of the variable is reached */
    if(std::numeric_limits<std::uint16_t>::max() <= prescaler)
    {
      return false;
    }

    /* Calculate timer period */
    period = (static_cast<std::uint32_t>(bus_clock_MHz) * 1e3) /
             (static_cast<std::uint32_t>(pwm_frequency_kHz) * (prescaler + 1));

    /* Check if prescaler has to be incremented */
    if(std::numeric_limits<std::uint16_t>::max() < period)
    {
      prescaler++;
    }
    else
    {
      break;
    }
  }

  /* Check if period is not fine enough */
  if(100u >= period)
  {
    return false;
  }

  /* Adjust timer parameters */
  _power_stage_tim.Init.Period = period;
  _power_stage_tim.Init.Prescaler = prescaler;
  if (HAL_TIM_PWM_Init(&_power_stage_tim) != HAL_OK)
  {
    Error_Handler();
  }

  /* Calculate factor for duty cycle raw value calculation */
  _power_stage_pwm_fac = static_cast<float>(period) / 100.0f;

  return true;
}

void Firmware::updateDeltaTime(void)
{
  /* Temp timestamp */
  static uint32_t timestamp = htim5.Instance->CNT;

  /* Calculate delta time */
  const uint32_t delta_time_raw = htim5.Instance->CNT - timestamp;
  _delta_time = static_cast<float>(delta_time_raw) * 0.000001f;


  /* Store last timestamp */
  timestamp = htim5.Instance->CNT;
}

/*! @} End of francor */
/* -------------------------------------------------------------------------------*/
