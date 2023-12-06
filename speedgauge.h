#ifndef SPEEDGAUGE_H
#define SPEEDGAUGE_H

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
#include <QFontDatabase>

class QDESIGNER_WIDGET_EXPORT SpeedGauge : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int LIMIT_SPEED READ getLimitSpeed WRITE setLimitSpeed)
    Q_PROPERTY(int CURRENT_SPEED READ getCurrentSpeed WRITE setCurrentSpeed)
    Q_PROPERTY(bool ON_01 READ getOn01 WRITE setOn01)
    Q_PROPERTY(bool ON_02 READ getOn02 WRITE setOn02)

public:
    int getLimitSpeed()                 { return m_nLimitSpeed; }
    void setLimitSpeed(int nSpeed)      { m_nLimitSpeed = nSpeed; update(); }
    int getCurrentSpeed()               { return m_nCurrentSpeed; }
    void setCurrentSpeed(int nSpeed)    { m_nCurrentSpeed = nSpeed; update(); }

    void setSpeedCode(int nCode)        { if( nCode != m_nSpeedCode ) { m_nSpeedCode = nCode; update(); } }

    bool getYardMode()                  { return m_bYardMode; }
    void setYardMode(bool bOn)          { m_bYardMode = bOn; update(); }
    bool getOn01()                      { return m_bOn01; }
    void setOn01(bool bOn)              { m_bOn01 = bOn; update(); }
    bool getOn02()                      { return m_bOn02; }
    void setOn02(bool bOn)              { m_bOn02 = bOn; update(); }

    // 속도 초과 시 제한속도 표시 깜빡거림. (2023.03.21)
    //
    bool getOverSpeed()                 { return m_bOverSpeed; }
    void setOverSpeed(bool bFlag);

public:
    explicit SpeedGauge(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

protected:
    int m_nLimitSpeed = 0;
    int m_nCurrentSpeed = 0;

    int m_nSpeedCode = 0;

    bool m_bYardMode = false;

    bool m_bOn01 = false;
    bool m_bOn02 = false;

    bool m_bOverSpeed = false;

    bool m_bLoadFont = false;
    QString m_strFontFamily;
};


#endif // SPEEDGAUGE_H
