#include "loader.h"
#include "parser.h"

void PrintHelp(void);

int main(int argc, char *argv[])
{
    /// @todo Modularizar para a classe Parser
    if(argc < 5){
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

        int   K     = atoi(argv[6]); /// Parametro K do K-NN
        int   fw    = atoi(argv[7]); /// Frames em uma codeword
        int   xw    = atoi(argv[8]); /// Numero de colunas de uma codeword
        int   yw    = atoi(argv[9]); /// Numero de linhas de uma codeword

        float predicted_MOS;
        predicted_MOS = loadedFile.predictMOS("codebook.txt",K,fw,xw,yw);
        loadedFile.compareLIVE(predicted_MOS);

    }else if(mode == "features"){

        int   fw    = atoi(argv[6]); /// Frames em uma codeword
        int   xw    = atoi(argv[7]); /// Numero de colunas de uma codeword
        int   yw    = atoi(argv[8]); /// Numero de linhas de uma codeword
        string s(argv[9]);

        loadedFile.printFeatures(s,fw,xw,yw);

    }else if(mode == "video"){

        int frame_atual = 0;
        int total_frames = loadedFile.getTotalFrameNr();

		loadedFile.callMetrics();

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
        printf("Invalid Option \n");
        PrintHelp();
        exit(0);
    }

    return 0;
}

void PrintHelp()
{
    printf("usage: qmetrics [mode] [file] [width] [height] [YUV] [mode options] \n"
           "mode: train,predict,video \n"
           "\t train: write a codebook  \n"
           "\t predict: predict a MOS using codebook \n"
           "\t video: show the video \n"
           "file: path to the .yuv file \n"
           "width: width of the video \n"
           "height: height of the video \n"
           "YUV: 0,1,2,3 \n"
           "\t 0 for 4:0:0 \n"
           "\t 1 for 4:2:0 \n"
           "\t 2 for 4:2:2 \n"
           "\t 3 for 4:4:4 \n"
           "mode options: specific options for the mode \n");
}
