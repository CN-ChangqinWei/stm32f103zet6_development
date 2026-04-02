#include "ring_buf.h"
#include <string.h>



RingBuf NewRingBuf(int size)
{
    RingBuf ring={0};
    if(MemoryPollIsInit()==0) MemoryPollInit();
    MemoryPollAllocPlus(size, &ring.unit);
    if (ring.unit == NULL || ring.unit->start == NULL) {
        ring.unit = NULL;
        ring.head = 0;
        ring.tail = 0;
        ring.len = 0;
        return ring;
    }
    ring.head = 0;
    ring.tail = 0;
    ring.len = 0;
    return ring;
}

uint8_t RingBufAddByte(RingBuf* ring, char byte)
{
    if (ring == NULL || ring->unit == NULL || ring->unit->start == NULL) {
        return 1;
    }
    int size = ring->unit->len;
    ring->unit->start[ring->tail] = byte;
    ring->tail = (ring->tail + 1) % size;
    if (ring->len < size) {
        ring->len++;
    } else {
        ring->head = (ring->head + 1) % size;
    }
    return 0;
}

uint8_t RingBufAddData(RingBuf* ring, char* data, int len)
{
    if (ring == NULL || ring->unit == NULL || ring->unit->start == NULL || data == NULL || len <= 0) {
        return 0;
    }
    int size = ring->unit->len;
    for (int i = 0; i < len; i++) {
        ring->unit->start[ring->tail] = data[i];
        ring->tail = (ring->tail + 1) % size;
        if (ring->len < size) {
            ring->len++;
        } else {
            ring->head = (ring->head + 1) % size;
        }
    }
    return 1;
}

char RingBufPop(RingBuf* ring)
{
    if (ring == NULL || ring->unit == NULL || ring->unit->start == NULL || ring->len == 0) {
        return 0;
    }
    char byte = ring->unit->start[ring->head];
    ring->head = (ring->head + 1) % ring->unit->len;
    ring->len--;
    return byte;
}

uint32_t RingBufRead(RingBuf* ring, char* buf, int len)
{
    if (ring == NULL || ring->unit == NULL || ring->unit->start == NULL || buf == NULL || len <= 0) {
        return 0;
    }
    int readLen = (len > ring->len) ? ring->len : len;
    int size = ring->unit->len;
    for (int i = 0; i < readLen; i++) {
        buf[i] = ring->unit->start[(ring->head + i) % size];
    }
    ring->head = (ring->head + readLen) % size;
    ring->len -= readLen;
    return readLen;
}
