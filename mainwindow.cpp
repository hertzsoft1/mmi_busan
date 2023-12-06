#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <dirent.h>
#include <stdio.h>
#include <time.h>

#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QKeyEvent>
#include <QtDebug>
#include <QMediaPlayer>

#include "dialogmode.h"
#include "dialogconfig.h"
#include "dialogpassword.h"


//- 팝업 시 버튼 눌림 금지
//- 설정은 속도 0에서만..
// 과속상태에서 모드변경시 사운드 처리
// HCR에 따라 화면 전환


//#############################################################################
//
// MainWindow constructor
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_nCurrentMode = SCREEN_WAIT;
    m_nBootCount = 0;

    ui->frame_Wait->setGeometry(0, 0, 1024, 768);
    //ui->Label_Disconnect->setVisible(false);

    ui->frame_TCR->setGeometry(0, 0, 1024, 768);
    ui->frame_TCR->setVisible(false);

    ui->frame_Main->setGeometry(0, 0, 1024, 768);
    ui->frame_Main->setVisible(false);

    // 속도계 초기화
    //
    ui->speed_gauge->setCurrentSpeed(0);
    ui->speed_gauge->setLimitSpeed(0);

    // 이미지 변수(Pixmap) 초기화
    //
    m_pix_Left.load(":/image/image/Left.png");
    m_pix_LeftOpen.load(":/image/image/LeftOpen.png");
    m_pix_Right.load(":/image/image/Right.png");
    m_pix_RightOpen.load(":/image/image/RightOpen.png");

    m_pix_ADC.load(":/image/image/ADC.png");
    m_pix_ADCOn.load(":/image/image/ADC_ON.png");
    m_pix_DOLP.load(":/image/image/DOLP.png");
    m_pix_DOLPOn.load(":/image/image/DOLP_ON.png");
    m_pix_DORP.load(":/image/image/DORP.png");
    m_pix_DORPOn.load(":/image/image/DORP_ON.png");

    m_pix_Mode.load(":/image/image/Button1.png");
    m_pix_Mode2.load(":/image/image/Button3.png");
    m_pix_Config.load(":/image/image/Button2.png");

    m_pix_Msg_Enable.load(":/image/image/Button4.png");
    m_pix_Msg_Disable.load(":/image/image/Button5.png");

    m_pix_Logo.load(":/image/image/Busan_Logo.png");

    // 화면 컨트롤에 이미지 설정
    //
    int w = ui->Button_Busan_Logo->width();
    int h = ui->Button_Busan_Logo->height();
    ui->Button_Busan_Logo->setPixmap(m_pix_Logo.scaled(w, h));

    w = ui->Picture_Door_Left->width();
    h = ui->Picture_Door_Left->height();
    ui->Picture_Door_Left->setPixmap(m_pix_Left.scaled(w, h));

    w = ui->Picture_Door_Right->width();
    h = ui->Picture_Door_Right->height();
    ui->Picture_Door_Right->setPixmap(m_pix_Right.scaled(w, h));

    w = ui->Picture_DOLP_L->width();
    h = ui->Picture_DOLP_L->height();
    ui->Picture_DOLP_L->setPixmap(m_pix_DOLP.scaled(w, h));

    w = ui->Picture_DOLP_R->width();
    h = ui->Picture_DOLP_R->height();
    ui->Picture_DOLP_R->setPixmap(m_pix_DORP.scaled(w, h));

    w = ui->Picture_ADC->width();
    h = ui->Picture_ADC->height();
    ui->Picture_ADC->setPixmap(m_pix_ADC.scaled(w, h));

    w = ui->Button_Mode->width();
    h = ui->Button_Mode->height();
    ui->Button_Mode->setPixmap(m_pix_Mode.scaled(w, h));

    w = ui->Button_Config->width();
    h = ui->Button_Config->height();
    ui->Button_Config->setPixmap(m_pix_Config.scaled(w, h));

    w = ui->Button_WC_OK->width();
    h = ui->Button_WC_OK->height();
    ui->Button_WC_OK->setPixmap(m_pix_Config.scaled(w, h));

    w = ui->Button_WC_Update->width();
    h = ui->Button_WC_Update->height();
    ui->Button_WC_Update->setPixmap(m_pix_Config.scaled(w, h));

    w = ui->Button_Msg_Up->width();
    h = ui->Button_Msg_Up->height();
    ui->Button_Msg_Up->setPixmap(m_pix_Config.scaled(w, h));
    ui->Button_Msg_Up->setPixmapDisable(m_pix_Msg_Disable.scaled(w, h));
    ui->Button_Msg_Up->setEnabled(false);
    m_bMsgUpBtnEnabled = false;

    w = ui->Button_Msg_Down->width();
    h = ui->Button_Msg_Down->height();
    ui->Button_Msg_Down->setPixmap(m_pix_Config.scaled(w, h));
    ui->Button_Msg_Down->setPixmapDisable(m_pix_Msg_Disable.scaled(w, h));
    ui->Button_Msg_Down->setEnabled(false);
    m_bMsgDownBtnEnabled = false;

    // 좌측 하단 메시지 표시 컨트롤 Array에 저장
    //
    m_LabelMsg[0] = ui->Label_Msg1;
    m_LabelMsg[1] = ui->Label_Msg2;
    m_LabelMsg[2] = ui->Label_Msg3;
    m_LabelMsg[3] = ui->Label_Msg4;
    m_LabelMsg[4] = ui->Label_Msg5;

    // 타이머 초기화
    //
    m_pTimerDisp = new QTimer(this);
    connect(m_pTimerDisp, SIGNAL(timeout()), this, SLOT(DisplayTimerCallback()));
    m_pTimerDisp->start(500);

    m_pTimerComm = new QTimer(this);
    connect(m_pTimerComm, SIGNAL(timeout()), this, SLOT(CommTimerCallback()));
    m_pTimerComm->start(100);

    // 1계 통신용 시리얼 초기화
    //
    m_serial1 = new QSerialPort();
    m_serial1->setPortName("/dev/ttymxc0");
    m_serial1->setBaudRate(QSerialPort::Baud19200);
    m_serial1->setDataBits(QSerialPort::Data8);
    m_serial1->setParity(QSerialPort::NoParity);
    m_serial1->setStopBits(QSerialPort::OneStop);
    m_serial1->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serial1, SIGNAL(readyRead()), this, SLOT(ReceiveSerialCallback()));
    if(m_serial1->open(QIODevice::ReadWrite) == true )
    {
        m_bConnect1 = true;
        qDebug() << "serial1 port open success!\n";
    }
    else
    {
        qDebug() << "serial1 port open failed!\n";
    }

    // 2계 통신용 시리얼 초기화
    //
    m_serial2 = new QSerialPort();
    m_serial2->setPortName("/dev/ttymxc2");
    m_serial2->setBaudRate(QSerialPort::Baud19200);
    m_serial2->setDataBits(QSerialPort::Data8);
    m_serial2->setParity(QSerialPort::NoParity);
    m_serial2->setStopBits(QSerialPort::OneStop);
    m_serial2->setFlowControl(QSerialPort::HardwareControl);

    connect(m_serial2, SIGNAL(readyRead()), this, SLOT(ReceiveSerialCallback2()));
    if(m_serial2->open(QIODevice::ReadWrite) == true )
    {
        m_bConnect2 = true;
        qDebug() << "serial2 port open success!\n";
    }
    else
    {
        qDebug() << "serial2 port open failed!\n";
    }

    // 오류메시지 정보와 비밀번호 정보를 파일에서 로드
    //
    Load_Message();
    LoadPassword();
    LoadGroupNo();

    // 차륜경 조정 버튼 초기화
    //
    ShowTachoButton(false);
    ui->Button_WC_OK->setVisible(false);

    // 테스트용 버튼 초기화
    //
    ui->pushButton->setVisible(false);
    ui->pushButton_2->setVisible(false);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_exit->setVisible(false);

    // 송수신용 패킷 구조체 초기화
    //
    memset(&m_Packet1, 0, sizeof(Packet));
    m_Packet1.source = 0x10;
    m_Packet1.active = false;
    m_Packet1.atc_speed = 0;
    m_Packet1.train_speed = 0;

    memset(&m_Packet2, 0, sizeof(Packet));
    m_Packet2.source = 0x20;
    m_Packet2.active = false;
    m_Packet2.atc_speed = 0;
    m_Packet2.train_speed = 0;

    memset(&m_ActivePacket, 0, sizeof(Packet));
    memset(&m_PrevActivePacket, 0, sizeof(Packet));

    memset(m_SendPacketBuffer1, 0, PACKET_SIZE + 6);
    memset(m_SendPacketBuffer2, 0, PACKET_SIZE + 6);

    memset(&m_SendPacket1, 0, sizeof(SendPacket));
    memset(&m_SendPacket2, 0, sizeof(SendPacket));

    memset(m_CurrRecvBuffer1, 0, 100);
    memset(m_CurrRecvBuffer2, 0, 100);

    m_SendPacket1.RM_ON = false;
    m_SendPacket1.RM_OFF = true;
    m_SendPacket2.RM_ON = false;
    m_SendPacket2.RM_OFF = true;

    m_PrevActivePacket.DIA1 = -1;
    m_PrevActivePacket.DIA2 = -1;
    m_PrevActivePacket.EB_M = true;
    m_PrevActivePacket.EB_P = true;

    ui->speed_gauge->setCurrentSpeed(m_Packet1.train_speed);
    ui->speed_gauge->setLimitSpeed(m_Packet1.atc_speed);
    ui->speed_gauge->setOn02(true);
    ui->speed_gauge->setYardMode(true);

    // MMI에 설정된 차륜경값 & 차륜경 설정 화면 초기화
    //
    m_SendPacket1.DIA1 = m_nTachoDIA1Index;
    m_SendPacket2.DIA2 = m_nTachoDIA1Index;
    m_SendPacket1.DIA2 = m_nTachoDIA2Index;
    m_SendPacket2.DIA1 = m_nTachoDIA2Index;

    LoadTachoDia();
    LoadBrightnessLevel();

    ui->frame_Wheel_Confirm->setVisible(false);

    // 마지막 통신 시간 초기화
    //
    m_LastRecvTime1 = QDateTime::currentDateTime();
    m_LastRecvTime2 = QDateTime::currentDateTime();

    // 패널 전면 'ACT' 램프 초기화
    //
    system("echo 0 > /sys/class/gpio/gpio13/value");

    // 로깅 폴더 생성
    //
    sprintf(m_szLoggingPath, "%s/log", QApplication::applicationDirPath().toStdString().c_str());
    if( access( m_szLoggingPath, F_OK) == -1)
    {
        if( mkdir(m_szLoggingPath, 0777) != 0 )
        {
            m_nRxBufferCount = 0;
        }
    }
    qDebug() << m_szLoggingPath;
    //DeleteOldLogFile();

    memset(m_szVersion, 0, 50);
    memset(m_szDate, 0, 50);
    memset(m_szTime, 0, 50);


}


//#############################################################################
//
//
//
MainWindow::~MainWindow()
{
    if( m_bConnect1 == true )
        m_serial1->close();
    if( m_bConnect2 == true )
        m_serial2->close();

    delete ui;
    delete m_pTimerDisp;
    delete m_pTimerComm;
    delete m_serial1;
    delete m_serial2;
}


//#############################################################################
//
// key press event callback
//
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_A:
        ui->frame_Wait->setVisible(false);
        ui->frame_TCR->setVisible(false);
        ui->frame_Main->setVisible(true);
        break;
    case Qt::Key_B:
        ui->frame_Wait->setVisible(true);
        ui->frame_TCR->setVisible(false);
        ui->frame_Main->setVisible(false);
        break;
    case Qt::Key_C:
        if( m_nMessageIndex > 0 )
        {
            --m_nMessageIndex;
            DrawMessage();
        }
        break;
    case Qt::Key_D:
        if( m_MessageList.size() > m_nMessageIndex + 5 )
        {
            ++m_nMessageIndex;
            DrawMessage();
        }
        break;
    }
}


//#############################################################################
//
// touch event callback
//
bool MainWindow::event(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        const QTouchEvent* touch = static_cast<QTouchEvent*>(event);
        const QList<QTouchEvent::TouchPoint> touchPoints = touch->touchPoints();
        for(const QTouchEvent::TouchPoint &touchPoint : touchPoints)
        {
            //qDebug() << "Touch event: " << touchPoint.state();
            if( touchPoint.state() == Qt::TouchPointPressed)
            {
                //qDebug() << "Send fake event: " << touchPoint.pos();
                QMouseEvent fake(QEvent::MouseButtonPress, touchPoint.pos(), touchPoint.screenPos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QApplication::sendEvent(this, static_cast<QEvent*>(&fake));
            }
        }
        break;
    }
    default:
        return QWidget::event(event);
    }

    return true;
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::LoadPassword()
{
    memset(m_szPassword, 0, 100);

    qDebug() << "LoadPassword >>>";
    QFile file(QApplication::applicationDirPath() + "/PASSWORD");
    if( file.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        qint64 nRead = file.read(m_szPassword, 50);
        if( nRead > 0 )
            m_szPassword[4] = 0x00;
        file.close();
    }
    else
    {
        m_szPassword[0] = '1';
        m_szPassword[1] = '2';
        m_szPassword[2] = '3';
        m_szPassword[3] = '4';
        m_szPassword[4] = 0x00;
    }
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::LoadGroupNo()
{
    memset(m_szGroupNo, 0, 20);

    qDebug() << "Load Group No >>>";
    QFile file(QApplication::applicationDirPath() + "/GROUPNO");
    if( file.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        qint64 nRead = file.read(m_szGroupNo, 20);
        if( nRead > 0 )
        {
            m_szGroupNo[2] = 0x00;
            m_nGroupNo = atoi(m_szGroupNo);
        }
        file.close();
    }
    else
    {
        m_szGroupNo[0] = '0';
        m_szGroupNo[1] = '0';
        m_szGroupNo[2] = 0x00;
    }
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::LoadVolumeLevel()
{
    char szCmd[100];

    memset(m_szVolumeValue, 0, 20);

    qDebug() << "Load Volume Level >>>";
    QFile file(QApplication::applicationDirPath() + "/VOLUME");
    if( file.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        qint64 nRead = file.read(m_szVolumeValue, 10);
        if( nRead > 0 )
        {
            m_szVolumeValue[2] = 0x00;
            m_nVolume = atoi(m_szVolumeValue);
            if( m_nVolume < 1 || m_nVolume > 10 )
            {
                m_nVolume = 5;
                m_szVolumeValue[0] = '5';
                m_szVolumeValue[1] = 0x00;

                sprintf(szCmd, "pacmd set-sink-volume 0 %d", m_nVolume * 10 * 650);
                system(szCmd);

                sprintf(szCmd, "echo %d > %s/VOLUME", m_nVolume, qPrintable(QApplication::applicationDirPath()));
                system(szCmd);
                system("sync && sync");
            }
        }
        file.close();
    }
    else
    {
        m_nVolume = 5;
        m_szVolumeValue[0] = '5';
        m_szVolumeValue[1] = 0x00;

        sprintf(szCmd, "pacmd set-sink-volume 0 %d", m_nVolume * 10 * 650);
        system(szCmd);

        sprintf(szCmd, "echo %d > %s/VOLUME", m_nVolume, qPrintable(QApplication::applicationDirPath()));
        system(szCmd);
        system("sync && sync");
    }
}



///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::LoadBrightnessLevel()
{
    char szCmd[100];

    memset(m_szBrightnessValue, 0, 20);

    qDebug() << "Load Brightness Level >>>";
    QFile file(QApplication::applicationDirPath() + "/BRIGHTNESS");
    if( file.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        qint64 nRead = file.read(m_szBrightnessValue, 10);
        if( nRead > 0 )
        {
            m_szBrightnessValue[2] = 0x00;
            m_nBrightness = atoi(m_szBrightnessValue);
            if( m_nBrightness < 1 || m_nBrightness > 5 )
            {
                m_nBrightness = 5;
                m_szBrightnessValue[0] = '5';
                m_szBrightnessValue[1] = 0x00;
            }
        }
        else
        {
            m_nBrightness = 5;
            m_szBrightnessValue[0] = '5';
            m_szBrightnessValue[1] = 0x00;
        }
        file.close();
    }
    else
    {
        m_nBrightness = 5;
        m_szBrightnessValue[0] = '5';
        m_szBrightnessValue[1] = 0x00;
    }

    sprintf(szCmd, "sudo echo %d > /sys/devices/platform/backlight/backlight/backlight/brightness", m_nBrightness + 4);
    system(szCmd);

    sprintf(szCmd, "echo %d > %s/BRIGHTNESS", m_nBrightness, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::LoadTachoDia()
{
    char szData[100];

    qDebug() << "LoadTachoDia >>>";
    QFile file1(QApplication::applicationDirPath() + "/DIA1");
    if( file1.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        if( file1.read(szData, 100) > 0 )
        {
            m_nTachoDIA1Value = atoi(szData);
            m_nTachoDIA1Index = 17 - ((m_nTachoDIA1Value - 780) / 5);
            m_SendPacket1.DIA1 = m_nTachoDIA1Index;
            m_SendPacket2.DIA2 = m_nTachoDIA1Index;

            ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
            //qDebug() << "Tacho #1 : " << QString::number(m_nTachoDIA1Value) << " - " << QString::number(m_nTachoDIA1Index);
        }
        file1.close();
    }

    QFile file2(QApplication::applicationDirPath() + "/DIA2");
    if( file2.open(QIODevice::ReadOnly|QIODevice::Text) == true )
    {
        if( file2.read(szData, 100) > 0 )
        {
            m_nTachoDIA2Value = atoi(szData);
            m_nTachoDIA2Index = 17 - ((m_nTachoDIA2Value - 780) / 5);
            m_SendPacket1.DIA2 = m_nTachoDIA2Index;
            m_SendPacket2.DIA1 = m_nTachoDIA2Index;

            ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
            //qDebug() << "Tacho #2 : " << QString::number(m_nTachoDIA2Value) << " - " << QString::number(m_nTachoDIA2Index);
        }
        file1.close();
    }

}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::SetTacho1DIA(int nDIA)
{
    m_nTachoDIA1Value = nDIA;
    m_nTachoDIA1Index = 17 - ((m_nTachoDIA1Value - 780) / 5);
    ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
int MainWindow::GetTacho1DIA()
{
    return m_nTachoDIA1Value;
}


///#########################################################################################
///
/// <summary>
///     차상장치에서 수신한 차륜경#1 값
/// </summary>
///
int MainWindow::GetTacho1DIA_Packet()
{
    return (860 - (m_ActivePacket.DIA1 * 5));
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void MainWindow::SetTacho2DIA(int nDIA)
{
    m_nTachoDIA2Value = nDIA;
    m_nTachoDIA2Index = 17 - ((m_nTachoDIA2Value - 780) / 5);
    ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
}



///#########################################################################################
///
/// <summary>
///
/// </summary>
///
int MainWindow::GetTacho2DIA()
{
    return m_nTachoDIA2Value;
}


///#########################################################################################
///
/// <summary>
///     차상장치에서 수신한 차륜경#2 값
/// </summary>
///
int MainWindow::GetTacho2DIA_Packet()
{
    return (860 - (m_ActivePacket.DIA2 * 5));
}


///#########################################################################################
///
/// SetPDTRequest1
///     - 1계에 수동 PDT 요청
///
///
void MainWindow::SetPDTRequest1()
{
    m_SendPacket1.manual_PDT_request = true;
    m_sendPDTTime1 = QDateTime::currentDateTime();
}

///#########################################################################################
///
/// SetPDTRequest1
///     - 1계에 수동 PDT 요청
///
///
void MainWindow::SetPDTRequest2()
{
    m_SendPacket2.manual_PDT_request = true;
    m_sendPDTTime2 = QDateTime::currentDateTime();
}


//#############################################################################
//
// serial receive callback
//
void MainWindow::ReceiveSerialCallback()
{
    QByteArray read_data = m_serial1->readAll();
    int nSize = read_data.size();
    if( nSize > 0 )
    {
        for(int i = 0; i < nSize; ++i )
        {
            int nNextIndex = (m_nRecvQueue1Rear + 1) % MAX_RECEIVE_BUFFER;
            if( nNextIndex != m_nRecvQueue1Front)
            {
                m_szReceivedData1[m_nRecvQueue1Rear] = read_data[i];
                m_nRecvQueue1Rear = nNextIndex;
            }
        }
    }
    //m_receivedData1.append(read_data);
}


//#############################################################################
//
// serial receive callback
//
void MainWindow::ReceiveSerialCallback2()
{
    QByteArray read_data = m_serial2->readAll();
    int nSize = read_data.size();
    if( nSize > 0 )
    {
        for(int i = 0; i < nSize; ++i )
        {
            int nNextIndex = (m_nRecvQueue2Rear + 1) % MAX_RECEIVE_BUFFER;
            if( nNextIndex != m_nRecvQueue2Front)
            {
                m_szReceivedData2[m_nRecvQueue2Rear] = read_data[i];
                m_nRecvQueue2Rear = nNextIndex;
            }
        }
    }
    //m_receivedData2.append(read_data);
}


//#############################################################################
//
// timer callback
//      - display current date & time
//
void MainWindow::DisplayTimerCallback()
{
    //char szTemp[100];
    int w, h;
    bool bActLamp = false;

    QDateTime currTime = QDateTime::currentDateTime();
    qint64 spanSec1 = m_LastRecvTime1.secsTo(currTime);
    qint64 spanSec2 = m_LastRecvTime2.secsTo(currTime);

    if( m_nRxBufferCount >= 50)
        WriteCommLogFile();

    if( m_bRedrawMessage == true )
    {
        DrawMessage();
        ui->Button_Msg_Up->setEnabled(false);
        m_bMsgUpBtnEnabled = false;
        if( m_MessageList.size() > (m_nMessageIndex + 5) )
        {
            ui->Button_Msg_Down->setEnabled(true);
            m_bMsgDownBtnEnabled = true;
        }
        else
        {
            ui->Button_Msg_Down->setEnabled(false);
            m_bMsgDownBtnEnabled = false;
        }

        if( m_bLogDownloadStatus == false )
        {
            if( m_bMsgLoggingOpen == true )
            {
                m_FileLoggingMsg.setFileName(m_szLoggingFileMsg);
                if( m_FileLoggingMsg.open(QIODevice::WriteOnly | QIODevice::Append) == true )
                {
                    m_FileLoggingMsg.write(m_strMessageToWrite.toLocal8Bit().data());
                    m_FileLoggingMsg.close();
                    m_strMessageToWrite = "";
                }
            }
        }

        m_bRedrawMessage = false;
    }

    // 사운드 초기화
    //
    if( m_nInitSoundStep < 100 )
        ++m_nInitSoundStep;
    if( m_nInitSoundStep < 10 )
    {
        if( m_nInitSoundStep == 1 )
        {
            qDebug() << "Launch pulseaudio deamon";
            system("pulseaudio -D");
        }
        else if( m_nInitSoundStep == 2 )
        {
            qDebug() << "set pulseaudio output port";
            system("pacmd set-sink-port 0 analog-output-speaker");
        }
        else if( m_nInitSoundStep == 3 )
        {
            system("pacmd set-sink-port 0 analog-output-headphones");
        }
        else if( m_nInitSoundStep == 4 )
        {
            qDebug() << "set pulseaudio audio volume";
            system("sudo pacmd set-sink-volume 0 32500");
        }
        else if( m_nInitSoundStep == 5 )
        {
            qDebug() << "load overspeed alarm sound";
            m_Sound_OverSpeed = new QSound(":/Sound/sound/ChimeLong.wav");
            m_Sound_OverSpeed->setLoops(QSound::Infinite);

            qDebug() << "load emergency brake sound";
            m_Sound_EB = new QSound(":/Sound/sound/ChimeShort.wav");
            m_Sound_EB->setLoops(QSound::Infinite);

            qDebug() << "load ATC code sound";
            m_Sound_Info = new QSound(":/Sound/sound/S_Info.wav");

            qDebug() << "load button click sound";
            m_Sound_Click = new QSound(":/Sound/sound/S_Click.wav");

            m_Sound_Volume = new QSound(":/Sound/sound/ChimeLong.wav");
        }
        else if( m_nInitSoundStep == 6 )
        {
            LoadVolumeLevel();
        }
    }

    if( m_SendPacket1.confirm_DIA == true )
    {
        ++m_nSendConfirmCount1;
        if( m_nSendConfirmCount1 >= 3 )
            m_SendPacket1.confirm_DIA = false;
    }
    if( m_SendPacket2.confirm_DIA == true )
    {
        ++m_nSendConfirmCount2;
        if( m_nSendConfirmCount2 >= 3 )
            m_SendPacket2.confirm_DIA = false;
    }

    // ATC 1/2 Active
    //
    if( spanSec1 > TIMEOUT_SEC )
    {
        ui->label_ATC_ATO_1->setStyleSheet("color: #FF0000; border: 2px solid #909090;");
    }
    else
    {
        if( m_Packet1.active == true )
            ui->label_ATC_ATO_1->setStyleSheet("color: #00FF00; border: 2px solid #909090;");
        else
            ui->label_ATC_ATO_1->setStyleSheet("color: #505050; border: 2px solid #909090;");
    }

    if( spanSec2 > TIMEOUT_SEC )
    {
        ui->label_ATC_ATO_2->setStyleSheet("color: #FF0000; border: 2px solid #909090;");
    }
    else
    {
        if( m_Packet2.active == true )
            ui->label_ATC_ATO_2->setStyleSheet("color: #00FF00; border: 2px solid #909090;");
        else
            ui->label_ATC_ATO_2->setStyleSheet("color: #505050; border: 2px solid #909090;");
    }

    ACTIVE_SYSTEM active = ACTIVE_NONE;
    if( m_Packet1.count > 0 )
    {
        bActLamp = true;
        if( m_Packet1.active == true )
        {
            active = ACTIVE_SYSTEM1;
            memcpy(&m_ActivePacket, &m_Packet1, sizeof(Packet));
        }
    }
    if( m_Packet2.count > 0 )
    {
        bActLamp = true;
        if( active == ACTIVE_NONE )
        {
            if( m_Packet2.active == true )
            {
                active = ACTIVE_SYSTEM2;
                memcpy(&m_ActivePacket, &m_Packet2, sizeof(Packet));
            }
        }
    }
    m_Packet1.count = 0;
    m_Packet2.count = 0;

    // ACT 램프 처리
    //
    if( m_bActLamp == false )
    {
        if( bActLamp == true )
        {
            system("echo 1 > /sys/class/gpio/gpio13/value");
            m_bActLamp = true;
        }
    }
    else
    {
        if( bActLamp == false )
        {
            system("echo 0 > /sys/class/gpio/gpio13/value");
            m_bActLamp = false;
        }
    }

    // 연결 대기 상태
    //
    if( m_nCurrentMode == SCREEN_WAIT )
    {
        if( m_nBootCount <= 400)
        {
            ++m_nBootCount;
            ui->ProgressBar_Boot->setValue(static_cast<int>((static_cast<double>(m_nBootCount) / 401.0) * 100));
        }

        //if( spanSec1 > CONNECT_TRY_SEC )
        //    AddMessage(ERROR_COMM_1);
        //if( spanSec2 > CONNECT_TRY_SEC )
        //    AddMessage(ERROR_COMM_2);

        // 로고 5회 이상 클릭 시 메인화면으로 전환
        //
        if( m_nLogoClick >= 5 )
        {
            //m_nCurrentMode = SCREEN_ON_DISCONNECT;
            ui->frame_Wait->setVisible(false);
            ui->frame_TCR->setVisible(false);
            ui->frame_Main->setVisible(true);
        }

        // 일정 시간 동안 통신 데이터가 입력되지 않으면 연결 실패 화면으로 전환
        //
        /*
        if( spanSec1 > CONNECT_TRY_SEC && spanSec2 > CONNECT_TRY_SEC )
        {
            m_nCurrentMode = SCREEN_DISCONNECT;
            ui->frame_Black->setVisible(true);
            ui->Label_Disconnect->setVisible(true);
            //ui->label_Logo->setVisible(false);
            ui->frame_Main->setVisible(false);
        }
        */

        // 통신 패킷을 10개 이상 받으면 연결 화면으로 전환
        //
        if( active != ACTIVE_NONE )
        {
            if( m_nInitSoundStep > 6 )
            {
                if( m_nRecvPacketCount1 > 10 || m_nRecvPacketCount2 > 10 )
                {
                    ui->ProgressBar_Boot->setValue(100);
                    if( m_ActivePacket.HCR == true )
                    {
                        m_nCurrentMode = SCREEN_ON;
                        ui->frame_Wait->setVisible(false);
                        ui->frame_TCR->setVisible(false);
                        ui->frame_Main->setVisible(true);
                    }
                    else
                    {
                        m_nCurrentMode = SCREEN_TCR;
                        ui->frame_Wait->setVisible(false);
                        ui->frame_TCR->setVisible(true);
                        ui->frame_Main->setVisible(false);
                    }
                }
            }
        }
        return;
    }
    // 연결 실패 상태
    //
    else if(m_nCurrentMode == SCREEN_TCR )
    {
        if( active != ACTIVE_NONE )
        {
            if( m_ActivePacket.HCR == true )
            {
                m_bRefreshSound = true;
                m_nCurrentMode = SCREEN_ON;
                ui->frame_Wait->setVisible(false);
                ui->frame_TCR->setVisible(false);
                ui->frame_Main->setVisible(true);
            }
        }
        if( m_bTCRTempShow == true )
        {
            ++m_nTCRTempCount;
            if( m_nTCRTempCount > 20)
            {
                m_bTCRTempShow = false;
                ui->frame_TCR->setVisible(true);
                ui->frame_Main->setVisible(false);
            }
        }
        m_Sound_OverSpeed->stop();
        m_Sound_EB->stop();

        //return;
    }
    // 연결 상태
    //
    else if(m_nCurrentMode == SCREEN_ON )
    {
        // 1계 연결 상태 체크
        //
        if( m_bConnectSystem1 == false )
        {
            if( m_nRecvPacketCount1 > 10 )
                m_bConnectSystem1 = true;
        }
        else
        {
            if( spanSec1 > TIMEOUT_SEC )
            {
                m_bConnectSystem1 = false;
                m_nRecvPacketCount1 = 0;
                if( m_bPrevConnectSystem1 == true )
                    AddMessage(1, ERROR_COMM_1);
            }
        }

        // 2계 연결 상태 체크
        //
        if( m_bConnectSystem2 == false )
        {
            if( m_nRecvPacketCount2 > 10 )
                m_bConnectSystem2 = true;
        }
        else
        {
            if( spanSec2 > TIMEOUT_SEC )
            {
                m_bConnectSystem2 = false;
                m_nRecvPacketCount2 = 0;
                if( m_bPrevConnectSystem2 == true )
                    AddMessage(2, ERROR_COMM_2);
            }
        }

        if( m_ActivePacket.HCR == false )
        {
            m_nCurrentMode = SCREEN_TCR;
            ui->frame_Wait->setVisible(false);
            ui->frame_TCR->setVisible(true);
            ui->frame_Main->setVisible(false);
            return;
        }

        // 1계와 2계가 모두 연결 종료 상태일 경우 처리
        //
        if( spanSec1 > TIMEOUT_SEC && spanSec2 > TIMEOUT_SEC )
        {
            m_nCurrentMode = SCREEN_ON_DISCONNECT;
            m_nRecvPacketCount1 = 0;
            m_nRecvPacketCount2 = 0;

            ui->speed_gauge->setCurrentSpeed(0);
            ui->speed_gauge->setSpeedCode(0);
            ui->speed_gauge->setLimitSpeed(0);
            ui->label_DrivingMode->setText("");
            ui->label_DoorMode->setText("");
            ui->label_P->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_B->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_EB->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_FSB->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_ZV->setStyleSheet("color: #505050; border: 2px solid #909090;");
            w = ui->Picture_Door_Left->width();
            h = ui->Picture_Door_Left->height();
            ui->Picture_Door_Left->setPixmap(m_pix_Left.scaled(w, h));
            w = ui->Picture_Door_Right->width();
            h = ui->Picture_Door_Right->height();
            ui->Picture_Door_Right->setPixmap(m_pix_Right.scaled(w, h));
            w = ui->Picture_ADC->width();
            h = ui->Picture_ADC->height();
            ui->Picture_ADC->setPixmap(m_pix_ADC.scaled(w, h));
            w = ui->Picture_DOLP_L->width();
            h = ui->Picture_DOLP_L->height();
            ui->Picture_DOLP_L->setPixmap(m_pix_DOLP.scaled(w, h));
            w = ui->Picture_DOLP_R->width();
            h = ui->Picture_DOLP_R->height();
            ui->Picture_DOLP_R->setPixmap(m_pix_DORP.scaled(w, h));
            ui->Button_Mode->setText("평상 모드");
            ui->Button_Mode->setStyleSheet("color: rgb(0, 0, 0);");
            w = ui->Button_Mode->width();
            h = ui->Button_Mode->height();
            ui->Button_Mode->setPixmap(m_pix_Mode.scaled(w, h));

            if( m_Sound_OverSpeed != nullptr )
                m_Sound_OverSpeed->stop();
            if( m_Sound_EB != nullptr )
                m_Sound_EB->stop();

            return;
        }
    }
    else if(m_nCurrentMode == SCREEN_ON_DISCONNECT )
    {
        if( active != ACTIVE_NONE )
        {
            if( m_nRecvPacketCount1 > 10 || m_nRecvPacketCount2 > 10 )
            {
                m_nCurrentMode = SCREEN_ON;
                m_bRedraw = true;
            }
        }

        memset(&m_PrevActivePacket, 0, sizeof(Packet));
        return;
    }

    /*
    if(m_ActivePacket.PWM == 99)
    {
        QApplication::quit();
    }
    */

    m_bCanConfig = true;
    // 1계와 2계가 모두 활성화되지 않았다면 화면 미처리
    //
    if( active == ACTIVE_NONE )
    {
        m_bWeelCheckUpdate = false;
        if( m_nRxBufferCount > 0)
            WriteCommLogFile();
        return;
    }

    // 차륜경 확인 팝업 처리
    //
    if( m_bWheelCheckPopup1 == true )
    {
        if( m_Packet1.DIA1 == m_nTachoDIA1Index && m_Packet1.DIA2 == m_nTachoDIA2Index )
        {
            m_bWheelCheckPopup1 = false;
            m_nSendConfirmCount1 = 0;
            m_SendPacket1.confirm_DIA = true;
        }
        /*
        if( m_ActivePacket.train_speed == 0 )
        {
            if( m_bWheelCheckShow == false )
            {
                if( m_bWeelCheckUpdate == true )
                    ui->Button_WC_Update->setVisible(false);
                else
                    ui->Button_WC_Update->setVisible(true);

                ui->label_wc_title->setText("1계 차륜경 확인");
                ui->label_Tacho1_Confirm->setText(QString::number(860 - ((m_Packet1.DIA1 - 1) * 5)));
                ui->label_Tacho2_Confirm->setText(QString::number(860 - ((m_Packet1.DIA2 - 1) * 5)));
                ui->frame_Wheel_Confirm->setVisible(true);

                m_bWheelCheckShow = true;
                m_nWheelCheckShow = 1;

                //m_bWeelCheckUpdate = false;
                ShowTachoButton(false);
            }
        }
        */
    }
    else if( m_bWheelCheckPopup2 == true )
    {
        if( m_Packet2.DIA2 == m_nTachoDIA1Index && m_Packet2.DIA1 == m_nTachoDIA2Index )
        {
            m_bWheelCheckPopup2 = false;
            m_nSendConfirmCount2 = 0;
            m_SendPacket2.confirm_DIA = true;
        }
        /*
        if( m_ActivePacket.train_speed == 0 )
        {
            if( m_bWheelCheckShow == false )
            {
                if( m_bWeelCheckUpdate == true )
                    ui->Button_WC_Update->setVisible(false);
                else
                    ui->Button_WC_Update->setVisible(true);

                ui->label_wc_title->setText("2계 차륜경 확인");
                ui->label_Tacho1_Confirm->setText(QString::number(860 - ((m_Packet2.DIA2 - 1)* 5)));
                ui->label_Tacho2_Confirm->setText(QString::number(860 - ((m_Packet2.DIA1 - 1)* 5)));
                ui->frame_Wheel_Confirm->setVisible(true);

                m_bWheelCheckShow = true;
                m_nWheelCheckShow = 2;

                //m_bWeelCheckUpdate = false;
                ShowTachoButton(false);
            }
        }
        */
    }
    else
    {
        if( m_bWheelCheckShow == true )
        {
            ui->frame_Wheel_Confirm->setVisible(false);
            m_bWheelCheckShow = false;
        }
    }

    // 속도 초과 시 제한속도 표시 깜빡거림. (2023.03.21)
    //
    if(m_ActivePacket.OverSpeed == true )
    {
        if( m_ActivePacket.DM_EM != true )
        {
            bool bFlag = ui->speed_gauge->getOverSpeed();
            bFlag = !bFlag;
            ui->speed_gauge->setOverSpeed(bFlag);
        }
    }
    else
    {
        ui->speed_gauge->setOverSpeed(false);
    }

    // 차량 속도가 0 이상이면 팝업 숨김
    //
    if( m_bWheelCheckShow == true )
    {
        if( m_ActivePacket.train_speed > 0 )
        {
            ui->frame_Wheel_Confirm->setVisible(false);
            m_bWheelCheckShow = false;
        }
    }

    // 시간 표시
    //
    sprintf(m_szDate, "%04d-%02d-%02d", m_ActivePacket.year + 2000, m_ActivePacket.month, m_ActivePacket.day);
    ui->label_Date->setText(m_szDate);
    sprintf(m_szTime, "%02d:%02d:%02d", m_ActivePacket.hour, m_ActivePacket.minute, m_ActivePacket.second);
    ui->label_Time->setText(m_szTime);

    // ATC version
    //
    sprintf(m_szVersion, "version %d.%d", m_ActivePacket.MajorVersion, m_ActivePacket.MinorVersion);

    // 시스템 타임을 수신한 패킷 시간으로 변경
    //
    if( m_bSetTime == false )
    {
        char szCmd[100];

        sprintf(szCmd, "sudo date -s \"%04d%02d%02d %02d:%02d:%02d\"", m_ActivePacket.year + 2000,
                m_ActivePacket.month, m_ActivePacket.day,
                m_ActivePacket.hour, m_ActivePacket.minute, m_ActivePacket.second);
        system(szCmd);
        m_bSetTime = true;
        DeleteOldLogFile();
    }

    // ATC 속도 코드 변경시 알림음 출력
    //      - 01-DE-DW 사이에 변경 시 알림음 미출력(2023.03.21)
    //
    if( m_PrevActivePacket.atc_Speed_Code != m_ActivePacket.atc_Speed_Code )
    {
        if( m_Sound_Info != nullptr )
        {
            if( m_ActivePacket.atc_Speed_Code == 1 ||
                m_ActivePacket.atc_Speed_Code == 7 ||
                m_ActivePacket.atc_Speed_Code == 8 )
            {
                if( ui->speed_gauge->getOn01() == false )
                    m_Sound_Info->play();
            }
            else
            {
                m_Sound_Info->play();
            }
        }
    }

    // 속도 표시
    //
    if( m_bRedraw == true || m_PrevActivePacket.train_speed != m_ActivePacket.train_speed )
        ui->speed_gauge->setCurrentSpeed(m_ActivePacket.train_speed);
    if( m_bRedraw == true || m_PrevActivePacket.atc_Speed_Code != m_ActivePacket.atc_Speed_Code )
        ui->speed_gauge->setSpeedCode(m_ActivePacket.atc_Speed_Code);
    if( m_bRedraw == true || m_PrevActivePacket.atc_speed != m_ActivePacket.atc_speed )
        ui->speed_gauge->setLimitSpeed(m_ActivePacket.atc_speed);


    // Driving Mode
    //
    if( m_bRedraw == true ||
        m_PrevActivePacket.DM_FA != m_ActivePacket.DM_FA ||
        m_PrevActivePacket.DM_AUTO != m_ActivePacket.DM_AUTO ||
        m_PrevActivePacket.DM_MCS != m_ActivePacket.DM_MCS ||
        m_PrevActivePacket.DM_YARD != m_ActivePacket.DM_YARD ||
        m_PrevActivePacket.DM_EM != m_ActivePacket.DM_EM )
    {
        if( m_ActivePacket.DM_FA == true )
            ui->label_DrivingMode->setText("FA");
        else if( m_ActivePacket.DM_AUTO == true )
            ui->label_DrivingMode->setText("AUTO");
        else if( m_ActivePacket.DM_MCS == true )
            ui->label_DrivingMode->setText("MCS");
        else if( m_ActivePacket.DM_YARD == true )
            ui->label_DrivingMode->setText("YARD");
        else if( m_ActivePacket.DM_EM == true )
            ui->label_DrivingMode->setText("FMC");
        else
            ui->label_DrivingMode->setText("AUTO");

        // FMC는 붉은색 나머지는 녹색 (2023.03.21)
        //
        if( m_ActivePacket.DM_EM == true )
            ui->label_DrivingMode->setStyleSheet("color: #FF0000; border: 2px solid #909090;");
        else
            ui->label_DrivingMode->setStyleSheet("color: #00FF00; border: 2px solid #909090;");
    }

    // Door Mode
    //
    if( m_bRedraw == true ||
        m_PrevActivePacket.DM_AA != m_ActivePacket.DM_AA ||
        m_PrevActivePacket.DM_AM != m_ActivePacket.DM_AM ||
        m_PrevActivePacket.DM_MM != m_ActivePacket.DM_MM )
    {
        if( m_ActivePacket.DM_AA == true )
            ui->label_DoorMode->setText("A/A");
        else if( m_ActivePacket.DM_AM == true )
            ui->label_DoorMode->setText("A/M");
        else if( m_ActivePacket.DM_MM == true )
            ui->label_DoorMode->setText("M/M");
        else
            ui->label_DoorMode->setText("A/M");
    }

    // Powering or Braking
    //
    if( m_bRedraw == true ||
        m_PrevActivePacket.TC_DR != m_ActivePacket.TC_DR ||
        m_PrevActivePacket.TC_BR != m_ActivePacket.TC_BR ||
        m_PrevActivePacket.TC_CS != m_ActivePacket.TC_CS ||
        m_PrevActivePacket.mascon_dr != m_ActivePacket.mascon_dr ||
        m_PrevActivePacket.mascon_br != m_ActivePacket.mascon_br ||
        m_PrevActivePacket.mascon_neu != m_ActivePacket.mascon_neu ||
        m_PrevActivePacket.DM_FA != m_ActivePacket.DM_FA ||
        m_PrevActivePacket.DM_AUTO != m_ActivePacket.DM_AUTO ||
        m_PrevActivePacket.DM_MCS != m_ActivePacket.DM_MCS ||
        m_PrevActivePacket.DM_YARD != m_ActivePacket.DM_YARD ||
        m_PrevActivePacket.DM_EM != m_ActivePacket.DM_EM)
    {
        if( m_ActivePacket.DM_MCS == true || m_ActivePacket.DM_YARD == true )
        {
            if(m_ActivePacket.mascon_dr == true )
                ui->label_P->setStyleSheet("color: #FFFFFF; border: 2px solid #909090;");
            else
                ui->label_P->setStyleSheet("color: #505050; border: 2px solid #909090;");

            if(m_ActivePacket.mascon_br == true )
                ui->label_B->setStyleSheet("color: #FFFFFF; border: 2px solid #909090;");
            else
                ui->label_B->setStyleSheet("color: #505050; border: 2px solid #909090;");
        }
        else if( m_ActivePacket.DM_FA == true || m_ActivePacket.DM_AUTO == true )
        {
            if( m_ActivePacket.TC_DR == true && m_ActivePacket.mascon_br == false )
                ui->label_P->setStyleSheet("color: #FFFFFF; border: 2px solid #909090;");
            else
                ui->label_P->setStyleSheet("color: #505050; border: 2px solid #909090;");

            if( m_ActivePacket.TC_BR == true ||
                m_ActivePacket.mascon_br == true ||
                (m_ActivePacket.TC_CS == true && m_ActivePacket.mascon_br == true))
                ui->label_B->setStyleSheet("color: #FFFFFF; border: 2px solid #909090;");
            else
                ui->label_B->setStyleSheet("color: #505050; border: 2px solid #909090;");
        }
        else
        {
            ui->label_P->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_B->setStyleSheet("color: #505050; border: 2px solid #909090;");
        }
    }
    if( m_bRedraw == true || m_PrevActivePacket.TC_CS != m_ActivePacket.TC_CS )
    {
        if( m_ActivePacket.TC_CS == true )
        {
            ui->label_P->setStyleSheet("color: #505050; border: 2px solid #909090;");
            ui->label_B->setStyleSheet("color: #505050; border: 2px solid #909090;");
        }
    }

    // EB
    //
    if( m_bRedraw == true ||
        m_PrevActivePacket.EB_M != m_ActivePacket.EB_M ||
        m_PrevActivePacket.EB_P != m_ActivePacket.EB_P )
    {
        if( m_ActivePacket.EB_M == false || m_ActivePacket.EB_P == false )
            ui->label_EB->setStyleSheet("color: #FF0000; border: 2px solid #909090;");
        else
            ui->label_EB->setStyleSheet("color: #505050; border: 2px solid #909090;");
    }

    // FSB
    //
    if( m_bRedraw == true || m_PrevActivePacket.FSB != m_ActivePacket.FSB )
    {
        if( m_ActivePacket.FSB == true )
            ui->label_FSB->setStyleSheet("color: #FFFF00; border: 2px solid #909090;");
        else
            ui->label_FSB->setStyleSheet("color: #505050; border: 2px solid #909090;");
    }

    // ZV
    //
    if( m_bRedraw == true || m_PrevActivePacket.ZVR != m_ActivePacket.ZVR )
    {
        if( m_ActivePacket.ZVR == true )
            ui->label_ZV->setStyleSheet("color: #00FF00; border: 2px solid #909090;");
        else
            ui->label_ZV->setStyleSheet("color: #505050; border: 2px solid #909090;");
    }

    // 속도 0에서만 설정 기능이 수행 가능하도록 플래그 셋팅
    //
    if( m_ActivePacket.ZVR == false && m_ActivePacket.DM_EM == false && m_nCurrentMode != SCREEN_TCR )
        m_bCanConfig = false;

    // EDL
    //
    if( m_bRedraw == true || m_PrevActivePacket.EDL != m_ActivePacket.EDL )
    {
        w = ui->Picture_Door_Left->width();
        h = ui->Picture_Door_Left->height();
        ui->Picture_Door_Left->setPixmap(m_ActivePacket.EDL == true ? m_pix_LeftOpen.scaled(w, h) : m_pix_Left.scaled(w, h));
    }

    // EDR
    //
    if( m_bRedraw == true || m_PrevActivePacket.EDR != m_ActivePacket.EDR )
    {
        w = ui->Picture_Door_Right->width();
        h = ui->Picture_Door_Right->height();
        ui->Picture_Door_Right->setPixmap(m_ActivePacket.EDR == true ? m_pix_RightOpen.scaled(w, h) : m_pix_Right.scaled(w, h));
    }

    // ADC
    //
    if( m_bRedraw == true || m_PrevActivePacket.train_door_close != m_ActivePacket.train_door_close )
    {
        w = ui->Picture_ADC->width();
        h = ui->Picture_ADC->height();
        ui->Picture_ADC->setPixmap(m_ActivePacket.train_door_close == true ? m_pix_ADCOn.scaled(w, h) : m_pix_ADC.scaled(w, h));
    }

    // DOLP_L
    //
    if( m_bRedraw == true || m_PrevActivePacket.DOLP_L != m_ActivePacket.DOLP_L )
    {
        w = ui->Picture_DOLP_L->width();
        h = ui->Picture_DOLP_L->height();
        ui->Picture_DOLP_L->setPixmap(m_ActivePacket.DOLP_L == true ? m_pix_DOLPOn.scaled(w, h) : m_pix_DOLP.scaled(w, h));
    }

    // DOLP_R
    //
    if( m_bRedraw == true || m_PrevActivePacket.DOLP_R != m_ActivePacket.DOLP_R )
    {
        w = ui->Picture_DOLP_R->width();
        h = ui->Picture_DOLP_R->height();
        ui->Picture_DOLP_R->setPixmap(m_ActivePacket.DOLP_R == true ? m_pix_DORPOn.scaled(w, h) : m_pix_DORP.scaled(w, h));
    }

    // 차륜경 확인 처리
    //
    if( m_bWheelCheckShow == true )
    {
        if( m_nWheelCheckShow == 1 )
        {
            ui->label_Tacho1_Confirm->setText(QString::number(860 - ((m_Packet1.DIA1 - 1) * 5)));
            ui->label_Tacho2_Confirm->setText(QString::number(860 - ((m_Packet1.DIA2 - 1) * 5)));

            if( m_Packet1.DIA1 == m_nTachoDIA1Index && m_Packet1.DIA2 == m_nTachoDIA2Index )
                ui->Button_WC_OK->setVisible(true);
            else
                ui->Button_WC_OK->setVisible(false);
        }
        else if( m_nWheelCheckShow == 2 )
        {
            ui->label_Tacho1_Confirm->setText(QString::number(860 - ((m_Packet2.DIA2 - 1) * 5)));
            ui->label_Tacho2_Confirm->setText(QString::number(860 - ((m_Packet2.DIA1 - 1) * 5)));

            if( m_Packet2.DIA2 == m_nTachoDIA1Index && m_Packet2.DIA1 == m_nTachoDIA2Index )
                ui->Button_WC_OK->setVisible(true);
            else
                ui->Button_WC_OK->setVisible(false);
        }
    }

    // 회복모드/평상모드 표시
    //
    if( m_bRedraw == true ||
        m_PrevActivePacket.RM_normal != m_ActivePacket.RM_normal ||
        m_PrevActivePacket.RM_recovery != m_ActivePacket.RM_recovery )
    {
        if( m_ActivePacket.RM_recovery == true )
        {
            ui->Button_Mode->setText("회복 모드");
            ui->Button_Mode->setStyleSheet("color: rgb(255, 255, 255);");
            w = ui->Button_Mode->width();
            h = ui->Button_Mode->height();
            ui->Button_Mode->setPixmap(m_pix_Mode2.scaled(w, h));
        }
        if( m_ActivePacket.RM_normal == true )
        {
            ui->Button_Mode->setText("평상 모드");
            ui->Button_Mode->setStyleSheet("color: rgb(0, 0, 0);");
            w = ui->Button_Mode->width();
            h = ui->Button_Mode->height();
            ui->Button_Mode->setPixmap(m_pix_Mode.scaled(w, h));
        }
    }

    bool bEB = m_ActivePacket.EB_P == false || m_ActivePacket.EB_M == false;

    if(m_nCurrentMode == SCREEN_ON )
    {
        // MCS/YARD 모드일 때 과속 시 경고음 출력(2023.03.21)
        //
        if( m_Sound_OverSpeed != nullptr )
        {
            if( m_PrevActivePacket.OverSpeed != m_ActivePacket.OverSpeed ||
                m_PrevActivePacket.DM_FA != m_ActivePacket.DM_FA ||
                m_PrevActivePacket.DM_AUTO != m_ActivePacket.DM_AUTO ||
                m_PrevActivePacket.DM_MCS != m_ActivePacket.DM_MCS ||
                m_PrevActivePacket.DM_YARD != m_ActivePacket.DM_YARD ||
                m_PrevActivePacket.DM_EM != m_ActivePacket.DM_EM ||
                m_bPrevEB != bEB ||
                m_bRefreshSound == true )
            {
                if( m_ActivePacket.DM_MCS == true || m_ActivePacket.DM_YARD == true )
                {
                    if( m_ActivePacket.OverSpeed == true && bEB == false )
                        m_Sound_OverSpeed->play();
                    else
                        m_Sound_OverSpeed->stop();
                }
                else
                {
                    m_Sound_OverSpeed->stop();
                }
            }
        }
        // 비상제동 사운드 출력 (2023.03.21)
        //
        if( m_Sound_EB != nullptr )
        {
            if( m_PrevActivePacket.DM_FA != m_ActivePacket.DM_FA ||
                m_PrevActivePacket.DM_AUTO != m_ActivePacket.DM_AUTO ||
                m_PrevActivePacket.DM_MCS != m_ActivePacket.DM_MCS ||
                m_PrevActivePacket.DM_YARD != m_ActivePacket.DM_YARD ||
                m_PrevActivePacket.DM_EM != m_ActivePacket.DM_EM ||
                m_PrevActivePacket.ZVR != m_ActivePacket.ZVR ||
                m_bPrevOn02 != ui->speed_gauge->getOn02() ||
                m_bPrevEB != bEB ||
                m_bRefreshSound == true )
            {
                if( m_ActivePacket.DM_EM == false && bEB == true )
                {
                    if( m_ActivePacket.ZVR == true && ui->speed_gauge->getOn02() == true )
                        m_Sound_EB->stop();
                    else
                        m_Sound_EB->play();
                }
                else
                {
                    m_Sound_EB->stop();
                }
            }
        }
    }

    m_bPrevEB = bEB;
    m_bPrevOn02 = ui->speed_gauge->getOn02();
    m_bRedraw = false;
    m_bRefreshSound = false;

    m_bPrevConnectSystem1 = m_bConnectSystem1;
    m_bPrevConnectSystem2 = m_bConnectSystem2;
    memcpy(&m_PrevActivePacket, &m_ActivePacket, sizeof(Packet));
}


//#############################################################################
//
// BCDToDecimal
//      - convert BCD to Decimal
//
unsigned char MainWindow::BCDToDecimal(unsigned char bcd)
{
    unsigned char decimal = 0;
    unsigned char temp = 0;

    temp = bcd & 0xF0;
    temp >>= 4;
    temp *= 10;
    decimal = temp;
    temp = bcd & 0x0F;
    decimal += temp;

    return decimal;
}

//#############################################################################
//
//  DeleteOldLogFile
//      - delete log file over 15 days
//
void MainWindow::DeleteOldLogFile()
{
    DIR* d;
    struct dirent* dir;

    char szYear[10];
    char szMonth[10];
    char szDay[10];
    int nYear;
    int nMonth;
    int nDay;

    time_t current_t;
    time_t file_t;
    struct tm* _tm;

    double diff_time;
    int diff_day;

    char filepath[512];

    time(&current_t);
    strftime(filepath, 20, "%Y-%m-%d %H:%M:%S", localtime(&current_t));
    qDebug() << filepath;

    szYear[4] = 0x00;
    szMonth[2] = 0x00;
    szDay[2] = 0x00;
    time(&file_t);
    _tm = localtime(&file_t);

    QDir remove_dir(QApplication::applicationDirPath() + "/log/");
    d = opendir(m_szLoggingPath);
    if(d)
    {
        while((dir = readdir(d)) != nullptr)
        {
            if( dir->d_name[8] == '_')
            {
                if( strstr(dir->d_name, ".dat") != nullptr)
                {
                    strncpy(szYear, dir->d_name, 4);
                    strncpy(szMonth, &dir->d_name[4], 2);
                    strncpy(szDay, &dir->d_name[6], 2);
                    nYear = atoi(szYear);
                    nMonth = atoi(szMonth);
                    nDay = atoi(szDay);

                    _tm->tm_year = nYear - 1900;
                    _tm->tm_mon = nMonth - 1;
                    _tm->tm_mday = nDay;
                    file_t = mktime(_tm);

                    diff_time = difftime(current_t, file_t);
                    diff_day = static_cast<int>(diff_time / (60*60*24));
                    strftime(filepath, 20, "%Y-%m-%d %H:%M:%S", localtime(&file_t));
                    qDebug() << filepath << " : " << QString::number(diff_time) << " - " << QString::number(diff_day);

                    if( diff_day > 15 )
                    {
                        remove_dir.remove(dir->d_name);
                        sprintf(filepath, "sudo rm %s/%s", m_szLoggingPath, dir->d_name);
                        system(filepath);
                        sprintf(filepath, "%s/%s", m_szLoggingPath, dir->d_name);
                        remove(filepath);
                        qDebug() << "Delete - " << dir->d_name;
                    }
                }
            }
        }
        closedir(d);
    }
    system("sync && sync");
}

void MainWindow::DeleteOldLogFile2()
{
    QDir dir(QApplication::applicationDirPath() + "/log/");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name|QDir::Reversed);
    foreach(QFileInfo item, dir.entryInfoList())
    {
        if( item.isFile() )
        {
        }
    }
}

//#############################################################################
//
//  WriteCommLogFile
//      - write comm. packet data to log file
//
void MainWindow::WriteCommLogFile()
{
    int nYear, nMonth, nDay;
    int nHour, nMinute, nSecond;

    if( m_nRxBufferCount <= 0)
        return;

    bool bHCR = (m_RxBufferList[0][14] & 0x10) > 0 ? true : false;
    if( bHCR == true )
    {
        if( m_LoggingMode == LOGGING_NONE || m_LoggingMode == LOGGING_HCR_OFF )
        {
            nYear = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][4]));
            nMonth = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][5]));
            nDay = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][6]));
            nHour = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][7]));
            nMinute = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][8]));
            nSecond = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][9]));
            if( nYear == 0 )
            {
                m_nRxBufferCount = 0;
                return;
            }

            sprintf(m_szLoggingFile, "%s/%04d%02d%02d_%02d%02d%02d_%02d_HCR.dat", m_szLoggingPath, nYear + 2000, nMonth, nDay, nHour, nMinute, nSecond, m_nGroupNo);
            m_LoggingMode = LOGGING_HCR_ON;
        }
    }
    else
    {
        if( m_LoggingMode == LOGGING_NONE || m_LoggingMode == LOGGING_HCR_ON )
        {
            nYear = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][4]));
            nMonth = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][5]));
            nDay = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][6]));
            nHour = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][7]));
            nMinute = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][8]));
            nSecond = BCDToDecimal(static_cast<unsigned char>(m_RxBufferList[m_nRxBufferCount-1][9]));
            if( nYear == 0 )
            {
                m_nRxBufferCount = 0;
                return;
            }

            sprintf(m_szLoggingFile, "%s/%04d%02d%02d_%02d%02d%02d_%02d.dat", m_szLoggingPath, nYear + 2000, nMonth, nDay, nHour, nMinute, nSecond, m_nGroupNo);
            m_LoggingMode = LOGGING_HCR_OFF;
        }
    }

    if( m_bLogDownloadStatus == false )
    {
        m_FileLogging.setFileName(m_szLoggingFile);
        if( m_FileLogging.open(QIODevice::WriteOnly | QIODevice::Append) == true )
        {
            for(int i = 0; i < m_nRxBufferCount; ++i )
                m_FileLogging.write(m_RxBufferList[i], PACKET_SIZE+6);
            m_FileLogging.close();
        }
    }
    m_nRxBufferCount = 0;
}


//#############################################################################
//
//  AddRxDataToBufferList
//      - add serial receive data to buffer list
//
void MainWindow::AddRxDataToBufferList(char *data)
{ 
    if( data == nullptr )
        return;

    memcpy(m_RxBufferList[m_nRxBufferCount], data, PACKET_SIZE+6);
    ++m_nRxBufferCount;
}

//#############################################################################
//
//  AddTxDataToBufferList
//      - add serial receive data to buffer list
//
void MainWindow::AddTxDataToBufferList(char *data)
{
    memcpy(m_TxBufferList[m_nTxBufferCount], data, PACKET_SIZE+6);
    ++m_nTxBufferCount;
    if( m_nTxBufferCount >= 50)
    {
        m_nTxBufferCount = 0;
    }
}

//#############################################################################
//
//  CommTimerCallback
//      - timer callback
//      - parsing receive packet
//
void MainWindow::CommTimerCallback()
{
    char chData = 0;
    int nNextIndex = 0;

    while( m_nRecvQueue1Front != m_nRecvQueue1Rear )
    {
        nNextIndex = (m_nRecvQueue1Front + 1) % MAX_RECEIVE_BUFFER;
        chData = m_szReceivedData1[m_nRecvQueue1Front];
        m_CurrRecvBuffer1[m_nBufferIndex1++] = chData;
        m_nRecvQueue1Front = nNextIndex;
        switch(m_nParsePhase1)
        {
        // STX
        case 0:
            if( chData == 0x02 )
            {
                ++m_nParsePhase1;
                m_nPacketIndex1 = 0;
                m_RecvCRC32_1 = 0;
            }
            break;

        // Sequence
        case 1:
            m_Packet1.sequence = static_cast<unsigned char>(chData);
            m_PacketBuffer1[m_nPacketIndex1++] = m_Packet1.sequence;
            ++m_nParsePhase1;
            break;

        // Destination
        case 2:
            m_Packet1.destination = static_cast<unsigned char>(chData);
            m_PacketBuffer1[m_nPacketIndex1++] = m_Packet1.destination;
            if( m_Packet1.destination != 0x15 )
            {
                m_nParsePhase1 = 0;
                m_nBufferIndex1 = 0;
                AddMessage(1, "Packet Destination Error", "RED");
            }
            else
            {
                ++m_nParsePhase1;
            }
            break;

        // Source
        case 3:
            m_Packet1.source = static_cast<unsigned char>(chData);
            m_PacketBuffer1[m_nPacketIndex1++] = m_Packet1.source;
            if( m_Packet1.source != 0x10 && m_Packet1.source != 0x20 )
            {
                m_nParsePhase1 = 0;
                m_nBufferIndex1 = 0;
                AddMessage(1, "Packet Source Error", "RED");
            }
            else
            {
                ++m_nParsePhase1;
            }
            break;

         // Data
         case 4:
            if( m_nPacketIndex1 >= PACKET_BUFFER_SIZE )
            {
                m_nParsePhase1 = 0;
                m_nBufferIndex1 = 0;
                AddMessage(1, "Packet Packet size Error", "RED");
            }

            m_PacketBuffer1[m_nPacketIndex1++] = static_cast<unsigned char>(chData);
            if( m_nPacketIndex1 >= PACKET_SIZE )
                ++m_nParsePhase1;
            break;

         // ETX
         case 5:
            if( chData != 0x03 )
            {
                m_nParsePhase1 = 0;
                m_nPacketIndex1 = 0;
                m_nBufferIndex1 = 0;
                AddMessage(1, "Packet ETX Error", "RED");
            }
            else
            {
                m_PacketBuffer1[m_nPacketIndex1++] = static_cast<unsigned char>(chData);
                m_CRC32_1 = CalcCRC32(m_PacketBuffer1, PACKET_SIZE + 1);
                m_RecvCRC32_1 = 0;
                m_nRecvCRCCount1 = 0;
                ++m_nParsePhase1;
            }
            break;

        // CRC
        case 6:
            m_RecvCRC32_1 += static_cast<unsigned char>(chData);
            ++m_nRecvCRCCount1;
            if( m_nRecvCRCCount1 >= 4)
            {
                if( m_CRC32_1 == m_RecvCRC32_1 )
                {
                    ParsePacket1();

                    AddRxDataToBufferList(m_CurrRecvBuffer1);
                    m_nBufferIndex1 = 0;

                    m_LastRecvTime1 = QDateTime::currentDateTime();
                    if( m_nRecvPacketCount1 < 100 )
                        ++m_nRecvPacketCount1;

                    SendPacketToDevice1();
                }
                else
                {
                    AddMessage(1, "Packet CRC Error", "RED");
                }

                m_nParsePhase1 = 0;
                m_nPacketIndex1 = 0;
                m_RecvCRC32_1 = 0;
                m_nRecvCRCCount1 = 0;
                m_nBufferIndex1 = 0;
            }
            else
            {
                m_RecvCRC32_1 <<= 8;
            }
            break;
        }
    }

    while( m_nRecvQueue2Front != m_nRecvQueue2Rear )
    {
        nNextIndex = (m_nRecvQueue2Front + 1) % MAX_RECEIVE_BUFFER;
        chData = m_szReceivedData2[m_nRecvQueue2Front];
        m_CurrRecvBuffer2[m_nBufferIndex2++] = chData;
        m_nRecvQueue2Front = nNextIndex;

        switch(m_nParsePhase2)
        {
        // STX
        case 0:
            if( chData == 0x02 )
            {
                ++m_nParsePhase2;
                m_nPacketIndex2 = 0;
                m_RecvCRC32_2 = 0;
            }
            break;

        // Sequence
        case 1:
            m_Packet2.sequence = static_cast<unsigned char>(chData);
            m_PacketBuffer2[m_nPacketIndex2++] = m_Packet2.sequence;
            ++m_nParsePhase2;
            break;

        // Destination
        case 2:
            m_Packet2.destination = static_cast<unsigned char>(chData);
            m_PacketBuffer2[m_nPacketIndex2++] = m_Packet2.destination;
            if( m_Packet2.destination != 0x15 )
            {
                m_nParsePhase2 = 0;
                m_nBufferIndex2 = 0;
                AddMessage(2, "Packet Destination Error", "RED");
            }
            else
            {
                ++m_nParsePhase2;
            }
            break;

        // Source
        case 3:
            m_Packet2.source = static_cast<unsigned char>(chData);
            m_PacketBuffer2[m_nPacketIndex2++] = m_Packet2.source;
            if( m_Packet2.source != 0x10 && m_Packet2.source != 0x20 )
            {
                m_nParsePhase2 = 0;
                m_nBufferIndex2 = 0;
                AddMessage(2, "Packet Source Error", "RED");
            }
            else
            {
                ++m_nParsePhase2;
            }
            break;

         // Data
         case 4:
            if( m_nPacketIndex2 >= PACKET_BUFFER_SIZE )
            {
                m_nParsePhase2 = 0;
                m_nBufferIndex2 = 0;
                AddMessage(2, "Packet Packet size Error", "RED");
            }

            m_PacketBuffer2[m_nPacketIndex2++] = static_cast<unsigned char>(chData);
            if( m_nPacketIndex2 >= PACKET_SIZE )
                ++m_nParsePhase2;
            break;

         // ETX
         case 5:
            if( chData != 0x03 )
            {
                m_nParsePhase2 = 0;
                m_nPacketIndex2 = 0;
                m_nBufferIndex2 = 0;
                AddMessage(2, "Packet ETX Error", "RED");
            }
            else
            {
                m_PacketBuffer2[m_nPacketIndex2++] = static_cast<unsigned char>(chData);
                m_CRC32_2 = CalcCRC32(m_PacketBuffer2, PACKET_SIZE + 1);
                m_RecvCRC32_2 = 0;
                m_nRecvCRCCount2 = 0;
                ++m_nParsePhase2;
            }
            break;

        // CRC
        case 6:
            m_RecvCRC32_2 += static_cast<unsigned char>(chData);
            ++m_nRecvCRCCount2;
            if( m_nRecvCRCCount2 >= 4)
            {
                if( m_CRC32_2 == m_RecvCRC32_2 )
                {
                    ParsePacket2();

                    AddRxDataToBufferList(m_CurrRecvBuffer2);
                    m_nBufferIndex2 = 0;

                    m_LastRecvTime2 = QDateTime::currentDateTime();
                    if( m_nRecvPacketCount2 < 100 )
                        ++m_nRecvPacketCount2;

                    SendPacketToDevice2();
                }
                else
                {
                    AddMessage(2, "Packet CRC Error", "RED");
                }

                m_nParsePhase2 = 0;
                m_nPacketIndex2 = 0;
                m_RecvCRC32_2 = 0;
                m_nRecvCRCCount2 = 0;
                m_nBufferIndex2 = 0;
            }
            else
            {
                m_RecvCRC32_2 <<= 8;
            }
            break;
        }
    }
}


//#############################################################################
//
// ParsePacket1
//      - 수신한 1계 시리얼 데이터에서 필요한 정보 추출
//
void MainWindow::ParsePacket1()
{
    ++m_Packet1.count;

    m_Packet1.year = BCDToDecimal(m_PacketBuffer1[3]);
    m_Packet1.month = BCDToDecimal(m_PacketBuffer1[4]);
    m_Packet1.day = BCDToDecimal(m_PacketBuffer1[5]);
    m_Packet1.hour = BCDToDecimal(m_PacketBuffer1[6]);
    m_Packet1.minute = BCDToDecimal(m_PacketBuffer1[7]);
    m_Packet1.second = BCDToDecimal(m_PacketBuffer1[8]);

    m_Packet1.train_speed = m_PacketBuffer1[9];
    m_Packet1.train_speed <<= 8;
    m_Packet1.train_speed += m_PacketBuffer1[10];
    double dSpeed = m_Packet1.train_speed;
    dSpeed = dSpeed / 10;
    dSpeed = floor(dSpeed + 0.5);
    m_Packet1.train_speed = static_cast<int>(dSpeed);
    m_Packet1.atc_speed = m_PacketBuffer1[11];

    m_Packet1.ZVR = (m_PacketBuffer1[12] & 0x20) > 0 ? true : false;
    m_Packet1.EDL = (m_PacketBuffer1[12] & 0x10) > 0 ? true : false;
    m_Packet1.EDR = (m_PacketBuffer1[12] & 0x08) > 0 ? true : false;
    m_Packet1.FSB = (m_PacketBuffer1[12] & 0x04) > 0 ? true : false;
    m_Packet1.EB_M = (m_PacketBuffer1[12] & 0x02) > 0 ? true : false;
    m_Packet1.EB_P = (m_PacketBuffer1[12] & 0x01) > 0 ? true : false;

    m_Packet1.active = (m_PacketBuffer1[13] & 0x80) > 0 ? true : false;
    m_Packet1.OverSpeed = (m_PacketBuffer1[13] & 0x40) > 0 ? true : false;
    m_Packet1.TCR = (m_PacketBuffer1[13] & 0x20) > 0 ? true : false;
    m_Packet1.HCR = (m_PacketBuffer1[13] & 0x10) > 0 ? true : false;
    m_Packet1.train_door_open = (m_PacketBuffer1[13] & 0x08) > 0 ? true : false;
    m_Packet1.train_door_close = (m_PacketBuffer1[13] & 0x04) > 0 ? true : false;
    m_Packet1.psd_open = (m_PacketBuffer1[13] & 0x02) > 0 ? true : false;
    m_Packet1.psd_close = (m_PacketBuffer1[13] & 0x01) > 0 ? true : false;

    m_Packet1.DM_FA = (m_PacketBuffer1[14] & 0x80) > 0 ? true : false;
    m_Packet1.DM_AUTO = (m_PacketBuffer1[14] & 0x40) > 0 ? true : false;
    m_Packet1.DM_MCS = (m_PacketBuffer1[14] & 0x20) > 0 ? true : false;
    m_Packet1.DM_YARD = (m_PacketBuffer1[14] & 0x10) > 0 ? true : false;
    m_Packet1.DM_EM = (m_PacketBuffer1[14] & 0x08) > 0 ? true : false;
    m_Packet1.DIR_RVS = (m_PacketBuffer1[14] & 0x04) > 0 ? true : false;
    m_Packet1.DIR_FWD = (m_PacketBuffer1[14] & 0x02) > 0 ? true : false;
    m_Packet1.DIR_NEU = (m_PacketBuffer1[14] & 0x01) > 0 ? true : false;

    m_Packet1.mascon_eb = (m_PacketBuffer1[15] & 0x08) > 0 ? true : false;
    m_Packet1.mascon_br = (m_PacketBuffer1[15] & 0x04) > 0 ? true : false;
    m_Packet1.mascon_dr = (m_PacketBuffer1[15] & 0x02) > 0 ? true : false;
    m_Packet1.mascon_neu = (m_PacketBuffer1[15] & 0x01) > 0 ? true : false;

    m_Packet1.DIA1 = m_PacketBuffer1[16];
    m_Packet1.DIA2 = m_PacketBuffer1[17];

    m_Packet1.ATC_STATUS = m_PacketBuffer1[18] & 0xc0;
    m_Packet1.ATC_STATUS >>= 6;
    m_Packet1.Wheel_DIA_Check_Request = (m_PacketBuffer1[18] & 0x20) > 0 ? true : false;
    m_Packet1.manual_PDT_START = (m_PacketBuffer1[18] & 0x10) > 0 ? true : false;
    m_Packet1.manual_PDT_NG = (m_PacketBuffer1[18] & 0x08) > 0 ? true : false;
    m_Packet1.manual_PDT_OK = (m_PacketBuffer1[18] & 0x04) > 0 ? true : false;
    m_Packet1.initial_PDT_NG = (m_PacketBuffer1[18] & 0x02) > 0 ? true : false;
    m_Packet1.initial_PDT_OK = (m_PacketBuffer1[18] & 0x01) > 0 ? true : false;

    m_Packet1.data_19 = m_PacketBuffer1[19];
    /*
    m_Packet1.ATC_receiver = (m_PacketBuffer1[19] & 0x80) > 0 ? true : false;
    m_Packet1.ATO_controller = (m_PacketBuffer1[19] & 0x40) > 0 ? true : false;
    m_Packet1.TCMS = (m_PacketBuffer1[19] & 0x20) > 0 ? true : false;
    m_Packet1.TACHO_2 = (m_PacketBuffer1[19] & 0x02) > 0 ? true : false;
    m_Packet1.TACHO_1 = (m_PacketBuffer1[19] & 0x01) > 0 ? true : false;
    */

    m_Packet1.RM_normal = (m_PacketBuffer1[20] & 0x40) > 0 ? true : false;
    m_Packet1.RM_recovery = (m_PacketBuffer1[20] & 0x80) > 0 ? true : false;

    m_Packet1.PWM = m_PacketBuffer1[21];

    m_Packet1.TC_DR = (m_PacketBuffer1[22] & 0x80) > 0 ? true : false;
    m_Packet1.TC_BR = (m_PacketBuffer1[22] & 0x40) > 0 ? true : false;
    m_Packet1.TC_CS = (m_PacketBuffer1[22] & 0x20) > 0 ? true : false;
    m_Packet1.DC_ADOL = (m_PacketBuffer1[22] & 0x10) > 0 ? true : false;
    m_Packet1.DC_ADOR = (m_PacketBuffer1[22] & 0x08) > 0 ? true : false;
    m_Packet1.DC_ADC = (m_PacketBuffer1[22] & 0x04) > 0 ? true : false;

    m_Packet1.cab_no_TC1 = (m_PacketBuffer1[23] & 0x80) > 0 ? true : false;
    m_Packet1.cab_no_TC2 = (m_PacketBuffer1[23] & 0x40) > 0 ? true : false;
    m_Packet1.DOLP_L = (m_PacketBuffer1[23] & 0x10) > 0 ? true : false;
    m_Packet1.DOLP_R = (m_PacketBuffer1[23] & 0x08) > 0 ? true : false;
    m_Packet1.DM_AA = (m_PacketBuffer1[23] & 0x04) > 0 ? true : false;
    m_Packet1.DM_AM = (m_PacketBuffer1[23] & 0x02) > 0 ? true : false;
    m_Packet1.DM_MM = (m_PacketBuffer1[23] & 0x01) > 0 ? true : false;

    //m_Packet1.formation_no = m_PacketBuffer1[27];
    m_Packet1.train_no = ((m_PacketBuffer1[25] & 0xF0) >> 4) * 1000;
    m_Packet1.train_no += (m_PacketBuffer1[25] & 0x0F) * 100;
    m_Packet1.train_no += ((m_PacketBuffer1[26] & 0xF0) >> 4) * 10;
    m_Packet1.train_no += (m_PacketBuffer1[26] & 0x0F);

    m_Packet1.atc_Speed_Code = (m_PacketBuffer1[34] & 0xF0);
    m_Packet1.atc_Speed_Code >>= 4;
    m_Packet1.MessageNo = m_PacketBuffer1[39];

    m_Packet1.MajorVersion = ((m_PacketBuffer1[55] & 0xF0) >> 4);
    m_Packet1.MinorVersion = (m_PacketBuffer1[55] & 0x0F);

    // Diagnostic Error
    //
    m_Packet1.data_40 = m_PacketBuffer1[40];
    m_Packet1.data_41 = m_PacketBuffer1[41];
    m_Packet1.data_42 = m_PacketBuffer1[42];
    /*
    m_Packet1.DE_ATIV = (m_PacketBuffer1[40] & 0x80) > 0 ? true : false;
    m_Packet1.DE_VOTING = (m_PacketBuffer1[40] & 0x40) > 0 ? true : false;
    m_Packet1.DE_RUNRTDGET = (m_PacketBuffer1[40] & 0x20) > 0 ? true : false;
    m_Packet1.DE_ATIVGET = (m_PacketBuffer1[40] & 0x10) > 0 ? true : false;
    m_Packet1.DE_DGS = (m_PacketBuffer1[40] & 0x02) > 0 ? true : false;

    m_Packet1.DE_MDHEAL = (m_PacketBuffer1[41] & 0x80) > 0 ? true : false;
    m_Packet1.DE_MNUALBYSET = (m_PacketBuffer1[41] & 0x40) > 0 ? true : false;
    m_Packet1.DE_INPUT = (m_PacketBuffer1[41] & 0x20) > 0 ? true : false;
    m_Packet1.DE_UNHEAL = (m_PacketBuffer1[41] & 0x10) > 0 ? true : false;
    m_Packet1.DE_EOT = (m_PacketBuffer1[41] & 0x08) > 0 ? true : false;
    m_Packet1.DE_EOTVAL = (m_PacketBuffer1[41] & 0x04) > 0 ? true : false;
    m_Packet1.DE_RUNRTD = (m_PacketBuffer1[41] & 0x02) > 0 ? true : false;
    m_Packet1.DE_RUNRTDSET = (m_PacketBuffer1[41] & 0x01) > 0 ? true : false;

    m_Packet1.DE_EBDATA = (m_PacketBuffer1[42] & 0x20) > 0 ? true : false;
    m_Packet1.DE_VDOTEMP = (m_PacketBuffer1[42] & 0x10) > 0 ? true : false;
    m_Packet1.DE_VDITEMP = (m_PacketBuffer1[42] & 0x08) > 0 ? true : false;
    m_Packet1.DE_OUTGOING = (m_PacketBuffer1[42] & 0x04) > 0 ? true : false;
    m_Packet1.DE_OUTPUTFB = (m_PacketBuffer1[42] & 0x02) > 0 ? true : false;
    m_Packet1.DE_MNUALBYCLR = (m_PacketBuffer1[42] & 0x01) > 0 ? true : false;
    */

    // ATC Error
    //
    m_Packet1.data_44 = m_PacketBuffer1[44];
    m_Packet1.data_45 = m_PacketBuffer1[45];
    m_Packet1.data_46 = m_PacketBuffer1[46];
    m_Packet1.data_47 = m_PacketBuffer1[47];
    /*
    m_Packet1.ATC_MSCSTATE = (m_PacketBuffer1[44] & 0x80) > 0 ? true : false;
    m_Packet1.ATC_DOORSTAT = (m_PacketBuffer1[44] & 0x40) > 0 ? true : false;
    m_Packet1.ATC_DRIVEMODE = (m_PacketBuffer1[44] & 0x20) > 0 ? true : false;
    m_Packet1.ATC_CABMOD = (m_PacketBuffer1[44] & 0x10) > 0 ? true : false;
    m_Packet1.ATC_ATC = (m_PacketBuffer1[44] & 0x04) > 0 ? true : false;

    m_Packet1.ATC_MAXSPEED = (m_PacketBuffer1[45] & 0x80) > 0 ? true : false;
    m_Packet1.ATC_WHEELDIA = (m_PacketBuffer1[45] & 0x40) > 0 ? true : false;
    m_Packet1.ATC_MANUALPDT = (m_PacketBuffer1[45] & 0x20) > 0 ? true : false;
    m_Packet1.ATC_INITPDT = (m_PacketBuffer1[45] & 0x10) > 0 ? true : false;
    m_Packet1.ATC_DIRSET = (m_PacketBuffer1[45] & 0x08) > 0 ? true : false;
    m_Packet1.ATC_TCSEL = (m_PacketBuffer1[45] & 0x04) > 0 ? true : false;
    m_Packet1.ATC_PSDSTATE = (m_PacketBuffer1[45] & 0x02) > 0 ? true : false;
    m_Packet1.ATC_REVERSING = (m_PacketBuffer1[45] & 0x01) > 0 ? true : false;

    m_Packet1.ATC_TRBSMODE = (m_PacketBuffer1[46] & 0x80) > 0 ? true : false;
    m_Packet1.ATC_TDCSMOD = (m_PacketBuffer1[46] & 0x40) > 0 ? true : false;
    m_Packet1.ATC_TZSSMODE = (m_PacketBuffer1[46] & 0x20) > 0 ? true : false;
    m_Packet1.ATC_TRSSMODE = (m_PacketBuffer1[46] & 0x10) > 0 ? true : false;
    m_Packet1.ATC_RESTRICTIO = (m_PacketBuffer1[46] & 0x08) > 0 ? true : false;
    m_Packet1.ATC_ATCCODE = (m_PacketBuffer1[46] & 0x04) > 0 ? true : false;
    m_Packet1.ATC_MAXPULSE = (m_PacketBuffer1[46] & 0x02) > 0 ? true : false;
    m_Packet1.ATC_DIFFSPEED = (m_PacketBuffer1[46] & 0x01) > 0 ? true : false;

    m_Packet1.ATC_ATCMODE = (m_PacketBuffer1[47] & 0x02) > 0 ? true : false;
    m_Packet1.ATC_DIRECT = (m_PacketBuffer1[47] & 0x01) > 0 ? true : false;
    */

    // Interface Error
    //
    m_Packet1.data_48 = m_PacketBuffer1[48];
    m_Packet1.data_49 = m_PacketBuffer1[49];
    m_Packet1.data_50 = m_PacketBuffer1[50];
    m_Packet1.data_51 = m_PacketBuffer1[51];
    /*
    m_Packet1.IF_IFS = (m_PacketBuffer1[48] & 0x01) > 0 ? true : false;

    m_Packet1.IF_ATCRINIT = (m_PacketBuffer1[49] & 0x80) > 0 ? true : false;
    m_Packet1.IF_ATCRW = (m_PacketBuffer1[49] & 0x40) > 0 ? true : false;
    m_Packet1.IF_ATCRR = (m_PacketBuffer1[49] & 0x20) > 0 ? true : false;
    m_Packet1.IF_CANINIT = (m_PacketBuffer1[49] & 0x10) > 0 ? true : false;
    m_Packet1.IF_TCMSINIT = (m_PacketBuffer1[49] & 0x08) > 0 ? true : false;
    m_Packet1.IF_TCMSW = (m_PacketBuffer1[49] & 0x04) > 0 ? true : false;
    m_Packet1.IF_TCMSR = (m_PacketBuffer1[49] & 0x02) > 0 ? true : false;

    m_Packet1.IF_VDIINIT = (m_PacketBuffer1[50] & 0x80) > 0 ? true : false;
    m_Packet1.IF_VDIW = (m_PacketBuffer1[50] & 0x40) > 0 ? true : false;
    m_Packet1.IF_VDIR = (m_PacketBuffer1[50] & 0x20) > 0 ? true : false;
    m_Packet1.IF_DIOINIT = (m_PacketBuffer1[50] & 0x10) > 0 ? true : false;
    m_Packet1.IF_ATOCINIT = (m_PacketBuffer1[50] & 0x08) > 0 ? true : false;
    m_Packet1.IF_ATOCW = (m_PacketBuffer1[50] & 0x04) > 0 ? true : false;
    m_Packet1.IF_ATOCR = (m_PacketBuffer1[50] & 0x02) > 0 ? true : false;

    m_Packet1.IF_VDOINIT = (m_PacketBuffer1[51] & 0x08) > 0 ? true : false;
    m_Packet1.IF_VDOW = (m_PacketBuffer1[51] & 0x04) > 0 ? true : false;
    m_Packet1.IF_VDOR = (m_PacketBuffer1[51] & 0x02) > 0 ? true : false;
    */

    // ATO Error
    //
    m_Packet1.data_52 = m_PacketBuffer1[52];
    m_Packet1.data_53 = m_PacketBuffer1[53];
    /*
    m_Packet1.ATO_RLYFB = (m_PacketBuffer1[52] & 0x20) > 0 ? true : false;
    m_Packet1.ATO_DOORCI = (m_PacketBuffer1[52] & 0x10) > 0 ? true : false;
    m_Packet1.ATO_DOOROP = (m_PacketBuffer1[52] & 0x08) > 0 ? true : false;
    m_Packet1.ATO_STNCODE = (m_PacketBuffer1[52] & 0x04) > 0 ? true : false;
    m_Packet1.ATO_ATOANT = (m_PacketBuffer1[52] & 0x02) > 0 ? true : false;
    m_Packet1.ATO_ATOSTOP = (m_PacketBuffer1[52] & 0x01) > 0 ? true : false;

    m_Packet1.ATO_PG32 = (m_PacketBuffer1[53] & 0x04) > 0 ? true : false;
    m_Packet1.ATO_PG2 = (m_PacketBuffer1[53] & 0x02) > 0 ? true : false;
    m_Packet1.ATO_PG1 = (m_PacketBuffer1[53] & 0x01) > 0 ? true : false;
    */

    // 차륜경 확인 팝업
    //
    if( m_PrevPacket1.Wheel_DIA_Check_Request == false && m_Packet1.Wheel_DIA_Check_Request == true )
        m_bWheelCheckPopup1 = true;
    if( m_PrevPacket1.Wheel_DIA_Check_Request == true && m_Packet1.Wheel_DIA_Check_Request == false )
        m_bWheelCheckPopup1 = false;

    // 메시지 처리
    //
    if( (m_PrevPacket1.MessageNo == 0 && m_Packet1.MessageNo != 0) ||
        m_PrevPacket1.MessageNo != m_Packet1.MessageNo )
    {
        AddMessage(1, m_Packet1.MessageNo);
        m_SendPacket1.MessageNo = m_Packet1.MessageNo;
    }

    /* version 1.14.3 - 중복메시지 제거
    if( m_PrevPacket1.manual_PDT_NG == false && m_Packet1.manual_PDT_NG == true )
        AddMessage(1, ERROR_MANUAL_PDT_1);
    if( m_PrevPacket1.initial_PDT_NG == false && m_Packet1.initial_PDT_NG == true )
        AddMessage(1, ERROR_INITIAL_PDT_1);
    */

    int i = 0;
    unsigned char cShift = 0x01;
    unsigned char cPrev = 0x00;
    unsigned char cCurr = 0x00;
    if( m_PrevPacket1.data_19 != m_Packet1.data_19)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_19 & cShift;
            cCurr = m_Packet1.data_19 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 190 + i);
        }
    }

    // Diagnostic Error
    //
    if( m_PrevPacket1.data_40 != m_Packet1.data_40)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_40 & cShift;
            cCurr = m_Packet1.data_40 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 400 + i);
        }
    }
    if( m_PrevPacket1.data_41 != m_Packet1.data_41)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_41 & cShift;
            cCurr = m_Packet1.data_41 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 410 + i);
        }
    }
    if( m_PrevPacket1.data_42 != m_Packet1.data_42)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_42 & cShift;
            cCurr = m_Packet1.data_42 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 420 + i);
        }
    }

    // ATC Error
    //
    if( m_PrevPacket1.data_44 != m_Packet1.data_44)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_44 & cShift;
            cCurr = m_Packet1.data_44 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 440 + i);
        }
    }
    if( m_PrevPacket1.data_45 != m_Packet1.data_45)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_45 & cShift;
            cCurr = m_Packet1.data_45 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 450 + i);
        }
    }
    if( m_PrevPacket1.data_46 != m_Packet1.data_46)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_46 & cShift;
            cCurr = m_Packet1.data_46 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 460 + i);
        }
    }
    if( m_PrevPacket1.data_47 != m_Packet1.data_47)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_47 & cShift;
            cCurr = m_Packet1.data_47 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 470 + i);
        }
    }

    // Interface Error
    //
    if( m_PrevPacket1.data_48 != m_Packet1.data_48)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_48 & cShift;
            cCurr = m_Packet1.data_48 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 480 + i);
        }
    }
    if( m_PrevPacket1.data_49 != m_Packet1.data_49)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_49 & cShift;
            cCurr = m_Packet1.data_49 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 490 + i);
        }
    }
    if( m_PrevPacket1.data_50 != m_Packet1.data_50)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_50 & cShift;
            cCurr = m_Packet1.data_50 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 500 + i);
        }
    }
    if( m_PrevPacket1.data_51 != m_Packet1.data_51)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_51 & cShift;
            cCurr = m_Packet1.data_51 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 510 + i);
        }
    }

    // ATO Error
    //
    if( m_PrevPacket1.data_52 != m_Packet1.data_52)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_52 & cShift;
            cCurr = m_Packet1.data_52 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 520 + i);
        }
    }
    if( m_PrevPacket1.data_53 != m_Packet1.data_53)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket1.data_53 & cShift;
            cCurr = m_Packet1.data_53 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(1, 530 + i);
        }
    }

    /*
    if( m_PrevPacket1.ATC_receiver == false && m_Packet1.ATC_receiver == true )
        AddMessage(1, ERROR_COMM_ATC_1);
    if( m_PrevPacket1.ATO_controller == false && m_Packet1.ATO_controller == true )
        AddMessage(1, ERROR_COMM_ATO_1);
    if( m_PrevPacket1.TCMS == false && m_Packet1.TCMS == true )
        AddMessage(1, ERROR_COMM_TCMS_1);
    if( m_PrevPacket1.TACHO_2 == false && m_Packet1.TACHO_2 == true )
        AddMessage(1, ERROR_COMM_TACHO2_1);
    if( m_PrevPacket1.TACHO_1 == false && m_Packet1.TACHO_1 == true )
        AddMessage(1, ERROR_COMM_TACHO1_1);

    if( m_PrevPacket1.ATC_MSCSTATE == false && m_Packet1.ATC_MSCSTATE == true )
        AddMessage(1, ERROR_ATC_MSCSTATE_1);
    if( m_PrevPacket1.ATC_DOORSTAT == false && m_Packet1.ATC_DOORSTAT == true )
        AddMessage(1, ERROR_ATC_DOORSTAT_1);
    if( m_PrevPacket1.ATC_DRIVEMODE == false && m_Packet1.ATC_DRIVEMODE == true )
        AddMessage(1, ERROR_ATC_DRIVEMODE_1);
    if( m_PrevPacket1.ATC_CABMOD == false && m_Packet1.ATC_CABMOD == true )
        AddMessage(1, ERROR_ATC_CABMOD_1);

    if( m_PrevPacket1.ATC_WHEELDIA == false && m_Packet1.ATC_WHEELDIA == true )
        AddMessage(1, ERROR_ATC_WHEELDIA_1);
    if( m_PrevPacket1.ATC_MANUALPDT == false && m_Packet1.ATC_MANUALPDT == true )
        AddMessage(1, ERROR_ATC_MANUALPDT_1);
    if( m_PrevPacket1.ATC_INITPDT == false && m_Packet1.ATC_INITPDT == true )
        AddMessage(1, ERROR_ATC_INITPDT_1);
    if( m_PrevPacket1.ATC_DIRSET == false && m_Packet1.ATC_DIRSET == true )
        AddMessage(1, ERROR_ATC_DIRSET_1);
    if( m_PrevPacket1.ATC_TCSEL == false && m_Packet1.ATC_TCSEL == true )
        AddMessage(1, ERROR_ATC_TCSEL_1);
    if( m_PrevPacket1.ATC_PSDSTATE == false && m_Packet1.ATC_PSDSTATE == true )
        AddMessage(1, ERROR_ATC_PSDSTATE_1);
    if( m_PrevPacket1.ATC_REVERSING == false && m_Packet1.ATC_REVERSING == true )
        AddMessage(1, ERROR_ATC_REVERSING_1);

    if( m_PrevPacket1.ATO_ATOSTOP == false && m_Packet1.ATO_ATOSTOP == true )
        AddMessage(1, ERROR_ATO_ATOSTOP_1);
    if( m_PrevPacket1.ATO_ATOANT == false && m_Packet1.ATO_ATOANT == true )
        AddMessage(1, ERROR_ATO_ATOANT_1);
    if( m_PrevPacket1.ATO_DOOROP == false && m_Packet1.ATO_DOOROP == true )
        AddMessage(1, ERROR_ATO_DOOROP_1);
    if( m_PrevPacket1.ATO_DOORCI == false && m_Packet1.ATO_DOORCI == true )
        AddMessage(1, ERROR_ATO_DOORCI_1);

    if( m_PrevPacket1.ATO_PG1 == false && m_Packet1.ATO_PG1 == true )
        AddMessage(1, ERROR_ATO_PG1_1);
    if( m_PrevPacket1.ATO_PG2 == false && m_Packet1.ATO_PG2 == true )
        AddMessage(1, ERROR_ATO_PG2_1);
    if( m_PrevPacket1.ATO_PG32 == false && m_Packet1.ATO_PG32 == true )
        AddMessage(1, ERROR_ATO_PG32_1);
    */

    memcpy(&m_PrevPacket1, &m_Packet1, sizeof(Packet));
}


//#############################################################################
//
// ParsePacket2
//      - 수신한 2계 시리얼 데이터에서 필요한 정보 추출
//
void MainWindow::ParsePacket2()
{
    ++m_Packet2.count;

    m_Packet2.year = BCDToDecimal(m_PacketBuffer2[3]);
    m_Packet2.month = BCDToDecimal(m_PacketBuffer2[4]);
    m_Packet2.day = BCDToDecimal(m_PacketBuffer2[5]);
    m_Packet2.hour = BCDToDecimal(m_PacketBuffer2[6]);
    m_Packet2.minute = BCDToDecimal(m_PacketBuffer2[7]);
    m_Packet2.second = BCDToDecimal(m_PacketBuffer2[8]);

    m_Packet2.train_speed = m_PacketBuffer2[9];
    m_Packet2.train_speed <<= 8;
    m_Packet2.train_speed += m_PacketBuffer2[10];
    double dSpeed = m_Packet2.train_speed;
    dSpeed = dSpeed / 10;
    dSpeed = floor(dSpeed + 0.5);
    m_Packet2.train_speed = static_cast<int>(dSpeed);
    m_Packet2.atc_speed = m_PacketBuffer2[11];

    m_Packet2.ZVR = (m_PacketBuffer2[12] & 0x20) > 0 ? true : false;
    m_Packet2.EDL = (m_PacketBuffer2[12] & 0x10) > 0 ? true : false;
    m_Packet2.EDR = (m_PacketBuffer2[12] & 0x08) > 0 ? true : false;
    m_Packet2.FSB = (m_PacketBuffer2[12] & 0x04) > 0 ? true : false;
    m_Packet2.EB_M = (m_PacketBuffer2[12] & 0x02) > 0 ? true : false;
    m_Packet2.EB_P = (m_PacketBuffer2[12] & 0x01) > 0 ? true : false;

    m_Packet2.active = (m_PacketBuffer2[13] & 0x80) > 0 ? true : false;
    m_Packet2.OverSpeed = (m_PacketBuffer2[13] & 0x40) > 0 ? true : false;
    m_Packet2.TCR = (m_PacketBuffer2[13] & 0x20) > 0 ? true : false;
    m_Packet2.HCR = (m_PacketBuffer2[13] & 0x10) > 0 ? true : false;
    m_Packet2.train_door_open = (m_PacketBuffer2[13] & 0x08) > 0 ? true : false;
    m_Packet2.train_door_close = (m_PacketBuffer2[13] & 0x04) > 0 ? true : false;
    m_Packet2.psd_open = (m_PacketBuffer2[13] & 0x02) > 0 ? true : false;
    m_Packet2.psd_close = (m_PacketBuffer2[13] & 0x01) > 0 ? true : false;

    m_Packet2.DM_FA = (m_PacketBuffer2[14] & 0x80) > 0 ? true : false;
    m_Packet2.DM_AUTO = (m_PacketBuffer2[14] & 0x40) > 0 ? true : false;
    m_Packet2.DM_MCS = (m_PacketBuffer2[14] & 0x20) > 0 ? true : false;
    m_Packet2.DM_YARD = (m_PacketBuffer2[14] & 0x10) > 0 ? true : false;
    m_Packet2.DM_EM = (m_PacketBuffer2[14] & 0x08) > 0 ? true : false;
    m_Packet2.DIR_RVS = (m_PacketBuffer2[14] & 0x04) > 0 ? true : false;
    m_Packet2.DIR_FWD = (m_PacketBuffer2[14] & 0x02) > 0 ? true : false;
    m_Packet2.DIR_NEU = (m_PacketBuffer2[14] & 0x01) > 0 ? true : false;

    m_Packet2.mascon_eb = (m_PacketBuffer2[15] & 0x08) > 0 ? true : false;
    m_Packet2.mascon_br = (m_PacketBuffer2[15] & 0x04) > 0 ? true : false;
    m_Packet2.mascon_dr = (m_PacketBuffer2[15] & 0x02) > 0 ? true : false;
    m_Packet2.mascon_neu = (m_PacketBuffer2[15] & 0x01) > 0 ? true : false;

    m_Packet2.DIA1 = m_PacketBuffer2[16];
    m_Packet2.DIA2 = m_PacketBuffer2[17];

    m_Packet2.ATC_STATUS = m_PacketBuffer2[18] & 0xc0;
    m_Packet2.ATC_STATUS >>= 6;
    m_Packet2.Wheel_DIA_Check_Request = (m_PacketBuffer2[18] & 0x20) > 0 ? true : false;
    m_Packet2.manual_PDT_START = (m_PacketBuffer2[18] & 0x10) > 0 ? true : false;
    m_Packet2.manual_PDT_NG = (m_PacketBuffer2[18] & 0x08) > 0 ? true : false;
    m_Packet2.manual_PDT_OK = (m_PacketBuffer2[18] & 0x04) > 0 ? true : false;
    m_Packet2.initial_PDT_NG = (m_PacketBuffer2[18] & 0x02) > 0 ? true : false;
    m_Packet2.initial_PDT_OK = (m_PacketBuffer2[18] & 0x01) > 0 ? true : false;

    m_Packet2.data_19 = m_PacketBuffer2[19];
    /*
    m_Packet2.ATC_receiver = (m_PacketBuffer2[19] & 0x80) > 0 ? true : false;
    m_Packet2.ATO_controller = (m_PacketBuffer2[19] & 0x40) > 0 ? true : false;
    m_Packet2.TCMS = (m_PacketBuffer2[19] & 0x20) > 0 ? true : false;
    m_Packet2.TACHO_2 = (m_PacketBuffer2[19] & 0x02) > 0 ? true : false;
    m_Packet2.TACHO_1 = (m_PacketBuffer2[19] & 0x01) > 0 ? true : false;
    */

    m_Packet2.RM_normal = (m_PacketBuffer2[20] & 0x40) > 0 ? true : false;
    m_Packet2.RM_recovery = (m_PacketBuffer2[20] & 0x80) > 0 ? true : false;

    m_Packet2.PWM = m_PacketBuffer2[21];

    m_Packet2.TC_DR = (m_PacketBuffer2[22] & 0x80) > 0 ? true : false;
    m_Packet2.TC_BR = (m_PacketBuffer2[22] & 0x40) > 0 ? true : false;
    m_Packet2.TC_CS = (m_PacketBuffer2[22] & 0x20) > 0 ? true : false;
    m_Packet2.DC_ADOL = (m_PacketBuffer2[22] & 0x10) > 0 ? true : false;
    m_Packet2.DC_ADOR = (m_PacketBuffer2[22] & 0x08) > 0 ? true : false;
    m_Packet2.DC_ADC = (m_PacketBuffer2[22] & 0x04) > 0 ? true : false;

    m_Packet2.cab_no_TC1 = (m_PacketBuffer2[23] & 0x80) > 0 ? true : false;
    m_Packet2.cab_no_TC2 = (m_PacketBuffer2[23] & 0x40) > 0 ? true : false;
    m_Packet2.DOLP_L = (m_PacketBuffer2[23] & 0x10) > 0 ? true : false;
    m_Packet2.DOLP_R = (m_PacketBuffer2[23] & 0x08) > 0 ? true : false;
    m_Packet2.DM_AA = (m_PacketBuffer2[23] & 0x04) > 0 ? true : false;
    m_Packet2.DM_AM = (m_PacketBuffer2[23] & 0x02) > 0 ? true : false;
    m_Packet2.DM_MM = (m_PacketBuffer2[23] & 0x01) > 0 ? true : false;

    // Train Number
    //
    //m_Packet2.formation_no = m_PacketBuffer1[27];
    m_Packet2.train_no = ((m_PacketBuffer2[25] & 0xF0) >> 4) * 1000;
    m_Packet2.train_no += (m_PacketBuffer2[25] & 0x0F) * 100;
    m_Packet2.train_no += ((m_PacketBuffer2[26] & 0xF0) >> 4) * 10;
    m_Packet2.train_no += (m_PacketBuffer2[26] & 0x0F);

    m_Packet2.atc_Speed_Code = (m_PacketBuffer2[34] & 0xF0);
    m_Packet2.atc_Speed_Code >>= 4;

    // Error Message ID
    //
    m_Packet2.MessageNo = m_PacketBuffer2[39];

    m_Packet2.MajorVersion = ((m_PacketBuffer2[55] & 0xF0) >> 4);
    m_Packet2.MinorVersion = (m_PacketBuffer2[55] & 0x0F);

    // Diagnostic Error
    //
    m_Packet2.data_40 = m_PacketBuffer2[40];
    m_Packet2.data_41 = m_PacketBuffer2[41];
    m_Packet2.data_42 = m_PacketBuffer2[42];
    /*
    m_Packet2.DE_ATIV = (m_PacketBuffer2[40] & 0x80) > 0 ? true : false;
    m_Packet2.DE_VOTING = (m_PacketBuffer2[40] & 0x40) > 0 ? true : false;
    m_Packet2.DE_RUNRTDGET = (m_PacketBuffer2[40] & 0x20) > 0 ? true : false;
    m_Packet2.DE_ATIVGET = (m_PacketBuffer2[40] & 0x10) > 0 ? true : false;
    m_Packet2.DE_DGS = (m_PacketBuffer2[40] & 0x02) > 0 ? true : false;

    m_Packet2.DE_MDHEAL = (m_PacketBuffer2[41] & 0x80) > 0 ? true : false;
    m_Packet2.DE_MNUALBYSET = (m_PacketBuffer2[41] & 0x40) > 0 ? true : false;
    m_Packet2.DE_INPUT = (m_PacketBuffer2[41] & 0x20) > 0 ? true : false;
    m_Packet2.DE_UNHEAL = (m_PacketBuffer2[41] & 0x10) > 0 ? true : false;
    m_Packet2.DE_EOT = (m_PacketBuffer2[41] & 0x08) > 0 ? true : false;
    m_Packet2.DE_EOTVAL = (m_PacketBuffer2[41] & 0x04) > 0 ? true : false;
    m_Packet2.DE_RUNRTD = (m_PacketBuffer2[41] & 0x02) > 0 ? true : false;
    m_Packet2.DE_RUNRTDSET = (m_PacketBuffer2[41] & 0x01) > 0 ? true : false;

    m_Packet2.DE_EBDATA = (m_PacketBuffer2[42] & 0x20) > 0 ? true : false;
    m_Packet2.DE_VDOTEMP = (m_PacketBuffer2[42] & 0x10) > 0 ? true : false;
    m_Packet2.DE_VDITEMP = (m_PacketBuffer2[42] & 0x08) > 0 ? true : false;
    m_Packet2.DE_OUTGOING = (m_PacketBuffer2[42] & 0x04) > 0 ? true : false;
    m_Packet2.DE_OUTPUTFB = (m_PacketBuffer2[42] & 0x02) > 0 ? true : false;
    m_Packet2.DE_MNUALBYCLR = (m_PacketBuffer2[42] & 0x01) > 0 ? true : false;
    */

    // ATC Error
    //
    m_Packet2.data_44 = m_PacketBuffer2[44];
    m_Packet2.data_45 = m_PacketBuffer2[45];
    m_Packet2.data_46 = m_PacketBuffer2[46];
    m_Packet2.data_47 = m_PacketBuffer2[47];
    /*
    m_Packet2.ATC_MSCSTATE = (m_PacketBuffer2[44] & 0x80) > 0 ? true : false;
    m_Packet2.ATC_DOORSTAT = (m_PacketBuffer2[44] & 0x40) > 0 ? true : false;
    m_Packet2.ATC_DRIVEMODE = (m_PacketBuffer2[44] & 0x20) > 0 ? true : false;
    m_Packet2.ATC_CABMOD = (m_PacketBuffer2[44] & 0x10) > 0 ? true : false;
    m_Packet2.ATC_ATC = (m_PacketBuffer2[44] & 0x04) > 0 ? true : false;

    m_Packet2.ATC_MAXSPEED = (m_PacketBuffer2[45] & 0x80) > 0 ? true : false;
    m_Packet2.ATC_WHEELDIA = (m_PacketBuffer2[45] & 0x40) > 0 ? true : false;
    m_Packet2.ATC_MANUALPDT = (m_PacketBuffer2[45] & 0x20) > 0 ? true : false;
    m_Packet2.ATC_INITPDT = (m_PacketBuffer2[45] & 0x10) > 0 ? true : false;
    m_Packet2.ATC_DIRSET = (m_PacketBuffer2[45] & 0x08) > 0 ? true : false;
    m_Packet2.ATC_TCSEL = (m_PacketBuffer2[45] & 0x04) > 0 ? true : false;
    m_Packet2.ATC_PSDSTATE = (m_PacketBuffer2[45] & 0x02) > 0 ? true : false;
    m_Packet2.ATC_REVERSING = (m_PacketBuffer2[45] & 0x01) > 0 ? true : false;

    m_Packet2.ATC_TRBSMODE = (m_PacketBuffer2[46] & 0x80) > 0 ? true : false;
    m_Packet2.ATC_TDCSMOD = (m_PacketBuffer2[46] & 0x40) > 0 ? true : false;
    m_Packet2.ATC_TZSSMODE = (m_PacketBuffer2[46] & 0x20) > 0 ? true : false;
    m_Packet2.ATC_TRSSMODE = (m_PacketBuffer2[46] & 0x10) > 0 ? true : false;
    m_Packet2.ATC_RESTRICTIO = (m_PacketBuffer2[46] & 0x08) > 0 ? true : false;
    m_Packet2.ATC_ATCCODE = (m_PacketBuffer2[46] & 0x04) > 0 ? true : false;
    m_Packet2.ATC_MAXPULSE = (m_PacketBuffer2[46] & 0x02) > 0 ? true : false;
    m_Packet2.ATC_DIFFSPEED = (m_PacketBuffer2[46] & 0x01) > 0 ? true : false;

    m_Packet2.ATC_ATCMODE = (m_PacketBuffer2[47] & 0x02) > 0 ? true : false;
    m_Packet2.ATC_DIRECT = (m_PacketBuffer2[47] & 0x01) > 0 ? true : false;
    */

    // Interface Error
    //
    m_Packet2.data_48 = m_PacketBuffer2[48];
    m_Packet2.data_49 = m_PacketBuffer2[49];
    m_Packet2.data_50 = m_PacketBuffer2[50];
    m_Packet2.data_51 = m_PacketBuffer2[51];
    /*
    m_Packet2.IF_IFS = (m_PacketBuffer2[48] & 0x01) > 0 ? true : false;

    m_Packet2.IF_ATCRINIT = (m_PacketBuffer2[49] & 0x80) > 0 ? true : false;
    m_Packet2.IF_ATCRW = (m_PacketBuffer2[49] & 0x40) > 0 ? true : false;
    m_Packet2.IF_ATCRR = (m_PacketBuffer2[49] & 0x20) > 0 ? true : false;
    m_Packet2.IF_CANINIT = (m_PacketBuffer2[49] & 0x10) > 0 ? true : false;
    m_Packet2.IF_TCMSINIT = (m_PacketBuffer2[49] & 0x08) > 0 ? true : false;
    m_Packet2.IF_TCMSW = (m_PacketBuffer2[49] & 0x04) > 0 ? true : false;
    m_Packet2.IF_TCMSR = (m_PacketBuffer2[49] & 0x02) > 0 ? true : false;

    m_Packet2.IF_VDIINIT = (m_PacketBuffer2[50] & 0x80) > 0 ? true : false;
    m_Packet2.IF_VDIW = (m_PacketBuffer2[50] & 0x40) > 0 ? true : false;
    m_Packet2.IF_VDIR = (m_PacketBuffer2[50] & 0x20) > 0 ? true : false;
    m_Packet2.IF_DIOINIT = (m_PacketBuffer2[50] & 0x10) > 0 ? true : false;
    m_Packet2.IF_ATOCINIT = (m_PacketBuffer2[50] & 0x08) > 0 ? true : false;
    m_Packet2.IF_ATOCW = (m_PacketBuffer2[50] & 0x04) > 0 ? true : false;
    m_Packet2.IF_ATOCR = (m_PacketBuffer2[50] & 0x02) > 0 ? true : false;

    m_Packet2.IF_VDOINIT = (m_PacketBuffer2[51] & 0x08) > 0 ? true : false;
    m_Packet2.IF_VDOW = (m_PacketBuffer2[51] & 0x04) > 0 ? true : false;
    m_Packet2.IF_VDOR = (m_PacketBuffer2[51] & 0x02) > 0 ? true : false;
    */

    // ATO Error
    //
    m_Packet2.data_52 = m_PacketBuffer2[52];
    m_Packet2.data_53 = m_PacketBuffer2[53];
    /*
    m_Packet2.ATO_RLYFB = (m_PacketBuffer2[52] & 0x20) > 0 ? true : false;
    m_Packet2.ATO_DOORCI = (m_PacketBuffer2[52] & 0x10) > 0 ? true : false;
    m_Packet2.ATO_DOOROP = (m_PacketBuffer2[52] & 0x08) > 0 ? true : false;
    m_Packet2.ATO_STNCODE = (m_PacketBuffer2[52] & 0x04) > 0 ? true : false;
    m_Packet2.ATO_ATOANT = (m_PacketBuffer2[52] & 0x02) > 0 ? true : false;
    m_Packet2.ATO_ATOSTOP = (m_PacketBuffer2[52] & 0x01) > 0 ? true : false;

    m_Packet2.ATO_PG32 = (m_PacketBuffer2[53] & 0x04) > 0 ? true : false;
    m_Packet2.ATO_PG2 = (m_PacketBuffer2[53] & 0x02) > 0 ? true : false;
    m_Packet2.ATO_PG1 = (m_PacketBuffer2[53] & 0x01) > 0 ? true : false;
    */
    // 차륜경 확인 팝업
    //
    if( m_PrevPacket2.Wheel_DIA_Check_Request == false && m_Packet2.Wheel_DIA_Check_Request == true )
        m_bWheelCheckPopup2 = true;
    if( m_PrevPacket2.Wheel_DIA_Check_Request == true && m_Packet2.Wheel_DIA_Check_Request == false )
        m_bWheelCheckPopup2 = false;

    // 메시지 처리
    //
    if( (m_PrevPacket2.MessageNo == 0 && m_Packet2.MessageNo != 0) ||
        m_PrevPacket2.MessageNo != m_Packet2.MessageNo )
    {
        m_SendPacket2.MessageNo = m_Packet2.MessageNo;
        AddMessage(2, m_Packet2.MessageNo);
    }
    /*
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_OVERSPEED && m_Packet2.MessageNo == MSG_ID_ATC_OVERSPEED )
        AddMessage(2, MSG_ATC_OVERSPEED_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_EB && m_Packet2.MessageNo == MSG_ID_ATC_EB )
        AddMessage(2, MSG_ATC_EB_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_INIT_OK && m_Packet2.MessageNo == MSG_ID_ATC_INIT_OK )
        AddMessage(2, MSG_ATC_INIT_OK_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_MANUAL_START && m_Packet2.MessageNo == MSG_ID_ATC_MANUAL_START )
        AddMessage(2, MSG_ATC_MANUAL_START_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_MANUAL_OK && m_Packet2.MessageNo == MSG_ID_ATC_MANUAL_OK )
        AddMessage(2, MSG_ATC_MANUAL_OK_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_NOCODE && m_Packet2.MessageNo == MSG_ID_ATC_NOCODE )
        AddMessage(2, MSG_ATC_NOCODE_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATC_PSDSTATE && m_Packet2.MessageNo == MSG_ID_ATC_PSDSTATE )
        AddMessage(2, MSG_ATC_PSDSTATE_2);

    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_INCHING && m_Packet2.MessageNo == MSG_ID_ATO_INCHING )
        AddMessage(2, MSG_ATO_INCHING_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_TASC && m_Packet2.MessageNo == MSG_ID_ATO_TASC )
        AddMessage(2, MSG_ATO_TASC_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_BERTH && m_Packet2.MessageNo == MSG_ID_ATO_BERTH )
        AddMessage(2, MSG_ATO_BERTH_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_PG1 && m_Packet2.MessageNo == MSG_ID_ATO_PG1 )
        AddMessage(2, MSG_ATO_PG1_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_PG2 && m_Packet2.MessageNo == MSG_ID_ATO_PG2 )
        AddMessage(2, MSG_ATO_PG2_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_PG32 && m_Packet2.MessageNo == MSG_ID_ATO_PG32 )
        AddMessage(2, MSG_ATO_PG32_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_ATS && m_Packet2.MessageNo == MSG_ID_ATO_ATS )
        AddMessage(2, MSG_ATO_ATS_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_DOOR && m_Packet2.MessageNo == MSG_ID_ATO_DOOR )
        AddMessage(2, MSG_ATO_DOOR_2);
    if( m_PrevPacket2.MessageNo != MSG_ID_ATO_STATION && m_Packet2.MessageNo == MSG_ID_ATO_STATION )
        AddMessage(2, MSG_ATO_STATION_2);
    */

    /* version 1.14.3 - 중복메시지 제거
    if( m_PrevPacket2.manual_PDT_NG == false && m_Packet2.manual_PDT_NG == true )
        AddMessage(2, ERROR_MANUAL_PDT_2);
    if( m_PrevPacket2.initial_PDT_NG == false && m_Packet2.initial_PDT_NG == true )
        AddMessage(2, ERROR_INITIAL_PDT_2);
    */

    int i = 0;
    unsigned char cShift = 0x01;
    unsigned char cPrev = 0x00;
    unsigned char cCurr = 0x00;
    if( m_PrevPacket2.data_19 != m_Packet2.data_19)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_19 & cShift;
            cCurr = m_Packet2.data_19 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 190 + i);
        }
    }

    // Diagnostic Error
    //
    if( m_PrevPacket2.data_40 != m_Packet2.data_40)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_40 & cShift;
            cCurr = m_Packet2.data_40 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 400 + i);
        }
    }
    if( m_PrevPacket2.data_41 != m_Packet2.data_41)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_41 & cShift;
            cCurr = m_Packet2.data_41 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 410 + i);
        }
    }
    if( m_PrevPacket2.data_42 != m_Packet2.data_42)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_42 & cShift;
            cCurr = m_Packet2.data_42 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 420 + i);
        }
    }

    // ATC Error
    //
    if( m_PrevPacket2.data_44 != m_Packet2.data_44)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_44 & cShift;
            cCurr = m_Packet2.data_44 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 440 + i);
        }
    }
    if( m_PrevPacket2.data_45 != m_Packet2.data_45)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_45 & cShift;
            cCurr = m_Packet2.data_45 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 450 + i);
        }
    }
    if( m_PrevPacket2.data_46 != m_Packet2.data_46)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_46 & cShift;
            cCurr = m_Packet2.data_46 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 460 + i);
        }
    }
    if( m_PrevPacket2.data_47 != m_Packet2.data_47)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_47 & cShift;
            cCurr = m_Packet2.data_47 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 470 + i);
        }
    }

    // Interface Error
    //
    if( m_PrevPacket2.data_48 != m_Packet2.data_48)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_48 & cShift;
            cCurr = m_Packet2.data_48 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 480 + i);
        }
    }
    if( m_PrevPacket2.data_49 != m_Packet2.data_49)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_49 & cShift;
            cCurr = m_Packet2.data_49 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 490 + i);
        }
    }
    if( m_PrevPacket2.data_50 != m_Packet2.data_50)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_50 & cShift;
            cCurr = m_Packet2.data_50 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 500 + i);
        }
    }
    if( m_PrevPacket2.data_51 != m_Packet2.data_51)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_51 & cShift;
            cCurr = m_Packet2.data_51 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 510 + i);
        }
    }

    // ATO Error
    //
    if( m_PrevPacket2.data_52 != m_Packet2.data_52)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_52 & cShift;
            cCurr = m_Packet2.data_52 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 520 + i);
        }
    }
    if( m_PrevPacket2.data_53 != m_Packet2.data_53)
    {
        cShift = 0x01;
        for(i = 0; i < 8; ++i)
        {
            cPrev = m_PrevPacket2.data_53 & cShift;
            cCurr = m_Packet2.data_53 & cShift;
            cShift <<= 1;
            if( cPrev == 0 && cCurr != 0 )
                AddMessage(2, 530 + i);
        }
    }

    /*
    if( m_PrevPacket2.ATC_receiver == false && m_Packet2.ATC_receiver == true )
        AddMessage(2, ERROR_COMM_ATC_2);
    if( m_PrevPacket2.ATO_controller == false && m_Packet2.ATO_controller == true )
        AddMessage(2, ERROR_COMM_ATO_2);
    if( m_PrevPacket2.TCMS == false && m_Packet2.TCMS == true )
        AddMessage(2, ERROR_COMM_TCMS_2);
    if( m_PrevPacket2.TACHO_2 == false && m_Packet2.TACHO_2 == true )
        AddMessage(2, ERROR_COMM_TACHO2_2);
    if( m_PrevPacket2.TACHO_1 == false && m_Packet2.TACHO_1 == true )
        AddMessage(2, ERROR_COMM_TACHO1_2);

    if( m_PrevPacket2.ATC_MSCSTATE == false && m_Packet2.ATC_MSCSTATE == true )
        AddMessage(2, ERROR_ATC_MSCSTATE_2);
    if( m_PrevPacket2.ATC_DOORSTAT == false && m_Packet2.ATC_DOORSTAT == true )
        AddMessage(2, ERROR_ATC_DOORSTAT_2);
    if( m_PrevPacket2.ATC_DRIVEMODE == false && m_Packet2.ATC_DRIVEMODE == true )
        AddMessage(2, ERROR_ATC_DRIVEMODE_2);
    if( m_PrevPacket2.ATC_CABMOD == false && m_Packet2.ATC_CABMOD == true )
        AddMessage(2, ERROR_ATC_CABMOD_2);

    if( m_PrevPacket2.ATC_WHEELDIA == false && m_Packet2.ATC_WHEELDIA == true )
        AddMessage(2, ERROR_ATC_WHEELDIA_2);
    if( m_PrevPacket2.ATC_MANUALPDT == false && m_Packet2.ATC_MANUALPDT == true )
        AddMessage(2, ERROR_ATC_MANUALPDT_2);
    if( m_PrevPacket2.ATC_INITPDT == false && m_Packet2.ATC_INITPDT == true )
        AddMessage(2, ERROR_ATC_INITPDT_2);
    if( m_PrevPacket2.ATC_DIRSET == false && m_Packet2.ATC_DIRSET == true )
        AddMessage(2, ERROR_ATC_DIRSET_2);
    if( m_PrevPacket2.ATC_TCSEL == false && m_Packet2.ATC_TCSEL == true )
        AddMessage(2, ERROR_ATC_TCSEL_2);
    if( m_PrevPacket2.ATC_PSDSTATE == false && m_Packet2.ATC_PSDSTATE == true )
        AddMessage(2, ERROR_ATC_PSDSTATE_2);
    if( m_PrevPacket2.ATC_REVERSING == false && m_Packet2.ATC_REVERSING == true )
        AddMessage(2, ERROR_ATC_REVERSING_2);

    if( m_PrevPacket2.ATO_ATOSTOP == false && m_Packet2.ATO_ATOSTOP == true )
        AddMessage(2, ERROR_ATO_ATOSTOP_2);
    if( m_PrevPacket2.ATO_ATOANT == false && m_Packet2.ATO_ATOANT == true )
        AddMessage(2, ERROR_ATO_ATOANT_2);
    if( m_PrevPacket2.ATO_DOOROP == false && m_Packet2.ATO_DOOROP == true )
        AddMessage(2, ERROR_ATO_DOOROP_2);
    if( m_PrevPacket2.ATO_DOORCI == false && m_Packet2.ATO_DOORCI == true )
        AddMessage(2, ERROR_ATO_DOORCI_2);

    if( m_PrevPacket2.ATO_PG1 == false && m_Packet2.ATO_PG1 == true )
        AddMessage(2, ERROR_ATO_PG1_2);
    if( m_PrevPacket2.ATO_PG2 == false && m_Packet2.ATO_PG2 == true )
        AddMessage(2, ERROR_ATO_PG2_2);
    if( m_PrevPacket2.ATO_PG32 == false && m_Packet2.ATO_PG32 == true )
        AddMessage(2, ERROR_ATO_PG32_2);
    */

    memcpy(&m_PrevPacket2, &m_Packet2, sizeof(Packet));
}


//#############################################################################
//
//  SendPacketToDevice1
//      - MMI send to DOSI_BTC #1
//
void MainWindow::SendPacketToDevice1()
{
    if( m_SendPacket1.manual_PDT_request == true )
    {
        QDateTime currTime = QDateTime::currentDateTime();
        qint64 spanSec = m_sendPDTTime1.secsTo(currTime);
        if( m_Packet1.manual_PDT_START == true || spanSec > 1 )
            m_SendPacket1.manual_PDT_request = false;
    }

    if( m_SendPacket1.MessageNo != 0 )
    {
        if( m_Packet1.MessageNo == 0 )
            m_SendPacket1.MessageNo = 0;
    }

    m_SendPacketBuffer1[0] = 0x02;
    m_SendPacketBuffer1[1] = static_cast<unsigned char>(m_nSendSequence1);
    ++m_nSendSequence1;
    if( m_nSendSequence1 > 255 )
        m_nSendSequence1 = 0;
    m_SendPacketBuffer1[2] = static_cast<unsigned char>(m_Packet1.source);
    m_SendPacketBuffer1[3] = 0x15;
    m_SendPacketBuffer1[4] = static_cast<unsigned char>(m_SendPacket1.DIA1);
    m_SendPacketBuffer1[5] = static_cast<unsigned char>(m_SendPacket1.DIA2);
    m_SendPacketBuffer1[6] = 0x00;
    if( m_SendPacket1.manual_PDT_request == true )
        m_SendPacketBuffer1[6] |= 0x80;
    if( m_SendPacket1.confirm_DIA == true )
        m_SendPacketBuffer1[6] |= 0x10;
    if( m_SendPacket1.RM_ON == true )
        m_SendPacketBuffer1[6] |= 0x02;
    if( m_SendPacket1.RM_OFF == true )
        m_SendPacketBuffer1[6] |= 0x01;
    m_SendPacketBuffer1[7] = static_cast<unsigned char>(m_nGroupNo);
    m_SendPacketBuffer1[12] = static_cast<unsigned char>(m_SendPacket1.MessageNo);
    m_SendPacketBuffer1[79] = 0x03;

    uint32_t crc32 = CalcCRC32(&m_SendPacketBuffer1[1], PACKET_SIZE + 1);
    m_SendPacketBuffer1[80] = static_cast<unsigned char>((crc32 & 0xFF000000) >> 24);
    m_SendPacketBuffer1[81] = static_cast<unsigned char>((crc32 & 0x00FF0000) >> 16);
    m_SendPacketBuffer1[82] = static_cast<unsigned char>((crc32 & 0x0000FF00) >> 8);
    m_SendPacketBuffer1[83] = static_cast<unsigned char>(crc32 & 0x000000FF);

    char buffer[100];
    memcpy(buffer, m_SendPacketBuffer1, PACKET_SIZE + 6);
    AddTxDataToBufferList(buffer);
    m_serial1->write(buffer, PACKET_SIZE + 6);
}


//#############################################################################
//
//  SendPacketToDevice2
//      - MMI send to DOSI_BTC #2
//
void MainWindow::SendPacketToDevice2()
{
    if( m_SendPacket2.manual_PDT_request == true )
    {
        QDateTime currTime = QDateTime::currentDateTime();
        qint64 spanSec = m_sendPDTTime2.secsTo(currTime);
        if( m_Packet2.manual_PDT_START == true || spanSec > 1)
            m_SendPacket2.manual_PDT_request = false;
    }

    if( m_SendPacket2.MessageNo != 0 )
    {
        if( m_Packet2.MessageNo == 0 )
            m_SendPacket2.MessageNo = 0;
    }

    m_SendPacketBuffer2[0] = 0x02;
    m_SendPacketBuffer2[1] = static_cast<unsigned char>(m_nSendSequence2);
    ++m_nSendSequence2;
    if( m_nSendSequence2 > 255 )
        m_nSendSequence2 = 0;
    m_SendPacketBuffer2[2] = static_cast<unsigned char>(m_Packet2.source);
    m_SendPacketBuffer2[3] = 0x15;
    m_SendPacketBuffer2[4] = static_cast<unsigned char>(m_SendPacket2.DIA1);
    m_SendPacketBuffer2[5] = static_cast<unsigned char>(m_SendPacket2.DIA2);
    m_SendPacketBuffer2[6] = 0x00;
    if( m_SendPacket2.manual_PDT_request == true )
        m_SendPacketBuffer2[6] |= 0x80;
    if( m_SendPacket2.confirm_DIA == true )
        m_SendPacketBuffer2[6] |= 0x10;
    if( m_SendPacket2.RM_ON == true )
        m_SendPacketBuffer2[6] |= 0x02;
    if( m_SendPacket2.RM_OFF == true )
        m_SendPacketBuffer2[6] |= 0x01;
    m_SendPacketBuffer2[7] = static_cast<unsigned char>(m_nGroupNo);
    m_SendPacketBuffer2[12] = static_cast<unsigned char>(m_SendPacket2.MessageNo);
    m_SendPacketBuffer2[79] = 0x03;

    uint32_t crc32 = CalcCRC32(&m_SendPacketBuffer2[1], PACKET_SIZE + 1);
    m_SendPacketBuffer2[80] = static_cast<unsigned char>((crc32 & 0xFF000000) >> 24);
    m_SendPacketBuffer2[81] = static_cast<unsigned char>((crc32 & 0x00FF0000) >> 16);
    m_SendPacketBuffer2[82] = static_cast<unsigned char>((crc32 & 0x0000FF00) >> 8);
    m_SendPacketBuffer2[83] = static_cast<unsigned char>(crc32 & 0x000000FF);

    char buffer[100];
    memcpy(buffer, m_SendPacketBuffer2, PACKET_SIZE + 6);
    AddTxDataToBufferList(buffer);
    m_serial2->write(buffer, PACKET_SIZE + 6);
}


//#############################################################################
//
//  Calculate CRC32
//
uint32_t MainWindow::CalcCRC32(const unsigned char* s, size_t n)
{
    unsigned char value;
    uint32_t remainder = 0xFFFFFFFFU;
    uint32_t CRC32_Table[] =
    {
        0x00000000U, 0x04C11DB7U, 0x09823B6EU, 0x0D4326D9U, 0x130476DCU, 0x17C56B6BU, 0x1A864DB2U, 0x1E475005U,
        0x2608EDB8U, 0x22C9F00FU, 0x2F8AD6D6U, 0x2B4BCB61U, 0x350C9B64U, 0x31CD86D3U, 0x3C8EA00AU, 0x384FBDBDU,
        0x4C11DB70U, 0x48D0C6C7U, 0x4593E01EU, 0x4152FDA9U, 0x5F15ADACU, 0x5BD4B01BU, 0x569796C2U, 0x52568B75U,
        0x6A1936C8U, 0x6ED82B7FU, 0x639B0DA6U, 0x675A1011U, 0x791D4014U, 0x7DDC5DA3U, 0x709F7B7AU, 0x745E66CDU,
        0x9823B6E0U, 0x9CE2AB57U, 0x91A18D8EU, 0x95609039U, 0x8B27C03CU, 0x8FE6DD8BU, 0x82A5FB52U, 0x8664E6E5U,
        0xBE2B5B58U, 0xBAEA46EFU, 0xB7A96036U, 0xB3687D81U, 0xAD2F2D84U, 0xA9EE3033U, 0xA4AD16EAU, 0xA06C0B5DU,
        0xD4326D90U, 0xD0F37027U, 0xDDB056FEU, 0xD9714B49U, 0xC7361B4CU, 0xC3F706FBU, 0xCEB42022U, 0xCA753D95U,
        0xF23A8028U, 0xF6FB9D9FU, 0xFBB8BB46U, 0xFF79A6F1U, 0xE13EF6F4U, 0xE5FFEB43U, 0xE8BCCD9AU, 0xEC7DD02DU,
        0x34867077U, 0x30476DC0U, 0x3D044B19U, 0x39C556AEU, 0x278206ABU, 0x23431B1CU, 0x2E003DC5U, 0x2AC12072U,
        0x128E9DCFU, 0x164F8078U, 0x1B0CA6A1U, 0x1FCDBB16U, 0x018AEB13U, 0x054BF6A4U, 0x0808D07DU, 0x0CC9CDCAU,
        0x7897AB07U, 0x7C56B6B0U, 0x71159069U, 0x75D48DDEU, 0x6B93DDDBU, 0x6F52C06CU, 0x6211E6B5U, 0x66D0FB02U,
        0x5E9F46BFU, 0x5A5E5B08U, 0x571D7DD1U, 0x53DC6066U, 0x4D9B3063U, 0x495A2DD4U, 0x44190B0DU, 0x40D816BAU,
        0xACA5C697U, 0xA864DB20U, 0xA527FDF9U, 0xA1E6E04EU, 0xBFA1B04BU, 0xBB60ADFCU, 0xB6238B25U, 0xB2E29692U,
        0x8AAD2B2FU, 0x8E6C3698U, 0x832F1041U, 0x87EE0DF6U, 0x99A95DF3U, 0x9D684044U, 0x902B669DU, 0x94EA7B2AU,
        0xE0B41DE7U, 0xE4750050U, 0xE9362689U, 0xEDF73B3EU, 0xF3B06B3BU, 0xF771768CU, 0xFA325055U, 0xFEF34DE2U,
        0xC6BCF05FU, 0xC27DEDE8U, 0xCF3ECB31U, 0xCBFFD686U, 0xD5B88683U, 0xD1799B34U, 0xDC3ABDEDU, 0xD8FBA05AU,
        0x690CE0EEU, 0x6DCDFD59U, 0x608EDB80U, 0x644FC637U, 0x7A089632U, 0x7EC98B85U, 0x738AAD5CU, 0x774BB0EBU,
        0x4F040D56U, 0x4BC510E1U, 0x46863638U, 0x42472B8FU, 0x5C007B8AU, 0x58C1663DU, 0x558240E4U, 0x51435D53U,
        0x251D3B9EU, 0x21DC2629U, 0x2C9F00F0U, 0x285E1D47U, 0x36194D42U, 0x32D850F5U, 0x3F9B762CU, 0x3B5A6B9BU,
        0x0315D626U, 0x07D4CB91U, 0x0A97ED48U, 0x0E56F0FFU, 0x1011A0FAU, 0x14D0BD4DU, 0x19939B94U, 0x1D528623U,
        0xF12F560EU, 0xF5EE4BB9U, 0xF8AD6D60U, 0xFC6C70D7U, 0xE22B20D2U, 0xE6EA3D65U, 0xEBA91BBCU, 0xEF68060BU,
        0xD727BBB6U, 0xD3E6A601U, 0xDEA580D8U, 0xDA649D6FU, 0xC423CD6AU, 0xC0E2D0DDU, 0xCDA1F604U, 0xC960EBB3U,
        0xBD3E8D7EU, 0xB9FF90C9U, 0xB4BCB610U, 0xB07DABA7U, 0xAE3AFBA2U, 0xAAFBE615U, 0xA7B8C0CCU, 0xA379DD7BU,
        0x9B3660C6U, 0x9FF77D71U, 0x92B45BA8U, 0x9675461FU, 0x8832161AU, 0x8CF30BADU, 0x81B02D74U, 0x857130C3U,
        0x5D8A9099U, 0x594B8D2EU, 0x5408ABF7U, 0x50C9B640U, 0x4E8EE645U, 0x4A4FFBF2U, 0x470CDD2BU, 0x43CDC09CU,
        0x7B827D21U, 0x7F436096U, 0x7200464FU, 0x76C15BF8U, 0x68860BFDU, 0x6C47164AU, 0x61043093U, 0x65C52D24U,
        0x119B4BE9U, 0x155A565EU, 0x18197087U, 0x1CD86D30U, 0x029F3D35U, 0x065E2082U, 0x0B1D065BU, 0x0FDC1BECU,
        0x3793A651U, 0x3352BBE6U, 0x3E119D3FU, 0x3AD08088U, 0x2497D08DU, 0x2056CD3AU, 0x2D15EBE3U, 0x29D4F654U,
        0xC5A92679U, 0xC1683BCEU, 0xCC2B1D17U, 0xC8EA00A0U, 0xD6AD50A5U, 0xD26C4D12U, 0xDF2F6BCBU, 0xDBEE767CU,
        0xE3A1CBC1U, 0xE760D676U, 0xEA23F0AFU, 0xEEE2ED18U, 0xF0A5BD1DU, 0xF464A0AAU, 0xF9278673U, 0xFDE69BC4U,
        0x89B8FD09U, 0x8D79E0BEU, 0x803AC667U, 0x84FBDBD0U, 0x9ABC8BD5U, 0x9E7D9662U, 0x933EB0BBU, 0x97FFAD0CU,
        0xAFB010B1U, 0xAB710D06U, 0xA6322BDFU, 0xA2F33668U, 0xBCB4666DU, 0xB8757BDAU, 0xB5365D03U, 0xB1F740B4U
    };

    /* Divide the data frame by the polynomial. a byte at time */
    //for (offset = startByte; offset < nBytes; offset++)
    for(size_t i = 0; i < n; ++i)
    {
        /* XOR-in next input byte into MSB of crc and get this MSB, that's our new intermediate divident */
        value = static_cast<unsigned char>((remainder ^ (static_cast<uint32_t>(s[i]) << 24)) >> 24);
        remainder = (remainder << 8) ^ CRC32_Table[value];
    }

    /* The final reminder is the CRC result */
    return (remainder);
}



///#########################################################################################
///
/// <summary>
///
/// </summary>
///
bool MainWindow::Load_Message()
{
    QDomDocument* xmlDoc = new QDomDocument();

    QFile file(QApplication::applicationDirPath() + "/Message.xml");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qDebug() << QApplication::applicationDirPath() + "/Message.xml not found";
        return false;
    }
    if(!xmlDoc->setContent(&file))
    {
        qDebug() << "Message.xml read failed";
        return false;
    }
    file.close();

    QDomNodeList nodeList = xmlDoc->elementsByTagName("Message");
    if (nodeList.count() == 0)
        return false;

    QString strNo = "";
    QString strMsgText = "";
    QString strMsgColor = "";
    int nNo = 0;
    for (int i = 0; i < nodeList.count(); ++i)
    {
        QDomNode node = nodeList.at(i);

        strNo = "";
        strMsgText = "";
        strMsgColor = "";

        QDomElement e = node.firstChildElement("NO");
        if( e.tagName() == "NO" )
            strNo = e.text();
        e = node.firstChildElement("MSG_TEXT");
        if( e.tagName() == "MSG_TEXT" )
            strMsgText = e.text();
        e = node.firstChildElement("MSG_COLOR");
        if( e.tagName() == "MSG_COLOR" )
            strMsgColor = e.text();

        if (strNo != "" && strMsgText != "" && strMsgColor != "")
        {
            nNo = strNo.toInt();
            m_dbMessageNoInt.append(nNo);
            m_dbMessageNo.append(strNo);
            m_dbMessageText.append(strMsgText);
            m_dbMessageColor.append(strMsgColor);
        }
    }

    char szMsg[100];
    sprintf(szMsg, "%d message item load!", m_dbMessageNo.count());
    qDebug() << szMsg;

    return true;
}


///#########################################################################################
///
/// <summary>
///     메시지 정보 리스트에서 ID로 검색
/// </summary>
/// <param name="nID">Message ID number</param>
/// <returns>메시지 정보가 들어가 있는 리스트 인덱스. 없으면 -1 리턴</returns>
///
int MainWindow::FindMessage(int nID)
{
    QString strMsg = "";
    QString strID;
    int nIndex = -1;

    strID.sprintf("%d", nID);
    for (int i = 0; i < m_dbMessageNoInt.count(); ++i)
    {
        if (m_dbMessageNoInt[i] == nID)
        {
            strMsg = m_dbMessageText[i];
            nIndex = i;
            break;
        }
    }

    return nIndex;
}


//#############################################################################
//
//  AddMessage
//      - add message string to list and display
//
void MainWindow::AddMessage(int nSystem, int nID)
{
    char szMsg[200];

    int nIndex = FindMessage(nID);
    if( nIndex < 0 )
        return;

    if( nSystem == 1 )
        sprintf(szMsg, "%02d/%02d %02d:%02d:%02d 1계 ",
                m_Packet1.month, m_Packet1.day,
                m_Packet1.hour, m_Packet1.minute, m_Packet1.second);
    else
        sprintf(szMsg, "%02d/%02d %02d:%02d:%02d 2계 ",
                m_Packet2.month, m_Packet2.day,
                m_Packet2.hour, m_Packet2.minute, m_Packet2.second);
    QString strAddMsg = szMsg;
    strAddMsg += m_dbMessageText[nIndex];
    m_strMessageToWrite += strAddMsg + "\r\n";
    m_MessageList.insert(0, strAddMsg);
    m_MessageColorList.insert(0, m_dbMessageColor[nIndex]);
    m_nMessageIndex = 0;
    m_bRedrawMessage = true;

    bool bNoDate = false;
    if( m_bMsgLoggingOpen == false )
    {
        if( nSystem == 1 )
        {
            sprintf(m_szLoggingFileMsg, "%s/%04d%02d%02d_%02d%02d%02d_%02d_MSG.dat", m_szLoggingPath,
                    m_Packet1.year + 2000, m_Packet1.month, m_Packet1.day,
                    m_Packet1.hour, m_Packet1.minute, m_Packet1.second, m_nGroupNo);
            if( m_Packet1.year == 0 )
                bNoDate = true;
        }
        else
        {
            sprintf(m_szLoggingFileMsg, "%s/%04d%02d%02d_%02d%02d%02d_%02d_MSG.dat", m_szLoggingPath,
                    m_Packet2.year + 2000, m_Packet2.month, m_Packet2.day,
                    m_Packet2.hour, m_Packet2.minute, m_Packet2.second, m_nGroupNo);
            if( m_Packet2.year == 0 )
                bNoDate = true;
        }
        if( bNoDate == false )
            m_bMsgLoggingOpen = true;
    }

}


//#############################################################################
//
//  AddMessage
//      - add message string to list and display
//
void MainWindow::AddMessage(int nSystem, QString strMsg, QString strColor)
{
    char szMsg[200];

    if( nSystem == 1 )
        sprintf(szMsg, "%02d/%02d %02d:%02d:%02d 1계 ",
                m_Packet1.month, m_Packet1.day,
                m_Packet1.hour, m_Packet1.minute, m_Packet1.second);
    else
        sprintf(szMsg, "%02d/%02d %02d:%02d:%02d 2계 ",
                m_Packet2.month, m_Packet2.day,
                m_Packet2.hour, m_Packet2.minute, m_Packet2.second);
    QString strAddMsg = szMsg;
    strAddMsg += strMsg;
    m_strMessageToWrite += strAddMsg + "\r\n";
    m_MessageList.insert(0, strAddMsg);
    m_MessageColorList.insert(0, strColor);
    m_nMessageIndex = 0;
    m_bRedrawMessage = true;

    bool bNoDate = false;
    if( m_bMsgLoggingOpen == false )
    {
        if( nSystem == 1 )
        {
            sprintf(m_szLoggingFileMsg, "%s/%04d%02d%02d_%02d%02d%02d_%02d_MSG.dat", m_szLoggingPath,
                    m_Packet1.year + 2000, m_Packet1.month, m_Packet1.day,
                    m_Packet1.hour, m_Packet1.minute, m_Packet1.second, m_nGroupNo);
            if( m_Packet1.year == 0 )
                bNoDate = true;
        }
        else
        {
            sprintf(m_szLoggingFileMsg, "%s/%04d%02d%02d_%02d%02d%02d_%02d_MSG.dat", m_szLoggingPath,
                    m_Packet2.year + 2000, m_Packet2.month, m_Packet2.day,
                    m_Packet2.hour, m_Packet2.minute, m_Packet2.second, m_nGroupNo);
            if( m_Packet2.year == 0 )
                bNoDate = true;
        }
        if( bNoDate == false )
            m_bMsgLoggingOpen = true;
    }
}


//#############################################################################
//
//  DrawMessage
//      - draw system message
//
void MainWindow::DrawMessage()
{
    int nCount = 0;
    int nSize = m_MessageList.size();
    if( nSize > 0 )
    {
        for(int i = m_nMessageIndex; i < nSize; ++i )
        {
            m_LabelMsg[nCount]->setText(m_MessageList[i]);
            QString strColor = m_MessageColorList[i].toUpper();
            if( strColor == "R" || strColor == "RED" )
                m_LabelMsg[nCount]->setStyleSheet("color: #FF0000;");
            else
                m_LabelMsg[nCount]->setStyleSheet("color: #FFFFFF;");
            ++nCount;
            if( nCount >= 5 )
                break;
        }
    }
}


//#############################################################################
//
//  on_Button_Msg_Up_clicked
//
void MainWindow::on_Button_Msg_Up_clicked()
{
    if( m_bWheelCheckShow == true )
        return;

    if( m_bMsgUpBtnEnabled == true && m_Sound_Click != nullptr )
        m_Sound_Click->play();

    int nCount = m_MessageList.size();
    if( m_nMessageIndex > 0 )
    {
        --m_nMessageIndex;
        DrawMessage();
        if( m_nMessageIndex == 0 )
        {
            ui->Button_Msg_Up->setEnabled(false);
            m_bMsgUpBtnEnabled = false;
        }
        if( nCount > (m_nMessageIndex + 5) )
        {
            ui->Button_Msg_Down->setEnabled(true);
            m_bMsgDownBtnEnabled = true;
        }
        else
        {
            ui->Button_Msg_Down->setEnabled(false);
            m_bMsgDownBtnEnabled = false;
        }
    }
}


//#############################################################################
//
//  on_Button_Msg_Down_clicked
//
void MainWindow::on_Button_Msg_Down_clicked()
{
    if( m_bWheelCheckShow == true )
        return;

    if( m_bMsgDownBtnEnabled == true && m_Sound_Click != nullptr )
        m_Sound_Click->play();

    int nCount = m_MessageList.size();
    if( nCount > (m_nMessageIndex + 5) )
    {
        ++m_nMessageIndex;
        DrawMessage();
        ui->Button_Msg_Up->setEnabled(true);
        m_bMsgUpBtnEnabled = true;
        if( nCount > (m_nMessageIndex + 5) )
        {
            ui->Button_Msg_Down->setEnabled(true);
            m_bMsgDownBtnEnabled = true;
        }
        else
        {
            ui->Button_Msg_Down->setEnabled(false);
            m_bMsgDownBtnEnabled = false;
        }
    }
}


//#############################################################################
//
//  on_Button_Mode_clicked
//      - mode selection button click handler
//
void MainWindow::on_Button_Mode_clicked()
{
    if( m_nCurrentMode == SCREEN_WAIT || m_nCurrentMode == SCREEN_TCR )
        return;

    if( m_bWheelCheckShow == true || m_bShowMode == true )
        return;

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_bShowMode = true;
    DialogMode dlg(m_bRecoveryMode, m_Sound_Click, this);
    dlg.setModal(true);

    dlg.move(187, 234);
    int dlgResult = dlg.exec();
    if( dlgResult == QDialog::Accepted )
    {
        m_bRecoveryMode = !m_bRecoveryMode;

        if(m_bRecoveryMode == true)
        {
            m_SendPacket1.RM_ON = true;
            m_SendPacket2.RM_ON = true;
            m_SendPacket1.RM_OFF = false;
            m_SendPacket2.RM_OFF = false;
        }
        else
        {
            m_SendPacket1.RM_ON = false;
            m_SendPacket2.RM_ON = false;
            m_SendPacket1.RM_OFF = true;
            m_SendPacket2.RM_OFF = true;
        }
    }
    m_bShowMode = false;
}


//#############################################################################
//
//  on_Button_Config_clicked
//      - mode selection button click handler
//
void MainWindow::on_Button_Config_clicked()
{
    qDebug() << "Config clicked";
    /*
    if( m_nCurrentMode == SCREEN_TCR )
    {
        return;
    }
    */
    if( m_bWheelCheckShow == true || m_bShowMode == true )
    {
        return;
    }

    if( m_bCanConfig == false )
    {
        return;
    }

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    DialogConfig dlg(this, m_nVolume, m_nBrightness);
    dlg.setModal(true);
    dlg.SetMainWindow(this);
    dlg.SetGroupNo(m_nGroupNo);
    dlg.SetSoundClick(m_Sound_Click);
    dlg.SetSoundVolume(m_Sound_Volume);

    dlg.move(0, 0);
    dlg.exec();

    m_nVolume = dlg.GetVolume();
    m_nBrightness = dlg.GetBrightness();
}


//#############################################################################
//
//  on_Button_WC_OK_clicked
//      - 1계/2계 차륜경 확인의 '확인' 버튼 처리
//
void MainWindow::on_Button_WC_OK_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nWheelCheckShow == 1 )
    {
        m_bWheelCheckPopup1 = false;

        m_nSendConfirmCount1 = 0;
        m_SendPacket1.confirm_DIA = true;
    }
    else if( m_nWheelCheckShow == 2 )
    {
        m_bWheelCheckPopup2 = false;

        m_nSendConfirmCount2 = 0;
        m_SendPacket2.confirm_DIA = true;
    }

    ui->frame_Wheel_Confirm->setVisible(false);
    m_bWheelCheckShow = false;
    m_nWheelCheckShow = -1;

    //m_bWeelCheckUpdate = false;

    char szCmd[100];
    sprintf(szCmd, "echo %d > %s/DIA1", m_nTachoDIA1Value, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    sprintf(szCmd, "echo %d > %s/DIA2", m_nTachoDIA2Value, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");
}


//#############################################################################
//
//  on_Button_WC_Update_clicked
//      - 1계/2계 차륜경 확인의 '변경' 버튼 처리
//
void MainWindow::on_Button_WC_Update_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_bWeelCheckPassword == false )
    {
        DialogPassword dlg(m_Sound_Click, this);
        dlg.setModal(true);
        dlg.SetPassword(m_szPassword);

        dlg.move(117, 38);
        if( dlg.exec() == QDialog::Accepted )
        {
            m_bWeelCheckUpdate = true;
            m_bWeelCheckPassword = true;
            ShowTachoButton(true);
        }
        else
        {
            return;
        }
    }
    else
    {
        m_bWeelCheckUpdate = true;
        ShowTachoButton(true);
    }
}


//#############################################################################
//
//  ShowTachoButton
//      - 1계/2계 차륜경 확인의 MMI 설정 Up/Down 버튼 보이기/감추기 처리
//
void MainWindow::ShowTachoButton(bool bEnable)
{
    ui->Button_Tacho1_Down->setVisible(bEnable);
    ui->Button_Tacho1_Up->setVisible(bEnable);

    ui->Button_Tacho2_Down->setVisible(bEnable);
    ui->Button_Tacho2_Up->setVisible(bEnable);
}


//#############################################################################
//
//  on_Button_Tacho1_Down_clicked
//      - Tacho #1 차륜경값 Down 버튼 처리
//
void MainWindow::on_Button_Tacho1_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA1Value > 780 )
    {
        SetTacho1DIA(m_nTachoDIA1Value - 5);
        ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
        m_SendPacket1.DIA1 = m_nTachoDIA1Index;
        m_SendPacket2.DIA2 = m_nTachoDIA1Index;

        if( m_ActivePacket.DIA1 == m_nTachoDIA1Index && m_ActivePacket.DIA2 == m_nTachoDIA2Index )
            ui->Button_WC_OK->setVisible(true);
        else
            ui->Button_WC_OK->setVisible(false);
    }
}


//#############################################################################
//
//  on_Button_Tacho1_Up_clicked
//      - Tacho #1 차륜경값 Up 버튼 처리
//
void MainWindow::on_Button_Tacho1_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA1Value < 860 )
    {
        SetTacho1DIA(m_nTachoDIA1Value + 5);
        ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
        m_SendPacket1.DIA1 = m_nTachoDIA1Index;
        m_SendPacket2.DIA2 = m_nTachoDIA1Index;

        if( m_ActivePacket.DIA1 == m_nTachoDIA1Index && m_ActivePacket.DIA2 == m_nTachoDIA2Index )
            ui->Button_WC_OK->setVisible(true);
        else
            ui->Button_WC_OK->setVisible(false);
    }
}


//#############################################################################
//
//  on_Button_Tacho2_Down_clicked
//      - Tacho #2 차륜경값 Down 버튼 처리
//
void MainWindow::on_Button_Tacho2_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA2Value > 780 )
    {
        SetTacho2DIA(m_nTachoDIA2Value - 5);
        ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
        m_SendPacket1.DIA2 = m_nTachoDIA2Index;
        m_SendPacket2.DIA1 = m_nTachoDIA2Index;

        if( m_ActivePacket.DIA1 == m_nTachoDIA1Index && m_ActivePacket.DIA2 == m_nTachoDIA2Index )
            ui->Button_WC_OK->setVisible(true);
        else
            ui->Button_WC_OK->setVisible(false);
    }
}


//#############################################################################
//
//  on_Button_Tacho2_Up_clicked
//      - Tacho #2 차륜경값 Up 버튼 처리
//
void MainWindow::on_Button_Tacho2_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA2Value < 860 )
    {
        SetTacho2DIA(m_nTachoDIA2Value + 5);
        ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
        m_SendPacket1.DIA2 = m_nTachoDIA2Index;
        m_SendPacket2.DIA1 = m_nTachoDIA2Index;

        if( m_ActivePacket.DIA1 == m_nTachoDIA1Index && m_ActivePacket.DIA2 == m_nTachoDIA2Index )
            ui->Button_WC_OK->setVisible(true);
        else
            ui->Button_WC_OK->setVisible(false);
    }
}



//############################################################################
//############################################################################
//############################################################################
//############################################################################
//
//
//
void MainWindow::on_pushButton_clicked()
{
    //this->setWindowState(Qt::WindowFullScreen);
    //this->setWindowFlag(Qt::FramelessWindowHint);
    //this->move(100, 100);

    char szMsg[100];
    sprintf(szMsg, "X : %d - Y : %d\n", this->pos().x(), this->pos().y());
    //qDebug() << szMsg;

    //QMediaPlayer* mp = new QMediaPlayer;
    //mp->setMedia(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/sound/Sound_ATS.wav"));
    //mp->play();

    //AddMessage(QString::number(m_nTest++));

    /*
    char buffer[100];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';
    buffer[3] = 0x00;
    m_serial->write(buffer, 3);

    m_Packet.year = 22;
    m_Packet.month = 7;
    m_Packet.day = 21;
    m_Packet.hour = 9;
    m_Packet.minute = 10;
    m_Packet.second = 11;
    */

    m_Packet1.train_speed = 75;
    m_Packet1.atc_speed = 1;

    m_Packet1.source = 0x20;
    m_Packet1.active = true;

    m_Packet1.DM_EM = true;
    m_Packet1.DM_MM = true;

    m_Packet1.TC_DR = true;

    m_Packet1.EB_M = true;
    m_Packet1.FSB = true;
    m_Packet1.ZVR = true;

    m_Packet1.DOLP_L = false;
    m_Packet1.DOLP_R = true;
    m_Packet1.train_door_close = true;
    m_Packet1.EDL = false;
    m_Packet1.EDR = true;

    //if( mkdir("/home/hzsoft/test", 0777) == -1 )
     //   qDebug() << strerror(errno);
}


void MainWindow::on_pushButton_2_clicked()
{
    /*
    DialogProtocol dlg(this);
    dlg.SetMainWindow(this);
    dlg.setModal(true);

    dlg.move(0, 0);
    dlg.exec();
    */
}

void MainWindow::on_pushButton_3_clicked()
{
    QApplication::quit();
}


void MainWindow::on_pushButton_exit_clicked()
{
    QApplication::quit();
}


void MainWindow::on_Button_Busan_Logo_clicked()
{
    ++m_nLogoClick;
}

void MainWindow::on_pushButton_TCR_clicked()
{
    m_bTCRTempShow = true;
    m_nTCRTempCount = 0;
    ui->frame_TCR->setVisible(false);
    ui->frame_Main->setVisible(true);
}

void MainWindow::on_Button_Config_pressed()
{
    qDebug() << "Config pressed";
}

void MainWindow::on_Button_Config_released()
{
    qDebug() << "Config released";
}
