#include "dtp_common.h"

uint16_t compute_crc16(const uint8_t* data, size_t len) 
{
    uint16_t crc = 0xFFFF;

    const uint16_t poly = 0x1021;

    for (size_t i = 0; i < len; ++i) 
    {
        crc ^= (uint16_t)data[i] << 8;

        for (int bit = 0; bit < 8; ++bit) 
        {
            crc = (crc & 0x8000) ? (crc << 1) ^ poly : crc << 1;
        }
    }

    return crc;
}

size_t buildFrame(uint8_t* buf, uint16_t origin, uint16_t dest,
    uint8_t code, uint8_t flags,
    const uint8_t* payload, uint16_t payloadLen) 
{
    size_t off = 0;

    uint16_t header = htons(HEADER_MAGIC);

    memcpy(buf + off, &header, sizeof(header)); off += 2;

    buf[off++] = 0x01;

    uint16_t origN = htons(origin);
    uint16_t destN = htons(dest);

    memcpy(buf + off, &origN, sizeof(origN)); off += 2;
    memcpy(buf + off, &destN, sizeof(destN)); off += 2;

    buf[off++] = code;
    buf[off++] = flags;

    uint16_t lenN = htons(payloadLen);

    memcpy(buf + off, &lenN, sizeof(lenN)); off += 2;

    if (payload && payloadLen > 0) 
    {
        memcpy(buf + off, payload, payloadLen);

        off += payloadLen;
    }
    uint16_t crc = compute_crc16(buf + 2, off - 2);
    uint16_t crcN = htons(crc);

    memcpy(buf + off, &crcN, sizeof(crcN)); off += 2;

    uint16_t footer = htons(FOOTER_MAGIC);

    memcpy(buf + off, &footer, sizeof(footer)); off += 2;

    return off;
}
