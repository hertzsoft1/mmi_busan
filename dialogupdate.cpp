#include "dialogupdate.h"
#include "ui_dialogupdate.h"

DialogUpdate::DialogUpdate(QSound* sound, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow),
    ui(new Ui::DialogUpdate)
{
    ui->setupUi(this);
    m_Sound = sound;

    int w = ui->Button_Yes->width();
    int h = ui->Button_Yes->height();
    QPixmap pix_Bg(":/image/image/Button2.png");
    ui->Button_Yes->setPixmap(pix_Bg.scaled(w, h));

    w = ui->Button_No->width();
    h = ui->Button_No->height();
    ui->Button_No->setPixmap(pix_Bg.scaled(w, h));
}

DialogUpdate::~DialogUpdate()
{
    delete ui;
}

void DialogUpdate::on_Button_Yes_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();

    QDialog::accept();
}

void DialogUpdate::on_Button_No_clicked()
{
    if( m_Sound != nullptr )
        m_Sound->play();

    QDialog::reject();
}
