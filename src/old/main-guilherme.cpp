#include "loader.h"
#include "limits.h"

void PrintHelp(void);

int main(int argc, char *argv[])
{
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

    }else if(mode == "metrics"){

        float DMOS = atof(argv[6]);

        //loadedFile.callMetrics();
        loadedFile.callMetrics2(DMOS);

    }else if(mode == "video"){

        int frame_atual = 0;
        int total_frames = loadedFile.getTotalFrameNr();

        while(true){
            loadedFile.callDebug(frame_atual);

            int c = cvWaitKey(20);
            if((char)c==27)                                        /// Tecla ESC
                break;
            if(((char)c==104) && frame_atual > 1)                  /// Tecla 'h'
                frame_atual--;
            if(((char)c==108) && (frame_atual < (total_frames-1))) /// Tecla 'l'
                frame_atual++;
            if(((char)c==106))                                     /// Tecla 'j'
                loadedFile.degradeFrame(frame_atual);
        }
    }else if(mode == "degrade"){
        int number = 1;
        int videonumber = atoi(argv[6]);
		char numstr[21];
		std::string output1(argv[7]);
		std::string output;

        for (number = 1;number <= videonumber; ++number){
			sprintf(numstr, "%d.yuv", number);
			output = output1 + numstr;
			loadedFile.degradeVideo(output);
			if(number!=videonumber)
				Loader loadedFile(output,sx,sy,yuv);
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
           "mode: train,predict,video,metrics \n"
           "\t train: write a codebook  \n"
           "\t predict: predict a MOS using codebook \n"
           "\t video: show the video \n"
           "\t metrics: call the metrics \n"
           "\t degrade: generate a video with defects \n"
           "file: path to the .yuv file \n"
           "width: width of the video \n"
           "height: height of the video \n"
           "YUV: 400,420,422,444 \n"
           "mode options: specific options for the mode \n");
}
