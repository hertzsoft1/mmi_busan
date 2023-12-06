#include <QtWidgets>

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>

#include "touchtranslator.h"

TouchTranslator::TouchTranslator()
{

}

bool TouchTranslator::eventFilter(QObject* obj, QEvent* event)
{
    if( obj == nullptr )
        return QObject::eventFilter(obj, event);
    if( event == nullptr )
        return QObject::eventFilter(obj, event);

    switch(event->type())
    {
    case QEvent::TouchBegin:
        break;
    case QEvent::TouchUpdate:
    {
        const QTouchEvent* touch = static_cast<QTouchEvent*>(event);
        const QList<QTouchEvent::TouchPoint> touchPoints = touch->touchPoints();
        for(const QTouchEvent::TouchPoint &touchPoint : touchPoints)
        {
            //qDebug() << "Touch event: " << touchPoint.state();
            if( touchPoint.state() == Qt::TouchPointPressed)
            {
                //qDebug() << "Send fake press event: " << touchPoint.startPos();
                //QPointF p = touchPoint.pos();
                //p.setX(p.x() + 10);
                //p.setY(p.y() + 10);
                QMouseEvent fake(QEvent::MouseButtonPress, touchPoint.pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QApplication::sendEvent(obj, static_cast<QEvent*>(&fake));
                return true;
            }
        }
        break;
    }
    case QEvent::TouchEnd:
    {
        const QTouchEvent* touch = static_cast<QTouchEvent*>(event);
        const QList<QTouchEvent::TouchPoint> touchPoints = touch->touchPoints();
        for(const QTouchEvent::TouchPoint &touchPoint : touchPoints)
        {
            //qDebug() << "Touch event: " << touchPoint.state();
            if( touchPoint.state() == Qt::TouchPointReleased)
            {
                //qDebug() << "Send fake release event: " << touchPoint.startPos();
                //QPointF p = touchPoint.pos();
                //p.setX(p.x() + 10);
                //p.setY(p.y() + 10);
                QMouseEvent fake(QEvent::MouseButtonRelease, touchPoint.pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QApplication::sendEvent(obj, static_cast<QEvent*>(&fake));
                return true;
            }
        }
        break;
    }
    /*
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    {
        const QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
        qDebug() << "Mouse event: " << mouse->type();
        qDebug() << "Mouse pos: " << mouse->pos();
        break;
    }
    */
    default:
        return QObject::eventFilter(obj, event);
    }

    return QObject::eventFilter(obj, event);
}
