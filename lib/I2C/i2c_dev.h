#pragma once

#include <stdint.h>

typedef struct {
    int fd;
    uint8_t addr;
} I2CDevice;

int i2c_dev_open(I2CDevice *dev, const char *path, uint8_t addr);
void i2c_dev_close(I2CDevice *dev);

int i2c_write_byte(I2CDevice *dev, uint8_t reg, uint8_t value);
int i2c_read_byte(I2CDevice *dev, uint8_t reg, uint8_t *value);
int i2c_read_word(I2CDevice *dev, uint8_t reg, uint16_t *value);



