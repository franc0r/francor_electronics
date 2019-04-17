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
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;

francor::Firmware  g_firmware_handle = francor::Firmware(htim1);
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
                                 GPIOPin(M1_EN_GPIO_Port, M1_EN_Pin),
                                 GPIOPin(M1_BRK_GPIO_Port, M1_BRK_Pin),
                                 GPIOPin(M1_DIR_GPIO_Port, M1_DIR_Pin)),

                   Motorcontroller(htim1, TIM_CHN_3, htim4,
                                  GPIOPin(M1_EN_GPIO_Port, M1_EN_Pin),
                                  GPIOPin(M1_BRK_GPIO_Port, M1_BRK_Pin),
                                  GPIOPin(M1_DIR_GPIO_Port, M1_DIR_Pin)),

                  Motorcontroller(htim1, TIM_CHN_4, htim8,
                                 GPIOPin(M1_EN_GPIO_Port, M1_EN_Pin),
                                 GPIOPin(M1_BRK_GPIO_Port, M1_BRK_Pin),
                                 GPIOPin(M1_DIR_GPIO_Port, M1_DIR_Pin))})
{

}

void Firmware::init()
{
  /* initialize power stage */
  initPowerStage(POWER_STAGE_PWM_FREQ_KHz, CPU_CLOCK_MHZ);

  /* initialize motor controllers */
  for(auto& motor : _motor_list)
  {
    if(!motor.init(_power_stage_pwm_fac))
    {
      Error_Handler();
    }
  }

}

void Firmware::update()
{

}
/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/

bool Firmware::initPowerStage(const uint8_t pwm_frequency_kHz,
                              const uint8_t cpu_clock_MHz)
{
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
    period = (static_cast<std::uint32_t>(cpu_clock_MHz) * 1e3) /
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
