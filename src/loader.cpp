#include "loader.h"

using namespace std;

Loader::Loader(string fN, int sX, int sY, int yuv)
    :fName(fN),sizeX(sX),sizeY(sY),format(yuv)
{
    long length;

    file    = fopen(fName.c_str(),"rb");
    if((length = getFileSize(file)) == -1){
        std::cout << "Erro ao ler tamanho do arquivo" << std::endl;
    }

    if     (format == _400)     yuvbuffersize = sizeX*sizeY;
    else if(format == _420)     yuvbuffersize = 3*sizeX*sizeY/2;
    else if(format == _422)     yuvbuffersize = 2*sizeX*sizeY;
    else if(format == _444)     yuvbuffersize = 3*sizeX*sizeY;

    total_frame_nr = length/yuvbuffersize;

    //uchar * ybuf = new uchar[total_frame_nr*sizeX*sizeY];
    ybuf   = new uchar[total_frame_nr*sizeX*sizeY];
    vector<cv::Mat> video(total_frame_nr);

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        fseek(file,yuvbuffersize*frame_nr,SEEK_SET);
        fread((char*)ybuf+(sizeX*sizeY*frame_nr), sizeX*sizeY,1,file);

        video[frame_nr] = cv::Mat(cv::Size(sizeX,sizeY), CV_8UC1, ybuf);
    }

    fclose(file);
}

Loader::~Loader()
{
}


long Loader::getFileSize(FILE * hFile)
{
    long lCurPos, lEndPos;
    /// Checa a validade do ponteiro
    if ( hFile == NULL )
    {
        return -1;
    }
    /// Guarda a posicao atual
    lCurPos = ftell ( hFile );
    /// Move para o fim e pega a posicao
    fseek (hFile,0,SEEK_END);
    lEndPos = ftell ( hFile );
    /// Restaura a posicao anterior
    fseek ( hFile, lCurPos, 0 );
    return lEndPos;
}


void    Loader::learnCodebook(string fCodebook)
{
    FILE * codebook;

    codebook = fopen(fCodebook.c_str(),"a");


//    for(int i = 0; i < bookFrameSize; ++i){

//        blockingVlachos(frameY.at(i));
//        blurringWinkler(frameY.at(i));

//    }



    //fprintf(codebook,"%f,%f,%f,%f\n",









}


void   Loader::callDebug() {

}

void   Loader::callMetrics() {
}

int    Loader::getTotalFrameNr() const
{
    return this->total_frame_nr;
}

void   Loader::setSize(int sx, int sy)
{
    this->sizeX = sx;
    this->sizeY = sy;
}
