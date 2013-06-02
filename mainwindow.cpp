#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SaleaeDeviceApi.h"
#include <QDebug>
#include <QTime>

void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data );
void __stdcall OnDisconnect( U64 device_id, void* user_data );
void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data );
void __stdcall OnError( U64 device_id, void* user_data );


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox_SampleRates->setModel(&m_SampleRatesModel);

    m_SampleTimeVector = new QVector<quint32>(1024*1024);
    m_SampleDataVector = new QVector<quint8>(1024*1024);
    m_SampleCount = 0;

    DevicesManagerInterface::RegisterOnConnect( &OnConnect, this );
    DevicesManagerInterface::RegisterOnDisconnect( &OnDisconnect, this );
    DevicesManagerInterface::BeginConnect();

}

MainWindow::~MainWindow()
{
    if(m_DeviceInterface != NULL)
    {
        m_DeviceInterface->Stop();
    }

    delete ui;
}

void MainWindow::onDeviceConnected(U64 device_id, GenericInterface* device_interface)
{
    qDebug() << "Device: " << device_id << " connected!";

    m_DeviceInterface = (LogicInterface*)device_interface;
    m_DeviceInterface->RegisterOnReadData(OnReadData, this);
    m_DeviceInterface->RegisterOnWriteData(OnWriteData, this);
    m_DeviceInterface->RegisterOnError(OnError, this);

    quint32 SampleRates[20];
    qint32 SampleRatesCount;
    SampleRatesCount = m_DeviceInterface->GetSupportedSampleRates(SampleRates, 20);

    m_SampleRates.clear();

    for(int i = SampleRatesCount-1; i > 0; i --)
    {
        m_SampleRates.append(QString::number(SampleRates[i]));
    }

    m_SampleRatesModel.setStringList(m_SampleRates);
    ui->comboBox_SampleRates->setCurrentIndex(0);
}

void MainWindow::onDeviceDisconnected(U64 device_id)
{
    qDebug() << "Device: " << device_id << " disconnected!";
    m_DeviceInterface = NULL;
}

void MainWindow::onInterfaceReadData( U8 *data, U32 data_length)
{
//    QByteArray dataArray((const char*)data, data_length);
//    qDebug() << QTime::currentTime().toString("HH:mm:ss:zzz") << data_length << *data;
////    qDebug() << dataArray.toHex();
///
    for(quint32 i = 0;
        i < data_length;
        i++, m_SampleTime++, data++, m_PrevData = *data)
    {
        if( *data != m_PrevData )
        {
            qDebug() << m_SampleTime << ":" << QString::number(*data, 16);

            if( m_SampleCount < m_SampleTimeVector->size())
            {
                *(m_SampleTimeVector->data() + m_SampleCount) = m_SampleTime;
                *(m_SampleDataVector->data() + m_SampleCount) = *data;
                m_SampleCount ++;
            }
            else
            {
                qDebug() << "Vector is overflowed !";
            }
        }
    }

//    DevicesManagerInterface::DeleteU8ArrayPtr( data );
}


void MainWindow::onInterfaceWriteData( U8 *data, U32 data_length)
{
    qDebug() << "WriteData: " << QTime::currentTime();
}

void MainWindow::onInterfaceError( )
{
    qDebug() << "ERROR: " << QTime::currentTime();
}


void MainWindow::on_pushButton_StartRead_clicked()
{
    if( m_DeviceInterface != NULL )
    {
        quint32 sampleRate = ui->comboBox_SampleRates->currentText().toUInt();
        m_DeviceInterface->SetSampleRateHz(sampleRate);
        m_SampleCount = 0;
        m_SampleTime = 0;
        m_PrevData = 0;
        m_DeviceInterface->ReadStart();
    }
}

void MainWindow::on_pushButton_StartWrite_clicked()
{
    if( m_DeviceInterface != NULL )
    {
        m_DeviceInterface->WriteStart();
    }
}

void MainWindow::on_pushButton_Stop_clicked()
{
    if( m_DeviceInterface != NULL )
    {
        m_DeviceInterface->Stop();
    }
}




void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data )
{
    MainWindow *window = (MainWindow *)user_data;
    window->onDeviceConnected(device_id, device_interface);
}


void __stdcall OnDisconnect( U64 device_id, void* user_data )
{
    MainWindow *window = (MainWindow *)user_data;
    window->onDeviceDisconnected(device_id);
}


void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
    MainWindow *window = (MainWindow *)user_data;
    window->onInterfaceReadData(data, data_length);
}


void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data )
{
    MainWindow *window = (MainWindow *)user_data;
    window->onInterfaceWriteData(data, data_length);
}


void __stdcall OnError( U64 device_id, void* user_data )
{
    MainWindow *window = (MainWindow *)user_data;
    window->onInterfaceError();
}


