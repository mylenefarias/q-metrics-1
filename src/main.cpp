#include "loader.h"

int main(int argc, char *argv[])
{

    if(argc < 4){
        printf("Argumentos invalidos \n");
    }

    string fN  = argv[1];
    int  sx    = atoi(argv[2]); /// Numero de colunas
    int  sy    = atoi(argv[3]); /// Numero de linhas
    int  yuv   = atoi(argv[4]);

    Loader loadedFile(fN,sx,sy,yuv);

    loadedFile.callDebug();
    loadedFile.callMetrics();

    return 0;
}
