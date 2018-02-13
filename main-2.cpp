#include "cardmatch.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CardMatch window;
    window.show();
    return a.exec();
}
