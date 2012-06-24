#include "loader.h"

Loader::Loader(string fN, int sX, int sY, int yuv)
    :fName(fN),sizeX(sX),sizeY(sY),format(yuv)
{
    long length;
    int  yuvbuffersize;

    if     (format == 400)     yuvbuffersize = sizeX*sizeY;
    else if(format == 420)     yuvbuffersize = 3*sizeX*sizeY/2;
    else if(format == 422)     yuvbuffersize = 2*sizeX*sizeY;
    else if(format == 444)     yuvbuffersize = 3*sizeX*sizeY;
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

    degrade_iteration = 0;
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
    cv::Mat  frame = frameY.at(i);
    cv::imshow("FRAME",frame);
}

void Loader::degradeFrame(int i)
{
    cv::Mat frame = frameY.at(i);
//    blockingFrame(frame);
      blurringFrame(frame);
//    ringingFrame(frame,-10.0f,RINGING_375ns);
//    noiseWhiteFrame(frame,0,110);

//    DEBUG
//    FILE * f_teste;
//    f_teste = fopen("teste.txt","a");
//    fprintf(f_teste,"%d : %f  \n", degrade_iteration, noise1Farias(frame));
//    fclose(f_teste);

    degrade_iteration += 1;

}

/// @todo melhorar a forma de gerar as componentes U e V do arquivo degradado
/// quando melhorar a forma de carregar as componentes U e V no construtor.
/// Do jeito atual carrega o arquivo de video duas vezes.
void Loader::degradeVideo(string degradedName)
{
    FILE * f_degraded;
    FILE * f_original;

    f_degraded = fopen(degradedName.c_str(),"w");

    // config. para 400
    int yuvbuffersize = sizeX*sizeY;
    int uvbuffersize = 0;

    uchar * uvbuffer;

    if(format != 400){
        f_original = fopen(fName.c_str(),"rb");
        if     (format == 420){
            yuvbuffersize = 3*sizeX*sizeY/2;
            uvbuffersize  = sizeX*sizeY/2;
        }else if(format == 422){
            yuvbuffersize = 2*sizeX*sizeY;
            uvbuffersize  = sizeX*sizeY;
        }else if(format == 444){
            yuvbuffersize = 3*sizeX*sizeY;
            uvbuffersize = 2*sizeX*sizeY;
        }
    }

    uvbuffer = new uchar[total_frame_nr*uvbuffersize];

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        degradeFrame(frame_nr);
        fwrite(frameY.at(frame_nr).data,sizeX*sizeY,1,f_degraded);

        if(format != 400)
        {
            fseek(f_original,sizeX*sizeY*(frame_nr+1),SEEK_SET);
            if(fread((uchar*)(uvbuffer+(uvbuffersize*frame_nr)),uvbuffersize,1,f_original) == 0){
                printf("Problema ao ler o arquivo .yuv \n");
            }
            fwrite(uvbuffer,uvbuffersize,1,f_degraded);
        }
    }

    delete uvbuffer;

    if(format != 400) fclose(f_original);
    fclose(f_degraded);
}

void Loader::dumpFrame(int i)
{
    cv::Mat frame = frameY.at(i);
    FILE * f_dump = fopen("dump","w");
    for(int i = 0; i < frame.rows; ++i){
        for(int j = 0; j < frame.cols; ++j){
            fprintf(f_dump,"%d;",frame.at<uchar>(i,j));
        }
        fprintf(f_dump,"\n");
    }
    fclose(f_dump);
}

void Loader::callDebug(int i)
{
    cv::Mat frame = frameY.at(i);
    cv::imshow("FRAME",frame);
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
    vector<double> buffer_contrast(frames_in_word);
    vector<double> buffer_texture(frames_in_word);

    for(int i = 0; i < (total_frame_nr/frames_in_word); ++i){

        for(int m = 0; m < (sizeX/word_sizeX); ++m){
            for(int n = 0; n < (sizeY/word_sizeY); ++n){

                for(int j = 0; j < frames_in_word; ++j){
                    cv::Mat word = (frameY.at((i*frames_in_word)+j))(cv::Rect(word_sizeX*m,word_sizeY*n,word_sizeX,word_sizeY));
                    cv::Mat temp(word.rows,word.cols,CV_64FC1);

                    buffer_blocking.push_back(blockingVlachos(word));
                    buffer_blurring.push_back(blurringWinkler(word));
                    buffer_contrast.push_back(contrastHess(word,temp));
                    buffer_texture.push_back(textureStd(word));
                }
                fprintf(codebook,"%f;%f;%f;%lf;%f\n",DMOS,pool_frame(buffer_blocking),
                                                          pool_frame(buffer_blurring),
                                                          pool_frame(buffer_contrast),
                                                          pool_frame(buffer_texture));
                buffer_blocking.clear();
                buffer_blurring.clear();
                buffer_contrast.clear();
                buffer_texture.clear();
            }
        }

    }

    fclose(codebook);
}

double   Loader::predictMOS(string fCodebook,int K,int frames_in_word,int word_sizeX,int word_sizeY){

    FILE * codebook;
    codebook = fopen(fCodebook.c_str(),"r");

    assert((total_frame_nr % frames_in_word) == 0);
    assert((sizeX % word_sizeX) == 0);
    assert((sizeY % word_sizeY) == 0);

    int total_words = (total_frame_nr/frames_in_word)*(sizeX/word_sizeX)*(sizeY/word_sizeY);

    vector<double> buffer_MOS(total_words);
    vector<double> buffer_blocking(frames_in_word);
    vector<double> buffer_blurring(frames_in_word);
    vector<double> buffer_contrast(frames_in_word);
    vector<double> buffer_texture(frames_in_word);

    int number_training_vectors = count_lines(codebook);

    int number_features = 4;
    int number_outputs  = 1;

    float * sample_vector  = new float[number_features];
    float * feature_vectors = new float[number_training_vectors * number_features];
    float * output_vectors  = new float[number_training_vectors * number_outputs];

    /// Faz a leitura do arquivo codebook para o algoritmo de K-Nearest Neighbors
    for(int i = 0,f = 0; i < number_training_vectors; ++i,f+=number_features){

        fscanf(codebook,
               "%f;%f;%f;%f;%f\n",
               &output_vectors[i],
               &feature_vectors[f],    /// blocking
               &feature_vectors[f+1],  /// blurring
               &feature_vectors[f+2],  /// contrast
               &feature_vectors[f+3]); /// texture
    }

    cv::Mat trainData(number_training_vectors,number_features,CV_32FC1,feature_vectors);
    cv::Mat outputData(number_training_vectors,1,CV_32FC1,output_vectors);
    cv::KNearest knn(trainData,outputData,cv::Mat(),true,K);

    cv::Mat sampleData(1,number_features,CV_32FC1,sample_vector);
    /// Calcula as features do video aberto
    for(int i = 0; i < (total_frame_nr/frames_in_word); ++i){

        for(int m = 0; m < (sizeX/word_sizeX); ++m){
            for(int n = 0; n < (sizeY/word_sizeY); ++n){

                for(int j = 0; j < frames_in_word; ++j){
                    cv::Mat word = (frameY.at((i*frames_in_word)+j))(cv::Rect(word_sizeX*m,word_sizeY*n,word_sizeX,word_sizeY));
                    cv::Mat temp(word.rows,word.cols,CV_64FC1);

                    buffer_blocking.push_back(blockingVlachos(word));
                    buffer_blurring.push_back(blurringWinkler(word));
                    buffer_contrast.push_back(contrastHess(word,temp));
                    buffer_texture.push_back(textureStd(word));
                }

                sample_vector[0] = pool_frame(buffer_blocking);
                sample_vector[1] = pool_frame(buffer_blurring);
                sample_vector[2] = pool_frame(buffer_contrast);
                sample_vector[3] = pool_frame(buffer_texture);

                buffer_MOS.push_back(knn.find_nearest(sampleData,K));

                buffer_blocking.clear();
                buffer_blurring.clear();
                buffer_contrast.clear();
                buffer_texture.clear();
            }
        }

    }

    float predicted_mos = mean(buffer_MOS);

    printf("Valor predito de MOS usando K-Nearest Neighbors: %f com K = %d\n",predicted_mos,K);

    delete [] output_vectors;
    delete [] feature_vectors;
    delete [] sample_vector;
    fclose(codebook);

    return predicted_mos;
}

/// @todo deixar mais robusta a leitura do .txt da LIVE a partir desta funcao
void   Loader::compareLIVE(double predicted_value)
{
    FILE * live_file = fopen("live_videos_MOS.txt","r");
    FILE * results = fopen("resultados.txt","a");

    char * video_name;
    video_name = (char *) malloc(20 * sizeof(char));
    float DMOS = 0;
    float var  = 0;

    while(true)
    {
        if(fscanf(live_file,"%s ,%f,%f\n",video_name,&DMOS,&var) == EOF){
            break;
        }else{
            if(strcmp(video_name,fName.c_str()) == 0)
            {
                break;
            }
        }

    }

    fprintf(results,"%s,%f,%f,%f\n",fName.c_str(),predicted_value,DMOS,var);

    free(video_name);
    fclose(results);
    fclose(live_file);

}

void   Loader::printFeatures(string fFeatures,int frames_in_word,int word_sizeX,int word_sizeY)
{
    FILE * features;

    features = fopen(fFeatures.c_str(),"a");
    std::cout << "Escrevendo as features no arquivo " << fFeatures << std::endl;

    assert((total_frame_nr % frames_in_word) == 0);
    assert((sizeX % word_sizeX) == 0);
    assert((sizeY % word_sizeY) == 0);

    vector<double> buffer_blocking(frames_in_word);
    vector<double> buffer_blurring(frames_in_word);
    vector<double> buffer_contrast(frames_in_word);
    vector<double> buffer_texture(frames_in_word);

    for(int i = 0; i < (total_frame_nr/frames_in_word); ++i){

        for(int m = 0; m < (sizeX/word_sizeX); ++m){
            for(int n = 0; n < (sizeY/word_sizeY); ++n){

                for(int j = 0; j < frames_in_word; ++j){
                    cv::Mat word = (frameY.at((i*frames_in_word)+j))(cv::Rect(word_sizeX*m,word_sizeY*n,word_sizeX,word_sizeY));
                    cv::Mat temp(word.rows,word.cols,CV_64FC1);

                    buffer_blocking.push_back(blockingVlachos(word));
                    buffer_blurring.push_back(blurringWinkler(word));
                    buffer_contrast.push_back(contrastHess(word,temp));
                    buffer_texture.push_back(textureStd(word));
                }
                fprintf(features,"%f;%f;%lf;%f\n",pool_frame(buffer_blocking),
                                                  pool_frame(buffer_blurring),
                                                  pool_frame(buffer_contrast),
                                                  pool_frame(buffer_texture));
                buffer_blocking.clear();
                buffer_blurring.clear();
                buffer_contrast.clear();
                buffer_texture.clear();
            }
        }

    }

    fclose(features);
}

void   Loader::callMetrics() {
	double avg_blur[4] = {0,0,0,0},min_blur[4] = {1000,1000,1000,1000},max_blur[4] = {-1,-1,-1,-1};
	FILE * f_output;
	string name = fName.substr(0,fName.size() - 4);

	name += ".txt";
    f_output = fopen(name.c_str() ,"w");

    fprintf(f_output,"Frame"
                     "\t"
                    "Winkler"
					"\t\t"
					"Winkler2"
					"\t"
					"CPBD"
					"\t\t"
					"Perceptual"
                     "\n");


	for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
		double blur0 = blurringWinkler(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur1 = blurringWinklerV2(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur2 = blurringCPBD(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur3 = blurringPerceptual(frameY.at(frame_nr));

		fprintf(f_output,"%d\t%f\t%f\t%f\t%f\n",frame_nr,blur0,blur1,blur2,blur3);

		avg_blur[0] += blur0;
		if( blur0 < min_blur[0])	min_blur[0] = blur0;
		else if(blur0 > max_blur[0])	max_blur[0] = blur0;
		avg_blur[1] += blur1;
		if( blur1 < min_blur[1])	min_blur[1] = blur1;
		else if(blur1 > max_blur[1])	max_blur[1] = blur1;
		avg_blur[2] += blur2;
		if( blur2 < min_blur[2])	min_blur[2] = blur2;
		else if(blur2 > max_blur[2])	max_blur[2] = blur2;
		avg_blur[3] += blur3;
		if( blur3 < min_blur[3])	min_blur[3] = blur3;
		else if(blur3 > max_blur[3])	max_blur[3] = blur3;


	}
	fprintf(f_output,"\nAvg =\t"
                    	 "%f\t%f\t%f\t%f"
                   	 "\nMax =\t"
                     	 "%f\t%f\t%f\t%f"
                     	 "\nMin =\t"
                     	 "%f\t%f\t%f\t%f"
                     	 "\n",avg_blur[0]/total_frame_nr,avg_blur[1]/total_frame_nr,avg_blur[2]/total_frame_nr,avg_blur[3]/total_frame_nr, max_blur[0], max_blur[1], max_blur[2], max_blur[3], min_blur[0], min_blur[1], min_blur[2], min_blur[3]);

	fclose(f_output);
}

void   Loader::callMetrics2(float DMOS) {

    FILE * f_output;
    f_output = fopen("teste_blur_bilateral.txt" ,"a");

    vector<double> buffer_Winkler;
    vector<double> buffer_Winkler2;
    vector<double> buffer_CPBD;
    vector<double> buffer_Perceptual;

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        buffer_Winkler.push_back(blurringWinkler(frameY.at(frame_nr), BW_EDGE_BILATERAL, 10, 200, 3));
        buffer_Winkler2.push_back(blurringWinklerV2(frameY.at(frame_nr), BW_EDGE_BILATERAL, 10, 200, 3));
        buffer_CPBD.push_back(blurringCPBD(frameY.at(frame_nr), BW_EDGE_BILATERAL, 10, 200, 3));
        buffer_Perceptual.push_back(blurringPerceptual(frameY.at(frame_nr)));

    }
    fprintf(f_output,"%f;%f;%f;%f;%f\n",median(buffer_Winkler),
                                        median(buffer_Winkler2),
                                        median(buffer_CPBD),
                                        median(buffer_Perceptual),
                                        DMOS);

    fclose(f_output);
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


