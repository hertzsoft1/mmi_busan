#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSound>
#include <QSoundEffect>

#include <QtXml/QtXml>
#include <QtXml/QDomDocument>

#define MAX_RECEIVE_BUFFER      4096

#define PACKET_SIZE             78

#define RECV_DATA_SIZE          64
#define SEND_DATA_SIZE          64

#define PACKET_BUFFER_SIZE      100

#define SCREEN_WAIT             0
#define SCREEN_TCR              1
#define SCREEN_ON               2
#define SCREEN_ON_DISCONNECT    3

#define TIMEOUT_SEC             2
#define CONNECT_TRY_SEC         30

// Active 상태인 시스템 구분자
//
typedef enum tagACTIVE_SYSTEM
{
    ACTIVE_NONE,
    ACTIVE_SYSTEM1,
    ACTIVE_SYSTEM2
} ACTIVE_SYSTEM;


typedef enum tagLOGGING_MODE
{
    LOGGING_NONE,
    LOGGING_HCR_ON,
    LOGGING_HCR_OFF
} LOGGING_MODE;


// Message Handler Code in Packet
//
#define MSG_ID_ATC_OVERSPEED    1
#define MSG_ID_ATC_EB           2
#define MSG_ID_ATC_INIT_OK      3
#define MSG_ID_ATC_MANUAL_START 4
#define MSG_ID_ATC_MANUAL_OK    5
#define MSG_ID_ATC_NOCODE       6
#define MSG_ID_ATC_PSDSTATE     7

#define MSG_ID_ATO_INCHING      51
#define MSG_ID_ATO_TASC         52
#define MSG_ID_ATO_BERTH        53
#define MSG_ID_ATO_PG1          54
#define MSG_ID_ATO_PG2          55
#define MSG_ID_ATO_PG32         56
#define MSG_ID_ATO_ATS          57
#define MSG_ID_ATO_DOOR         58
#define MSG_ID_ATO_STATION      59

// Message Database ID
//
#define MSG_ATC_OVERSPEED_1     0
#define MSG_ATC_OVERSPEED_2     1
#define MSG_ATC_EB_1            2
#define MSG_ATC_EB_2            3
#define MSG_ATC_INIT_OK_1       4
#define MSG_ATC_INIT_OK_2       5
#define MSG_ATC_MANUAL_START_1  6
#define MSG_ATC_MANUAL_START_2  7
#define MSG_ATC_MANUAL_OK_1     8
#define MSG_ATC_MANUAL_OK_2     9
#define MSG_ATC_NOCODE_1        10
#define MSG_ATC_NOCODE_2        11
#define MSG_ATC_PSDSTATE_1      12
#define MSG_ATC_PSDSTATE_2      13

#define MSG_ATO_INCHING_1       100
#define MSG_ATO_INCHING_2       101
#define MSG_ATO_TASC_1          102
#define MSG_ATO_TASC_2          103
#define MSG_ATO_BERTH_1         104
#define MSG_ATO_BERTH_2         105
#define MSG_ATO_PG1_1           106
#define MSG_ATO_PG1_2           107
#define MSG_ATO_PG2_1           108
#define MSG_ATO_PG2_2           109
#define MSG_ATO_PG32_1          110
#define MSG_ATO_PG32_2          111
#define MSG_ATO_ATS_1           112
#define MSG_ATO_ATS_2           113
#define MSG_ATO_DOOR_1          114
#define MSG_ATO_DOOR_2          115
#define MSG_ATO_STATION_1       116
#define MSG_ATO_STATION_2       117

#define ERROR_COMM_1            1000
#define ERROR_COMM_2            1001

#define ERROR_INITIAL_PDT       186
#define ERROR_MANUAL_PDT        187

#define ERROR_COMM_ATC          190
#define ERROR_COMM_ATO          191
#define ERROR_COMM_TCMS         192
#define ERROR_COMM_TACHO2       196
#define ERROR_COMM_TACHO1       197

#define ERROR_DE_ATIV_1         4000
#define ERROR_DE_ATIV_2         4001
#define ERROR_DE_VOTING_1       4002
#define ERROR_DE_VOTING_2       4003
#define ERROR_DE_RUNRTDGET_1    4004
#define ERROR_DE_RUNRTDGET_2    4005
#define ERROR_DE_ATIVGET_1      4006
#define ERROR_DE_ATIVGET_2      4007
#define ERROR_DE_DGS_1          4008
#define ERROR_DE_DGS_2          4009

#define ERROR_DE_MDHEAL_1       4100
#define ERROR_DE_MDHEAL_2       4101
#define ERROR_DE_MNUALBYSET_1   4102
#define ERROR_DE_MNUALBYSET_2   4103
#define ERROR_DE_INPUT_1        4104
#define ERROR_DE_INPUT_2        4105
#define ERROR_DE_UNHEAL_1       4106
#define ERROR_DE_UNHEAL_2       4107
#define ERROR_DE_EOT_1          4108
#define ERROR_DE_EOT_2          4109
#define ERROR_DE_EOTVAL_1       4110
#define ERROR_DE_EOTVAL_2       4111
#define ERROR_DE_RUNRTD_1       4112
#define ERROR_DE_RUNRTD_2       4113
#define ERROR_DE_RUNRTDSET_1    4114
#define ERROR_DE_RUNRTDSET_2    4115

#define ERROR_DE_EBDATA_1       4200
#define ERROR_DE_EBDATA_2       4201
#define ERROR_DE_VDOTEMP_1      4202
#define ERROR_DE_VDOTEMP_2      4203
#define ERROR_DE_VDITEMP_1      4204
#define ERROR_DE_VDITEMP_2      4205
#define ERROR_DE_OUTGOING_1     4206
#define ERROR_DE_OUTGOING_2     4207
#define ERROR_DE_OUTPUTFB_1     4208
#define ERROR_DE_OUTPUTFB_2     4209
#define ERROR_DE_MNUALBYCLR_1   4210
#define ERROR_DE_MNUALBYCLR_2   4211

#define ERROR_ATC_MSCSTATE_1    4400
#define ERROR_ATC_MSCSTATE_2    4401
#define ERROR_ATC_DOORSTAT_1    4402
#define ERROR_ATC_DOORSTAT_2    4403
#define ERROR_ATC_DRIVEMODE_1   4404
#define ERROR_ATC_DRIVEMODE_2   4405
#define ERROR_ATC_CABMOD_1      4406
#define ERROR_ATC_CABMOD_2      4407
#define ERROR_ATC_ATC_1         4408
#define ERROR_ATC_ATC_2         4409

#define ERROR_ATC_MAXSPEED_1    4500
#define ERROR_ATC_MAXSPEED_2    4501
#define ERROR_ATC_WHEELDIA_1    4502
#define ERROR_ATC_WHEELDIA_2    4503
#define ERROR_ATC_MANUALPDT_1   4504
#define ERROR_ATC_MANUALPDT_2   4505
#define ERROR_ATC_INITPDT_1     4506
#define ERROR_ATC_INITPDT_2     4507
#define ERROR_ATC_DIRSET_1      4508
#define ERROR_ATC_DIRSET_2      4509
#define ERROR_ATC_TCSEL_1       4510
#define ERROR_ATC_TCSEL_2       4511
#define ERROR_ATC_PSDSTATE_1    4512
#define ERROR_ATC_PSDSTATE_2    4513
#define ERROR_ATC_REVERSING_1   4514
#define ERROR_ATC_REVERSING_2   4515

#define ERROR_ATC_TRBSMODE_1    4600
#define ERROR_ATC_TRBSMODE_2    4601
#define ERROR_ATC_TDCSMOD_1     4602
#define ERROR_ATC_TDCSMOD_2     4603
#define ERROR_ATC_TZSSMODE_1    4604
#define ERROR_ATC_TZSSMODE_2    4605
#define ERROR_ATC_TRSSMODE_1    4606
#define ERROR_ATC_TRSSMODE_2    4607
#define ERROR_ATC_RESTRICTIO_1  4608
#define ERROR_ATC_RESTRICTIO_2  4609
#define ERROR_ATC_ATCCODE_1     4610
#define ERROR_ATC_ATCCODE_2     4611
#define ERROR_ATC_MAXPULSE_1    4612
#define ERROR_ATC_MAXPULSE_2    4613
#define ERROR_ATC_DIFFSPEED_1   4614
#define ERROR_ATC_DIFFSPEED_2   4615

#define ERROR_ATC_ATCMODE_1     4700
#define ERROR_ATC_ATCMODE_2     4701
#define ERROR_ATC_DIRECT_1      4702
#define ERROR_ATC_DIRECT_2      4703

#define ERROR_IF_IFS_1          4800
#define ERROR_IF_IFS_2          4801

#define ERROR_IF_ATCRINIT_1     4900
#define ERROR_IF_ATCRINIT_2     4901
#define ERROR_IF_ATCRW_1        4902
#define ERROR_IF_ATCRW_2        4903
#define ERROR_IF_ATCRR_1        4904
#define ERROR_IF_ATCRR_2        4905
#define ERROR_IF_CANINIT_1      4906
#define ERROR_IF_CANINIT_2      4907
#define ERROR_IF_TCMSINIT_1     4908
#define ERROR_IF_TCMSINIT_2     4909
#define ERROR_IF_TCMSW_1        4910
#define ERROR_IF_TCMSW_2        4911
#define ERROR_IF_TCMSR_1        4912
#define ERROR_IF_TCMSR_2        4913

#define ERROR_IF_VDIINIT_1      5000
#define ERROR_IF_VDIINIT_2      5001
#define ERROR_IF_VDIW_1         5002
#define ERROR_IF_VDIW_2         5003
#define ERROR_IF_VDIR_1         5004
#define ERROR_IF_VDIR_2         5005
#define ERROR_IF_DIOINIT_1      5006
#define ERROR_IF_DIOINIT_2      5007
#define ERROR_IF_ATOCINIT_1     5008
#define ERROR_IF_ATOCINIT_2     5009
#define ERROR_IF_ATOCW_1        5010
#define ERROR_IF_ATOCW_2        5011
#define ERROR_IF_ATOCR_1        5012
#define ERROR_IF_ATOCR_2        5013

#define ERROR_IF_VDOINIT_1      5100
#define ERROR_IF_VDOINIT_2      5101
#define ERROR_IF_VDOW_1         5102
#define ERROR_IF_VDOW_2         5103
#define ERROR_IF_VDOR_1         5104
#define ERROR_IF_VDOR_2         5105

#define ERROR_ATO_RLYFB_1       5200
#define ERROR_ATO_RLYFB_2       5201
#define ERROR_ATO_DOORCI_1      5202
#define ERROR_ATO_DOORCI_2      5203
#define ERROR_ATO_DOOROP_1      5204
#define ERROR_ATO_DOOROP_2      5205
#define ERROR_ATO_STNCODE_1     5206
#define ERROR_ATO_STNCODE_2     5207
#define ERROR_ATO_ATOANT_1      5208
#define ERROR_ATO_ATOANT_2      5209
#define ERROR_ATO_ATOSTOP_1     5210
#define ERROR_ATO_ATOSTOP_2     5211

#define ERROR_ATO_PG32_1        5300
#define ERROR_ATO_PG32_2        5301
#define ERROR_ATO_PG2_1         5302
#define ERROR_ATO_PG2_2         5303
#define ERROR_ATO_PG1_1         5304
#define ERROR_ATO_PG1_2         5305

typedef struct tagPacket
{
    int count;

    unsigned char sequence;
    unsigned char destination;
    unsigned char source;

    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;

    int train_speed;
    int atc_speed;

    bool active;

    bool ZVR;
    bool EDL;
    bool EDR;
    bool FSB;
    bool EB_M;
    bool EB_P;

    bool DM_FA;
    bool DM_AUTO;
    bool DM_MCS;
    bool DM_YARD;
    bool DM_EM;

    bool DIR_FWD;
    bool DIR_RVS;
    bool DIR_NEU;

    bool OverSpeed;
    bool TCR;
    bool HCR;

    bool train_door_open;
    bool train_door_close;

    bool psd_open;
    bool psd_close;

    bool mascon_eb;
    bool mascon_br;
    bool mascon_dr;
    bool mascon_neu;

    int DIA1;
    int DIA2;

    int ATC_STATUS;

    bool Wheel_DIA_Check_Request;
    bool manual_PDT_START;
    bool manual_PDT_NG;
    bool manual_PDT_OK;
    bool initial_PDT_NG;
    bool initial_PDT_OK;

    unsigned char data_19;
    /*
    bool ATC_receiver;
    bool ATO_controller;
    bool TCMS;
    bool TACHO_1;
    bool TACHO_2;
    */

    int PWM;

    bool TC_DR;
    bool TC_BR;
    bool TC_CS;

    bool DC_ADOL;
    bool DC_ADOR;
    bool DC_ADC;

    bool RM_normal;
    bool RM_recovery;

    bool train_berth;

    bool cab_no_TC1;
    bool cab_no_TC2;

    bool DOLP_L;
    bool DOLP_R;

    bool DM_AA;
    bool DM_AM;
    bool DM_MM;

    int formation_no;
    int train_no;

    int atc_Speed_Code;
    int MessageNo;

    int MajorVersion;
    int MinorVersion;

    // Diagnostic Error
    //
    unsigned char data_40;
    unsigned char data_41;
    unsigned char data_42;
    /*
    bool DE_ATIV;
    bool DE_VOTING;
    bool DE_RUNRTDGET;
    bool DE_ATIVGET;
    bool DE_DGS;
    bool DE_MDHEAL;
    bool DE_MNUALBYSET;
    bool DE_INPUT;
    bool DE_UNHEAL;
    bool DE_EOT;
    bool DE_EOTVAL;
    bool DE_RUNRTD;
    bool DE_RUNRTDSET;
    bool DE_EBDATA;
    bool DE_VDOTEMP;
    bool DE_VDITEMP;
    bool DE_OUTGOING;
    bool DE_OUTPUTFB;
    bool DE_MNUALBYCLR;
    */

    // ATC Error
    //
    unsigned char data_44;
    unsigned char data_45;
    unsigned char data_46;
    unsigned char data_47;
    /*
    bool ATC_MSCSTATE;
    bool ATC_DOORSTAT;
    bool ATC_DRIVEMODE;
    bool ATC_CABMOD;
    bool ATC_ATC;
    bool ATC_MAXSPEED;
    bool ATC_WHEELDIA;
    bool ATC_MANUALPDT;
    bool ATC_INITPDT;
    bool ATC_DIRSET;
    bool ATC_TCSEL;
    bool ATC_PSDSTATE;
    bool ATC_REVERSING;
    bool ATC_TRBSMODE;
    bool ATC_TDCSMOD;
    bool ATC_TZSSMODE;
    bool ATC_TRSSMODE;
    bool ATC_RESTRICTIO;
    bool ATC_ATCCODE;
    bool ATC_MAXPULSE;
    bool ATC_DIFFSPEED;
    bool ATC_ATCMODE;
    bool ATC_DIRECT;
    */

    // Interface Error
    //
    unsigned char data_48;
    unsigned char data_49;
    unsigned char data_50;
    unsigned char data_51;
    /*
    bool IF_IFS;
    bool IF_ATCRINIT;
    bool IF_ATCRW;
    bool IF_ATCRR;
    bool IF_CANINIT;
    bool IF_TCMSINIT;
    bool IF_TCMSW;
    bool IF_TCMSR;
    bool IF_VDIINIT;
    bool IF_VDIW;
    bool IF_VDIR;
    bool IF_DIOINIT;
    bool IF_ATOCINIT;
    bool IF_ATOCW;
    bool IF_ATOCR;
    bool IF_VDOINIT;
    bool IF_VDOW;
    bool IF_VDOR;
    */

    // ATO Error
    //
    unsigned char data_52;
    unsigned char data_53;
    /*
    bool ATO_RLYFB;
    bool ATO_DOORCI;
    bool ATO_DOOROP;
    bool ATO_STNCODE;
    bool ATO_ATOANT;
    bool ATO_ATOSTOP;
    bool ATO_PG32;
    bool ATO_PG2;
    bool ATO_PG1;
    */
} Packet;


typedef struct tagSendPacket
{
    unsigned char sequence;
    unsigned char destination;
    unsigned char source;

    int DIA1;
    int DIA2;

    bool manual_PDT_request;
    bool confirm_DIA;

    bool RM_ON;
    bool RM_OFF;

    int formation_no;
    int MessageNo;

} SendPacket;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void DisplayTimerCallback();
    void CommTimerCallback();
    void ReceiveSerialCallback();
    void ReceiveSerialCallback2();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent* event) override;

private slots:
    void on_pushButton_clicked();
    void on_Button_Mode_clicked();
    void on_Button_Config_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_exit_clicked();

    void on_Button_WC_OK_clicked();
    void on_Button_WC_Update_clicked();

    void on_Button_Msg_Up_clicked();
    void on_Button_Msg_Down_clicked();

    void on_Button_Tacho1_Down_clicked();
    void on_Button_Tacho1_Up_clicked();
    void on_Button_Tacho2_Down_clicked();
    void on_Button_Tacho2_Up_clicked();

    void on_Button_Busan_Logo_clicked();
    void on_pushButton_TCR_clicked();
    void on_Button_Config_pressed();
    void on_Button_Config_released();

public:
    Packet m_ActivePacket;
    Packet m_PrevActivePacket;

    int m_nTachoDIA1Index = 8;
    int m_nTachoDIA2Index = 8;
    int m_nTachoDIA1Value = 820;
    int m_nTachoDIA2Value = 820;

    // 1계와의 통신 처리를 위한 변수
    //
    Packet m_Packet1;
    Packet m_PrevPacket1;

    char m_CurrRecvBuffer1[500];
    int m_nBufferIndex1 = 0;

    unsigned char m_PacketBuffer1[PACKET_BUFFER_SIZE + 500];
    unsigned char m_SendPacketBuffer1[PACKET_BUFFER_SIZE + 500];

    bool m_bConnectSystem1 = false;
    bool m_bPrevConnectSystem1 = false;

    // 2계와의 통신 처리를 위한 변수
    //
    Packet m_Packet2;
    Packet m_PrevPacket2;

    char m_CurrRecvBuffer2[500];
    int m_nBufferIndex2 = 0;

    unsigned char m_PacketBuffer2[PACKET_BUFFER_SIZE + 500];
    unsigned char m_SendPacketBuffer2[PACKET_BUFFER_SIZE + 500];

    bool m_bConnectSystem2 = false;
    bool m_bPrevConnectSystem2 = false;

    // 송수신 시리얼 데이터 저장용 버퍼
    // (STX - CRC32까지 수신 데이터 저장)
    //
    int m_nRxBufferCount = 0;
    char m_RxBufferList[100][100];

    int m_nTxBufferCount = 0;
    char m_TxBufferList[100][100];

    // PDT 수동 시험
    //
    QDateTime m_sendPDTTime1;
    QDateTime m_sendPDTTime2;

    void SetPDTRequest1();
    void SetPDTRequest2();

    void SetTacho1DIA(int nDIA);
    void SetTacho2DIA(int nDIA);
    int GetTacho1DIA();
    int GetTacho2DIA();

    int GetTacho1DIA_Packet();
    int GetTacho2DIA_Packet();

    char* getPasswrod() { return m_szPassword; }
    char* getGroupNo()  { return m_szGroupNo; }
    void setGroupNo(char* szGroup)   { memcpy(m_szGroupNo, szGroup, 2); m_nGroupNo = atoi(szGroup); }
    char* getVersion()   { return m_szVersion; }

    char* getDate()     { return m_szDate; }
    char* getTime()     { return m_szTime; }

    void SetLogDownloadStatus(bool bStatus)     { m_bLogDownloadStatus = bStatus; }

private:
    void LoadPassword();
    void LoadTachoDia();
    void LoadGroupNo();
    void LoadVolumeLevel();
    void LoadBrightnessLevel();
    void ShowTachoButton(bool bEnable);

    LOGGING_MODE m_LoggingMode = LOGGING_NONE;
    bool m_bMsgLoggingOpen = false;
    bool m_bLogDownloadStatus = false;
    QFile m_FileLogging;
    QFile m_FileLoggingMsg;
    char m_szLoggingPath[200];
    char m_szLoggingFile[200];
    char m_szLoggingFileMsg[200];
    void AddRxDataToBufferList(char *data);
    void AddTxDataToBufferList(char *data);
    void WriteCommLogFile();
    void DeleteOldLogFile2();

public:
    int getCurrentMode()        { return m_nCurrentMode; }
    void ResetTCRTempCount()    { m_nTCRTempCount = 0; }
    void DeleteOldLogFile();

private:
    Ui::MainWindow *ui;

    int m_nCurrentMode = SCREEN_WAIT;    // 화면 전환을 위한 변수
    int m_nVolume = 5;
    int m_nBrightness = 5;

    int m_nBootCount = 0;
    bool m_bSetTime = false;            // 최초 패킷 수신 시 시간설정을 위한 변수
    bool m_bRedraw = false;             // 1계/2계 통신 차단 후 재접속 시 화면 갱신을 위한 변수
    bool m_bRefreshSound = false;       // TCR 모드에서 벗어날 때 사운드를 다시 재생하기 위한 변수
    bool m_bActLamp = false;            // 전면 ACT 램프 켜짐 여부
    bool m_bRecoveryMode = false;       // 회복모드 선택 여부
    bool m_bPrevEB = false;             // EB 상태 저장
    bool m_bPrevOn02 = false;
    bool m_bShowMode = false;
    bool m_bCanConfig = true;

    bool m_bTCRTempShow = false;
    int m_nTCRTempCount = 0;

    char m_szPassword[100];             // MMI 파일시스템에 저장된 패스워드(4자리)
    char m_szGroupNo[50];               // 편성번호
    char m_szVolumeValue[50];
    char m_szBrightnessValue[50];
    char m_szVersion[100];

    char m_szDate[100];
    char m_szTime[100];
    int m_nGroupNo = 0;

    int m_nInitSoundStep = 0;
    QSound* m_Sound_OverSpeed = nullptr;
    QSound* m_Sound_EB = nullptr;
    QSound* m_Sound_Info = nullptr;
    QSound* m_Sound_Click = nullptr;
    QSound* m_Sound_Volume = nullptr;
    QSoundEffect* m_Sound_Test;


    bool m_bWeelCheckUpdate = false;
    bool m_bWeelCheckPassword = false;

    bool m_bWheelCheckPopup1 = false;
    bool m_bWheelCheckPopup2 = false;

    bool m_bWheelCheckShow = false;
    int m_nWheelCheckShow = -1;

    QTimer *m_pTimerDisp;
    QTimer *m_pTimerComm;

    QPixmap m_pix_Left;
    QPixmap m_pix_LeftOpen;
    QPixmap m_pix_Right;
    QPixmap m_pix_RightOpen;

    QPixmap m_pix_ADC;
    QPixmap m_pix_ADCOn;
    QPixmap m_pix_DOLP;
    QPixmap m_pix_DOLPOn;
    QPixmap m_pix_DORP;
    QPixmap m_pix_DORPOn;

    QPixmap m_pix_Mode;
    QPixmap m_pix_Mode2;
    QPixmap m_pix_Config;

    QPixmap m_pix_Msg_Enable;
    QPixmap m_pix_Msg_Disable;

    QPixmap m_pix_Logo;
    int m_nLogoClick = 0;

private:
    bool m_bConnect1 = false;
    QSerialPort * m_serial1;
    QByteArray m_receivedData1;

    int m_nRecvQueue1Front = 0;
    int m_nRecvQueue1Rear = 0;
    char m_szReceivedData1[MAX_RECEIVE_BUFFER];
    QMutex m_mutex1;

    bool m_bConnect2 = false;
    QSerialPort * m_serial2;
    QByteArray m_receivedData2;

    int m_nRecvQueue2Front = 0;
    int m_nRecvQueue2Rear = 0;
    char m_szReceivedData2[MAX_RECEIVE_BUFFER];
    QMutex m_mutex2;

private:

    // 1계와의 통신 처리를 위한 변수
    //
    int m_nPacketIndex1 = 0;
    int m_nParsePhase1 = 0;
    uint32_t m_CRC32_1 = 0;
    uint32_t m_RecvCRC32_1 = 0;
    int m_nRecvCRCCount1 = 0;
    int m_nSendSequence1 = 0;

    SendPacket m_SendPacket1;

    QDateTime m_LastRecvTime1;
    int m_nRecvPacketCount1 = 0;

    // 2계와의 통신 처리를 위한 변수
    //
    int m_nPacketIndex2 = 0;
    int m_nParsePhase2 = 0;
    uint32_t m_CRC32_2 = 0;
    uint32_t m_RecvCRC32_2 = 0;
    int m_nRecvCRCCount2 = 0;
    int m_nSendSequence2 = 0;

    SendPacket m_SendPacket2;

    QDateTime m_LastRecvTime2;
    int m_nRecvPacketCount2 = 0;

    int m_nSendConfirmCount1 = 0;
    int m_nSendConfirmCount2 = 0;

    void ParsePacket1();
    void ParsePacket2();
    void SendPacketToDevice1();
    void SendPacketToDevice2();
    uint32_t CalcCRC32(const unsigned char* s, size_t n);
    unsigned char BCDToDecimal(unsigned char bcd);

private:
    // 메시지 화면 처리를 위한 변수
    //
    bool m_bRedrawMessage = false;
    QStringList m_MessageList;
    QStringList m_MessageColorList;
    int m_nMessageIndex = 0;
    QLabel* m_LabelMsg[5];
    QString m_strMessageToWrite = "";
    int m_nTest = 1;

    bool m_bMsgUpBtnEnabled = false;
    bool m_bMsgDownBtnEnabled = false;

    void AddMessage(int nSystem, int nID);
    void AddMessage(int nSystem, QString strMsg, QString strColor);
    void DrawMessage();

    // 미리 정의된 메시지 정보
    //
    QList<int> m_dbMessageNoInt;
    QStringList m_dbMessageNo;
    QStringList m_dbMessageText;
    QStringList m_dbMessageColor;

    bool Load_Message();
    int FindMessage(int nID);

};

#endif // MAINWINDOW_H
