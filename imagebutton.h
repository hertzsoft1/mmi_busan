#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QPushButton>

class ImageButton : public QPushButton
{
    Q_OBJECT

public:
    ImageButton(const QString& text, QWidget* parent = nullptr);
    ImageButton(QWidget* parent = nullptr);

    void setEnabled(bool bEnabled)              { m_bEnabled = bEnabled; update(); }
    void setPixmap(const QPixmap& pm)           { m_pixmap = pm; update(); }
    void setPixmapDisable(const QPixmap& pm)    { m_pixmapDisable = pm; update(); }
protected:
    void paintEvent(QPaintEvent* e);

    bool m_bEnabled = true;
    QPixmap m_pixmap;
    QPixmap m_pixmapDisable;
};

#endif // IMAGEBUTTON_H
