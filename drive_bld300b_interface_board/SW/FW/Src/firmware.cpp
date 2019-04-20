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
extern TIM_HandleTypeDef htim5;
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
                                 _delta_time(0.0f)
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
    //motor.enable();
  }

  //_motor_list[0].enable();
  //_motor_list[0].disableBrake();
  //_motor_list[0].setDutyCycle(10.0f);
}

void Firmware::update()
{
  /* Update timestamp */
  updateDeltaTime();

  static float timer_message = 0.0f;

  timer_message += _delta_time;

  if(timer_message > 1.0f)
  {
    char buffer[64];
    const uint32_t time = static_cast<uint32_t>(_delta_time * 1000000.0f);
    sprintf(buffer, "Delta Time: %d usec\r\n", time);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10u);
    timer_message = 0.0f;
  }

  /* Update motor list */
  for(Motorcontroller& motor : _motor_list)
  {
    motor.update(_delta_time);
  }

  /*static uint32_t rx_timestamp = 0u;

  const uint32_t delta_time = HAL_GetTick() - rx_timestamp;

  for(Motorcontroller& motor : _motor_list)
  {
    motor.update(static_cast<float>(delta_time) * 0.001f);
  }

  static uint8_t state_old = 0U;
  static int8_t dir_old = 0;
  const uint8_t state_new = _motor_list[0]._hall_sensor_state;
  const int8_t dir_new = _motor_list[0]._direction;
  if(state_old != state_new || dir_old != dir_new)
  {
    char buffer[64];
    sprintf(buffer, "Sensor value: %d %d %d | %d | Direction: %i\r\n", (state_new >> 2) & 0x01, (state_new >> 1) & 0x01, (state_new >> 0) & 0x01, state_new, dir_new);
    HAL_UART_Transmit(&huart2, (uint8_t*)(buffer), strlen(buffer), 10u);
    state_old = state_new;
    dir_old = dir_new;
  }*/

  /*if(delta_time > 200u)
  {
    for(auto& pwm : _pwm_list)
    {
      pwm = 0.0f;
    }
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

  HAL_UART_Receive(&huart2, (uint8_t*)rx_buffer, rx_buffer_size, 10u);

  if(rx_buffer[0] != 0)
  {
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    sscanf((char*)rx_buffer, "SPD %d %d %d %d\r\n", &i1, &i2, &i3, &i4);

    _pwm_list[0] = static_cast<float>(i1) * 0.01f;
    _pwm_list[1] = static_cast<float>(i1) * 0.01f;
    _pwm_list[2] = static_cast<float>(i1) * 0.01f;
    _pwm_list[3] = static_cast<float>(i1) * 0.01f;

    for(uint8_t idx = 0u; idx < rx_buffer_size; idx++)
    {
      rx_buffer[0] = 0u;
    }

    rx_timestamp = HAL_GetTick();
  }*/

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
