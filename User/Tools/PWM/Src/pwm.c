#include"pwm.h"

struct PWM NewPWM(TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef config,uint32_t channel){
    struct PWM res ={
        .tim=tim,
        .config=config,
        .channel=channel
    };
    return res;
}
uint8_t PWMStart(struct PWM* pwm){
    if(pwm == NULL){
        return 1;
    }
    if(HAL_TIM_PWM_Start(pwm->tim,pwm->channel) != HAL_OK){
        return 1;
    }
    return 0;
}
uint8_t PWMClose(struct PWM* pwm){
    if(pwm == NULL){
        return 1;
    }
    if(HAL_TIM_PWM_Stop(pwm->tim,pwm->channel) != HAL_OK){
        return 1;
    }
    return 0;
}
uint8_t PWMSet(struct PWM* pwm,uint16_t pluse){ //直接设置pluse
    if(pwm == NULL){
        return 1;
    }
    __HAL_TIM_SET_COMPARE(pwm->tim,pwm->channel,pluse);
    return 0;
}
uint8_t PWMSetByRate(struct PWM* pwm,uint32_t num,uint32_t den){//使用分数的形式控制百分比设置PWM
    if(pwm == NULL || den == 0){
        return 1;
    }
    uint32_t arr = pwm->tim->Instance->ARR;
    uint16_t pluse = (uint16_t)((arr * num) / den);
    __HAL_TIM_SET_COMPARE(pwm->tim,pwm->channel,pluse);
    return 0;
}
