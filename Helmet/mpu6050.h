#include "mbed.h"

#ifndef MPU6050_H
#define MPU6050_H

enum {
    MAX_ACC_2G,
    MAX_ACC_4G,
    MAX_ACC_8G,
    MAX_ACC_16G,
};

enum {
    MAX_GYRO_250degpersec,
    MAX_GYRO_500degpersec,
    MAX_GYRO_1000degpersec,
    MAX_GYRO_2000degpersec,
};

class MPU6050
{
public:
    MPU6050(PinName i2c_sda, PinName i2c_scl);
    ~MPU6050();
    void readAccelemeter(double &acc_x, double &acc_y, double &acc_z);
    void readGyroscope(double &gyro_x, double &gyro_y, double &gyro_z);
    void setMaxScale(int max_acc, int max_gyro);
    void getMaxScale(int &acc, int &gyro);
    void setSleep(bool sleep);
private:
    I2C i2c;
    int address;
    double rate_accelemeter;
    double rate_gyroscope;
    char readByte(char reg);
    void writeByte(char reg, char data);
};

#endif // MPU6050_H
