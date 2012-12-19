#include "loader.h"
//using namespace std; // prefer using std::string;
using namespace std;
#include <string>

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
    uchar * ybuf = new uchar[total_frame_nr*sizeX*sizeY]; // so Y (onde estao os U e V??)

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        fseek(file,yuvbuffersize*frame_nr,SEEK_SET);
        if(fread((uchar*)(ybuf+(sizeX*sizeY*frame_nr)), sizeX*sizeY,1,file) == 0){
            printf("Problema ao ler o arquivo .yuv \n");
        }
        frameY.push_back(cv::Mat(cv::Size(sizeX,sizeY), CV_8UC1, (ybuf+(sizeX*sizeY*frame_nr))));
    }

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
  // show only Y frame (color??)
    cv::Mat  frame = frameY.at(i);
    cv::imshow("FRAME",frame);
}

void Loader::degradecombineFrame(int i, std::string deg, double scale[3])
{
    /// Read original frame in a format that is easy for OpenCV
    cv::Mat frame = frameY.at(i);
    /// This function creates degraded frames from 
    /// the original frame and combines 
    /// them with different weights
    
    if   (deg == "blockandblur") {
		cv::Mat frame_block = frame.clone(); /// cria uma frame para blocking              
		cv::Mat frame_blur = frame.clone();  /// cria uma frame para blurring
		blockingFrame(frame_block);  /// add blockiness to the blocking frame
		blurringFrame(frame_blur);   /// add blurriness to the blurriness frame
		cv::addWeighted(frame_block, scale[1], frame_blur, scale[2], scale[3], frame); /// combines them
    }
    degrade_iteration += 1;
}

void Loader::degradeFrame(int i, std::string deg)
{
    /// Read original frame in a format that is easy for OpenCV
    cv::Mat frame = frameY.at(i);
    /// This function creates degraded frames from 
    /// the original frame 
    
    if   (deg == "block") {
	  blockingFrame(frame);
    }
    else if   (deg == "blur") {
	  blurringFrame(frame);
    }
    else if   (deg == "ring") {
	  ringingFrame(frame,-10.0f,RINGING_375ns);
    }
    else if   (deg == "noise") {
	  noiseWhiteFrame(frame,0,110);
    }
    degrade_iteration += 1;
}
/// @todo melhorar a forma de gerar as componentes U e V do arquivo degradado
/// quando melhorar a forma de carregar as componentes U e V no construtor.
/// Do jeito atual carrega o arquivo de video duas vezes.
void Loader::degradeVideo(string degradedName, std::string deg)
{
    // files of degraded and original videos
    FILE * f_degraded;
    FILE * f_original;
    // open file for degraded video
    f_degraded = fopen(degradedName.c_str(),"w"); /// open file where degraded video will be stored

    // initial configuration is for 400 (only luma)
    int yuvbuffersize = sizeX*sizeY;
    int uvbuffersize = 0;
    // read format of the video
    uchar * uvbuffer;
    uchar *  ybuffer;
    std::cout << "Formato: " << format << std::endl;
    
    // Set correct frame size for the color format
    if(format != 400){
        f_original = fopen(fName.c_str(),"rb");
        if  (format == 420){
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
    // buffers to store the UV (chroma) and Y (luma) information
    uvbuffer = new uchar[uvbuffersize];
    ybuffer = new uchar[sizeX*sizeY];
    
    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        degradeFrame(frame_nr, deg);
        
        if(format != 400)
        {
            fseek(f_original,yuvbuffersize*(frame_nr),SEEK_SET);
	    /// Read Y
	    if(fread((uchar*)(ybuffer),sizeX*sizeY,1,f_original) == 0){
                printf("Problema ao ler o arquivo .yuv \n");
            }
            //fwrite(ybuffer,sizeX*sizeY,1,f_degraded);
	    fwrite(frameY.at(frame_nr).data,sizeX*sizeY,1,f_degraded);

	    if(fread((uchar*)(uvbuffer),uvbuffersize,1,f_original) == 0){
                printf("Problema ao ler o arquivo .yuv \n");
            }
            fwrite(uvbuffer,uvbuffersize,1,f_degraded);
        }
    }

    delete uvbuffer;
    delete ybuffer;
    ///if(format != 400) /// why??
      ///fclose(f_original);
    fclose(f_degraded);
}
/// Esta função é indêntica a anterior exceto pela chamada a combine ---
/// mas, a  ideia é mudar isto!
void Loader::degradecombineVideo(string degradedName, std::string deg, double scale[3])
{
    // files of degraded and original videos
    FILE * f_degraded;
    FILE * f_original;
    // open file for degraded video
    f_degraded = fopen(degradedName.c_str(),"w"); /// open file where degraded video will be stored

    // initial configuration is for 400 (only luma)
    int yuvbuffersize = sizeX*sizeY;
    int uvbuffersize = 0;
    // read format of the video
    uchar * uvbuffer;
    uchar *  ybuffer;
    std::cout << "Formato: " << format << std::endl;
    
    // Set correct frame size for the color format
    if(format != 400){
        f_original = fopen(fName.c_str(),"rb");
        if  (format == 420){
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
    // buffers to store the UV (chroma) and Y (luma) information
    uvbuffer = new uchar[uvbuffersize];
    ybuffer = new uchar[sizeX*sizeY];
    
    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        degradecombineFrame(frame_nr, deg, scale);
        
        if(format != 400)
        {
            fseek(f_original,yuvbuffersize*(frame_nr),SEEK_SET);
	    /// Read Y
	    if(fread((uchar*)(ybuffer),sizeX*sizeY,1,f_original) == 0){
                printf("Problema ao ler o arquivo .yuv \n");
            }
            //fwrite(ybuffer,sizeX*sizeY,1,f_degraded);
	    fwrite(frameY.at(frame_nr).data,sizeX*sizeY,1,f_degraded);

	    if(fread((uchar*)(uvbuffer),uvbuffersize,1,f_original) == 0){
                printf("Problema ao ler o arquivo .yuv \n");
            }
            fwrite(uvbuffer,uvbuffersize,1,f_degraded);
        }
    }

    delete uvbuffer;
    delete ybuffer;
    ///if(format != 400) /// why??
      ///fclose(f_original);
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

void Loader::callDebug2(int i)
{

   cv::Mat frame = frameY.at(i);
   cv::Mat packetloss = pckerr.at(i);

   cv::imshow("FRAME",frame);
   cv::imshow("PACKETERR",packetloss);
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

	std::string rfscanf = 0;
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

        rfscanf = fscanf(codebook,
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
    double avg_block[2] = {0,0},min_block[2] = {1000,1000},max_block[2] = {-1,-1};
    double avg_packet[3] = {0,0,0},min_packet[3] = {1000,1000,1000},max_packet[3] = {-1,-1,-1};
    //FILE  *f_output; 
    FILE  *f_avg,*f_param;
    string name = fName.substr(0,fName.size() - 4);

    name += ".txt";
    //f_output = fopen(name.c_str() ,"w");//Onde fica os resultados ppar cada frame do video
    f_avg = fopen("Avg_values.txt" , "r+");//Onde fica a media dos resultados para todos os videos testados
    if(f_avg == NULL){
      f_avg = fopen("Avg_values.txt" , "w");
      fprintf(f_avg,"Video Name"
                    "\tWinkler"
		    "\t\tWinkler2"
		    "\tCPBD"
		    "\t\tPerceptual"
                    "\tWang"
                    "\t\tVlachos"
                    "\t\tImpairments"
                    "\tXiaRui"
                    "\t\tBabu"
                     "\n");
    }
    fseek(f_avg,0,2);


    f_param = fopen("Parametros.txt" , "r+");
    if(f_param == NULL){
      f_param = fopen("Parametros.txt" , "w");
      fprintf(f_avg,"Video Name"
                    "\tInter"
		    "\t\tIntra"
		    "\tSpatial Act"
		    "\t\tContrast"
                     "\n");
    }
    fseek(f_param,0,2);

/*
    fprintf(f_output,"Frame"
                    "\tWinkler"
		    "\t\tWinkler2"
		    "\tCPBD"
		    "\t\tPerceptual"
                    "\tWang"
                    "\t\tVlachos"
                    "\t\tImpairments"
                    "\tXiaRui"
                    "\t\tBabu"
                    "\n");
*/
        cv::Mat frameE(sizeX,sizeY,CV_8UC1);

        double inter=0,intra=0,spatial=0,contrast=0;
	for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){//Realiza as metricas em cada frame 
                double a,b,c,d;
                //frameE = cv::Mat::zeros(sizeY,sizeX,CV_8UC1);
                //cv::Mat frameE = cv::Mat::zeros(sizeY,sizeX,CV_8UC1);

		double blur0 = 0.000;//blurringWinkler(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur1 = 0.000;//blurringWinklerV2(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur2 = 0.000;//blurringCPBD(frameY.at(frame_nr), BW_EDGE_CANNY, 10, 200, 3);
		double blur3 = 0.000;//blurringPerceptual(frameY.at(frame_nr));
		double block0 = 0.000;//blockingWang(frameY.at(frame_nr));
		double block1 = 0.000;//blockingVlachos(frameY.at(frame_nr));
                double packet0 = packetLossImpairments(frameY.at(frame_nr),frameE,50/*35*/, 1, 0.15, (1/255) , 1, &a,&b,&c,&d);
                double packet1 = 0.000;//packetLossHuaXiaRui(frameY.at(frame_nr));            para folhas talvez tenha que mudar o 1.35
                double packet2 = 0.000;//packetLossBabu(frameY.at(frame_nr));

		//fprintf(f_output,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
                //        frame_nr, blur0, blur1, blur2, blur3, block0, block1, packet0, packet1, packet2);

                //adiciona matrix com as intensidade das perdas de pacote
                //pckerr.push_back(frameE.clone());

                //medias do packetLossImpairments()
                inter += a/total_frame_nr;
                intra += b/total_frame_nr;
                spatial += c/total_frame_nr;
                contrast += d/total_frame_nr;


                //Borragem
		avg_blur[0] += blur0/total_frame_nr;
		if( blur0 < min_blur[0])	min_blur[0] = blur0;
		else if(blur0 > max_blur[0])	max_blur[0] = blur0;
		avg_blur[1] += blur1/total_frame_nr;
		if( blur1 < min_blur[1])	min_blur[1] = blur1;
		else if(blur1 > max_blur[1])	max_blur[1] = blur1;
		avg_blur[2] += blur2/total_frame_nr;
		if( blur2 < min_blur[2])	min_blur[2] = blur2;
		else if(blur2 > max_blur[2])	max_blur[2] = blur2;
		avg_blur[3] += blur3/total_frame_nr;
		if( blur3 < min_blur[3])	min_blur[3] = blur3;
		else if(blur3 > max_blur[3])	max_blur[3] = blur3;
                //Blocagem
		avg_block[0] += block0/total_frame_nr;
		if( block0 < min_block[0])	min_block[0] = block0;
		else if(block0 > max_block[0])	max_block[0] = block0;
		avg_block[1] += block1/total_frame_nr;
		if( block1 < min_block[1])	min_block[1] = block1;
		else if(block1 > max_block[1])	max_block[1] = block1;
                //Perda de Pacote
		avg_packet[0] += packet0/total_frame_nr;
		if( packet0 < min_packet[0])    min_packet[0] = packet0;
		else if(packet0 > max_packet[0])    max_packet[0] = packet0;
		avg_packet[1] += packet1/total_frame_nr;
		if( packet1 < min_packet[1])	min_packet[1] = packet1;
		else if(packet1 > max_packet[1])    max_packet[1] = packet1;
		avg_packet[2] += packet2/total_frame_nr;
		if( packet2 < min_packet[2])	min_packet[2] = packet2;
		else if(packet2 > max_packet[2])    max_packet[2] = packet2;

	}
/*
	fprintf(f_output,"\nAvg =\t"
                     "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f"
                   	 "\nMax =\t"
                     "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f"
                     "\nMin =\t"
                     "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f"
                     "\n", avg_blur[0], avg_blur[1], avg_blur[2], avg_blur[3], avg_block[0], avg_block[1], avg_packet[0], avg_packet[1], avg_packet[2],
                           max_blur[0], max_blur[1], max_blur[2], max_blur[3], max_block[0], max_block[1], max_packet[0], max_packet[1], max_packet[2],
                           min_blur[0], min_blur[1], min_blur[2], min_blur[3], min_block[0], min_block[1], min_packet[0], min_packet[1], min_packet[2]);
*/
        fprintf(f_avg,"%s\t"
                      "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",fName.c_str(), avg_blur[0], avg_blur[1], avg_blur[2], avg_blur[3],
                                                             avg_block[0], avg_block[1], avg_packet[0], avg_packet[1], avg_packet[2]);


        fprintf(f_param,"%s\t"
                        "%f\t%f\t%f\t%f\n",fName.c_str(),inter,intra,spatial,contrast);
                 	 
  //fclose(f_output);
  fclose(f_avg);
  fclose(f_param);
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
    int enter = fscanf(codebook,"%*[^\n]");
    int carac = fscanf(codebook,"%*c");
    int lines = 0;
    while (EOF != enter && EOF != carac)
        ++lines;
    rewind(codebook);
    return lines;
}


