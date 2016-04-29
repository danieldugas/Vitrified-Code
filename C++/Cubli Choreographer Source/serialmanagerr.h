#ifndef SERIALMANAGERR_H
#define SERIALMANAGERR_H

#include <QtSerialPort/QSerialPort>


class SerialManagerr
{
public:
    SerialManagerr();
    ~SerialManagerr();

private:
    QSerialPort serialPort;
};

#endif // SERIALMANAGERR_H
