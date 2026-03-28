#include"serial_test.h"
#include "serial.h"
#include "stm32f1xx_hal_gpio.h"
#include<string.h>
void SerialTest(Serial* serial){
    SerialStartRecvIT(serial);
    while(1){
        HAL_Delay(5000);
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5));
        SerialSendUseOtherBuf(serial,(uint8_t*)"testing\n",(uint32_t)strlen("testing\n"));
    }
}
