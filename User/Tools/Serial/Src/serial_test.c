#include"serial_test.h"
#include "serial.h"

void SerialTest(Serial* serial){
    while(1){
        if(serial->recvFinishFlag){
            SerialSendUseOtherBuf(serial, serial->recvBuf, serial->rxLen);
            serial->recvFinishFlag=0;
        }
    }
}
