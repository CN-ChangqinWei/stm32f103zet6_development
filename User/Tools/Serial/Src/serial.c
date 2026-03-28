#include"serial.h"

Serial seria1;

Serial NewSerial(UART_HandleTypeDef* uart,
    uint8_t * recvBuf,
    uint32_t  recvLen,
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
    
    #ifdef HAL_DMA_MODULE_ENABLED
    serial.dmaTX = dmaTX;
    serial.dmaRX=dmaRX;
    #endif
    return serial;
}
uint8_t SerialsInit(){

    return 0;
}
void  SerialHandler(Serial* serial){
    uint32_t tmp_flag = 0;
	uint32_t temp;
	tmp_flag =__HAL_UART_GET_FLAG(serial->uart,UART_FLAG_IDLE); //获取IDLE标志位
	if((tmp_flag != RESET))//idle标志被置位
	{ 
		__HAL_UART_CLEAR_IDLEFLAG(serial->uart);//清除标志位
		//temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
		//temp = huart1.Instance->DR; //读取数据寄存器中的数据
		//这两句和上面那句等效
		HAL_UART_DMAStop(serial->uart); //
		temp  =  __HAL_DMA_GET_COUNTER(serial->dmaRX);// 获取DMA中未传输的数据个数   
		//temp  = hdma_usart1_rx.Instance->NDTR;//读取NDTR寄存器 获取DMA中未传输的数据个数，
		//这句和上面那句等效
		serial->rxLen =  serial->recvLen - temp; //总计数减去未传输的数据个数，得到已经接收的数据个数
		serial->recvFinishFlag = 1;	// 接受完成标志位置1	
	 }
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
