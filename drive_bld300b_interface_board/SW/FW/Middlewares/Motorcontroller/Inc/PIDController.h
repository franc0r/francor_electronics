/**
 * @file PIDController.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief PID Controller class
 *
 * @version 0.1
 * @date 2019-04-19
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 *
 */
#ifndef FRANCOR_PID_CONTROLLER_H_
#define FRANCOR_PID_CONTROLLER_H_

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

class PIDController
{
 public:

  /**
   * @brief Create new instance of PID controller
   */
  PIDController(void);

  /**
   * @brief Initialize PID controller
   *
   * @param kp P-Factor
   * @param ki I-Factor
   * @param kd D-Factor
   * @param limit_min Maximum output value
   * @param limit_max Minimum output value
   */
  void init(const float kp, const float ki, const float kd,
            const float limit_min, const float limit_max);

  /**
   * @brief Updates the controller
   *
   * @param target_value Target value to set
   * @param delta_time
   * @return
   */
  const float update(const float target_value,
                     const float current_value,
                     const float delta_time);

 private:

  float _kp;  //!< P-Factor
  float _ki;  //!< I-Factor
  float _kd;  //!< D-Factor
  float _limit_min; //!< Minimal output value
  float _limit_max; //!< Maximum output value

  float _integral_value;  //!< I value

};

/* -------------------------------------------------------------------------------*/

}; /* namespace francor */
/*! @} End of francor */

#endif /* __cplusplus */

#endif /* FRANCOR_PID_CONTROLLER_H_ */
