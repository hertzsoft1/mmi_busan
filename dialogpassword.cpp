#include "dialogpassword.h"
#include "ui_dialogpassword.h"

#include "dialogmsg.h"

#include <QMessageBox>

DialogPassword::DialogPassword(QSound* sound, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogPassword)
{
    ui->setupUi(this);

    m_Sound = sound;

    ui->Button_Enter->setVisible(false);
}

DialogPassword::~DialogPassword()
{
    delete ui;
}

void DialogPassword::on_Button_Enter_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();

    /*
    DialogMsg dlgMsg(this);
    dlgMsg.setModal(true);
    dlgMsg.move(0, 0);

    QMessageBox msgBox;

    if( m_nCount != 4 )
    {
        //dlgMsg.setMessage("비밀번호 4자리를 입력하셔야 합니다.");
        //dlgMsg.exec();
        msgBox.setText("비밀번호 4자리를 입력하셔야 합니다.");
        msgBox.exec();
        return;
    }

    if( m_szPassword[0] == m_szEnter[0] &&
        m_szPassword[1] == m_szEnter[1] &&
        m_szPassword[2] == m_szEnter[2] &&
        m_szPassword[3] == m_szEnter[3] )
    {
        QDialog::accept();
    }
    else
    {
        //dlgMsg.setMessage("비밀번호가 일치하지 않습니다.");
        //dlgMsg.exec();
        msgBox.setText("비밀번호가 일치하지 않습니다.");
        msgBox.exec();
        return;
    }
    */
    QDialog::accept();
}


void DialogPassword::on_Button_Cancel_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();

    QDialog::reject();
}


void DialogPassword::AddInput(char cInput)
{
    if( m_nCount < 4 )
    {
        m_szEnter[m_nCount] = cInput;
        if( m_nCount == 0 )
            ui->label_P1->setText(QString(cInput));
        else if( m_nCount == 1 )
            ui->label_P2->setText(QString(cInput));
        else if( m_nCount == 2 )
            ui->label_P3->setText(QString(cInput));
        else if( m_nCount == 3 )
            ui->label_P4->setText(QString(cInput));

        ++m_nCount;
        if( m_nCount == 4 )
        {
            if( m_szEnter[0] == 'M')
                ui->Button_Enter->setVisible(true);
        }
    }
}

void DialogPassword::on_Button_Del_clicked()
{
    ui->Button_Enter->setVisible(false);

    if( m_Sound != nullptr )
        m_Sound->play();

    if( m_nCount > 0 )
    {
        m_szEnter[m_nCount] = 0x00;
        if( m_nCount == 4 )
            ui->label_P4->setText("");
        else if( m_nCount == 3 )
            ui->label_P3->setText("");
        else if( m_nCount == 2 )
            ui->label_P2->setText("");
        else if( m_nCount == 1 )
            ui->label_P1->setText("");

        --m_nCount;
    }
}

void DialogPassword::on_Button_1_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('1');
}

void DialogPassword::on_Button_2_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('2');
}

void DialogPassword::on_Button_3_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('3');
}

void DialogPassword::on_Button_4_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('4');
}

void DialogPassword::on_Button_5_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('5');
}

void DialogPassword::on_Button_6_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('6');
}

void DialogPassword::on_Button_7_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('7');
}

void DialogPassword::on_Button_8_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('8');
}

void DialogPassword::on_Button_9_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('9');
}

void DialogPassword::on_Button_0_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('0');
}


void DialogPassword::on_Button_Not1_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();
    AddInput('M');
}
