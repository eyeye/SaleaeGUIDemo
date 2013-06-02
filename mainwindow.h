#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SaleaeDeviceApi.h"
#include <QStringListModel>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void onDeviceConnected(U64 device_id, GenericInterface* device_interface);
    void onDeviceDisconnected(U64 device_id);

    void onInterfaceReadData( U8* data, U32 data_length);
    void onInterfaceWriteData( U8* data, U32 data_length);
    void onInterfaceError( );

private slots:
    void on_pushButton_StartRead_clicked();

    void on_pushButton_StartWrite_clicked();

    void on_pushButton_Stop_clicked();

private:
    Ui::MainWindow *ui;
    LogicInterface* m_DeviceInterface;
    QStringList m_SampleRates;
    QStringListModel m_SampleRatesModel;
    quint32 m_SampleTime;
    quint8 m_PrevData;

    QVector<quint32>* m_SampleTimeVector;
    QVector<quint8>* m_SampleDataVector;
    qint32 m_SampleCount;

};

#endif // MAINWINDOW_H
