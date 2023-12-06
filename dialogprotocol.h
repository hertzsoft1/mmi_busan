#ifndef DIALOGPROTOCOL_H
#define DIALOGPROTOCOL_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class DialogProtocol;
}

class DialogProtocol : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProtocol(QWidget *parent = nullptr);
    ~DialogProtocol();

    void SetMainWindow(MainWindow* main);

private slots:
    void on_Button_Exit_clicked();
    void TimerCallback();

private:
    Ui::DialogProtocol *ui;
    MainWindow* m_Main = nullptr;
    QTimer *m_pTimer;
};

#endif // DIALOGPROTOCOL_H
