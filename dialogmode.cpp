#include "dialogmode.h"
#include "ui_dialogmode.h"

//#############################################################################
//
//
DialogMode::DialogMode(bool bMode, QSound* sound, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogMode)
{
    ui->setupUi(this);
    m_Sound_Click = sound;

    int w = ui->Button_Yes->width();
    int h = ui->Button_Yes->height();
    QPixmap pix_Bg(":/image/image/Button2.png");
    ui->Button_Yes->setPixmap(pix_Bg.scaled(w, h));

    w = ui->Button_No->width();
    h = ui->Button_No->height();
    ui->Button_No->setPixmap(pix_Bg.scaled(w, h));

    m_bRecoveryMode = bMode;
    if( m_bRecoveryMode == true )
        ui->Label_Msg->setText("평상모드로 변경 하시겠습니까?");
    else
        ui->Label_Msg->setText("회복모드로 변경 하시겠습니까?");
}


//#############################################################################
//
//
DialogMode::~DialogMode()
{
    delete ui;
}


//#############################################################################
//
//
void DialogMode::on_Button_Yes_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    //setResult(QDialog::DialogCode::Accepted);
    QDialog::accept();
}


//#############################################################################
//
//
void DialogMode::on_Button_No_clicked()
{
    if( m_Sound_Click != nullptr )
        m_Sound_Click->play();

    //setResult(QDialog::DialogCode::Rejected);
    QDialog::reject();
}
