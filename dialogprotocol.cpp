#include "dialogprotocol.h"
#include "ui_dialogprotocol.h"

DialogProtocol::DialogProtocol(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogProtocol)
{
    ui->setupUi(this);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerCallback()));
    m_pTimer->start(500);
}

DialogProtocol::~DialogProtocol()
{
    delete ui;
}

void DialogProtocol::SetMainWindow(MainWindow* main)
{
    m_Main = main;
}

void DialogProtocol::on_Button_Exit_clicked()
{
    m_pTimer->stop();
    QDialog::accept();
}

void DialogProtocol::TimerCallback()
{
    QString msg;
    QString msg2;
    //char buffer[200];

    if( m_Main == nullptr )
        return;

    /*
    sprintf(buffer, "Sequence : 0x%02x\r\n", m_Main->m_Packet.sequence);
    msg += buffer;
    sprintf(buffer, "Destination : 0x%02x\r\n", m_Main->m_Packet.destination);
    msg += buffer;
    sprintf(buffer, "Source : 0x%02x\r\n", m_Main->m_Packet.source);
    msg += buffer;

    sprintf(buffer, "Date : %d-%d-%d %d:%d:%d\r\n", m_Main->m_Packet.year + 2000, m_Main->m_Packet.month, m_Main->m_Packet.day, m_Main->m_Packet.hour, m_Main->m_Packet.minute, m_Main->m_Packet.second);
    msg += buffer;

    sprintf(buffer, "Train Speed : %d\r\n", m_Main->m_Packet.train_speed);
    msg += buffer;
    sprintf(buffer, "ATC Speed : %d\r\n", m_Main->m_Packet.atc_speed);
    msg += buffer;

    sprintf(buffer, "Active : %s\r\n", m_Main->m_Packet.active == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "ZVR : %s\r\n", m_Main->m_Packet.ZVR == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "EDL : %s\r\n", m_Main->m_Packet.EDL == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "EDR : %s\r\n", m_Main->m_Packet.EDR == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "FSB : %s\r\n", m_Main->m_Packet.FSB == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "EB : %s\r\n", m_Main->m_Packet.EB == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "DM_FA : %s\r\n", m_Main->m_Packet.DM_FA == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "DM_AUTO : %s\r\n", m_Main->m_Packet.DM_AUTO == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "DM_MCS : %s\r\n", m_Main->m_Packet.DM_MCS == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "DM_YARD : %s\r\n", m_Main->m_Packet.DM_YARD == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "DM_EM : %s\r\n", m_Main->m_Packet.DM_EM == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "DIR_FWD : %s\r\n", m_Main->m_Packet.DIR_FWD == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "DIR_RVS : %s\r\n", m_Main->m_Packet.DIR_RVS == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "HCR : %s\r\n", m_Main->m_Packet.HCR == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "Door Open : %s\r\n", m_Main->m_Packet.train_door_open == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "Door Close : %s\r\n", m_Main->m_Packet.train_door_open == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "PSD Open : %s\r\n", m_Main->m_Packet.psd_open == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "PSD Close : %s\r\n", m_Main->m_Packet.psd_close == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "Mascon EB : %s\r\n", m_Main->m_Packet.mascon_eb == true ? "true" : "false");
    msg += buffer;

    sprintf(buffer, "DIA1 : %d\r\n", m_Main->m_Packet.DIA1);
    msg += buffer;
    sprintf(buffer, "DIA2 : %d\r\n", m_Main->m_Packet.DIA2);
    msg += buffer;

    sprintf(buffer, "ATC Status : %d\r\n", m_Main->m_Packet.ATC_STATUS);
    msg += buffer;

    sprintf(buffer, "Manual PDT Start : %s\r\n", m_Main->m_Packet.manual_PDT_START == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "Manual PDT NG : %s\r\n", m_Main->m_Packet.manual_PDT_NG == true ? "true" : "false");
    msg += buffer;
    sprintf(buffer, "Manual PDT OK : %s\r\n", m_Main->m_Packet.manual_PDT_OK == true ? "true" : "false");
    msg += buffer;

    ui->Label_1->setText(msg);

    sprintf(buffer, "Initial PDT NG : %s\r\n", m_Main->m_Packet.initial_PDT_NG == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Initial PDT OK : %s\r\n", m_Main->m_Packet.initial_PDT_OK == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "ATC Receiver : %s\r\n", m_Main->m_Packet.ATC_receiver == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "ATO Controller : %s\r\n", m_Main->m_Packet.ATO_controller == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "TCMS : %s\r\n", m_Main->m_Packet.TCMS == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Tacho #1 : %s\r\n", m_Main->m_Packet.TACHO_1 == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Tacho #2 : %s\r\n", m_Main->m_Packet.TACHO_2 == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "PWM : %d\r\n", m_Main->m_Packet.PWM);
    msg2 += buffer;

    sprintf(buffer, "Traction DR : %s\r\n", m_Main->m_Packet.TC_DR == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Traction BR : %s\r\n", m_Main->m_Packet.TC_BR == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Traction CS : %s\r\n", m_Main->m_Packet.TC_CS == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "Door Control ADO : %s\r\n", m_Main->m_Packet.DC_ADO == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Door Control ADC : %s\r\n", m_Main->m_Packet.DC_ADC == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "Recovery Mode Normal : %s\r\n", m_Main->m_Packet.RM_normal == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Recovery Mode Recovery : %s\r\n", m_Main->m_Packet.RM_recovery == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Train Berth : %s\r\n", m_Main->m_Packet.train_berth == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "Cab TC1 : %s\r\n", m_Main->m_Packet.cab_no_TC1 == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "Cab TC2 : %s\r\n", m_Main->m_Packet.cab_no_TC2 == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "TCMS DOLP(L) : %s\r\n", m_Main->m_Packet.DOLP_L == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "TCMS DOLP(R) : %s\r\n", m_Main->m_Packet.DOLP_R == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "DOOR MODE A/A : %s\r\n", m_Main->m_Packet.DM_AA == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "DOOR MODE A/M : %s\r\n", m_Main->m_Packet.DM_AM == true ? "true" : "false");
    msg2 += buffer;
    sprintf(buffer, "DOOR MODE M/M : %s\r\n", m_Main->m_Packet.DM_MM == true ? "true" : "false");
    msg2 += buffer;

    sprintf(buffer, "Formation : %d\r\n", m_Main->m_Packet.formation_no);
    msg2 += buffer;
    sprintf(buffer, "Train No : %d\r\n", m_Main->m_Packet.train_no);
    msg2 += buffer;

    ui->Label_2->setText(msg2);
    */
}
