/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define M1_SPD_Pin GPIO_PIN_0
#define M1_SPD_GPIO_Port GPIOC
#define M2_SPD_Pin GPIO_PIN_1
#define M2_SPD_GPIO_Port GPIOC
#define M3_SPD_Pin GPIO_PIN_2
#define M3_SPD_GPIO_Port GPIOC
#define M4_SDP_Pin GPIO_PIN_3
#define M4_SDP_GPIO_Port GPIOC
#define M1_HA_Pin GPIO_PIN_0
#define M1_HA_GPIO_Port GPIOA
#define M1_HB_Pin GPIO_PIN_1
#define M1_HB_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define M3_EN_Pin GPIO_PIN_4
#define M3_EN_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define M2_HA_Pin GPIO_PIN_6
#define M2_HA_GPIO_Port GPIOA
#define M2_HB_Pin GPIO_PIN_7
#define M2_HB_GPIO_Port GPIOA
#define M3_BRK_Pin GPIO_PIN_4
#define M3_BRK_GPIO_Port GPIOC
#define M3_DIR_Pin GPIO_PIN_5
#define M3_DIR_GPIO_Port GPIOC
#define M2_HC_Pin GPIO_PIN_0
#define M2_HC_GPIO_Port GPIOB
#define M2_ALM_Pin GPIO_PIN_1
#define M2_ALM_GPIO_Port GPIOB
#define M1_ALM_Pin GPIO_PIN_2
#define M1_ALM_GPIO_Port GPIOB
#define M1_HC_Pin GPIO_PIN_10
#define M1_HC_GPIO_Port GPIOB
#define M1_DIR_Pin GPIO_PIN_12
#define M1_DIR_GPIO_Port GPIOB
#define M1_EN_Pin GPIO_PIN_13
#define M1_EN_GPIO_Port GPIOB
#define M1_BRK_Pin GPIO_PIN_14
#define M1_BRK_GPIO_Port GPIOB
#define M2_EN_Pin GPIO_PIN_15
#define M2_EN_GPIO_Port GPIOB
#define M4_HA_Pin GPIO_PIN_6
#define M4_HA_GPIO_Port GPIOC
#define M4_HB_Pin GPIO_PIN_7
#define M4_HB_GPIO_Port GPIOC
#define M4_HC_Pin GPIO_PIN_8
#define M4_HC_GPIO_Port GPIOC
#define M2_BRK_Pin GPIO_PIN_9
#define M2_BRK_GPIO_Port GPIOC
#define M1_PWM_Pin GPIO_PIN_8
#define M1_PWM_GPIO_Port GPIOA
#define M2_PWM_Pin GPIO_PIN_9
#define M2_PWM_GPIO_Port GPIOA
#define M3_PWM_Pin GPIO_PIN_10
#define M3_PWM_GPIO_Port GPIOA
#define M4_PWM_Pin GPIO_PIN_11
#define M4_PWM_GPIO_Port GPIOA
#define M2_DIR_Pin GPIO_PIN_12
#define M2_DIR_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define M4_ALM_Pin GPIO_PIN_15
#define M4_ALM_GPIO_Port GPIOA
#define M4_EN_Pin GPIO_PIN_10
#define M4_EN_GPIO_Port GPIOC
#define M4_BRK_Pin GPIO_PIN_11
#define M4_BRK_GPIO_Port GPIOC
#define M4_DIR_Pin GPIO_PIN_12
#define M4_DIR_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define M3_ALM_Pin GPIO_PIN_5
#define M3_ALM_GPIO_Port GPIOB
#define M3_HA_Pin GPIO_PIN_6
#define M3_HA_GPIO_Port GPIOB
#define M3_HB_Pin GPIO_PIN_7
#define M3_HB_GPIO_Port GPIOB
#define M3_HC_Pin GPIO_PIN_8
#define M3_HC_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
