#include "loader.h"

void PrintHelp(void);

int main(int argc, char *argv[])
{
    if(argc < 10){
        PrintHelp();
        exit(0);
    }

    string fN  = argv[1];       /// Nome do arquivo .yuv
    int  sx    = atoi(argv[2]); /// Numero de colunas
    int  sy    = atoi(argv[3]); /// Numero de linhas
    int  yuv   = atoi(argv[4]); /// Ver definicoes em global.h
    float DMOS = atof(argv[5]); /// DMOS do arquivo .yuv
    int   fw    = atoi(argv[6]); /// Frames em uma codeword
    int   xw    = atoi(argv[7]); /// Numero de colunas de uma codeword
    int   yw    = atoi(argv[8]); /// Numero de linhas de uma codeword
    int   video = atoi(argv[9]);  /// 1 para ligar o video


    Loader loadedFile(fN,sx,sy,yuv);

    loadedFile.writeCodebook("codebook.txt",DMOS,fw,xw,yw);

    int frame_atual = 0;
    int total_frames = loadedFile.getTotalFrameNr();

    if(video){
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
