#ifndef DIALOGMODE_H
#define DIALOGMODE_H

#include <QDialog>
#include <QSound>

namespace Ui {
class DialogMode;
}

class DialogMode : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMode(bool bMode, QSound* sound = nullptr, QWidget *parent = nullptr);
    ~DialogMode();

private slots:
    void on_Button_Yes_clicked();
    void on_Button_No_clicked();

private:
    Ui::DialogMode *ui;
    int m_bRecoveryMode = false;

    QSound* m_Sound_Click = nullptr;
};

#endif // DIALOGMODE_H
