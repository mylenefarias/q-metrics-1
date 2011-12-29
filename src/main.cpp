#include "loader.h"

void PrintHelp(void);

int main(int argc, char *argv[])
{
    if(argc < 4){
        PrintHelp();
    }

    string fN  = argv[1];
    int  sx    = atoi(argv[2]); /// Numero de colunas
    int  sy    = atoi(argv[3]); /// Numero de linhas
    int  yuv   = atoi(argv[4]);
    float DMOS = atof(argv[5]);

    Loader loadedFile(fN,sx,sy,yuv);

    loadedFile.callDebug();
    loadedFile.writeCodebook("codebook.txt",DMOS);


    int frame_atual = 1;
    int total_frames = loadedFile.getTotalFrameNr();

    while(true){

        loadedFile.showFrame(frame_atual);

        int c = cvWaitKey(20);
        if((char)c==27)                                        /// Tecla ESC
            break;
        if(((char)c==104) && frame_atual > 1)                  /// Tecla 'h'
            frame_atual--;
        if(((char)c==108) && (frame_atual < (total_frames-1))) /// Tecla 'l'
            frame_atual++;

    }


    return 0;
}

void PrintHelp()
{
    printf("usage: qmetrics [options] [file] \n"
           "  -h --help : print this help \n"
           "  \n"
           "  \n");
}
