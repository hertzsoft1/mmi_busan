#include "imagebutton.h"

#include <QPainter>

ImageButton::ImageButton(QWidget* parent)
    : QPushButton (parent)
{

}

ImageButton::ImageButton(const QString& text, QWidget* parent)
    : QPushButton (text, parent)
{

}

void ImageButton::paintEvent(QPaintEvent *)
{
    int w = this->width();
    int h = this->height();

    QPainter painter(this);

    if( m_bEnabled )
    {
        if( this->isDown() )
        {
            painter.drawPixmap(1, 1, m_pixmap);
            painter.drawText(QRect(0, 0, w, h), Qt::AlignCenter | Qt::AlignHCenter, this->text());
        }
        else
        {
            painter.drawPixmap(0, 0, m_pixmap);
            painter.drawText(QRect(0, 0, w, h), Qt::AlignCenter | Qt::AlignHCenter, this->text());
        }
    }
    else
    {
        painter.drawPixmap(0, 0, m_pixmapDisable);
        painter.drawText(QRect(0, 0, w, h), Qt::AlignCenter | Qt::AlignHCenter, this->text());
    }
}
