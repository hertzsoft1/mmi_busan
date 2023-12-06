#include "dialogmsg.h"
#include "ui_dialogmsg.h"


//#############################################################################
//
//
DialogMsg::DialogMsg(QSound* sound, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogMsg)
{
    ui->setupUi(this);
    m_Sound = sound;

    int w = ui->Button_OK->width();
    int h = ui->Button_OK->height();
    QPixmap pix_Bg(":/image/image/Button2.png");
    ui->Button_OK->setPixmap(pix_Bg.scaled(w, h));

    ui->Label_Msg_2->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
}


//#############################################################################
//
//
DialogMsg::~DialogMsg()
{
    delete ui;
}


//#############################################################################
//
//
void DialogMsg::setMessage(QString msg)
{
    ui->Label_Msg->setText(msg);
}


void DialogMsg::setMessage2(QString msg)
{
    ui->Label_Msg_2->setText(msg);
}

void DialogMsg::setProgress(int nValue)
{
    ui->progressBar->setValue(nValue);
}

void DialogMsg::setDownloadMode()
{
    m_bDownloadMode = true;
    ui->Button_OK->setVisible(false);
    ui->Label_Msg_2->setVisible(true);
    ui->progressBar->setVisible(true);
}


//#############################################################################
//
//
void DialogMsg::on_Button_OK_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();

    QDialog::accept();
    QDialog::close();
}
