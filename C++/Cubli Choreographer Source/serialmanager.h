#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include <QObject>

#include "mainwindow.h"
#include "settingsdialog.h"
#include "com_protocol.h"

class MainWindow;
class SettingsDialog;

class SerialManager
{
public:
    SerialManager();
    SerialManager(MainWindow *parentWindow);
    ~SerialManager();
    int openSerialPort();
    void write(QByteArray writeData);
    void showSettingsDialog();
    QByteArray read();

    // SHOULD BE PRIVATE
    QSerialPort *serial;

private:
    MainWindow *parent;
    SettingsDialog *settings;


};

#endif // SERIALMANAGER_H
