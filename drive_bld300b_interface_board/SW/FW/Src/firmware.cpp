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

const static uint8_t rx_buffer_size = 33;
volatile uint8_t rx_buffer[rx_buffer_size];
/* -------------------------------------------------------------------------------*/

/* Interrupt Functions -----------------------------------------------------------*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  g_firmware_handle.HAL_TIM_IC_CaptureCallback(htim);
}

void UART_DMAReceiveCplt(UART_HandleTypeDef* huart)
{
  g_firmware_handle.UART_DMAReceiveCplt(huart);
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
                                 GPIOPin(M4_DIR_GPIO_Port, M4_DIR_Pin))}),
                                 _pwm_list()
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

    /* Enable motor with brake */
    motor.enable();
  }


}

void Firmware::update()
{
  static uint32_t rx_timestamp = 0u;

  const uint32_t delta_time = HAL_GetTick() - rx_timestamp;

  if(delta_time > 600u)
  {
    for(uint8_t idx = 0u; idx < NUM_DRIVES; idx++)
    {
      _pwm_list[idx] = 0.0f;
      _motor_list[idx].enableBrake();
      _motor_list[idx].setDutyCycle(0.0f);
    }
  }


  HAL_UART_Receive(&huart2, (uint8_t*)rx_buffer, rx_buffer_size, 10u);

  if(rx_buffer[0] == 'S' && rx_buffer[1] == 'P' && rx_buffer[2] == 'D' &&
     rx_buffer[rx_buffer_size - 1] == '\n' && rx_buffer[rx_buffer_size - 2] == '\r')
  {
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    sscanf((char*)rx_buffer, "SPD %d %d %d %d\r\n", &i1, &i2, &i3, &i4);

    _pwm_list[0] = static_cast<float>(i1) * 0.01f;
    _pwm_list[1] = static_cast<float>(i2) * 0.01f;
    _pwm_list[2] = static_cast<float>(i3) * 0.01f;
    _pwm_list[3] = static_cast<float>(i4) * 0.01f;

    for(uint8_t idx = 0u; idx < rx_buffer_size; idx++)
    {
      rx_buffer[0] = 0u;
    }

    for(uint8_t idx = 0u; idx < NUM_DRIVES; idx++)
    {
      if(_pwm_list[idx] == 0.0f)
      {
        _motor_list[idx].enableBrake();
        _motor_list[idx].setDutyCycle(0.0f);
      }
      else
      {
        _motor_list[idx].disableBrake();
        _motor_list[idx].setDutyCycle(_pwm_list[idx]);
      }
    }

    rx_timestamp = HAL_GetTick();
  }

  HAL_Delay(10u);

}

void Firmware::HAL_TIM_IC_CaptureCallback(const TIM_HandleTypeDef* htim)
{
  /* Call motor interrupts */
  for(auto& motor : _motor_list)
  {
    motor.HallInterrupt(htim);
  }
}

void Firmware::UART_DMAReceiveCplt(const UART_HandleTypeDef* huart)
{
  /* Extract data */


}

/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/

bool Firmware::initPowerStage(const uint8_t pwm_frequency_kHz,
                              const uint8_t cpu_clock_MHz)
{
  /* Calculate bus clock */
  uint8_t bus_clock_MHz = cpu_clock_MHz;

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
