#include <QtGui/QApplication>
#include "mumblepahelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MumblePAHelper w;
    w.show();

    return a.exec();
}
