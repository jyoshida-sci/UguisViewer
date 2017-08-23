#include "uguisviewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UguisViewer w;
    w.show();

    return a.exec();
}
