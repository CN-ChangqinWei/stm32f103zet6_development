#include "serial_comm.h"
#include "serial.h"

SerialComm NewSerialComm(Serial* serial) {
    
    SerialComm serialComm ;
    serialComm.serial = serial;
    serialComm.tempRecvBuf = NULL;
    serialComm.tempRecvLen = 0;
    serialComm.tempRecvCur = 0;
    //SerialStartRecvIT(serial);
    return serialComm;
}

void DeleteSerialComm(SerialComm* serialComm) {
    if (serialComm != NULL) {
        MemoryPollFree(serialComm);
    }
}

uint32_t SerialCommSend(void* instance, uint8_t* data, uint32_t len) {
    if (instance == NULL || data == NULL || len == 0) return 0;
    
    SerialComm* serialComm = (SerialComm*)instance;
    if (serialComm->serial == NULL) return 0;
    
    // 使用 Serial 的发送函数发送数据
    return SerialSendUseOtherBuf(serialComm->serial, data, len);
}

uint32_t SerialCommRecv(void* instance, uint8_t* data, uint32_t len) {
    if (instance == NULL || data == NULL || len == 0) return 0;
    
    SerialComm* serialComm = (SerialComm*)instance;
    if (serialComm->serial == NULL) return 0;
    
    // 使用 Serial 模块的阻塞接收函数
    uint32_t readLen = SerialReadBytes(serialComm->serial, data, len);
    
    return readLen;
}

CommInterface GetSerialCommInterface(void) {
    CommInterface interface;
    interface.send = SerialCommSend;
    interface.recv = SerialCommRecv;
    return interface;
}

Communication NewCommunicationFromSerial(SerialComm* instance) {
    Communication comm = {0};
    
    if (instance == NULL) return comm;
    
    // 设置接口
    comm.interface = GetSerialCommInterface();
    comm.instance = instance;
    comm.statu = MODE_LEN;
    comm.recvLen = 0;
    comm.packageBuf = NULL;
    
    return comm;
}
