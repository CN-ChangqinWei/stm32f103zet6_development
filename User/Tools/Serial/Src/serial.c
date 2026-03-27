#include"serial.h"

Serial NewSerial(UART_HandleTypeDef* uart,
    uint8_t * recvBuf,
    uint32_t  recvLen,
    uint8_t * sendBuf,
    uint32_t  sendLen
    #ifdef HAL_DMA_MODULE_ENABLED
    ,DMA_HandleTypeDef* dma
    #endif
){
    Serial serial;
    serial.uart = uart;
    serial.recvBuf = recvBuf;
    serial.recvLen = recvLen;
    serial.sendBuf = sendBuf;
    serial.sendLen = sendLen;
    #ifdef HAL_DMA_MODULE_ENABLED
    serial.dma = dma;
    #endif
    return serial;
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
    if(serial->dma != NULL) {
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
    if(serial->dma != NULL) {
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
    if(serial->dma != NULL) {
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
    if(serial->dma != NULL) {
        HAL_UART_Receive_DMA(serial->uart, buf, len);
    } else
    #endif
    {
        HAL_UART_Receive(serial->uart, buf, len, HAL_MAX_DELAY);
    }
    return len;
}
