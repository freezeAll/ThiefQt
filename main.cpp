#include <QApplication>
#include <QPushButton>
#include "novelwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NovelWindow w;
    w.show();
    return QApplication::exec();
}
