#include "loader.h"
#include "parser.h"

void PrintHelp(void);

int main(int argc, char *argv[])
{
    /// @todo Modularizar para a classe Parser
    if(argc < 1){
        PrintHelp();
        exit(0);
    }

    string mode = argv[1];        /// Modo de execucao
    string fN   = argv[2];        /// Nome do arquivo .yuv
    int  sx     = atoi(argv[3]);  /// Numero de colunas
    int  sy     = atoi(argv[4]);  /// Numero de linhas
    int  yuv    = atoi(argv[5]);  /// Tipo do arquivo .yuv - Ver definicoes em global.h

    Loader loadedFile(fN,sx,sy,yuv);

    if(mode == "train"){

        float DMOS  = atof(argv[6]); /// DMOS do arquivo .yuv
        int   fw    = atoi(argv[7]); /// Frames em uma codeword
        int   xw    = atoi(argv[8]); /// Numero de colunas de uma codeword
        int   yw    = atoi(argv[9]); /// Numero de linhas de uma codeword

        loadedFile.writeCodebook("codebook.txt",DMOS,fw,xw,yw);

    }else if(mode == "predict"){

        loadedFile.predictMOS("codebook.txt",500);

    }else if(mode == "video"){

        int frame_atual = 0;
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


    }else{
        printf("Opcao Invalida \n");
        PrintHelp();
        exit(0);
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
