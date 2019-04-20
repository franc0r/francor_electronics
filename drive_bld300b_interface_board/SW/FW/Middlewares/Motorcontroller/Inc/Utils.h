/**
 * @file Utils.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Utils used for other classes and functions
 *
 * @version 0.1
 * @date 2019-04-17
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus /* Compile only in cpp */

/* Includes ----------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include <math.h>
#include <stdbool.h>
#include <limits>
#include <cstdio>
#include <cinttypes>
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
//! FRANCOR namespace providing all classes, functions etc.
//! developed by FRANCOR e.V.
namespace francor
{

/* Enum Definitions --------------------------------------------------------------*/

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


/* -------------------------------------------------------------------------------*/

/* Struct Definitions ------------------------------------------------------------*/

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

}; /* namespace francor */
/*! @} End of francor */

#endif /* __cplusplus */

#endif /* UTILS_H_ */
