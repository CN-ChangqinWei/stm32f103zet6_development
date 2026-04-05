#ifndef _SERIAL_H
#define _SERIAL_H
#include "stm32f1xx_hal.h"
#include "ring_buf.h"
#include"cmsis_os.h"
#define _SERIAL_BUF_SIZE    255
typedef struct{
    UART_HandleTypeDef* uart;
    RingBuf   recvRingBuf;//接收环形缓冲区
    uint8_t * sendBuf;//发送数组的指针
    uint32_t  sendLen;//发送数组的长度
    uint32_t  rxLen;
    uint32_t  txLen;
    uint8_t   recvFinishFlag;//接收完成标志
    uint8_t   rxTmp;//中断接收临时字节
    #ifdef HAL_DMA_MODULE_ENABLED
    DMA_HandleTypeDef* dmaTX;//如果dma不为0那么recvbuf和sendbuf
    DMA_HandleTypeDef* dmaRX;

    #endif

} Serial ;
extern Serial* serial1;
extern uint8_t sendBuf1[255];
Serial* NewSerial(UART_HandleTypeDef* uart,
    int       recvBufSize,
    uint8_t * sendBuf,
    uint32_t  sendLen
    #ifdef HAL_DMA_MODULE_ENABLED
    ,DMA_HandleTypeDef* dmaTX,
    DMA_HandleTypeDef* dmaRX
    
    #endif
);
void DeleteSerial(Serial* serial);

void SerialStartRecvIT(Serial* serial);
uint8_t SerialRecvIT(Serial* serial);
uint8_t* SerialRecvPause(Serial* serial, uint8_t* buf, uint32_t len, uint32_t timeout);
uint8_t SerialsInit();
void    SerialHandler(Serial* serial);
uint8_t SerialSetRecvBuf(Serial* serial, int size);
uint8_t SerialSetSendBuf(Serial* serial,uint8_t* buf, uint32_t len);
uint32_t SerialSend(Serial* serial,uint32_t len);//发送指定字节 如果dma存在且实例中有搬运的地址与结构体设定相同，那么用dma搬运
uint32_t SerialRecv(Serial* serial);//接收字节到recvbuf,dma要求同send
uint32_t SerialSendUseOtherBuf(Serial* serial,uint8_t* buf,uint32_t len);//发送其它的buf如果buf和sendbuf相同那么使用SerialSend
uint32_t SerialRecvUseOtherBuf(Serial* serial,uint8_t* buf,uint32_t len);//接收数据到其它buf 如果buf相同和send同一种处置方式
int      SerialBufLen(Serial* serial);//获取对应serial实体缓冲区剩余数据的字节数
uint32_t SerialReadBytes(Serial* serial,char* buf,int len);//获取指定长度数据到buf,返回值是实际长度

#endif
