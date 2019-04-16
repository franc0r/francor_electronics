/**
 * @file motorcontroller.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Motorcontroller class
 *
 * @version 0.1
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 *
 */

#ifndef FRANCOR_MOTORCONTROLLER_H_
#define FRANCOR_MOTORCONTROLLER_H_

#ifdef __cplusplus /* Compile only in cpp */

/* Includes ----------------------------------------------------------------------*/
#include <stdbool.h>
#include <limits>
#include <cstdio>
#include <cinttypes>

#include "stm32f4xx_hal.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
//! FRANCOR namespace providing all classes, functions etc.
//! developed by FRANCOR e.V.
namespace francor
{

/* Enum Struct Definitions -------------------------------------------------------*/

/**
 * @brief Enumeration of timer channels
 */
enum TIM_Channel
{
  TIM_CHN_1   = 0x0000U,//!< TIM_CHN_1
  TIM_CHN_2   = 0x0004U,//!< TIM_CHN_2
  TIM_CHN_3   = 0x0008U,//!< TIM_CHN_3
  TIM_CHN_4   = 0x000CU,//!< TIM_CHN_4
  TIM_CHN_ALL = 0x0018U,//!< TIM_CHN_ALL
  TIM_CHN_NONE= 0xFFFFU //!< TIM_CHN_NONE
};

/**
 * @brief Representation of a GPIO Pin
 */
struct GPIOPin
{
  /**
   * @brief Default constructor
   */
  GPIOPin() : port(nullptr), pin(0) {}

  /**
   * @brief Constructs a new GPIO pin instance
   *
   * @param gpio_port Port of the pin
   * @param gpio_pin  Pin
   */
  GPIOPin(GPIO_TypeDef* gpio_port,
          uint16_t gpio_pin) :
            port(gpio_port),
            pin(gpio_pin) {}

  GPIO_TypeDef*   port; //!< GPIO Port
  uint16_t        pin;  //!< Pin
};

/* -------------------------------------------------------------------------------*/

/* Class Definitions -------------------------------------------------------------*/

/**
 * @brief This class represents the BLD 300B motorcontroller
 *        connected to the interface board
 */
class Motorcontroller
{
 public:

  /**
   * @brief Constructs a new motorcontroller instance
   *
   * @param tim_pwm Handle of timer used to generate PWM signal
   * @param tim_pwm_chnl PWM channel of timer used for this drive
   * @param tim_hall Handle of timer used to read hall sensors
   */
  Motorcontroller(TIM_HandleTypeDef& tim_pwm,
                  const TIM_Channel tim_pwm_chnl,
                  TIM_HandleTypeDef& tim_hall);


  /**
   * @brief Initialize the motorcontroller and its components
   *
   * @param enable_pin Enable control GPIO pin
   * @param brake_pin  Brake control GPIO pin
   * @param direction_pin Direction control GPIO pin
   *
   * @return true: success false: error
   */
  const bool init(const GPIOPin& enable_pin,
                  const GPIOPin& brake_pin,
                  const GPIOPin& direction_pin);



 private:

  TIM_HandleTypeDef&  _tim_pwm_handle;  //!< Handle of timer used for PWM generation
  const TIM_Channel   _tim_pwm_chnl;    //!< Channel used for PWM generation

  TIM_HandleTypeDef&  _tim_hall_handle; //!< Handle of timer used to read hall sensors

  GPIOPin   _enable_pin;    //!< Pin to enable drive
  GPIOPin   _brake_pin;     //!< Pin to activate/deactivate brake
  GPIOPin   _direction_pin; //!< Pin to set direction


  bool  _is_initialized; //!< Flag stores true if init() was called

};

/* -------------------------------------------------------------------------------*/

}; /* namespace francor */
/*! @} End of francor */

#endif /* __cplusplus */

#endif /* FRANCOR_MOTORCONTROLLER_H_ */
