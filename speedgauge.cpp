
#include "speedgauge.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>

#include <math.h>
#include <string>
#include <unistd.h>
#include <QtDebug>

SpeedGauge::SpeedGauge(QWidget *parent)
    : QWidget(parent)
{
    m_nLimitSpeed = 0;
    m_nCurrentSpeed = 0;

    m_bOn01 = false;
    m_bOn02 = false;

    //if( access("./font/DS-DIGII.TTF", F_OK) != -1)
    {
        int id = QFontDatabase::addApplicationFont(":/fonts/font/DS-DIGII.TTF");
        m_strFontFamily = QFontDatabase::applicationFontFamilies(id).at(0);
        //qDebug() << m_strFontFamily;
        m_bLoadFont = true;
    }
}


void SpeedGauge::setOverSpeed(bool bFlag)
{
    if( bFlag != m_bOverSpeed )
    {
        m_bOverSpeed = bFlag;
        update();
    }
}

void SpeedGauge::paintEvent(QPaintEvent *)
{
    int w = width();
    int h = w;
    int cx = w / 2;
    int cy = h / 2;
    double rad;
    int x, y;
    int ex, ey;
    int nAdd = 75;
    int nDoubleAdd = 150;
    int nLimitSpeed[5] { 25, 40, 55, 65, 75};
    int nSpeedCode[5] { 2, 3, 4, 5, 6 };

    QRectF rectOut(20 + nAdd, 20 + nAdd, w - 40 - nDoubleAdd, h - 40 - nDoubleAdd);
    QRectF rectIn(38 + nAdd, 38 + nAdd, w - 76 - nDoubleAdd, h - 76 - nDoubleAdd);
    QRectF rectLine(9 + nAdd, 9 + nAdd, w - 18 - nDoubleAdd, h - 18 - nDoubleAdd);
    QRectF rectLine2(49 + nAdd, 49 + nAdd, w - 98 - nDoubleAdd, h - 98 - nDoubleAdd);

    QPen linePen(Qt::white, 8, Qt::SolidLine, Qt::FlatCap);
    QPen outPen(QColor(85, 0, 0), 15, Qt::SolidLine, Qt::FlatCap);
    QPen outPen2(Qt::red, 15, Qt::SolidLine, Qt::FlatCap);
    QPen innerPen(QColor(0, 50, 0), 15, Qt::SolidLine, Qt::FlatCap);
    QPen innerPen2(Qt::green, 15, Qt::SolidLine, Qt::FlatCap);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if( m_nSpeedCode == 1 || m_nSpeedCode == 7 || m_nSpeedCode == 8 )
        m_bOn01 = true;
    else
        m_bOn01 = false;

    if( m_nSpeedCode == 0 || m_nSpeedCode >= 9 )
        m_bOn02 = true;
    else
        m_bOn02 = false;

    // 제한속도 & 현재 속도 표시
    //
    //int nLimit = m_bYardMode == true ? 25 : m_nLimitSpeed;
    int nLimit = m_nLimitSpeed;
    for(int i = 0; i < 101; ++i)
    {
        if( i <= nLimit )
        {
            painter.setPen(outPen2);
            painter.drawArc(rectOut, -144 * 16 - 40 * i, -2 * 16);
        }
        else
        {
            painter.setPen(outPen);
            painter.drawArc(rectOut, -144 * 16 - 40 * i, -2 * 16);
        }

        if( i <= m_nCurrentSpeed )
        {
            painter.setPen(innerPen2);
            painter.drawArc(rectIn, -144 * 16 - 40 * i, -2 * 16);
        }
        else
        {
            painter.setPen(innerPen);
            painter.drawArc(rectIn, -144 * 16 - 40 * i, -2 * 16);
        }
    }
    painter.setPen(linePen);
    painter.drawArc(rectLine, -143 * 16, -254 * 16);
    painter.drawArc(rectLine2, -143 * 16, -254 * 16);


    // 제한 속도 원 표시
    //
    QPen penWhite(Qt::white, 3, Qt::SolidLine, Qt::FlatCap);
    QPen penGreen(Qt::green, 3, Qt::SolidLine, Qt::FlatCap);
    QPen penRed(Qt::red, 3, Qt::SolidLine, Qt::FlatCap);
    QBrush circleOff(Qt::black);

    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(38);
    painter.setFont(font);

    painter.setBrush(circleOff);
    for(int i = 0; i < 5; ++i)
    {
        rad = ((nLimitSpeed[i] * 2.5) + 145) * M_PI / 180;
        x = static_cast<int>((cx - 30) * cos(rad) + cx);
        y = static_cast<int>((cy - 30) * sin(rad) + cy);
        if( m_nSpeedCode == nSpeedCode[i])
            painter.setPen(penGreen);
        else
            painter.setPen(penWhite);
        painter.drawEllipse(x - 33, y - 33, 66, 66);
        painter.drawText(QRect(x - 33, y - 33, 66, 66), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nLimitSpeed[i]));
    }

    // 01/02 표시
    //
    rad = 132 * M_PI / 180;
    x = static_cast<int>((cx - 105) * cos(rad) + cx);
    y = static_cast<int>((cy - 105) * sin(rad) + cy);
    if( m_bOn01 == true )
        painter.setPen(penRed);
    else
        painter.setPen(penWhite);
    painter.drawEllipse(x - 33, y - 33, 66, 66);
    painter.drawText(QRect(x - 33, y - 33, 66, 66), Qt::AlignCenter | Qt::AlignHCenter, "01");

    rad = 48 * M_PI / 180;
    x = static_cast<int>((cx - 105) * cos(rad) + cx);
    y = static_cast<int>((cy - 105) * sin(rad) + cy);
    if( m_bOn02 == true )
        painter.setPen(penRed);
    else
        painter.setPen(penWhite);
    painter.drawEllipse(x - 33, y - 33, 66, 66);
    painter.drawText(QRect(x - 33, y - 33, 66, 66), Qt::AlignCenter | Qt::AlignHCenter, "02");


    // 내부 눈금
    //
    float fStepDegree = 250 / 20.0f;
    linePen.setWidth(6);
    painter.setPen(linePen);
    for(int i = 0; i <= 20; ++i)
    {
        rad = static_cast<double>(((i * fStepDegree) + 145.0f) * M_PI / 180.0f);
        if((i % 2) == 0)
        {
            x = static_cast<int>((cx - 73 - nAdd) * cos(rad) + cx);
            y = static_cast<int>((cy - 73 - nAdd) * sin(rad) + cy);
        }
        else
        {
            x = static_cast<int>((cx - 65 - nAdd) * cos(rad) + cx);
            y = static_cast<int>((cy - 65 - nAdd) * sin(rad) + cy);
        }
        ex = static_cast<int>((cx - 50 - nAdd) * cos(rad) + cx);
        ey = static_cast<int>((cy - 50 - nAdd) * sin(rad) + cy);

        painter.drawLine(x, y, ex, ey);
    }

    rad = 143.5 * M_PI / 180;
    x = static_cast<int>((cx - 50 - nAdd) * cos(rad) + cx);
    y = static_cast<int>((cy - 50 - nAdd) * sin(rad) + cy);
    ex = static_cast<int>((cx - 10 - nAdd) * cos(rad) + cx);
    ey = static_cast<int>((cy - 10 - nAdd) * sin(rad) + cy);
    painter.drawLine(x, y, ex, ey);

    rad = (143.5 + 253) * M_PI / 180;
    x = static_cast<int>((cx - 50 - nAdd) * cos(rad) + cx);
    y = static_cast<int>((cy - 50 - nAdd) * sin(rad) + cy);
    ex = static_cast<int>((cx - 10 - nAdd) * cos(rad) + cx);
    ey = static_cast<int>((cy - 10 - nAdd) * sin(rad) + cy);
    painter.drawLine(x, y, ex, ey);

    // 속도 문자 표시
    //
    font = painter.font();
    font.setPixelSize(29);
    painter.setFont(font);

    fStepDegree = 250.0f / 5.0f;
    int nAdjustX[6] { -5, -2, 3, 0, 0, -10 };
    int nAdjustY[6] {  5, -5, -10, -10, -5, 5 };
    for(int i = 0; i <= 5; ++i )
    {
        rad = static_cast<double>(((i * fStepDegree) + 145) * M_PI / 180.0f);
        x = static_cast<int>((cx - 175) * cos(rad) + cx);
        y = static_cast<int>((cy - 175 ) * sin(rad) + cy);

        painter.drawText(QRect(x - 30 + nAdjustX[i], y - 15 + nAdjustY[i], 60, 35),
                         Qt::AlignCenter | Qt::AlignHCenter, QString::number(i * 20));
    }

    QPen linePen2(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap);
    painter.setPen(linePen2);

    painter.drawRect(cx - 100, cy - 115, 196, 86);
    painter.drawRect(cx - 100, cy + 25, 196, 86);

    font = painter.font();
    font.setPixelSize(27);
    painter.setFont(font);
    painter.setPen(Qt::white);

    painter.drawText(QRect(cx - 100, cy - 158, 200, 40), Qt::AlignCenter | Qt::AlignHCenter, "제한속도");
    painter.drawText(QRect(cx - 100, cy - 18, 200, 40), Qt::AlignCenter | Qt::AlignHCenter, "현재속도");
    painter.drawText(QRect(cx - 100, cy + 115, 200, 40), Qt::AlignCenter | Qt::AlignHCenter, "km/h");

    painter.save();
    if( m_bLoadFont == true)
    {
        QFont digitFont(m_strFontFamily);
        digitFont.setPixelSize(90);
        painter.setFont(digitFont);
    }

    QColor dimRed(45, 0, 0);
    QColor dimBlue(0, 45, 0);
    int nStep = 176 / 3;

    // 제한 속도 디지탈 표시
    // 속도 초과 시 제한속도 표시 깜빡거림 처리를 위해 초과 시 제한속도 0 (2023.03.21)
    //
    int nSpeed = m_bOverSpeed == true ? 0 : m_nLimitSpeed;
    int nTemp = nSpeed / 100;
    if( nSpeed < 100 )
        nTemp = 8;
    painter.setPen(nSpeed < 100 ? dimRed : Qt::red);
    painter.drawText(QRect(cx - 90, cy - 115, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    nTemp = nSpeed % 100;
    nTemp = nTemp / 10;
    if( nSpeed < 10 )
        nTemp = 8;
    painter.setPen(nSpeed < 10 ? dimRed : Qt::red);
    painter.drawText(QRect(cx - 90 + nStep, cy - 115, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    if( m_bOverSpeed == true )
        nTemp = 8;
    else
        nTemp = nSpeed % 10;
    painter.setPen(m_bOverSpeed == true ? dimRed : Qt::red);
    painter.drawText(QRect(cx - 90 + nStep*2, cy - 115, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    // 현재 속도 디지탈 표시
    //
    nTemp = m_nCurrentSpeed / 100;
    if( m_nCurrentSpeed < 100 )
        nTemp = 8;
    painter.setPen(m_nCurrentSpeed < 100 ? dimBlue : Qt::green);
    painter.drawText(QRect(cx - 90, cy + 25, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    nTemp = m_nCurrentSpeed % 100;
    nTemp = nTemp / 10;
    if( m_nCurrentSpeed < 10 )
        nTemp = 8;
    painter.setPen(m_nCurrentSpeed < 10 ? dimBlue : Qt::green);
    painter.drawText(QRect(cx - 90 + nStep, cy + 25, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    nTemp = m_nCurrentSpeed % 10;
    painter.setPen( Qt::green);
    painter.drawText(QRect(cx - 90 + nStep*2, cy + 25, nStep, 86), Qt::AlignCenter | Qt::AlignHCenter, QString::number(nTemp));

    painter.restore();
}
