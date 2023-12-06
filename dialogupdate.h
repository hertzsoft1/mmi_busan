#ifndef DIALOGUPDATE_H
#define DIALOGUPDATE_H

#include <QDialog>
#include <QSound>

namespace Ui {
class DialogUpdate;
}

class DialogUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUpdate(QSound* sound = nullptr, QWidget *parent = nullptr);
    ~DialogUpdate();

private slots:
    void on_Button_Yes_clicked();
    void on_Button_No_clicked();

private:
    Ui::DialogUpdate *ui;
    QSound* m_Sound = nullptr;
};

#endif // DIALOGUPDATE_H
