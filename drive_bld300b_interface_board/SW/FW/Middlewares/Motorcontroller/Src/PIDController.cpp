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
#include "PIDController.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
using namespace francor;

/* Class Public Functions --------------------------------------------------------*/

PIDController::PIDController() :
_kp(0.0f),
_ki(0.0f),
_kd(0.0f),
_limit_min(0.0f),
_limit_max(0.0f),
_integral_value(0.0f)
{

}



void PIDController::init(const float kp, const float ki, const float kd,
                         const float limit_min, const float limit_max)
{
  /* Set parameters */
  _kp = kp;
  _ki = ki;
  _kd = kd;
  _limit_min = limit_min;
  _limit_max = limit_max;
  _integral_value = 0.0f;
}

const float PIDController::update(const float target_value,
                                  const float current_value,
                                  const float delta_time)
{

  /* Error calculation */
  const float e = target_value - current_value;

  /* Update integral */
  const float new_p = _kp * e;
  const float new_i = _integral_value + e * _ki * delta_time;
  const float value = new_p + new_i;

  /* Calculate new output */
  if(value > _limit_max)
  {
    return _limit_max;
  }
  else if(value < _limit_min)
  {
    return _limit_min;
  }

  _integral_value = new_i;

  return value;
}

/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/

/*! @} End of francor */
