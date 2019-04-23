/**
 * @file firmware.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Firmware Header
 *
 * @version 0.1
 * @date 2019-04-26
 *
 * @copyright Copyright (c) 2019 FRANCOR e.V.
 */

#ifndef FIRMWARE_H_
#define FIRMWARE_H_

#ifdef __cplusplus /* Compile only in cpp */

/* Includes ----------------------------------------------------------------------*/
#include <array>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Motorcontroller.h"
/* -------------------------------------------------------------------------------*/

/** @addtogroup francor
 * @{
 */
//! FRANCOR namespace providing all classes, functions etc.
//! developed by FRANCOR e.V.
namespace francor
{

/* Defines -----------------------------------------------------------------------*/

constexpr uint8_t CPU_CLOCK_MHZ             = 180u; //!< CPU Clock in MHz
constexpr uint8_t POWER_STAGE_PWM_FREQ_KHz  = 10u;  //!< PS PWM frequency kHz

constexpr uint8_t NUM_DRIVES = 4u;  //!< Specifies the number of drives

/* -------------------------------------------------------------------------------*/

/* Class Definitions -------------------------------------------------------------*/

/**
 * @brief This class handles the complete firmware of
 *        the microcontroller. All functions (despite HAL functions)
 *        are controlled from here.
 */
class Firmware
{
public:

  /**
   * @brief Constructs a new firmware instance
   */
  Firmware(TIM_HandleTypeDef& power_stage_tim);

  /**
   * @brief Initializes the firmware and all of its components
   */
  void init(void);

  /**
   * @brief Updates the firmware and all of its components
   */
  void update(void);

  /**
   * @brief Callback function for CCR interrupt
   *
   * @param htim Handle of timer which triggered the interrupt
   *
   */
  void HAL_TIM_IC_CaptureCallback(const TIM_HandleTypeDef* htim);

  /**
   * @brief Callback function DMA receive complete
   *
   * @param huart
   */
  void UART_DMAReceiveCplt(const UART_HandleTypeDef* huart);

private:

  /**
   * @brief Initializes the powerstage PWM generation
   *
   * @param pwm_frequency_kHz PWM frequency in kHz
   * @param cpu_clock_MHz CPU clock in MHz
   *
   * @return true: success false: error
   */
  bool initPowerStage(const uint8_t pwm_frequency_kHz,
                      const uint8_t bus_clock_MHz);

  TIM_HandleTypeDef&  _power_stage_tim;     //!< Handle of powerstage timer
  float               _power_stage_pwm_fac; //!< Factor to calculate raw pwm value from percentage

  std::array<Motorcontroller, NUM_DRIVES> _motor_list; //!< List containing all motors
  std::array<float, NUM_DRIVES> _pwm_list; //!< List containing desired pwm

};

/* -------------------------------------------------------------------------------*/

}; /* namespace francor */
/*! @} End of francor */

#endif /* __cplusplus */

/* Public functions --------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup francor_c_interface
 * @{
 */
//! FRANCOR public functions C API

/**
 * @brief Initializes the firmware and calls Firmware::init()
 */
void Firmware_Init(void);

/**
 * @brief Updates the firmware components (none blocking)
 */
void Firmware_Update(void);

/*! @} End of francor_c_interface */

#ifdef __cplusplus
}
#endif
/* -------------------------------------------------------------------------------*/

#endif /* FIRMWARE_H_ */
