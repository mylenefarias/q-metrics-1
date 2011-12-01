#include <QtCore/QCoreApplication>
#include <QStringList>

#include "loader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList      arguments;

    arguments = QCoreApplication::arguments();

    QString fN  = arguments.at(1);
    int     sx  = arguments.at(2).toInt();
    int     sy  = arguments.at(3).toInt();
    int     yuv = arguments.at(4).toInt();

#if 0
    ///retirando o qt
    char * fN  = argv[1];
    int  sx    = atoi(argv[2]); /// Numero de colunas
    int  sy    = atoi(argv[3]); /// Numero de linhas
    int  yuv   = atoi(argv[4]);
#endif

    Loader loadedFile(fN,sx,sy,yuv);

    loadedFile.callDebug();
    loadedFile.callMetrics();

    a.quit();
    return 0;
}
