/**
 * @file motorcontroller.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 * @brief Source of Motorcontroller class
 *
 * @version 0.1
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 */

/* Includes ----------------------------------------------------------------------*/
#include "Motorcontroller.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
using namespace francor;

/* Class Public Functions --------------------------------------------------------*/

Motorcontroller::Motorcontroller(TIM_HandleTypeDef& tim_pwm,
                                 const TIM_Channel tim_pwm_chnl,
                                 TIM_HandleTypeDef& tim_hall,
                                 const GPIOPin& enable_pin,
                                 const GPIOPin& brake_pin,
                                 const GPIOPin& direction_pin) :
                                     _tim_pwm_handle(tim_pwm),
                                     _tim_pwm_chnl(tim_pwm_chnl),
                                     _tim_hall_handle(tim_hall),
                                     _enable_pin(enable_pin),
                                     _brake_pin(brake_pin),
                                     _direction_pin(direction_pin),
                                     _duty_cycle_factor(0.0f),
                                     _hall_timer_count(0u),
                                     _hall_event(false),
                                     _current_speed_rpm(0.0f),
                                     _enabled(false),
                                     _is_initialized(false)
{

}


const bool Motorcontroller::init(const float duty_cycle_factor)
{
  /* Check if function was already called */
  if(_is_initialized) return false;

  /* Store variables */
  _duty_cycle_factor = duty_cycle_factor;

  /* Disable drive */
  HAL_GPIO_WritePin(_enable_pin.port, _enable_pin.pin, GPIO_PIN_SET);

  /* Enable brake */
  HAL_GPIO_WritePin(_brake_pin.port, _brake_pin.pin, GPIO_PIN_RESET);

  /* Set direction to FWD */
  HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_RESET);

  /* Start hall encoder */
  if(HAL_OK != HAL_TIMEx_HallSensor_Start_IT(&_tim_hall_handle))
  {
    return false;
  }

  /* Reset values */
  _enabled = false;
  _hall_event = false;

  /* Class is initialized */
  _is_initialized = true;

  return true;
}

const bool Motorcontroller::update(void)
{
  /* Check if class is initialized */
  if(!_is_initialized) return false;

  static const float weight = 0.2f;
  static uint32_t hall_event_timestamp = 0u;

  /* Check if new hall value is available */
  if(_hall_event)
  {
    _hall_event = false;
    const uint16_t hall_timer_cnt = _hall_timer_count;

    if(hall_timer_cnt > 10u)
    {
      /* Calculate rpm value */
      const float delta_time = static_cast<float>(hall_timer_cnt) * 0.0001f;
      const float speed_rpm = 60.0f / (delta_time * 3.0f * 15.0f);

      /* Update rpm with smooth factor */
      _current_speed_rpm = (1.0f-weight) * _current_speed_rpm + (weight) * speed_rpm;

      /* Limit speed */
      if(fabs(_current_speed_rpm) < 1.0f)
      {
        _current_speed_rpm = 0.0f;
      }
    }

    /* Store last timestamp */
    hall_event_timestamp = HAL_GetTick();
  }

  const uint32_t delta_time = HAL_GetTick() - hall_event_timestamp;
  if(delta_time > 50u)
  {
    /* Update rpm with smooth factor */
    _current_speed_rpm = (0.75f) * _current_speed_rpm;

    /* Limit speed */
    if(fabs(_current_speed_rpm) < 1.0f)
    {
      _current_speed_rpm = 0.0f;
    }

    /* Store last timestamp */
    hall_event_timestamp = HAL_GetTick();
  }

  return true;
}

const bool Motorcontroller::enable()
{
  /* Check if class is initialized */
  if(!_is_initialized) return false;

  /* Check if function was already called */
  if(_enabled) return true;

  /* Reset PWM values */
  setDutyCycle(0.0f);

  /* Set direction to FWD */
  HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_RESET);

  /* Set brake active */
  HAL_GPIO_WritePin(_brake_pin.port, _brake_pin.pin, GPIO_PIN_SET);

  /* Set enable pin */
  HAL_GPIO_WritePin(_enable_pin.port, _enable_pin.pin, GPIO_PIN_RESET);

  /* Enable PWM depending on channel */
  switch(_tim_pwm_chnl)
  {
    case TIM_CHN_1:
    {
      HAL_TIM_PWM_Start(&_tim_pwm_handle, TIM_CHN_1);
    }break;

    case TIM_CHN_2:
    {
      HAL_TIM_PWM_Start(&_tim_pwm_handle, TIM_CHN_2);
    }break;

    case TIM_CHN_3:
    {
      HAL_TIM_PWM_Start(&_tim_pwm_handle, TIM_CHN_3);
    }break;

    case TIM_CHN_4:
    {
      HAL_TIM_PWM_Start(&_tim_pwm_handle, TIM_CHN_4);
    }break;

    default:
    {
      return false;
    }break;
  }

  /* Drive is enabled */
  _enabled = true;

  return true;
}

const bool Motorcontroller::disable()
{
  /* Check if class is initialized */
  if(!_is_initialized) return false;

  /* Check if function was already called*/
  if(!_enabled) return true;

  /* Reset PWM values */
  setDutyCycle(0.0f);

  /* Reset enable pin */
  HAL_GPIO_WritePin(_enable_pin.port, _enable_pin.pin, GPIO_PIN_SET);

  /* Set brake inactive */
  HAL_GPIO_WritePin(_brake_pin.port, _brake_pin.pin, GPIO_PIN_SET);

  /* Reset direction */
  HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_RESET);

  /* Drive is disabled */
  _enabled = false;

  return true;
}

void Motorcontroller::enableBrake()
{
  HAL_GPIO_WritePin(_brake_pin.port, _brake_pin.pin, GPIO_PIN_SET);
}

void Motorcontroller::disableBrake()
{
  HAL_GPIO_WritePin(_brake_pin.port, _brake_pin.pin, GPIO_PIN_RESET);
}

void Motorcontroller::HallInterrupt(const TIM_HandleTypeDef* htim)
{
  /* Check if interrupt was triggered by timer of this controller */
  if(htim->Instance == _tim_hall_handle.Instance)
  {
    /* New hall event store value */
    _hall_event = true;
    _hall_timer_count = htim->Instance->CCR1;
  }
}

/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/

const bool Motorcontroller::setDutyCycle(const float duty_cycle)
{
  /* Check correct range */
  if(duty_cycle < -100.0f || duty_cycle > 100.0f)
    return false;

  /* Calculate raw duty cycle */
  uint16_t raw_value = static_cast<uint16_t>(_duty_cycle_factor * abs(duty_cycle));

  /* Limit raw value */
  if(raw_value > _tim_pwm_handle.Init.Period)
    raw_value = _tim_pwm_handle.Init.Period;

  /* Check direction */
  if(duty_cycle >= 100.0f)
  {
    HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_RESET);
  }

  /* Set duty cycle depending on channel */
  switch(_tim_pwm_chnl)
  {
    case TIM_CHN_1:
    {
      _tim_pwm_handle.Instance->CCR1 = raw_value;
    }break;

    case TIM_CHN_2:
    {
      _tim_pwm_handle.Instance->CCR2 = raw_value;
    }break;

    case TIM_CHN_3:
    {
      _tim_pwm_handle.Instance->CCR3 = raw_value;
    }break;

    case TIM_CHN_4:
    {
      _tim_pwm_handle.Instance->CCR4 = raw_value;
    }break;

    default:
    {
      return false;
    }break;
  }


  return true;
}

/* -------------------------------------------------------------------------------*/

/*! @} End of francor */
