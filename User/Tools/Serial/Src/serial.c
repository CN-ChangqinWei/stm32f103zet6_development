#include"serial.h"

Serial serial1;
uint8_t recvBuf1[_SERIAL_BUF_SIZE]={0};
uint8_t sendBuf1[_SERIAL_BUF_SIZE]={0};
Serial NewSerial(UART_HandleTypeDef* uart,
    uint8_t * recvBuf,
    uint32_t  recvLen,
    uint32_t  curHead,
    uint8_t * sendBuf,
    uint32_t  sendLen
    #ifdef HAL_DMA_MODULE_ENABLED
    ,DMA_HandleTypeDef* dmaTX,
    DMA_HandleTypeDef* dmaRX
    #endif
){
    Serial serial={0};
    serial.uart = uart;
    serial.recvBuf = recvBuf;
    serial.recvLen = recvLen;
    
    serial.sendBuf = sendBuf;
    serial.sendLen = sendLen;
    //SerialStartRecvIT(&serial);
    #ifdef HAL_DMA_MODULE_ENABLED
    serial.dmaTX = dmaTX;
    serial.dmaRX=dmaRX;
    #endif
    return serial;
}
uint8_t SerialsInit(){
    return 0;
}

void SerialStartRecvIT(Serial* serial){
   __HAL_UART_ENABLE_IT(serial->uart, UART_IT_RXNE);
   __HAL_UART_ENABLE_IT(serial->uart, UART_IT_IDLE);
//    serial->recvCur++;
//    serial->recvCur%=serial->recvLen;
}
void SerialStopRecvIT(Serial* serial){
    __HAL_UART_DISABLE_IT(serial->uart,UART_IT_RXNE);
}
uint8_t SerialRecvIT(Serial* serial){
    uint8_t data=serial->recvBuf[serial->recvCur++];
    serial->recvCur%=serial->recvLen;
    HAL_UART_Receive_IT(serial->uart,serial->recvBuf+serial->recvCur,1);
    
    return data;
}

uint8_t* SerialRecvPause(Serial* serial, uint8_t* buf, uint32_t len, uint32_t timeout) {
    if (serial == NULL || buf == NULL || len == 0 || serial->uart == NULL) return NULL;
    //SerialStopRecvIT(serial);
    HAL_StatusTypeDef status = HAL_UART_Receive(serial->uart, buf, len, timeout);
    
    if (status == HAL_OK) {
        serial->rxLen = len;
        serial->recvFinishFlag = 1;
        return buf;
    }
    
    //SerialStartRecvIT(serial);
    return NULL;
}

void  SerialHandler(Serial* serial){
    uint8_t data = SerialRecvIT(serial);
    SerialSendUseOtherBuf(serial,&data,1);
}




uint8_t SerialSetRecvBuf(Serial* serial, uint8_t* buf, uint32_t len){
    if(serial == NULL || buf == NULL) return 1;
    serial->recvBuf = buf;
    serial->recvLen = len;
    return 0;
}

uint8_t SerialSetSendBuf(Serial* serial, uint8_t* buf, uint32_t len){
    if(serial == NULL || buf == NULL) return 1;
    serial->sendBuf = buf;
    serial->sendLen = len;
    return 0;
}

uint32_t SerialSend(Serial* serial, uint32_t len){
    if(serial == NULL || serial->sendBuf == NULL || serial->uart == NULL) return 0;
    if(len > serial->sendLen) len = serial->sendLen;
    
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaTX != NULL) {
        HAL_UART_Transmit_DMA(serial->uart, serial->sendBuf, len);
    } else
    #endif
    {
        HAL_UART_Transmit(serial->uart, serial->sendBuf, len, HAL_MAX_DELAY);
    }
    return len;
}

uint32_t SerialRecv(Serial* serial){
    if(serial == NULL || serial->recvBuf == NULL || serial->uart == NULL) return 0;
    
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaRX != NULL) {
        HAL_UART_Receive_DMA(serial->uart, serial->recvBuf, serial->recvLen);
    } else
    #endif
    {
        HAL_UART_Receive(serial->uart, serial->recvBuf, serial->recvLen, HAL_MAX_DELAY);
    }
    return serial->recvLen;
}

uint32_t SerialSendUseOtherBuf(Serial* serial, uint8_t* buf, uint32_t len){
    if(serial == NULL || buf == NULL || serial->uart == NULL) return 0;
    
    if(buf == serial->sendBuf) {
        return SerialSend(serial, len);
    }
    
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaTX != NULL) {
        HAL_UART_Transmit_DMA(serial->uart, buf, len);
    } else
    #endif
    {
        HAL_UART_Transmit(serial->uart, buf, len, HAL_MAX_DELAY);
    }
    return len;
}

uint32_t SerialRecvUseOtherBuf(Serial* serial, uint8_t* buf, uint32_t len){
    if(serial == NULL || buf == NULL || serial->uart == NULL) return 0;
    
    if(buf == serial->recvBuf) {
        return SerialRecv(serial);
    }
    
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaRX != NULL) {
        HAL_UART_Receive_DMA(serial->uart, buf, len);
    } else
    #endif
    {
        HAL_UART_Receive(serial->uart, buf, len, HAL_MAX_DELAY);
    }
    return len;
}


