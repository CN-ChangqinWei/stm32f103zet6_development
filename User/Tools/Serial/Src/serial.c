#include"serial.h"
#include"portable.h"
#include"stdio.h"
#include"string.h"
extern UART_HandleTypeDef huart2;
Serial* serial1=NULL;
uint8_t sendBuf1[_SERIAL_BUF_SIZE]={0};
uint8_t ringBuf[_SERIAL_BUF_SIZE]={0};
extern DMA_HandleTypeDef hdma_usart2_rx;
//#define _SERIAL_BUF_USE_STATIC
Serial* NewSerial(UART_HandleTypeDef* uart,
    int       recvBufSize,
    uint8_t * sendBuf,
    uint32_t  sendLen
    #ifdef HAL_DMA_MODULE_ENABLED
    ,DMA_HandleTypeDef* dmaTX,
    DMA_HandleTypeDef* dmaRX
    #endif
){
    Serial* serial=(Serial*)pvPortMalloc(sizeof(Serial));
    if(serial == NULL) return NULL;
    serial->uart = uart;
    #ifdef _SERIAL_BUF_USE_STATIC
    serial->recvRingBuf = NewRingBufByOther(recvBufSize,ringBuf);
    #else
    serial->recvRingBuf = NewRingBuf(recvBufSize);
    #endif
    
    
    serial->sendBuf = sendBuf;
    serial->sendLen = sendLen;
    #ifdef HAL_DMA_MODULE_ENABLED
    serial->dmaTX = dmaTX;
    serial->dmaRX=dmaRX;
    if(serial->dmaRX!=NULL){
        serial->bufDmaRX=pvPortMalloc(_SERIAL_DMA_BUF_SIZE);
        
    }
    if(serial->dmaTX!=NULL){
        serial->bufDmaTX=pvPortMalloc(_SERIAL_DMA_BUF_SIZE);
        
    }
    #endif
    return serial;
}

void DeleteSerial(Serial* serial){
    if(serial == NULL) return;
    if(serial->recvRingBuf.buffer != NULL){
        vPortFree(serial->recvRingBuf.buffer);
    }
    if(serial->bufDmaRX) vPortFree(serial->bufDmaRX);
    if(serial->bufDmaTX) vPortFree(serial->bufDmaTX);
    vPortFree(serial);
}

uint8_t SerialsInit(){
    serial1 = NewSerial(&huart2,_SERIAL_BUF_SIZE,sendBuf1, _SERIAL_BUF_SIZE, 0, &hdma_usart2_rx);
    if(serial1 == NULL) return 1;
    //SerialStartRecvIT(serial1);
    SerialStartRecvDMA(serial1);
    return 0;
}

void SerialStartRecvIT(Serial* serial){
    if(serial == NULL || serial->uart == NULL) return;
    HAL_UART_Receive_IT(serial->uart, (uint8_t*)&serial->rxTmp, 1);

}
void SerialStopRecvIT(Serial* serial){
    if(serial == NULL || serial->uart == NULL) return;
    RingBufAddByte(&serial->recvRingBuf,serial->rxTmp);
    //SerialStartRecvIT(serial);
}
void SerialStartRecvDMA(Serial* serial){
    serial->rCurDmaRX=0;
    serial->wCurDmaRX=0;
    HAL_UARTEx_ReceiveToIdle_DMA(serial->uart,serial->bufDmaRX,_SERIAL_DMA_BUF_SIZE);
    __HAL_UART_ENABLE_IT(serial->uart,UART_IT_IDLE);
    __HAL_DMA_DISABLE_IT(serial->dmaRX, DMA_IT_HT);
}
uint8_t SerialRecvDMA(Serial* serial){//移动dma读写指针并且把dma缓冲区的内容存入recvRingbuf
    if(serial == NULL || serial->dmaRX == NULL || serial->bufDmaRX == NULL) return 1;
    
    // 获取DMA剩余传输计数
    int nBytesRemain = __HAL_DMA_GET_COUNTER(serial->dmaRX);
    // 计算已接收字节数 = 缓冲区总大小 - 剩余计数
    int nBytesRecv = _SERIAL_DMA_BUF_SIZE - nBytesRemain;
    
    if(nBytesRecv == 0) return 0; // 没有新数据
    
    // 获取写入指针位置（相对于DMA缓冲区起始地址）
    int wPos = serial->wCurDmaRX;
    
    // 计算本次需要读取的字节数
    int bytesToRead = nBytesRecv;
    
    // 处理环形缓冲区环绕的情况
    if(wPos + bytesToRead > _SERIAL_DMA_BUF_SIZE) {
        // 第一部分：从当前位置到缓冲区末尾
        int firstPart = _SERIAL_DMA_BUF_SIZE - wPos;
        RingBufAddData(&serial->recvRingBuf, serial->bufDmaRX + wPos, firstPart);
        // 第二部分：从缓冲区开头到剩余数据
        int secondPart = bytesToRead - firstPart;
        RingBufAddData(&serial->recvRingBuf, serial->bufDmaRX, secondPart);
        serial->wCurDmaRX = secondPart;
    } else {
        // 没有环绕，直接读取
        RingBufAddData(&serial->recvRingBuf, serial->bufDmaRX + wPos, bytesToRead);
        serial->wCurDmaRX = wPos + bytesToRead;
        if(serial->wCurDmaRX >= _SERIAL_DMA_BUF_SIZE) {
            serial->wCurDmaRX = 0;
        }
    }
    SerialStartRecvDMA(serial);
    return 0;
}
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//     if(huart == serial1.uart){
//         RingBufAddByte(&serial1.recvRingBuf, serial1.rxTmp);
//         HAL_UART_Receive_IT(serial1.uart, &serial1.rxTmp, 1);
//     }
// }

uint8_t SerialRecvIT(Serial* serial){
    RingBufAddByte(&serial->recvRingBuf,serial->rxTmp);
    HAL_UART_Receive_IT(serial->uart, (uint8_t*)&serial->rxTmp, 1);
    // SerialSendUseOtherBuf(serial,(uint8_t*)&serial->recvRingBuf.len,sizeof(serial->recvRingBuf.len));
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
    if(serial == NULL ) return;
   
    //SerialSendUseOtherBuf(serial, (uint8_t*)&serial->rxTmp, 1);
    SerialRecvIT(serial);
    // SerialReadBytes(serial,(uint8_t*)&byte,1);
    // SerialSendUseOtherBuf(serial, (uint8_t*)&serial->rxTmp, 1);
    //SerialSendUseOtherBuf(serial, (uint8_t*)&serial->rxTmp, sizeof(uint32_t));
    //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5));
}

void  SerialDmaHandler(Serial* serial){
    if(serial == NULL ) return;
    //__HAL_UART_CLEAR_IDLEFLAG(serial->uart);
    SerialRecvDMA(serial);
    #ifdef _DEBUG
    int len=SerialBufLen(serial);
    char* buf = pvPortMalloc(len);
    SerialReadBytes(serial,buf,len);
    SerialSendUseOtherBuf(serial,buf,len);
    SerialSendUseOtherBuf(serial,"idel\n",strlen("idel\n"));
    #endif
    
}


uint8_t SerialSetRecvBuf(Serial* serial, int size){
    if(serial == NULL || size <= 0) return 1;
    SerialStopRecvIT(serial);
    if(serial->recvRingBuf.buffer != NULL) {
        vPortFree(serial->recvRingBuf.buffer);
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

int     SerialBufLen(Serial* serial){
    if(serial == NULL || serial->recvRingBuf.buffer == NULL) return 0;
    // 关中断保护，防止len被中断修改时读取到不一致值
    // uint32_t primask = __get_PRIMASK();
    // __disable_irq();
    int len = serial->recvRingBuf.len;
    //__set_PRIMASK(primask);
    return len;
}//获取对应serial实体缓冲区剩余数据的字节数

uint32_t SerialReadBytes(Serial* serial,char* buf,int len){
    if(serial == NULL || buf == NULL || len <= 0 || serial->recvRingBuf.buffer == NULL) return 0;
    return RingBufRead(&serial->recvRingBuf, buf, len);
}//获取指定长度数据到buf,返回值是实际长度
