#ifndef TOUCHTRANSLATOR_H
#define TOUCHTRANSLATOR_H

#include <QObject>

class TouchTranslator : public QObject
{
    Q_OBJECT
public:
    TouchTranslator();

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // TOUCHTRANSLATOR_H
