#include "dialogconfig.h"
#include "ui_dialogconfig.h"

#include "dialogupdate.h"
#include "dialogmsg.h"
#include "dialogpassword.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

//#############################################################################
//
//
DialogConfig::DialogConfig(QWidget *parent, int nVolume, int nBrightness) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    m_Main = static_cast<MainWindow*>(parent);

    m_nCurrentMode = CONFIG_MODE_NONE;
    ui->Frame_Env->setGeometry(0, 0, 800, 768);
    ui->Frame_Group->setGeometry(0, 0, 800, 768);
    ui->Frame_Wheel->setGeometry(0, 0, 800, 768);
    ui->Frame_Log->setGeometry(0, 0, 800, 768);
    ui->Frame_Comm->setGeometry(0, 0, 800, 768);
    ui->Frame_Update->setGeometry(0, 0, 800, 768);
    ui->Frame_Download->setGeometry(0, 0, 800, 768);
    ui->Frame_Env->setVisible(false);
    ui->Frame_Group->setVisible(false);
    ui->Frame_Wheel->setVisible(false);
    ui->Frame_Log->setVisible(false);
    ui->Frame_Comm->setVisible(false);
    ui->Frame_Update->setVisible(false);
    ui->Frame_Download->setVisible(false);

    // initialize timer
    //
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerCallback()));
    m_pTimer->start(500);

    m_pTimerDownload = new QTimer(this);
    connect(m_pTimerDownload, SIGNAL(timeout()), this, SLOT(TimerDownloadCallback()));
    m_pTimerDownload->stop();
    m_nDownloadStep = 0;

    m_nVolume = nVolume;
    ui->Label_Volumn->setText(QString::number(m_nVolume));

    m_nBrightness = nBrightness;
    ui->Label_Bright->setText(QString::number(m_nBrightness));

    m_pix_Config.load(":/image/image/Button2.png");

    //
    //
    ui->table_Log->setColumnWidth(0, 390);
    ui->table_Log->setColumnWidth(1, 160);
    ui->table_Log->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_nDownloadMode = 0;
    ui->radioButton_1->setChecked(true);
    m_nDownloadMonth = 1;
    ui->Label_Month->setText("1월");
    m_nDownloadDay = 1;
    ui->Label_Day->setText("1일");
}


//#############################################################################
//
//
DialogConfig::~DialogConfig()
{
    delete m_pTimer;
    delete ui;
}


//#############################################################################
//
//
void DialogConfig::SetMainWindow(MainWindow* main)
{
    m_Main = main;
}


//#############################################################################
//
// timer callback
//
void DialogConfig::TimerCallback()
{
    QString msgRX1;
    QString msgRX2;
    QString msgTX1;
    QString msgTX2;
    int i = 0;
    char szTemp[50];

    /*
    if(m_Main->getCurrentMode() != SCREEN_ON )
    {
        m_pTimer->stop();
        QDialog::reject();
    }
    */

    if( m_Main == nullptr )
        return;

    m_Main->ResetTCRTempCount();

    // 시간 표시
    //
    ui->label_Date->setText(m_Main->getDate());
    ui->label_Time->setText(m_Main->getTime());
    ui->label_Version->setText(m_Main->getVersion());

    if( m_nCurrentMode == CONFIG_MODE_COMM )
    {
        for(i = 0; i < RECV_DATA_SIZE; ++i)
        {
            sprintf(szTemp, "%02x ", m_Main->m_CurrRecvBuffer1[i]);
            msgRX1 += szTemp;
            sprintf(szTemp, "%02x ", m_Main->m_CurrRecvBuffer2[i]);
            msgRX2 += szTemp;
        }
        ui->Label_Recv_1->setText(msgRX1);
        ui->Label_Recv_2->setText(msgRX2);

        for(i = 0; i < SEND_DATA_SIZE; ++i)
        {
            sprintf(szTemp, "%02x ", m_Main->m_SendPacketBuffer1[i]);
            msgTX1 += szTemp;
            sprintf(szTemp, "%02x ", m_Main->m_SendPacketBuffer2[i]);
            msgTX2 += szTemp;
        }
        ui->Label_Send_1->setText(msgTX1);
        ui->Label_Send_2->setText(msgTX2);
    }
}


//#############################################################################
//
// timer callback
//
void DialogConfig::TimerDownloadCallback()
{
    m_pTimerDownload->stop();

    if( m_nDownloadStep == 0 )
    {
        m_pDialogMsg = new DialogMsg(m_Sound_Click, this);
        m_pDialogMsg->setDownloadMode();
        m_pDialogMsg->move(274, 259);
        m_pDialogMsg->setMessage("로그 데이터 다운로드 진행 중...");
        m_pDialogMsg->show();
        ++m_nDownloadStep;
        m_nDownloadSubstep = 0;
    }
    else if( m_nDownloadStep == 1 )
    {
        QString strCmd;
        QString fileName;
        QString strValue;
        int nValue = 0;
        bool bCopy = true;
        if( m_nDownloadSubstep == 0 )
        {
            if( m_nDownloadMode == 0 )
            {
                QModelIndex index = m_Selections.at(m_nDownloadIndex);

                fileName = ui->table_Log->item(index.row(), 0)->text();
                m_pDialogMsg->setMessage2(fileName);
            }
            else if( m_nDownloadMode == 1 )
            {
                fileName = ui->table_Log->item(m_nDownloadIndex, 0)->text();
                strValue = fileName.mid(4, 2);
                nValue = strValue.toInt();
                if( nValue != m_nDownloadMonth )
                    bCopy = false;
                else
                    m_pDialogMsg->setMessage2(fileName);
            }
            else
            {
                fileName = ui->table_Log->item(m_nDownloadIndex, 0)->text();
                strValue = fileName.mid(6, 2);
                nValue = strValue.toInt();
                if( nValue != m_nDownloadDay )
                    bCopy = false;
                else
                    m_pDialogMsg->setMessage2(fileName);
            }

            if( bCopy == true )
            {
                strCmd = "sudo cp " + QApplication::applicationDirPath() + "/log/" + fileName + " " + m_szDownloadPath + "/" + fileName;
                system(strCmd.toStdString().c_str());
            }
            ++m_nDownloadIndex;
            float fProgress = (static_cast<float>(m_nDownloadIndex) / static_cast<float>(m_nDownloadCount)) * 100;
            m_pDialogMsg->setProgress(static_cast<int>(fProgress));
            if( m_nDownloadIndex >= m_nDownloadCount )
                ++m_nDownloadStep;

            m_nDownloadSubstep = 1;
        }
        else
        {
            system("sync && sync");
            m_nDownloadSubstep = 0;
        }
    }
    else if( m_nDownloadStep == 2 )
    {
        //system("sync && sync");
        system("sudo umount /dev/sda1");
        ++m_nDownloadStep;
    }
    else if( m_nDownloadStep == 3 )
    {
        m_pDialogMsg->hide();
        m_pDialogMsg->close();
        delete m_pDialogMsg;
        ++m_nDownloadStep;
    }
    else if( m_nDownloadStep == 4 )
    {
        DialogMsg dlgMsg(m_Sound_Click, this);
        dlgMsg.setModal(true);
        dlgMsg.move(274, 259);

        dlgMsg.setMessage("로그 데이터 백업을 완료하였습니다.");
        dlgMsg.exec();

        ui->Frame_Download->setVisible(false);
        ui->Frame_Log->setVisible(true);

        return;
    }

    m_pTimerDownload->start(100);
}



//#############################################################################
//
//
void DialogConfig::SwitchMode()
{
    ui->Frame_Env->setVisible(false);
    ui->Frame_Group->setVisible(false);
    ui->Frame_Wheel->setVisible(false);
    ui->Frame_Log->setVisible(false);
    ui->Frame_Comm->setVisible(false);
    ui->Frame_Update->setVisible(false);
    ui->Frame_Download->setVisible(false);

    if(m_nCurrentMode == CONFIG_MODE_ENV )
        ui->Frame_Env->setVisible(true);
    else if(m_nCurrentMode == CONFIG_MODE_GROUP )
        ui->Frame_Group->setVisible(true);
    else if(m_nCurrentMode == CONFIG_MODE_WHEEL )
        ui->Frame_Wheel->setVisible(true);
    else if(m_nCurrentMode == CONFIG_MODE_LOG )
        ui->Frame_Log->setVisible(true);
    else if(m_nCurrentMode == CONFIG_MODE_COMM )
        ui->Frame_Comm->setVisible(true);
    else if(m_nCurrentMode == CONFIG_MODE_UPDATE )
        ui->Frame_Update->setVisible(true);
}


//#############################################################################
//
//
void DialogConfig::EnableButton(bool bSet)
{
    ui->Button_Env->setEnabled(bSet);
    ui->Button_Group->setEnabled(bSet);
    ui->Button_Wheel->setEnabled(bSet);
    ui->Button_PDT->setEnabled(bSet);
    ui->Button_Log->setEnabled(bSet);
    ui->Button_Comm->setEnabled(bSet);
    ui->Button_Update->setEnabled(bSet);
    ui->Button_Exit->setEnabled(bSet);
}


//#############################################################################
//
//
void DialogConfig::on_Button_Env_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nOrgBrightness = m_nBrightness;
    m_nOrgVolume = m_nVolume;

    ui->label_GroupNo->setText(m_Main->getGroupNo());
    ui->label_Version->setText(m_Main->getVersion());
    ui->label_Wheel1->setText(QString::number(m_Main->GetTacho1DIA()));
    ui->label_Wheel2->setText(QString::number(m_Main->GetTacho2DIA()));
    m_nCurrentMode = CONFIG_MODE_ENV;
    SwitchMode();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Group_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Frame_Env->setVisible(false);
    ui->Frame_Group->setVisible(false);
    ui->Frame_Log->setVisible(false);
    ui->Frame_Comm->setVisible(false);
    ui->Frame_Update->setVisible(false);
    ui->Frame_Download->setVisible(false);

    char *szPwd = m_Main->getPasswrod();
    EnableButton(false);

    DialogPassword dlg(m_Sound_Click, this);
    dlg.setModal(true);
    dlg.SetPassword(szPwd);

    dlg.move(380, 114);
    if( dlg.exec() == QDialog::Accepted )
    {
        char* szGroupNo = m_Main->getGroupNo();
        memcpy(m_szEnter, szGroupNo, 2);
        m_nCount = 2;

        ui->label_G1->setText(QString(szGroupNo[0]));
        ui->label_G2->setText(QString(szGroupNo[1]));
        m_nCurrentMode = CONFIG_MODE_GROUP;
        SwitchMode();
        EnableButton(true);
    }
    else
    {
        EnableButton(true);
        return;
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_PDT_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_Main != nullptr )
    {
        m_Main->SetPDTRequest1();
        m_Main->SetPDTRequest2();
    }

    m_pTimer->stop();

    QDialog::accept();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    int nCount = 0;

    //ui->table_Log->clear();
    ui->table_Log->setRowCount(0);

    //
    //
    QDir dir(QApplication::applicationDirPath() + "/log/");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name|QDir::Reversed);
    foreach(QFileInfo item, dir.entryInfoList())
    {
        if( item.isFile() )
        {
            nCount = ui->table_Log->rowCount();
            ui->table_Log->insertRow(nCount);
            ui->table_Log->setItem(nCount, 0, new QTableWidgetItem(item.fileName()));
            QString str;
            str.sprintf("%'d Kb", static_cast<int>(item.size() / 1000) + 1);
            QTableWidgetItem* newItem = new QTableWidgetItem(str);
            newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->table_Log->setItem(nCount, 1, newItem);
        }
    }
    nCount = ui->table_Log->rowCount();
    ui->label_log_file_no->setText(QString::number(nCount));

    struct statvfs vfs;
    double dSize = 0;
    QString strSize;

    statvfs("/", &vfs);
    dSize = static_cast<double>(vfs.f_bavail*vfs.f_bsize)/(1024*1024*1024);
    strSize.sprintf("%.1f GB", dSize);
    ui->label_log_storage_Value->setText(strSize);

    if( nCount > 0 )
        ui->table_Log->selectRow(0);
    m_bSelectAll = false;

    m_nCurrentMode = CONFIG_MODE_LOG;
    SwitchMode();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Comm_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nCurrentMode = CONFIG_MODE_COMM;
    SwitchMode();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Update_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Frame_Env->setVisible(false);
    ui->Frame_Group->setVisible(false);
    ui->Frame_Log->setVisible(false);
    ui->Frame_Comm->setVisible(false);
    ui->Frame_Update->setVisible(false);
    ui->Frame_Download->setVisible(false);

    char *szPwd = m_Main->getPasswrod();
    EnableButton(false);

    DialogPassword dlgPwd(m_Sound_Click, this);
    dlgPwd.setModal(true);
    dlgPwd.SetPassword(szPwd);

    dlgPwd.move(380, 114);
    if( dlgPwd.exec() != QDialog::Accepted )
    {
        EnableButton(true);
        return;
    }

    m_nCurrentMode = CONFIG_MODE_UPDATE;
    SwitchMode();

    DialogMsg dlgMsg(m_Sound_Click, this);
    dlgMsg.setModal(true);
    dlgMsg.move(274, 259);

    DialogUpdate dlg(m_Sound_Click, this);
    dlg.setModal(true);

    dlg.move(312, 259);
    int dlgResult = dlg.exec();
    if( dlgResult == QDialog::Accepted )
    {
        if( access("/tmp/usb", F_OK) == -1)
        {
            if( mkdir("/tmp/usb", 0777) != 0 )
            {
                dlgMsg.setMessage("USB 접근용 임시폴더 생성에 실패했습니다.");
                dlgMsg.exec();
                EnableButton(true);
                return;
            }
        }

        dlgMsg.setMessage("소프트웨어 업데이트용 USB를 삽입하십시오.");
        dlgMsg.exec();
        system("sudo mount /dev/sda1 /tmp/usb");
        system("sync && sync");

        bool bCopy = false;
        if( access("/tmp/usb/mmi_busan", F_OK) == 0)
        {
            system("sudo rm /home/debian/mmi/mmi_busan");
            system("sync && sync");
            system("sudo cp /tmp/usb/mmi_busan /home/debian/mmi/mmi_busan");
            system("sync && sync");
            bCopy = true;
        }
        if( access("/tmp/usb/Message.xml", F_OK) == 0)
        {
            if( access("/home/debian/mmi/Message.xml", F_OK) == 0 )
            {
                system("sudo rm /home/debian/mmi/Message.xml");
                system("sync && sync");
            }
            system("sudo cp /tmp/usb/Message.xml /home/debian/mmi/Message.xml");
            bCopy = true;
        }
        if( access("/tmp/usb/DIA1", F_OK) == 0)
        {
            if( access("/home/debian/mmi/DIA1", F_OK) == 0 )
            {
                system("sudo rm /home/debian/mmi/DIA1");
                system("sync && sync");
            }
            system("sudo cp /tmp/usb/DIA1 /home/debian/mmi/DIA1");
            bCopy = true;
        }
        if( access("/tmp/usb/DIA2", F_OK) == 0)
        {
            if( access("/home/debian/mmi/DIA2", F_OK) == 0 )
            {
                system("sudo rm /home/debian/mmi/DIA2");
                system("sync && sync");
            }
            system("sudo cp /tmp/usb/DIA2 /home/debian/mmi/DIA2");
            bCopy = true;
        }
        if( access("/tmp/usb/PASSWORD", F_OK) == 0)
        {
            if( access("/home/debian/mmi/PASSWORD", F_OK) == 0 )
            {
                system("sudo rm /home/debian/mmi/PASSWORD");
                system("sync && sync");
            }
            system("sudo cp /tmp/usb/PASSWORD /home/debian/mmi/PASSWORD");
            bCopy = true;
        }

        if( bCopy == false )
        {
            dlgMsg.setMessage("USB 인식 실패 혹은 소프트웨어 확인 실패");
            dlgMsg.exec();
            EnableButton(true);
            return;
        }
        system("sync && sync");
        system("sudo umount /dev/sda1");

        dlgMsg.setMessage("소프트웨어 복사 완료. 시스템 재기동 후 적용됩니다.");
        dlgMsg.exec();
    }
    EnableButton(true);
}


//#############################################################################
//
//
void DialogConfig::on_Button_Exit_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_pTimer->stop();

    QDialog::accept();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Bright_Up_clicked()
{
    char szCmd[200];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nBrightness < 5 )
    {
        ++m_nBrightness;
        ui->Label_Bright->setText(QString::number(m_nBrightness));

        sprintf(szCmd, "sudo echo %d > /sys/devices/platform/backlight/backlight/backlight/brightness", m_nBrightness + 4);
        system(szCmd);
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Bright_Down_clicked()
{
    char szCmd[200];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nBrightness > 1 )
    {
        --m_nBrightness;
        ui->Label_Bright->setText(QString::number(m_nBrightness));

        sprintf(szCmd, "sudo echo %d > /sys/devices/platform/backlight/backlight/backlight/brightness", m_nBrightness + 4);
        system(szCmd);
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Volumn_Up_clicked()
{
    char szCmd[100];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nVolume < 10 )
    {
        ++m_nVolume;
        ui->Label_Volumn->setText(QString::number(m_nVolume));

        sprintf(szCmd, "pacmd set-sink-volume 0 %d", m_nVolume * 10 * 650);
        system(szCmd);

        sprintf(szCmd, "echo %d > %s/VOLUME", m_nVolume, qPrintable(QApplication::applicationDirPath()));
        system(szCmd);
        system("sync && sync");

        if( m_Sound_1 != nullptr )
        {
            m_Sound_1->stop();
            m_Sound_1->play();
        }
    }

}


//#############################################################################
//
//
void DialogConfig::on_Button_Volumn_Down_clicked()
{
    char szCmd[100];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nVolume > 1 )
    {
        --m_nVolume;
        ui->Label_Volumn->setText(QString::number(m_nVolume));

        sprintf(szCmd, "pacmd set-sink-volume 0 %d", m_nVolume * 10 * 650);
        system(szCmd);

        sprintf(szCmd, "echo %d > %s/VOLUME", m_nVolume, qPrintable(QApplication::applicationDirPath()));
        system(szCmd);
        system("sync && sync");

        if( m_Sound_1 != nullptr )
        {
            m_Sound_1->stop();
            m_Sound_1->play();
        }
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Wheel_clicked()
{
    char* szPwd = m_Main->getPasswrod();

    DialogPassword dlg(m_Sound_Click, this);
    dlg.setModal(true);
    dlg.SetPassword(szPwd);

    dlg.move(380, 114);
    if( dlg.exec() == QDialog::Accepted )
    {

        if( m_Main != nullptr )
        {
            m_nTachoDIA1Index = m_Main->m_nTachoDIA1Index;
            m_nTachoDIA2Index = m_Main->m_nTachoDIA2Index;
            m_nTachoDIA1Value = m_Main->m_nTachoDIA1Value;
            m_nTachoDIA2Value = m_Main->m_nTachoDIA2Value;

            ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
            ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));

            ui->label_Tacho1_Confirm->setText(QString::number(860 - ((m_Main->m_ActivePacket.DIA1 - 1) * 5)));
            ui->label_Tacho2_Confirm->setText(QString::number(860 - ((m_Main->m_ActivePacket.DIA2 - 1) * 5)));
        }

        m_nCurrentMode = CONFIG_MODE_WHEEL;
        SwitchMode();
    }
}



//#############################################################################
//
//
void DialogConfig::on_Button_Log_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_bSelectAll == true )
        return;

    QModelIndexList selection = ui->table_Log->selectionModel()->selectedRows();
    if( selection.count() == 0 )
        return;

    int nIndex = selection.at(0).row();
    if( nIndex > 0 )
    {
        ui->table_Log->selectionModel()->select(selection.at(0), QItemSelectionModel::Deselect);
        ui->table_Log->selectRow(nIndex - 1);
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_SelectAll_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->table_Log->clearSelection();
    ui->table_Log->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->table_Log->selectAll();
    m_bSelectAll = true;
/*
    int nCount = ui->table_Log->rowCount();
    if( nCount > 0 )
    {
        for(int i = 0; i < nCount; ++i )
            ui->table_Log->selectRow(i);
    }
*/
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_bSelectAll == true )
        return;

    QModelIndexList selection = ui->table_Log->selectionModel()->selectedRows();
    if( selection.count() == 0 )
        return;

    int nCount = ui->table_Log->rowCount();
    int nIndex = selection.at(0).row();
    if( nIndex < nCount - 1)
    {
        ui->table_Log->selectionModel()->select(selection.at(0), QItemSelectionModel::Deselect);
        ui->table_Log->selectRow(nIndex + 1);
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_UnselectAll_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->table_Log->clearSelection();
    ui->table_Log->setSelectionMode(QAbstractItemView::SingleSelection);

    int nCount = ui->table_Log->rowCount();
    if( nCount > 0 )
        ui->table_Log->selectRow(0);

    m_bSelectAll = false;
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_Download_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nDownloadMode = 0;
    ui->radioButton_1->setChecked(true);
    m_nDownloadMonth = 1;
    ui->Label_Month->setText("1월");
    m_nDownloadDay = 1;
    ui->Label_Day->setText("1일");

    ui->Frame_Log->setVisible(false);
    ui->Frame_Download->setVisible(true);
    /*
    m_Selections = ui->table_Log->selectionModel()->selectedRows();
    if( m_Selections.count() == 0 )
        return;

    DialogMsg dlgMsg(m_Sound_Click, this);
    dlgMsg.setModal(true);
    dlgMsg.move(274, 259);

    if( access("/tmp/usb", F_OK) == -1)
    {
        if( mkdir("/tmp/usb", 0777) != 0 )
        {
            dlgMsg.setMessage("USB 접근용 임시폴더 생성에 실패했습니다.");
            dlgMsg.exec();
            return;
        }
    }

    dlgMsg.setMessage("로그 데이터 백업용 USB를 삽입하십시오.");
    dlgMsg.exec();
    dlgMsg.close();

    if( access("/dev/sda1", F_OK) == -1)
    {
        dlgMsg.setMessage("USB 인식에 실패했습니다.");
        dlgMsg.exec();
        return;
    }

    system("sudo mount /dev/sda1 /tmp/usb");
    system("sync && sync");

    bool bAccess = false;
    QDateTime dt = QDateTime::currentDateTime();

    sprintf(m_szDownloadPath, "/tmp/usb/log_%02d_%04d%02d%02d",
            m_nGroupNo, dt.date().year(), dt.date().month(), dt.date().day());
    if( access(m_szDownloadPath, F_OK) == -1)
    {
        if( mkdir(m_szDownloadPath, 0777) == 0 )
            bAccess = true;
    }
    else
    {
        bAccess = true;
    }
    if( bAccess == false )
    {
        dlgMsg.setMessage("USB 인식에 실패했습니다.");
        dlgMsg.exec();
        return;
    }

    m_nDownloadStep = 0;
    m_nDownloadSubstep = 0;
    m_nDownloadCount = m_Selections.count();
    m_nDownloadIndex = 0;
    m_pTimerDownload->start(100);
    */

    /*
        if( m_Main != nullptr )
        {
            m_Main->DisplayTimerCallback();
            m_Main->CommTimerCallback();
            m_Main->ReceiveSerialCallback();
            m_Main->ReceiveSerialCallback2();
        }

    QString strCmd;
    for(int i = 0; i < selection.count(); ++i )
    {
        QModelIndex index = selection.at(i);

        QString fileName = ui->table_Log->item(index.row(), 0)->text();
        dlgMsg.setMessage(fileName);

        //strCmd = "sudo cp " + QApplication::applicationDirPath() + "/log/" + fileName + " /tmp/usb/log/" + fileName;
        system(strCmd.toStdString().c_str());
    }

    system("sync && sync");
    system("sudo umount /dev/sda1");

    dlgMsg.setMessage("로그 데이터 백업을 완료하였습니다.");
    dlgMsg.exec();
    */
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_Delete_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    char *szPwd = m_Main->getPasswrod();

    DialogPassword dlg(m_Sound_Click, this);
    dlg.setModal(true);
    dlg.SetPassword(szPwd);

    dlg.move(380, 114);
    if( dlg.exec() == QDialog::Accepted )
    {
        QModelIndexList selection = ui->table_Log->selectionModel()->selectedRows();
        if( selection.count() == 0 )
            return;

        QDir dir(QApplication::applicationDirPath() + "/log/");
        for(int i = selection.count() - 1; i >= 0; --i )
        {
            QModelIndex index = selection.at(i);

            QString fileName = ui->table_Log->item(index.row(), 0)->text();
            dir.remove(fileName);
            ui->table_Log->removeRow(index.row());
        }
    }
}


//#############################################################################
//
//
void DialogConfig::on_Button_Log_DeleteAll_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    char *szPwd = m_Main->getPasswrod();

    DialogPassword dlg(m_Sound_Click, this);
    dlg.setModal(true);
    dlg.SetPassword(szPwd);

    dlg.move(380, 114);
    if( dlg.exec() == QDialog::Accepted )
    {
        QDir dir(QApplication::applicationDirPath() + "/log/");
        for(int i = ui->table_Log->rowCount() - 1; i >= 0; --i )
        {
            QString fileName = ui->table_Log->item(i, 0)->text();
            dir.remove(fileName);
        }
        ui->table_Log->setRowCount(0);
    }
}




//#############################################################################
//
//
void DialogConfig::on_Button_Env_OK_clicked()
{
    char szCmd[100];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    sprintf(szCmd, "echo %d > %s/BRIGHTNESS", m_nBrightness, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");

    sprintf(szCmd, "echo %d > %s/VOLUME", m_nVolume, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}


//#############################################################################
//
//
void DialogConfig::on_Button_Env_Cancel_clicked()
{
    char szCmd[200];

    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nBrightness = m_nOrgBrightness;
    ui->Label_Bright->setText(QString::number(m_nBrightness));
    sprintf(szCmd, "sudo echo %d > /sys/devices/platform/backlight/backlight/backlight/brightness", m_nBrightness + 4);
    system(szCmd);

    m_nVolume = m_nOrgVolume;
    ui->Label_Volumn->setText(QString::number(m_nVolume));
    sprintf(szCmd, "pacmd set-sink-volume 0 %d", m_nVolume * 10 * 650);
    system(szCmd);

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}

void DialogConfig::on_Button_Enter_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    char szCmd[100];
    m_szEnter[2] = 0x00;
    sprintf(szCmd, "echo %s > %s/GROUPNO", m_szEnter, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");
    m_Main->setGroupNo(m_szEnter);

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}

void DialogConfig::on_Button_Cancel_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}


void DialogConfig::AddInput(char cInput)
{
    if( m_nCount < 2 )
    {
        m_szEnter[m_nCount] = cInput;
        if( m_nCount == 0 )
            ui->label_G1->setText(QString(cInput));
        else if( m_nCount == 1 )
            ui->label_G2->setText(QString(cInput));

        ++m_nCount;
        if( m_nCount == 2 )
            ui->Button_Enter->setVisible(true);
    }
}

void DialogConfig::on_Button_1_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('1');
}

void DialogConfig::on_Button_2_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('2');
}

void DialogConfig::on_Button_3_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('3');
}

void DialogConfig::on_Button_4_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('4');
}

void DialogConfig::on_Button_5_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('5');
}

void DialogConfig::on_Button_6_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('6');
}

void DialogConfig::on_Button_7_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('7');
}

void DialogConfig::on_Button_8_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('8');
}

void DialogConfig::on_Button_9_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('9');
}

void DialogConfig::on_Button_0_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    AddInput('0');
}

void DialogConfig::on_Button_Del_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nCount > 0 )
    {
        m_szEnter[m_nCount] = 0x00;
        if( m_nCount == 2 )
            ui->label_G2->setText("");
        else if( m_nCount == 1 )
            ui->label_G1->setText("");

        --m_nCount;
    }
    ui->Button_Enter->setVisible(false);
}

///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void DialogConfig::SetTacho1DIA(int nDIA)
{
    m_nTachoDIA1Value = nDIA;
    m_nTachoDIA1Index = 17 - ((m_nTachoDIA1Value - 780) / 5);
}


///#########################################################################################
///
/// <summary>
///
/// </summary>
///
void DialogConfig::SetTacho2DIA(int nDIA)
{
    m_nTachoDIA2Value = nDIA;
    m_nTachoDIA2Index = 17 - ((m_nTachoDIA2Value - 780) / 5);
}


void DialogConfig::on_Button_Tacho1_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA1Value > 780 )
    {
        SetTacho1DIA(m_nTachoDIA1Value - 5);
        ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
    }

}

void DialogConfig::on_Button_Tacho1_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA1Value < 860 )
    {
        SetTacho1DIA(m_nTachoDIA1Value + 5);
        ui->label_Tacho1_Set->setText(QString::number(m_nTachoDIA1Value));
    }
}

void DialogConfig::on_Button_Tacho2_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA2Value > 780 )
    {
        SetTacho2DIA(m_nTachoDIA2Value - 5);
        ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
    }
}

void DialogConfig::on_Button_Tacho2_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nTachoDIA2Value < 860 )
    {
        SetTacho2DIA(m_nTachoDIA2Value + 5);
        ui->label_Tacho2_Set->setText(QString::number(m_nTachoDIA2Value));
    }
}

void DialogConfig::on_Button_Wheel_OK_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_Main->m_nTachoDIA1Index = m_nTachoDIA1Index;
    m_Main->m_nTachoDIA2Index = m_nTachoDIA2Index;
    m_Main->m_nTachoDIA1Value = m_nTachoDIA1Value;
    m_Main->m_nTachoDIA2Value = m_nTachoDIA2Value;

    char szCmd[100];
    sprintf(szCmd, "echo %d > %s/DIA1", m_nTachoDIA1Value, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    sprintf(szCmd, "echo %d > %s/DIA2", m_nTachoDIA2Value, qPrintable(QApplication::applicationDirPath()));
    system(szCmd);
    system("sync && sync");

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}

void DialogConfig::on_Button_Wheel_Cancel_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    m_nCurrentMode = CONFIG_MODE_NONE;
    SwitchMode();
}

void DialogConfig::on_Button_Download_Cancel_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Frame_Download->setVisible(false);
    ui->Frame_Log->setVisible(true);
}

void DialogConfig::on_Button_Download_OK_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    DialogMsg dlgMsg(m_Sound_Click, this);
    dlgMsg.setModal(true);
    dlgMsg.move(274, 259);

    if( m_nDownloadMode == 0 )
    {
        m_Selections = ui->table_Log->selectionModel()->selectedRows();
        if( m_Selections.count() == 0 )
        {
            dlgMsg.setMessage("다운로드할 로그 파일이 선택되지 않았습니다.");
            dlgMsg.exec();

            ui->Frame_Download->setVisible(false);
            ui->Frame_Log->setVisible(true);
            return;
        }
    }

    if( access("/tmp/usb", F_OK) == -1)
    {
        if( mkdir("/tmp/usb", 0777) != 0 )
        {
            dlgMsg.setMessage("USB 접근용 임시폴더 생성에 실패했습니다.");
            dlgMsg.exec();
            return;
        }
    }

    dlgMsg.setMessage("로그 데이터 백업용 USB를 삽입하십시오.");
    dlgMsg.exec();
    dlgMsg.close();

    if( access("/dev/sda1", F_OK) == -1)
    {
        dlgMsg.setMessage("USB 인식에 실패했습니다.");
        dlgMsg.exec();
        return;
    }

    system("sudo mount /dev/sda1 /tmp/usb");
    system("sync && sync");

    bool bAccess = false;
    QDateTime dt = QDateTime::currentDateTime();

    sprintf(m_szDownloadPath, "/tmp/usb/log_%02d_%04d%02d%02d",
            m_nGroupNo, dt.date().year(), dt.date().month(), dt.date().day());
    if( access(m_szDownloadPath, F_OK) == -1)
    {
        if( mkdir(m_szDownloadPath, 0777) == 0 )
            bAccess = true;
    }
    else
    {
        bAccess = true;
    }
    if( bAccess == false )
    {
        dlgMsg.setMessage("USB 인식에 실패했습니다.");
        dlgMsg.exec();
        return;
    }

    if( m_nDownloadMode == 0 )
    {
        m_nDownloadCount = m_Selections.count();
    }
    else
    {
        m_nDownloadCount = ui->table_Log->rowCount();
    }

    m_nDownloadStep = 0;
    m_nDownloadSubstep = 0;
    m_nDownloadIndex = 0;
    m_pTimerDownload->start(100);

}

void DialogConfig::on_Button_Month_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nDownloadMonth < 12 )
    {
        ++m_nDownloadMonth;
        ui->Label_Month->setText(QString::number(m_nDownloadMonth) + "월");
    }
}

void DialogConfig::on_Button_Month_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nDownloadMonth > 1 )
    {
        --m_nDownloadMonth;
        ui->Label_Month->setText(QString::number(m_nDownloadMonth) + "월");
    }
}

void DialogConfig::on_Button_Day_Up_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nDownloadDay < 31 )
    {
        ++m_nDownloadDay;
        ui->Label_Day->setText(QString::number(m_nDownloadDay) + "일");
    }
}

void DialogConfig::on_Button_Day_Down_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    if( m_nDownloadDay > 1 )
    {
        --m_nDownloadDay;
        ui->Label_Day->setText(QString::number(m_nDownloadDay) + "일");
    }
}

void DialogConfig::on_radioButton_1_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Button_Month_Up->setEnabled(false);
    ui->Button_Month_Down->setEnabled(false);
    ui->Button_Day_Up->setEnabled(false);
    ui->Button_Day_Down->setEnabled(false);

    m_nDownloadMode = 0;
}

void DialogConfig::on_radioButton_2_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Button_Month_Up->setEnabled(true);
    ui->Button_Month_Down->setEnabled(true);
    ui->Button_Day_Up->setEnabled(false);
    ui->Button_Day_Down->setEnabled(false);

    m_nDownloadMode = 1;
}

void DialogConfig::on_radioButton_3_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    ui->Button_Month_Up->setEnabled(false);
    ui->Button_Month_Down->setEnabled(false);
    ui->Button_Day_Up->setEnabled(true);
    ui->Button_Day_Down->setEnabled(true);

    m_nDownloadMode = 2;
}
