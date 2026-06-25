#include "i2c_dev.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <string.h>

int i2c_dev_open(I2CDevice *dev, const char *path, uint8_t addr) {
    if(dev == NULL || path == NULL) {
        return -1;
    }

    dev->fd = open(path, O_RDWR);
    if (dev->fd < 0) {
        printf("Failed to open i2c device %s\n", path);
        return -1;
    }

    if(ioctl(dev->fd, I2C_SLAVE, addr) < 0) {
        printf("Failed to set slave address\n");
        close(dev->fd);
        dev->fd = -1;
        return -1;
    }

    dev->addr = addr;
    return 0;
}

void i2c_dev_close(I2CDevice *dev) {
    if(dev != NULL && dev->fd >= 0) {
        close(dev->fd);
        dev->fd = -1;
    }
}

int i2c_write_byte(I2CDevice *dev, uint8_t reg, uint8_t value) {
    uint8_t buffer[2];

    if(dev == NULL || dev->fd < 0) {
        return -1;
    }

    buffer[0] = reg;
    buffer[1] = value;

    return write(dev->fd, buffer, 2);
}

int i2c_read_byte(I2CDevice *dev, uint8_t reg, uint8_t *value){
    if(dev == NULL || dev->fd < 0 || value == NULL) {
        return -1;
    }

    if(write(dev->fd, &reg, 1) != 1) {
        return -1;
    }
    
    return read(dev->fd, value, 1) == 1 ? 0 : -1;
}

int i2c_read_word(I2CDevice *dev, uint8_t reg, uint16_t *value) {
    uint8_t buffer[2];

    if(dev == NULL || dev->fd < 0 || value == NULL) {
        return -1;
    }

    if(write(dev->fd, &reg, 1) != 1) {
        return -1;
    }

    if(read(dev->fd, buffer, 2) != 2) {
        return -1;
    }

    *value = ((uint16_t)buffer[1] << 8) | buffer[0];

    return 0;
}



