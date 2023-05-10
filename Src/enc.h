#ifndef _ENCODER_
#define _ENCODER_

#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"


/*
 * Configure tim_3 to encoder mode
 */
static void tim_3_config(void)
{
  /*  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X4_TI12);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1,
                          LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2,
                          LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV16_N5);
    LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV16_N5);
    LL_TIM_SetAutoReload(TIM3, 64);
    LL_TIM_EnableCounter(TIM3);*/

//    LL_TIM_EnableIT_CC1(TIM3);
//    LL_TIM_EnableCounter(TIM3);
    /*
     * Setup NVIC
     */
//    NVIC_EnableIRQ(TIM3_IRQn);
//    NVIC_SetPriority(TIM3_IRQn, 2);
    return;
}

#endif
