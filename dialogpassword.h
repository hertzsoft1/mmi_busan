#ifndef DIALOGPASSWORD_H
#define DIALOGPASSWORD_H

#include <QDialog>
#include <QSound>

#include "mainwindow.h"

namespace Ui {
class DialogPassword;
}

class DialogPassword : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPassword(QSound* sound = nullptr, QWidget *parent = nullptr);
    ~DialogPassword();

    void SetMainWindow(MainWindow* main);
    void SetPassword(char* szPasswd)    { memcpy(m_szPassword, szPasswd, 5); }

private slots:
    void on_Button_Enter_clicked();
    void on_Button_Del_clicked();
    void on_Button_1_clicked();
    void on_Button_2_clicked();
    void on_Button_3_clicked();
    void on_Button_4_clicked();
    void on_Button_5_clicked();
    void on_Button_6_clicked();
    void on_Button_7_clicked();
    void on_Button_8_clicked();
    void on_Button_9_clicked();
    void on_Button_0_clicked();

    void on_Button_Cancel_clicked();

    void on_Button_Not1_clicked();

private:
    Ui::DialogPassword *ui;
    MainWindow* m_Main = nullptr;
    QSound* m_Sound = nullptr;

    char m_szPassword[100];
    char m_szEnter[10];
    int m_nCount = 0;

    void AddInput(char cInput);
};

#endif // DIALOGPASSWORD_H
