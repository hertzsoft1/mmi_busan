#include "mainwindow.h"
#include <QApplication>

#include "touchtranslator.h"

static TouchTranslator translator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlag(Qt::FramelessWindowHint);
    w.move(0, 0);
    //w.setWindowState(Qt::WindowFullScreen);
    w.show();

    a.installEventFilter(&translator);
    w.installEventFilter(&translator);
    /*
    const QList<QObject*> objects = w.findChildren<QObject *>();
    for(QObject* object : objects)
    {
        object->installEventFilter(&translator);
    }
    */

    return a.exec();
}
