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

/// @note Este prototipo da funcao eh temporario, ainda muito baguncado
void    Loader::writeCodebook(string fCodebook,float DMOS)
{
    FILE * codebook;

    codebook = fopen(fCodebook.c_str(),"a");
    std::cout << "Escrevendo no codebook ... " << fCodebook << std::endl;

    /// Palpites iniciais
    /// ------
    /// Para o caso do banco de dados da LIVE, todos os videos sao 768x432;
    /// 768 = 2⁸ x 3
    /// 432 = 2⁴ x 3³
    /// Como gcd(768,432) = 48, vamos tentar dividir em (16x9) blocos de 48x48
    /// ------
    /// Os videos tem 217,250 ou 500 frames;
    /// Tentaremos com 10 frames no comeco;

    int block_frames = 10;
    int block_sizeX  = 48;
    int block_sizeY  = 48;

    assert((total_frame_nr % block_frames) == 0);
    assert((sizeX % block_sizeX) == 0);
    assert((sizeY % block_sizeY) == 0);

    vector<double> buffer_blocking(block_frames);
    vector<double> buffer_blurring(block_frames);

    for(int i = 0; i < (total_frame_nr/block_frames); ++i){

        for(int m = 0; m < (sizeX/block_sizeX); ++m){
            for(int n = 0; n < (sizeY/block_sizeY); ++n){

                for(int j = 0; j < block_frames; ++j){
                    cv::Mat block = (frameY.at((i*block_frames)+j))(cv::Rect(block_sizeX*m,block_sizeY*n,block_sizeX,block_sizeY));
                    buffer_blocking.push_back(blockingVlachos(block));
                    buffer_blurring.push_back(blurringWinkler(block));
                }
                fprintf(codebook,"%f;%f;%f;\n",DMOS,mean(buffer_blocking),mean(buffer_blurring));
                buffer_blocking.clear();
                buffer_blurring.clear();
            }
        }

    }

    fclose(codebook);

    //fprintf(codebook,"%f\n",blockingVlachos(block));
    //fprintf(codebook,"%f\n",blurringWinkler(block));

    //fprintf(codebook,"%f\n",blockingVlachos(frameY.at(i)));
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

