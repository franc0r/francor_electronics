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
                                 const GPIOPin& direction_pin,
                                 const GPIOPin& hall_a,
                                 const GPIOPin& hall_b,
                                 const GPIOPin& hall_c) :
                                     _controller(),
                                     _target_speed_rpm(0.0f),
                                     _duty_cycle(0.0f),
                                     _tim_pwm_handle(tim_pwm),
                                     _tim_pwm_chnl(tim_pwm_chnl),
                                     _tim_hall_handle(tim_hall),
                                     _enable_pin(enable_pin),
                                     _brake_pin(brake_pin),
                                     _direction_pin(direction_pin),
                                     _hall_a(hall_a),
                                     _hall_b(hall_b),
                                     _hall_c(hall_c),
                                     _hall_sensor_state(0u),
                                     _direction(0),
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

  /* Init controller */
  _controller.init(0.3f, 0.3f, 0.0f, -50.0f, 50.0f);

  /* Reset values */
  _direction = 0;
  _duty_cycle = 0.0f;
  _enabled = false;
  _hall_event = false;

  /* Class is initialized */
  _is_initialized = true;

  return true;
}

const bool Motorcontroller::update(const float delta_time)
{
  /* Check if class is initialized */
  if(!_is_initialized) return false;

  static const float normal_weight = 0.3f;
  static const float peak_weight = 0.1f;
  static const float peak_deviation = 20.0f;
  static const float no_signal_weight = 0.5f;
  static const float min_speed_cut_off = 0.5f;

  static float timer_no_hall_event = 0.0f;

  /* Check if new hall value is available */
  if(_hall_event)
  {
    _hall_event = false;
    const uint16_t hall_timer_cnt = _hall_timer_count;

    if(hall_timer_cnt > 10u)
    {
      /* Calculate rpm value */
      float current_speed_abs = fabs(_current_speed_rpm);
      const float delta_time = static_cast<float>(hall_timer_cnt) * 0.0001f;
      const float speed_rpm = 60.0f / (delta_time * 3.0f * 15.0f);

      /* Calculate direction */
      static float direction_filtered = 0.0f;
      static uint8_t old_hall_state = 0u;
      int8_t new_direction = 0u;
      if(MOTOR_HALL_STATE_TABLE[old_hall_state] == _hall_sensor_state)
      {
        new_direction = 1;
      }
      else
      {
        new_direction = -1;
      }
      old_hall_state = _hall_sensor_state;

      /* Update rpm with smooth factor */
      const float new_speed = calculateWeightedValued(current_speed_abs, speed_rpm, normal_weight);

      /* Check if deviation is to big */
      const float delta = new_speed - current_speed_abs;

      /*
       * Check if the deviation between the current speed value and the
       * new speed value is greater than the peak deviation. If the deviation
       * is bigger trust value only with peak_weight ( smaller value ).
       */
      if(fabs(delta) <= peak_deviation)
      {
    	  /* Deviation of 30.0f is ok */
        current_speed_abs = new_speed;

    	  /* Calculate direction */
    	  direction_filtered = calculateWeightedValued(direction_filtered, new_direction, normal_weight);

      }
      else
      {
        current_speed_abs = calculateWeightedValued(current_speed_abs, speed_rpm, peak_weight);

        /* Calculate direction */
        direction_filtered = calculateWeightedValued(direction_filtered, new_direction, peak_weight);
      }

      if(direction_filtered >= 0.2f)
      {
        _direction = 1;
      }
      else if(direction_filtered <= -0.2f)
      {
        _direction = -1;
      }

      _current_speed_rpm = static_cast<float>(_direction) * current_speed_abs;

      /* Limit low speed limit */
      if(fabs(_current_speed_rpm) < min_speed_cut_off)
      {
        _current_speed_rpm = 0.0f;
        _direction = 0;
      }
    }

    /* Store last timestamp */
    timer_no_hall_event = 0.0f;
  }
  else
  {
    /* Increase timer */
    timer_no_hall_event += delta_time;

    if(timer_no_hall_event > 1.0f)
    {
      _current_speed_rpm = 0.0f;
      _direction = 0;
    }
  }


  if(_enabled && delta_time != 0.0f)
  {
   // const float new_duty_cylce = _controller.update(_target_speed_rpm, _current_speed_rpm, delta_time);
   // _duty_cycle = calculateWeightedValued(_duty_cycle, new_duty_cylce, 0.5f);
   // setDutyCycle(_duty_cycle);
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
  _target_speed_rpm = 0.0f;
  _duty_cycle = 0.0f;
  setDutyCycle(_duty_cycle);

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
  _target_speed_rpm = 0.0f;
  _duty_cycle = 0.0f;
  setDutyCycle(_duty_cycle);

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
    _hall_sensor_state = 0u;
    _hall_sensor_state = (uint8_t)(HAL_GPIO_ReadPin(_hall_c.port, _hall_c.pin))
                          | (uint8_t)(HAL_GPIO_ReadPin(_hall_b.port, _hall_b.pin) << 1u)
                          | (uint8_t)(HAL_GPIO_ReadPin(_hall_a.port, _hall_a.pin) << 2u);
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
  if(duty_cycle >= 0.0f)
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

const float Motorcontroller::calculateWeightedValued(const float v1,
                                                     const float v2,
                                                     const float weight)
{
  return (1.0f - weight) * v1 + weight * v2;
}

/* -------------------------------------------------------------------------------*/

/*! @} End of francor */
