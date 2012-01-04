#include "loader.h"

Loader::Loader(string fN, int sX, int sY, int yuv)
    :fName(fN),sizeX(sX),sizeY(sY),format(yuv)
{
    long length;
    int  yuvbuffersize;

    if     (format == _400)     yuvbuffersize = sizeX*sizeY;
    else if(format == _420)     yuvbuffersize = 3*sizeX*sizeY/2;
    else if(format == _422)     yuvbuffersize = 2*sizeX*sizeY;
    else if(format == _444)     yuvbuffersize = 3*sizeX*sizeY;
    else{
        printf("Formato de YUV invalido \n");
        exit(1);
    }

    file    = fopen(fName.c_str(),"rb");
    if((length = getFileSize(file)) == -1){
        std::cout << "Erro ao ler tamanho do arquivo" << std::endl;
    }

    std::cout << "Tamanho do arquivo: " << length << std::endl;


    total_frame_nr = length/yuvbuffersize;

    std::cout << "Numero total de frames: " << total_frame_nr << std::endl;

    /// @todo Arrumar um jeito melhor de carregar o arquivo yuv
    uchar * ybuf = new uchar[total_frame_nr*sizeX*sizeY];

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        fseek(file,yuvbuffersize*frame_nr,SEEK_SET);
        if(fread((uchar*)(ybuf+(sizeX*sizeY*frame_nr)), sizeX*sizeY,1,file) == 0){
            printf("Problema ao ler o arquivo .yuv \n");
        }
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
    cv::imshow("Frame",frameY.at(i));
    return;
}

void    Loader::writeCodebook(string fCodebook,float DMOS,int frames_in_word,int word_sizeX,int word_sizeY)
{
    FILE * codebook;

    codebook = fopen(fCodebook.c_str(),"a");
    std::cout << "Escrevendo no codebook ... " << fCodebook << std::endl;

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
                fprintf(codebook,"%f;%f;%f;\n",DMOS,pool_frame(buffer_blocking),pool_frame(buffer_blurring));
                buffer_blocking.clear();
                buffer_blurring.clear();
            }
        }

    }
    fclose(codebook);
}

double   Loader::predictMOS(string fCodebook,int K){

    FILE * codebook;
    codebook = fopen(fCodebook.c_str(),"r");

    int number_samples_vectors  = 1;
    int number_training_vectors = count_lines(codebook);

    int number_features = 2;
    int number_outputs  = 1;

    float * sample_vectors  = new float[number_samples_vectors  * number_features];
    float * feature_vectors = new float[number_training_vectors * number_features];
    float * output_vectors  = new float[number_training_vectors * number_outputs];

    /// Faz a leitura do arquivo codebook para o algoritmo de k-NN
    for(int i = 0; i < number_training_vectors; ++i){

        fscanf(codebook,
               "%f;%f;%f;\n",
               &output_vectors[i],
               &feature_vectors[i], /// blocking
               &feature_vectors[i + number_training_vectors]); /// blurring

    }

    /// Calcula as features do video aberto
    /// @todo terminar de calcular as features localmente
    sample_vectors[0] = blockingVlachos(frameY.at(0));
    sample_vectors[1] = blurringWinkler(frameY.at(0));

    cv::Mat sampleData(number_samples_vectors,number_features,CV_32FC1,sample_vectors);
    cv::Mat trainData(number_training_vectors,number_features,CV_32FC1,feature_vectors);
    cv::Mat outputData(number_training_vectors,1,CV_32FC1,output_vectors);

    cv::KNearest knn(trainData,outputData,cv::Mat(),true,K);
    float predicted_mos = knn.find_nearest(sampleData,K);
    printf("Valor predito de MOS usando kNN: %f com k = %d\n",predicted_mos,K);

    delete [] output_vectors;
    delete [] feature_vectors;
    delete [] sample_vectors;

    return predicted_mos;
}



void   Loader::callDebug() {
}

void   Loader::callMetrics() {
}

int    Loader::getTotalFrameNr() const
{
    return this->total_frame_nr;
}

///=============================================================================
/// Funcoes privadas
///=============================================================================
double   Loader::pool_frame(vector<double> v)
{
    return mean<double>(v);
}

int  Loader::count_lines(FILE * codebook)
{
    int lines = 0;
    while (EOF != (fscanf(codebook,"%*[^\n]"), fscanf(codebook,"%*c")))
      ++lines;
    rewind(codebook);
    return lines;
}
