#include "main_win.h"

#include <QApplication>
#include <QDebug>

struct ParaInfo {
    QString strFilePath;
    QString strDstDir;

    bool IsValid() {
        return !strFilePath.isEmpty() && !strDstDir.isEmpty();
    }
};

void ParseArgs(int argc, char* argv[], ParaInfo& pi)
{
    for (int i=1; i<argc; ++i)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            pi.strFilePath = argv[i + 1];
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            pi.strDstDir = argv[i+1];
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ParaInfo pi;
    ParseArgs(argc, argv, pi);
	main_win w(pi.strFilePath, pi.strDstDir);

	if (!pi.IsValid())
	{
        w.show();
        qDebug() << "ModelConvertor.exe -f xxx.skp -d X:/xxx/";
	}
    else
    {
        w.ConvertSkp2Cim(pi.strFilePath);
    }
	return a.exec();
}
