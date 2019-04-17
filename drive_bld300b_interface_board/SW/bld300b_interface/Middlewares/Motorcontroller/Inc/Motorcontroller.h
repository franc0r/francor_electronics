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
#include "Utils.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
//! FRANCOR namespace providing all classes, functions etc.
//! developed by FRANCOR e.V.
namespace francor
{

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
                  TIM_HandleTypeDef& tim_hall,
                  const GPIOPin& enable_pin,
                  const GPIOPin& brake_pin,
                  const GPIOPin& direction_pin);


  /**
   * @brief Initialize the motorcontroller and its components
   *
   * @param enable_pin Enable control GPIO pin
   * @param brake_pin  Brake control GPIO pin
   * @param direction_pin Direction control GPIO pin
   *
   * @return true: success false: error
   */
  const bool init(const float duty_cycle_factor);

  /**
   * @brief Enable drive
   *
   * @return true: success false: error
   */
  const bool enable();

  /**
   * @brief Disable drive
   *
   * @return true: success false: error
   */
  const bool disable();

  /**
   * @brief Enables the brake
   */
  void enableBrake(void);

  /**
   * @brief Disables the brake
   */
  void disableBrake(void);


 //private:

  /**
   * @brief Set duty cycle of powerstage
   *
   * @param duty_cycle Duty cycle in percentage [-100;100]
   * @return
   */
  const bool setDutyCycle(const float duty_cycle);

  TIM_HandleTypeDef&  _tim_pwm_handle;  //!< Handle of timer used for PWM generation
  const TIM_Channel   _tim_pwm_chnl;    //!< Channel used for PWM generation

  TIM_HandleTypeDef&  _tim_hall_handle; //!< Handle of timer used to read hall sensors

  const GPIOPin   _enable_pin;        //!< Pin to enable drive
  const GPIOPin   _brake_pin;         //!< Pin to activate/deactivate brake
  const GPIOPin   _direction_pin;     //!< Pin to set direction

  float     _duty_cycle_factor; //!< Duty cycle factor to calculate raw value

  bool  _enabled; //!< Saves true if drive is enabled

  bool  _is_initialized; //!< Flag stores true if init() was called

};

/* -------------------------------------------------------------------------------*/

}; /* namespace francor */
/*! @} End of francor */

#endif /* __cplusplus */

#endif /* FRANCOR_MOTORCONTROLLER_H_ */
