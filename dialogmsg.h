#ifndef DIALOGMSG_H
#define DIALOGMSG_H

#include <QDialog>
#include <QSound>

namespace Ui {
class DialogMsg;
}

class DialogMsg : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMsg(QSound* sound = nullptr, QWidget *parent = nullptr);
    ~DialogMsg();

    void setMessage(QString msg);
    void setMessage2(QString msg);
    void setProgress(int nValue);
    void setDownloadMode();

private slots:
    void on_Button_OK_clicked();

private:
    Ui::DialogMsg *ui;
    QSound* m_Sound = nullptr;

    bool m_bDownloadMode = false;
};

#endif // DIALOGMSG_H
