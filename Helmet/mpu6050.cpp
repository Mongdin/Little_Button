#include "mpu6050.h"

static const double RATE_ACC_LIST[4] = {
    16384.0,
    8192.0,
    4096.0,
    2048.0,
};

static const double RATE_GYRO_LIST[4] = {
    131.0,
    65.5,
    32.8,
    16.4,
};

MPU6050::MPU6050(PinName i2c_sda, PinName i2c_scl) : i2c(i2c_sda, i2c_scl), address(0x68 << 1), rate_accelemeter(RATE_ACC_LIST[0]), rate_gyroscope(RATE_GYRO_LIST[0])
{
    setSleep(false);
    setMaxScale(MAX_ACC_2G, MAX_GYRO_250degpersec);
}

MPU6050::~MPU6050()
{
}

char MPU6050::readByte(char reg)
{
    int result_write = i2c.write(address, &reg, 1);
    if(result_write) {
        // failed
    }
    char read_data;
    int result_read = i2c.read(address, &read_data, 1);
    if(result_read) {
        // failed
    }
    return read_data;
}

void MPU6050::writeByte(char reg, char data)
{
    char buf[2];
    buf[0] = reg;
    buf[1] = data;
    int result = i2c.write(address, buf, 2);
    if(result) {
        // failed
    }
}

void MPU6050::readAccelemeter(double &acc_x, double &acc_y, double &acc_z)
{
    char h, l;
    short x, y, z;
    // read X
    h = readByte(0x3B);
    l = readByte(0x3C);
    x = h << 8 | l;
    // read Y
    h = readByte(0x3D);
    l = readByte(0x3E);
    y = h << 8 | l;
    // read Z
    h = readByte(0x3F);
    l = readByte(0x40);
    z = h << 8 | l;
    acc_x = (double)x / rate_accelemeter;
    acc_y = (double)y / rate_accelemeter;
    acc_z = (double)z / rate_accelemeter;
}

void MPU6050::readGyroscope(double &gyro_x, double &gyro_y, double &gyro_z)
{
    char h, l;
    short x, y, z;
    // read X
    h = readByte(0x43);
    l = readByte(0x44);
    x = h << 8 | l;
    // read Y
    h = readByte(0x45);
    l = readByte(0x46);
    y = h << 8 | l;
    // read Z
    h = readByte(0x47);
    l = readByte(0x48);
    z = h << 8 | l;
    gyro_x = (double)x / rate_gyroscope;
    gyro_y = (double)y / rate_gyroscope;
    gyro_z = (double)z / rate_gyroscope;
}

void MPU6050::getMaxScale(int &acc, int &gyro)
{
    char reg;
    reg = readByte(0x1C);
    acc = reg;
    reg = readByte(0x1B);
    gyro = reg;
}

void MPU6050::setMaxScale(int max_acc, int max_gyro)
{
    switch(max_acc) {
    case MAX_ACC_2G:
        writeByte(0x1C, 0x00);
        break;
    case MAX_ACC_4G:
        writeByte(0x1C, 0x08);
        break;
    case MAX_ACC_8G:
        writeByte(0x1C, 0x10);
        break;
    case MAX_ACC_16G:
        writeByte(0x1C, 0x18);
        break;
    }
    rate_accelemeter = RATE_ACC_LIST[max_acc];
    switch(max_gyro) {
    case MAX_GYRO_250degpersec:
        writeByte(0x1B, 0x00);
        break;
    case MAX_GYRO_500degpersec:
        writeByte(0x1B, 0x08);
        break;
    case MAX_GYRO_1000degpersec:
        writeByte(0x1B, 0x10);
        break;
    case MAX_GYRO_2000degpersec:
        writeByte(0x1B, 0x18);
        break;
    }
    rate_gyroscope = RATE_GYRO_LIST[max_gyro];
}

void MPU6050::setSleep(bool sleep)
{
    const char reg_sleep = 0x6B;
    const char current_state = readByte(reg_sleep);
    char write_data;
    if(sleep) {
        write_data = current_state | 0x40;
    } else {
        write_data = current_state & (~0x40);
    }
    writeByte(reg_sleep, write_data);
}
