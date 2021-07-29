#include "main_win.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString strFilePath = "";
    if (argc > 1)
    {
        strFilePath = QString(argv[1]);
    }

    main_win w(strFilePath);
    w.show();
    return a.exec();
}
