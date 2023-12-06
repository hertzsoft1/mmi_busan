#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>
#include "mainwindow.h"
#include "dialogmsg.h"

namespace Ui {
class DialogConfig;
}

#define CONFIG_MODE_NONE    0
#define CONFIG_MODE_ENV     1
#define CONFIG_MODE_GROUP   2
#define CONFIG_MODE_WHEEL   3
#define CONFIG_MODE_PDT     4
#define CONFIG_MODE_LOG     5
#define CONFIG_MODE_COMM    6
#define CONFIG_MODE_UPDATE  7

class DialogConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfig(QWidget *parent = nullptr, int nVolume = 5, int nBrightness = 3);
    ~DialogConfig();

    void SetGroupNo(int nGroupNo)       { m_nGroupNo = nGroupNo; }
    void SetVolume(int nVolume)         { m_nVolume = nVolume; }
    int GetVolume()                     { return m_nVolume; }

    void SetBrightness(int nBrightness) { m_nBrightness = nBrightness; }
    int GetBrightness()                 { return m_nBrightness; }

    void SetMainWindow(MainWindow* main);
    void SetSoundClick(QSound* sound)   { m_Sound_Click = sound; }
    void SetSoundVolume(QSound* sound)  { m_Sound_1 = sound; }

public slots:
    void TimerCallback();
    void TimerDownloadCallback();

private slots:
    void on_Button_Exit_clicked();

    void on_Button_Bright_Up_clicked();
    void on_Button_Bright_Down_clicked();

    void on_Button_Volumn_Up_clicked();
    void on_Button_Volumn_Down_clicked();

    void on_Button_Comm_clicked();
    void on_Button_Wheel_clicked();
    void on_Button_PDT_clicked();
    void on_Button_Log_clicked();
    void on_Button_Update_clicked();

    /*
    void on_Button_Tacho1_Down_clicked();
    void on_Button_Tacho1_Up_clicked();
    void on_Button_Tacho2_Down_clicked();
    void on_Button_Tacho2_Up_clicked();
    void on_Button_Wheel_OK_clicked();
    */

    void on_Button_Log_Up_clicked();
    void on_Button_Log_SelectAll_clicked();
    void on_Button_Log_Down_clicked();
    void on_Button_Log_UnselectAll_clicked();
    void on_Button_Log_Download_clicked();
    void on_Button_Log_Delete_clicked();
    void on_Button_Log_DeleteAll_clicked();

    void on_Button_Env_clicked();
    void on_Button_Env_OK_clicked();
    void on_Button_Env_Cancel_clicked();

    void on_Button_Group_clicked();

    void on_Button_Enter_clicked();
    void on_Button_Cancel_clicked();
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
    void on_Button_Del_clicked();

    void on_Button_Tacho1_Down_clicked();
    void on_Button_Tacho1_Up_clicked();
    void on_Button_Tacho2_Down_clicked();
    void on_Button_Tacho2_Up_clicked();
    void on_Button_Wheel_OK_clicked();
    void on_Button_Wheel_Cancel_clicked();

    void on_Button_Download_Cancel_clicked();
    void on_Button_Download_OK_clicked();
    void on_Button_Month_Up_clicked();
    void on_Button_Month_Down_clicked();
    void on_Button_Day_Up_clicked();
    void on_Button_Day_Down_clicked();
    void on_radioButton_1_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();

private:
    void SwitchMode();
    void EnableButton(bool bSet);

private:
    Ui::DialogConfig *ui;
    MainWindow* m_Main = nullptr;
    DialogMsg* m_pDialogMsg = nullptr;

    QSound* m_Sound_Click = nullptr;
    QSound* m_Sound_1 = nullptr;

    QTimer *m_pTimer;
    QTimer *m_pTimerDownload;

    QPixmap m_pix_Config;

    int m_nGroupNo = 0;

    int m_nDownloadMode = 0;
    int m_nDownloadMonth = 1;
    int m_nDownloadDay = 1;

    QModelIndexList m_Selections;
    int m_nDownloadStep = 0;
    int m_nDownloadSubstep = 0;
    int m_nDownloadCount = 0;
    int m_nDownloadIndex = 0;
    char m_szDownloadPath[200];

    int m_nCurrentMode = CONFIG_MODE_NONE;

    int m_nBrightness = 3;
    int m_nVolume = 5;

    int m_nOrgBrightness = 0;
    int m_nOrgVolume = 0;

    int m_nTachoDIA1Index = 8;
    int m_nTachoDIA2Index = 8;
    int m_nTachoDIA1Value = 820;
    int m_nTachoDIA2Value = 820;

    int m_nCount = 0;
    char m_szEnter[10];

    bool m_bSelectAll = false;

    void AddInput(char cInput);

    void SetTacho1DIA(int nDIA);
    void SetTacho2DIA(int nDIA);

};

#endif // DIALOGCONFIG_H
