#ifndef ISPI_H
#define ISPI_H

#include <cstdint>
#include <cstdlib>

class ISpi {
    public:
    virtual bool transmit(const uint8_t *data, size_t len) = 0;
    virtual bool receive(uint8_t *data, size_t len) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
};

#endif