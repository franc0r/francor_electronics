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
#include "motorcontroller.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
using namespace francor;

/* Class Public Functions --------------------------------------------------------*/

Motorcontroller::Motorcontroller(TIM_HandleTypeDef& tim_pwm,
                                 const TIM_Channel tim_pwm_chnl,
                                 TIM_HandleTypeDef& tim_hall) :
                                     _tim_pwm_handle(tim_pwm),
                                     _tim_pwm_chnl(tim_pwm_chnl),
                                     _tim_hall_handle(tim_hall),
                                     _enable_pin(),
                                     _brake_pin(),
                                     _direction_pin(),
                                     _is_initialized(false)
{

}


const bool Motorcontroller::init(const GPIOPin& enable_pin,
                                 const GPIOPin& brake_pin,
                                 const GPIOPin& direction_pin)
{
  /* Check if function was already called */
  if(_is_initialized) return false;

  /* Check for nullptrs */
  if(nullptr == enable_pin.port ||
      nullptr == brake_pin.port ||
      nullptr == direction_pin.port)
  {
    return false;
  }

  /* Store variables */
  _enable_pin = enable_pin;
  _brake_pin = brake_pin;
  _direction_pin = direction_pin;

  /* Disable drive */
  HAL_GPIO_WritePin(_enable_pin.port, _enable_pin.pin, GPIO_PIN_SET);

  /* Disable brake */
  HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_SET);

  /* Set direction to FWD */
  HAL_GPIO_WritePin(_direction_pin.port, _direction_pin.pin, GPIO_PIN_RESET);

  /* Class is initialized */
  _is_initialized = true;

  return true;
}
/* -------------------------------------------------------------------------------*/

/* Class Private Functions -------------------------------------------------------*/
/* -------------------------------------------------------------------------------*/

/*! @} End of francor */
