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
/* -------------------------------------------------------------------------------*/

/* Private Variables -------------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;

francor::Firmware  g_firmware_handle = francor::Firmware(htim1);
/* -------------------------------------------------------------------------------*/

/* Interrupt Functions -----------------------------------------------------------*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  g_firmware_handle.HAL_TIM_IC_CaptureCallback(htim);
}

/* -------------------------------------------------------------------------------*/

/* Public Functions --------------------------------------------------------------*/

void Firmware_Init(void)
{
  g_firmware_handle.init();
}

void Firmware_Update(void)
{
  g_firmware_handle.update();
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
                                 GPIOPin(M1_DIR_GPIO_Port, M1_DIR_Pin)),

                  Motorcontroller(htim1, TIM_CHN_2, htim3,
                                 GPIOPin(M2_EN_GPIO_Port, M2_EN_Pin),
                                 GPIOPin(M2_BRK_GPIO_Port, M2_BRK_Pin),
                                 GPIOPin(M2_DIR_GPIO_Port, M2_DIR_Pin)),

                   Motorcontroller(htim1, TIM_CHN_3, htim4,
                                  GPIOPin(M3_EN_GPIO_Port, M3_EN_Pin),
                                  GPIOPin(M3_BRK_GPIO_Port, M3_BRK_Pin),
                                  GPIOPin(M3_DIR_GPIO_Port, M3_DIR_Pin)),

                  Motorcontroller(htim1, TIM_CHN_4, htim8,
                                 GPIOPin(M4_EN_GPIO_Port, M4_EN_Pin),
                                 GPIOPin(M4_BRK_GPIO_Port, M4_BRK_Pin),
                                 GPIOPin(M4_DIR_GPIO_Port, M4_DIR_Pin))})
{

}

void Firmware::init()
{
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
  }

   _motor_list[0].enable();
   _motor_list[0].disableBrake();
   _motor_list[0]._target_speed_rpm = 10.0f;
}

void Firmware::update()
{
  static float old_speed = 0.0f;
  static bool active = true;

  /*const float speed = _motor_list[0].getCurrentSpeedRPM();
  if(speed != old_speed)
  {
    const uint32_t rpm_fp1 = (uint32_t)(speed);
    const uint32_t rpm_fp2 = (uint32_t)(speed * 1000.0f) % 1000u;

    char buffer[32];
    sprintf(buffer, "Speed: %li.%li\r\n", rpm_fp1, rpm_fp2);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 100u);
    old_speed = speed;
  }

  if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
  {
	if(active)
	{
		_motor_list[0].setDutyCycle(0.0f);
		active = false;
	}
	else
	{
		_motor_list[0].setDutyCycle(50.0f);
		active = true;
	}
    HAL_Delay(100u);
  }*/

  /* Update motors */
  static uint32_t time_stamp = 0U;
  static float timer_mc_update = 0.0f;
  static float timer_print = 0.0f;

  const float delta_time = static_cast<float>(HAL_GetTick() - time_stamp) * 0.001f;
  time_stamp = HAL_GetTick();
  timer_print += delta_time;

  /* Check if delta time >= 1 */
  for(auto& motor : _motor_list)
  {
    if(!motor.update(delta_time))
    {
      Error_Handler();
    }
  }


  /* Print message */
  if(timer_print > 0.03f)
  {
    char buffer[32];
    sprintf(buffer, "Speed: %i.%i Duty: %i\r\n", (int)(_motor_list[0]._current_speed_rpm),
                                                 (int)(_motor_list[0]._current_speed_rpm * 1000.0f) % 1000,
                                            (int)(_motor_list[0]._duty_cycle));
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 100u);
    timer_print = 0.0f;
  }
}

void Firmware::HAL_TIM_IC_CaptureCallback(const TIM_HandleTypeDef* htim)
{
  /* Call motor interrupts */
  for(auto& motor : _motor_list)
  {
    motor.HallInterrupt(htim);
  }
}

/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/

bool Firmware::initPowerStage(const uint8_t pwm_frequency_kHz,
                              const uint8_t cpu_clock_MHz)
{
  /* Calculate bus clock */
  uint8_t bus_clock_MHz = cpu_clock_MHz;
  switch(_power_stage_tim.Init.ClockDivision)
  {
    case TIM_CLOCKDIVISION_DIV2:
    {
      bus_clock_MHz = cpu_clock_MHz / 2u;
    }break;
    case TIM_CLOCKDIVISION_DIV4:
    {
      bus_clock_MHz = cpu_clock_MHz / 4u;
    }break;
  }

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
  _power_stage_tim.Instance->ARR = period;
  _power_stage_tim.Instance->PSC = prescaler;

  /* Calculate factor for duty cycle raw value calculation */
  _power_stage_pwm_fac = static_cast<float>(period) / 100.0f;

  return true;
}

/*! @} End of francor */
/* -------------------------------------------------------------------------------*/
