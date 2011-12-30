#include "loader.h"

using namespace std;

Loader::Loader(string fN, int sX, int sY, int yuv)
    :fName(fN),sizeX(sX),sizeY(sY),format(yuv)
{
    long length;
    int  yuvbuffersize;

    file    = fopen(fName.c_str(),"rb");
    if((length = getFileSize(file)) == -1){
        std::cout << "Erro ao ler tamanho do arquivo" << std::endl;
    }

    std::cout << "Tamanho do arquivo: " << length << std::endl;

    if     (format == _400)     yuvbuffersize = sizeX*sizeY;
    else if(format == _420)     yuvbuffersize = 3*sizeX*sizeY/2;
    else if(format == _422)     yuvbuffersize = 2*sizeX*sizeY;
    else if(format == _444)     yuvbuffersize = 3*sizeX*sizeY;

    total_frame_nr = length/yuvbuffersize;

    std::cout << "Numero total de frames: " << total_frame_nr << std::endl;

    /// @todo Arrumar um jeito melhor de carregar o arquivo yuv
    uchar * ybuf = new uchar[total_frame_nr*sizeX*sizeY];

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        fseek(file,yuvbuffersize*frame_nr,SEEK_SET);
        fread((uchar*)(ybuf+(sizeX*sizeY*frame_nr)), sizeX*sizeY,1,file);

        frameY.push_back(cv::Mat(cv::Size(sizeX,sizeY), CV_8UC1, (ybuf+(sizeX*sizeY*frame_nr))));
    }

    //delete ybuf;
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

void    Loader::showFrame(int i)
{
    //    stringstream title;
    //    title << "Frame ";
    //    title << i;
    //    title << "/";
    //    title << total_frame_nr;

    //    cv::imshow(title.str(),frameY.at(i));

    cv::imshow("Frame",frameY.at(i));
    return;
}

void    Loader::writeCodebook(string fCodebook,float DMOS,int frames_in_word,int word_sizeX,int word_sizeY)
{
    FILE * codebook;

    codebook = fopen(fCodebook.c_str(),"a");
    std::cout << "Escrevendo no codebook ... " << fCodebook << std::endl;

    /// Palpites iniciais para divisao de blocos
    /// ------
    /// Para o caso do banco de dados da LIVE, todos os videos sao 768x432;
    /// 768 = 2⁸ x 3
    /// 432 = 2⁴ x 3³
    /// Como gcd(768,432) = 48, vamos tentar dividir em (16x9) blocos de 48x48
    /// ------
    /// Os videos tem 217,250 ou 500 frames;
    /// Tentaremos com 50 frames para 250 ou 500 e 31 para 217.

    assert((total_frame_nr % frames_in_word) == 0);
    assert((sizeX % word_sizeX) == 0);
    assert((sizeY % word_sizeY) == 0);

    vector<double> buffer_blocking(frames_in_word);
    vector<double> buffer_blurring(frames_in_word);

    for(int i = 0; i < (total_frame_nr/frames_in_word); ++i){

        for(int m = 0; m < (sizeX/word_sizeX); ++m){
            for(int n = 0; n < (sizeY/word_sizeY); ++n){

                for(int j = 0; j < frames_in_word; ++j){
                    cv::Mat word = (frameY.at((i*frames_in_word)+j))(cv::Rect(word_sizeX*m,word_sizeY*n,word_sizeX,word_sizeY));
                    buffer_blocking.push_back(blockingVlachos(word));
                    buffer_blurring.push_back(blurringWinkler(word));
                }
                fprintf(codebook,"%f;%f;%f;\n",DMOS,mean(buffer_blocking),mean(buffer_blurring));
                buffer_blocking.clear();
                buffer_blurring.clear();
            }
        }

    }
    fclose(codebook);
}
double   Loader::mean(vector<double> v)
{
    double s = 0;
    for(unsigned int i = 0; i < v.size(); ++i)
        s += v.at(i);
    s /= v.size();
    return s;
}


void   Loader::callDebug() {
}

void   Loader::callMetrics() {
}

int    Loader::getTotalFrameNr() const
{
    return this->total_frame_nr;
}

