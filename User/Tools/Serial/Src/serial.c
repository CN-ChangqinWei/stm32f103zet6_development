#include"serial.h"
#include"memory_poll.h"

Serial serial1;
uint8_t sendBuf1[_SERIAL_BUF_SIZE]={0};
Serial NewSerial(UART_HandleTypeDef* uart,
    int       recvBufSize,
    uint8_t * sendBuf,
    uint32_t  sendLen
    #ifdef HAL_DMA_MODULE_ENABLED
    ,DMA_HandleTypeDef* dmaTX,
    DMA_HandleTypeDef* dmaRX
    #endif
){
    Serial serial={0};
    serial.uart = uart;
    serial.recvRingBuf = NewRingBuf(recvBufSize);
    serial.sendBuf = sendBuf;
    serial.sendLen = sendLen;
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
    if(serial == NULL || serial->uart == NULL) return;
    HAL_UART_Receive_IT(serial->uart, &serial->rxTmp, 1);
}
void SerialStopRecvIT(Serial* serial){
    if(serial == NULL || serial->uart == NULL) return;
    HAL_UART_AbortReceive_IT(serial->uart);
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//     if(huart == serial1.uart){
//         RingBufAddByte(&serial1.recvRingBuf, serial1.rxTmp);
//         HAL_UART_Receive_IT(serial1.uart, &serial1.rxTmp, 1);
//     }
// }

uint8_t SerialRecvIT(Serial* serial){
    return RingBufPop(&serial->recvRingBuf);
}

uint8_t* SerialRecvPause(Serial* serial, uint8_t* buf, uint32_t len, uint32_t timeout) {
    if (serial == NULL || buf == NULL || len == 0 || serial->uart == NULL) return NULL;
    SerialStopRecvIT(serial);
    HAL_StatusTypeDef status = HAL_UART_Receive(serial->uart, buf, len, timeout);
    
    if (status == HAL_OK) {
        serial->rxLen = len;
        serial->recvFinishFlag = 1;
        SerialStartRecvIT(serial);
        return buf;
    }
    
    SerialStartRecvIT(serial);
    return NULL;
}

void  SerialHandler(Serial* serial){
    if(serial == NULL || serial->recvRingBuf.len == 0) return;
    uint8_t data = SerialRecvIT(serial);
}




uint8_t SerialSetRecvBuf(Serial* serial, int size){
    if(serial == NULL || size <= 0) return 1;
    SerialStopRecvIT(serial);
    if(serial->recvRingBuf.unit != NULL) {
        MemoryPollFree(serial->recvRingBuf.unit->start);
        MemoryPollFree(serial->recvRingBuf.unit);
    }
    serial->recvRingBuf = NewRingBuf(size);
    SerialStartRecvIT(serial);
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
    if(serial == NULL || serial->uart == NULL) return 0;
    
    uint32_t recvLen = 0;
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaRX != NULL) {
        uint8_t tmpBuf[_SERIAL_BUF_SIZE];
        recvLen = HAL_UART_Receive_DMA(serial->uart, tmpBuf, _SERIAL_BUF_SIZE);
        for(uint32_t i = 0; i < recvLen; i++) {
            RingBufAddByte(&serial->recvRingBuf, tmpBuf[i]);
        }
    } else
    #endif
    {
        uint8_t tmpBuf[_SERIAL_BUF_SIZE];
        HAL_StatusTypeDef status = HAL_UART_Receive(serial->uart, tmpBuf, _SERIAL_BUF_SIZE, HAL_MAX_DELAY);
        if(status == HAL_OK) {
            recvLen = _SERIAL_BUF_SIZE;
            for(uint32_t i = 0; i < recvLen; i++) {
                RingBufAddByte(&serial->recvRingBuf, tmpBuf[i]);
            }
        }
    }
    return recvLen;
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
    
    #ifdef HAL_DMA_MODULE_ENABLED
    if(serial->dmaRX != NULL) {
        HAL_UART_Receive_DMA(serial->uart, buf, len);
    } else
    #endif
    {
        HAL_UART_Receive(serial->uart, buf, len, HAL_MAX_DELAY);
    }
    
    for(uint32_t i = 0; i < len; i++) {
        RingBufAddByte(&serial->recvRingBuf, buf[i]);
    }
    return len;
}

int      SerialBufLen(Serial* serial){
    if(serial == NULL || serial->recvRingBuf.unit == NULL) return 0;
    return serial->recvRingBuf.len;
}//获取对应serial实体缓冲区剩余数据的字节数

uint32_t SerialReadBytes(Serial* serial,char* buf,int len){
    if(serial == NULL || buf == NULL || len <= 0 || serial->recvRingBuf.unit == NULL) return 0;
    return RingBufRead(&serial->recvRingBuf, buf, len);
}//获取指定长度数据到buf,返回值是实际长度
