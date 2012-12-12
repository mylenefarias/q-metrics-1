#include "metric.h"

double   blockingWang(const cv::Mat & src)
{
    double block_metric = 0;

    int subcols = (int) (src.cols/8);
    int subrows = (int) (src.rows/8);

    /// Diferenca do sinal na horizontal
    cv::Mat d_h1 = src(cv::Rect(1,0,src.cols-1,src.rows)).clone();
    cv::Mat d_h2 = src(cv::Rect(0,0,src.cols-1,src.rows)).clone();
    d_h1.convertTo(d_h1,CV_32SC1);
    d_h2.convertTo(d_h2,CV_32SC1);
    /// O tipo da subtracao eh igual aos tipos dos operandos
    cv::Mat d_h = d_h1 - d_h2;

    /// Blocagem estimada horizontalmente
    double B_h = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < subcols-1; ++j){
            B_h += (double) abs(d_h.at<int>(i,8*(j+1)));
        }
    }
    B_h = B_h/(src.rows * (subcols-1));

    /// Diferenca do sinal na vertical
    cv::Mat d_v1 = src(cv::Rect(0,1,src.cols,src.rows-1)).clone();
    cv::Mat d_v2 = src(cv::Rect(0,0,src.cols,src.rows-1)).clone();
    d_v1.convertTo(d_h1,CV_32SC1);
    d_v2.convertTo(d_h2,CV_32SC1);
    cv::Mat d_v = d_v1 - d_v2;

    /// Blocagem estimada verticalmente
    double B_v = 0;

    for(int i = 0; i < subrows-1; ++i){
        for(int j = 0; j < src.cols; ++j){
            B_v += (double) abs(d_v.at<int>(8*(i+1),j));
        }
    }
    B_v = B_v/(src.cols * (subrows-1));

    block_metric = (B_h+B_v)/2;
    return block_metric;
}

double   blockingVlachos(const cv::Mat & src)
{
    double block_metric;

    double inter_similarity;
    double intra_similarity;

    int subrows = src.rows/8;
    int subcols = src.cols/8;

    cv::Mat src_w(src.rows,src.cols,CV_64FC1);
    windowHamming(src,src_w);

    cv::Mat s1(subrows,subcols,CV_64FC1);
    cv::Mat s2(subrows,subcols,CV_64FC1);
    cv::Mat s3(subrows,subcols,CV_64FC1);
    cv::Mat s4(subrows,subcols,CV_64FC1);
    cv::Mat s5(subrows,subcols,CV_64FC1);
    cv::Mat s6(subrows,subcols,CV_64FC1);
    cv::Mat s7(subrows,subcols,CV_64FC1);

    downsample(src_w,s1,7,7);
    downsample(src_w,s2,0,7);
    downsample(src_w,s3,7,0);
    downsample(src_w,s4,0,0);
    downsample(src_w,s5,0,1);
    downsample(src_w,s6,1,0);
    downsample(src_w,s7,1,1);

    intra_similarity = maxCorr2D(s4,s5) + maxCorr2D(s4,s6) + maxCorr2D(s4,s7);
    inter_similarity = maxCorr2D(s1,s2) + maxCorr2D(s1,s3) + maxCorr2D(s1,s4);

    if(inter_similarity != 0){
        block_metric = (intra_similarity/inter_similarity);
    }else{
        block_metric = 0;
    }

    return block_metric;
}

double  blurringWinkler(const cv::Mat & src,BlurWinklerOptions options,double threshold1,double threshold2,int aperture_size)
{
    double blur_index = 0;

    cv::Mat edges(src.rows,src.cols,CV_8UC1);

    if(options == BW_EDGE_CANNY){
        cv::Canny(src,edges,threshold1,threshold2,aperture_size);
    }else if(options == BW_EDGE_SOBEL){
        cv::Sobel(src,edges,CV_8UC1,1,0);
    }else if(options == BW_EDGE_SCHARR){
        cv::Canny(src,edges,threshold1,threshold2,CV_SCHARR);
    }else if(options == BW_EDGE_BILATERAL){
        cv::Mat temp(src.rows,src.cols,src.type());
        cv::bilateralFilter(src,temp,-1,100,3);
        cv::Canny(temp,edges,threshold1,threshold2,aperture_size);
    }

    unsigned int edge_counter = 0; /// Contador de bordas detectadas
    int c_start;                   /// Indice do maximo ou minimo local vindo pela direita da borda
    int c_end;                     /// Indice do maximo ou minimo local vindo pela esquerda da borda
    int k;

    uchar  max = 0;
    uchar  min = 255;

    int length = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){

            c_start = -1;
            c_end   = -1;

            if(edges.at<uchar>(i,j) > 0){
                edge_counter++;

                /** Lado esquerdo da borda */
                /** Condicao de contorno */
                if(j == 0){
                    c_start = 0;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            max = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }else /* (src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) < 0 */{
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            min = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }
                }

                /** Lado direito da borda */
                /** Condicao de contorno */
                if(j == (src.cols-1)){
                    c_end = src.cols-1;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            max = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }else /* (src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) <= 0 */ {
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            min = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }
                }

                assert((c_end - c_start) >= 0);
                length += (c_end - c_start);

            } /* if(edges.at<uchar>(i,j) > 0) */
        } /* for em j */
    } /* for em i*/

    if(edge_counter != 0){
        blur_index  = length/edge_counter;
    }else{
        blur_index  = 0;
    }

    return blur_index;
}

/** O frame eh borrado e tratado antes de detectar as bordas*/
double  blurringWinklerV2(const cv::Mat & src,BlurWinklerOptions options,double threshold1,double threshold2,int aperture_size)
{
    double blur_index = 0;

    cv::Mat edges(src.rows,src.cols,CV_8UC1);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));

    if(options == BW_EDGE_CANNY){
    	cv::GaussianBlur(src,edges,cv::Size(3,3),0);  //@todo Testar qual o melhor tamanho do filtro
    	cv::morphologyEx(edges,edges,cv::MORPH_OPEN,element);
    	cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        cv::Canny(edges,edges,threshold1,threshold2,aperture_size);
    }else if(options == BW_EDGE_SOBEL){
   	cv::GaussianBlur(src,edges,cv::Size(3,3),0); 
   	cv::morphologyEx(edges,edges,cv::MORPH_OPEN,element);
   	cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        cv::Sobel(edges,edges,CV_8UC1,1,0);
    }else if(options == BW_EDGE_SCHARR){
   	cv::GaussianBlur(src,edges,cv::Size(3,3),0);
   	cv::morphologyEx(edges,edges,cv::MORPH_OPEN,element);
   	cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        cv::Sobel(edges,edges,CV_8UC1,1,0,CV_SCHARR);
    }

    unsigned int edge_counter = 0; /// Contador de bordas detectadas
    int c_start;                   /// Indice do maximo ou minimo local vindo pela direita da borda
    int c_end;                     /// Indice do maximo ou minimo local vindo pela esquerda da borda
    int k;

    uchar  max = 0;
    uchar  min = 255;

    int length = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){

            c_start = -1;
            c_end   = -1;

            if(edges.at<uchar>(i,j) > 0){
                edge_counter++;

                /** Lado esquerdo da borda */
                /** Condicao de contorno */
                if(j == 0){
                    c_start = 0;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            max = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }else /* (src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) < 0 */{
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            min = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }
                }

                /** Lado direito da borda */
                /** Condicao de contorno */
                if(j == (src.cols-1)){
                    c_end = src.cols-1;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            max = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }else /* (src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) <= 0 */ {
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            min = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }
                }

                assert((c_end - c_start) >= 0);
                length += (c_end - c_start);

            } /* if(edges.at<uchar>(i,j) > 0) */
        } /* for em j */
    } /* for em i*/

    if(edge_counter != 0){
        blur_index  = length/edge_counter;
    }else{
        blur_index  = 0;
    }

    return blur_index;
}

double  blurringCPBD(const cv::Mat &src,BlurWinklerOptions options,double threshold1,double threshold2,int aperture_size)
{
    double blur_index = 0;

    cv::Mat edges(src.rows,src.cols,CV_8UC1);

    if(options == BW_EDGE_CANNY){
        cv::Canny(src,edges,threshold1,threshold2,aperture_size);
    }else if(options == BW_EDGE_SOBEL){
        cv::Sobel(src,edges,CV_8UC1,1,0);
    }else if(options == BW_EDGE_SCHARR){
        cv::Canny(src,edges,threshold1,threshold2,CV_SCHARR);
    }else if(options == BW_EDGE_BILATERAL){
        cv::Mat temp(src.rows,src.cols,src.type());
        cv::bilateralFilter(src,temp,-1,100,3);
        cv::Canny(temp,edges,threshold1,threshold2,aperture_size);
    }

    unsigned int edge_counter = 0; /// Contador de bordas detectadas
    int c_start;                   /// Indice do maximo ou minimo local vindo pela direita da borda
    int c_end;                     /// Indice do maximo ou minimo local vindo pela esquerda da borda
    int k;

    uchar  Max = 0;
    uchar  min = 255;

    int length = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){

            c_start = -1;
            c_end   = -1;

            if(edges.at<uchar>(i,j) > 0){
                edge_counter++;

                /** Lado esquerdo da borda */
                /** Condicao de contorno */
                if(j == 0){
                    c_start = 0;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        Max = src.at<uchar>(i,k);
                        while((Max <= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            Max = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }else /* (src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) < 0 */{
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            min = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                    }
                }

                /** Lado direito da borda */
                /** Condicao de contorno */
                if(j == (src.cols-1)){
                    c_end = src.cols-1;
                }else{
                    /** Verifica a primeira derivada */
                    if((src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        Max = src.at<uchar>(i,k);
                        while((Max <= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            Max = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }else /* (src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) <= 0 */ {
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            min = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                    }
                }

                assert((c_end - c_start) >= 0);
                length += (c_end - c_start);
		edges.at<uchar>(i,j) = (c_end - c_start);


            } /* if(edges.at<uchar>(i,j) > 0) */
        } /* for em j */
    } /* for em i*/

    if(edge_counter == 0) return 0.0;

	double P, CPBD_V = 0,CPBD_H = 0;
	int Beta = 0.5;//least squares fitting
	cv::Mat contrastV(src.rows,src.cols,CV_8UC1);
	cv::Mat contrastH(src.rows,src.cols,CV_8UC1);
	
	filterHantaoV(src,contrastV);
	filterHantaoH(src,contrastH);
	for(int i = 0; i < src.rows; ++i){/*Atribui o valor 5 ou 3 dependendo do contraste*/
        	for(int j = 0; j < src.cols; ++j){
			if(contrastV.at<uchar>(i,j) <= 50)
				contrastV.at<uchar>(i,j) = 5;
			else
				contrastV.at<uchar>(i,j) = 3;

			if(contrastH.at<uchar>(i,j) <= 50)
				contrastH.at<uchar>(i,j) = 5;
			else
				contrastH.at<uchar>(i,j) = 3;
		}
	}
	
	for(int i = 0; i < src.rows; ++i){/*Calcula a probabilidade acumulada*/
        	for(int j = 0; j < src.cols; ++j){
			if(edges.at<uchar>(i,j) > 0){
				if(edges.at<uchar>(i,j)/contrastV.at<uchar>(i,j) > 0)
					P = 1 - exp( -(edges.at<uchar>(i,j)/contrastV.at<uchar>(i,j)) ^Beta );
				else
					P = 1 - exp( (edges.at<uchar>(i,j)/contrastV.at<uchar>(i,j)) ^Beta );
				if(P <= (1-exp(-1)) ) //63%
					CPBD_V += P/edge_counter; //Normalizado

				if(edges.at<uchar>(i,j)/contrastH.at<uchar>(i,j) > 0)
					P = 1 - exp( -(edges.at<uchar>(i,j)/contrastH.at<uchar>(i,j)) ^Beta );
				else
					P = 1 - exp( (edges.at<uchar>(i,j)/contrastH.at<uchar>(i,j)) ^Beta );
				if(P <= (1-exp(-1)) ) 
					CPBD_H += P/edge_counter; 			
			}
		}
	}

	blur_index = cv::max(CPBD_V, CPBD_H);

	return blur_index;
}

double  blurringPerceptual(const cv::Mat &src)
{
    double blur_index = 0;

    cv::Mat smoothV(src.rows,src.cols,CV_8UC1);
    cv::Mat smoothH(src.rows,src.cols,CV_8UC1);
    cv::blur(src,smoothV,cv::Size(1,9));
    cv::blur(src,smoothH,cv::Size(9,1));


    double difS_V = 0, difS_H = 0, difB_V = 0, difB_H = 0;
    double somaV = 0, somaH = 0, varV = 0, varH = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){
		
		if(i >= 1){
			difS_V = abs(src.at<uchar>(i,j) - src.at<uchar>(i-1,j));
			difB_V = abs(smoothV.at<uchar>(i,j) - smoothV.at<uchar>(i-1,j));
		}
 		if(j >=1){
			difS_H = abs(src.at<uchar>(i,j) - src.at<uchar>(i,j-1));
 			difB_H = abs(smoothH.at<uchar>(i,j) - smoothH.at<uchar>(i,j-1));		
		}

		varV += cv::max(0.0, difS_V - difB_V);
		varH += cv::max(0.0, difS_H - difB_H);
		somaV += difS_V;
		somaH += difS_H;
	}
    }

    blur_index = cv::max((somaV - varV)/somaV,(somaH - varH)/somaH);

	return blur_index;
}

double ringing1Farias(const cv::Mat &src)
{
    return noise2Farias(src);
}

double ringing2Farias(const cv::Mat & src,BlurWinklerOptions options,double threshold1,double threshold2,int aperture_size,int oscillation_threshold)
{
    double ring_index = 0;

    cv::Mat edges(src.rows,src.cols,CV_8UC1);

    if(options == BW_EDGE_CANNY){
        cv::Canny(src,edges,threshold1,threshold2,aperture_size);
    }else if(options == BW_EDGE_SOBEL){
        cv::Sobel(src,edges,CV_8UC1,1,0);
    }else if(options == BW_EDGE_SCHARR){
        cv::Canny(src,edges,threshold1,threshold2,CV_SCHARR);
    }else if(options == BW_EDGE_BILATERAL){
        cv::Mat temp(src.rows,src.cols,src.type());
        cv::bilateralFilter(src,temp,-1,100,3);
        cv::Canny(temp,edges,threshold1,threshold2,aperture_size);
    }

    unsigned int edge_counter = 0; /// Contador de bordas detectadas
    int c_start;                   /// Posicao do primeiro extremo em relacao a posicao da borda
    int c_end;                     /// Posicao do ultimo extremo em relacao a borda
    int k;
    bool found_edge;               /// Utilizado para verificar se nao foi encontrado outra borda enquanto procura por extremos

    uchar  max = 0;
    uchar  min = 255;

    enum find_state {
        FIND_MIN,
        FIND_MAX
    };
    find_state state;

    int previous_extreme_val;
    int previous_extreme_pos;
    int cur_oscillation;
    int max_oscillation;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){

            if(edges.at<uchar>(i,j) > 0){
                edge_counter++;

                /** Lado esquerdo da borda */
                if(j == 0) {
                    /** Condicao de contorno */
                }else{
                    /** Verifica a primeira derivada para achar o ponto P1, primeiro extremo local indo para a esquerda */
                    if((src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            max = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                        previous_extreme_val = max;
                        state = FIND_MIN;
                    }else /* (src.at<uchar>(i,j-1) - src.at<uchar>(i,j)) < 0 */{
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k-1))&&(k>1)){
                            k--;
                            min = src.at<uchar>(i,k);
                            c_end  = k;
                        }
                        previous_extreme_val = min;
                        state = FIND_MAX;
                    }

                    previous_extreme_pos = c_end;
                    max_oscillation = 0;
                    found_edge = false;

                    /** Acha os extremos locais anteriores ate atingir o limiar de oscilacao ou uma outra borda marcada pelo detector */
                    do
                    {
                        k = previous_extreme_pos; /// Considera-se que o ponto extremo nunca sera marcado pelo detector de bordas
                        if(state == FIND_MAX){
                            max = src.at<uchar>(i,k);
                            while((max <= src.at<uchar>(i,k-1))&&(k>1)){
                                k--;
                                if(edges.at<uchar>(i,k) > 0){
                                    found_edge = true;
                                    break;
                                }
                                max = src.at<uchar>(i,k);
                            }
                            if(!found_edge){
                                cur_oscillation = abs(max - previous_extreme_val);
                                if(cur_oscillation > max_oscillation) max_oscillation = cur_oscillation;
                                previous_extreme_val = max;
                                previous_extreme_pos = k;
                                state = FIND_MIN;
                            }
                        }else /* state == FIND_MIN */ {
                            min = src.at<uchar>(i,k);
                            while((min >= src.at<uchar>(i,k-1))&&(k>1)){
                                k--;
                                if(edges.at<uchar>(i,k) > 0){
                                    found_edge = true;
                                    break;
                                }
                                min = src.at<uchar>(i,k);
                            }
                            if(!found_edge){
                                cur_oscillation = abs(min - previous_extreme_val);
                                if(cur_oscillation > max_oscillation) max_oscillation = cur_oscillation;
                                previous_extreme_val = min;
                                previous_extreme_pos = k;
                                state = FIND_MAX;
                            }
                        }
                    } while((cur_oscillation >= oscillation_threshold) && (found_edge == false));

                    c_start = previous_extreme_pos;
                    ring_index +=  max_oscillation * abs(c_start - c_end);
                }


                /** Lado direito da borda */
                if(j == (src.cols-1)){
                    /** Condicao de contorno */
                }else{
                    /** Verifica a primeira derivada para achar o ponto P2, primeiro extremo local indo para a direita */
                    if((src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) > 0){
                        k   = j;
                        max = src.at<uchar>(i,k);
                        while((max <= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            max = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                        previous_extreme_val = max;
                        state = FIND_MIN;
                    }else /* (src.at<uchar>(i,j+1) - src.at<uchar>(i,j)) <= 0 */ {
                        k   = j;
                        min = src.at<uchar>(i,k);
                        while((min >= src.at<uchar>(i,k+1))&&(k<src.cols)){
                            k++;
                            min = src.at<uchar>(i,k);
                            c_start  = k;
                        }
                        previous_extreme_val = min;
                        state = FIND_MAX;
                    }

                    previous_extreme_pos = c_start;
                    max_oscillation = 0;
                    found_edge = false;

                    /** Acha os extremos locais anteriores ate atingir o limiar de oscilacao ou uma outra borda marcada pelo detector */
                    do
                    {
                        k = previous_extreme_pos; /// Considera-se que o ponto extremo nunca sera marcado pelo detector de bordas
                        if(state == FIND_MAX){
                            max = src.at<uchar>(i,k);
                            while((max <= src.at<uchar>(i,k+1))&&(k<src.cols)){
                                k++;
                                if(edges.at<uchar>(i,k) > 0){
                                    found_edge = true;
                                    break;
                                }
                                max = src.at<uchar>(i,k);
                            }
                            if(!found_edge){
                                cur_oscillation = abs(max - previous_extreme_val);
                                if(cur_oscillation > max_oscillation) max_oscillation = cur_oscillation;
                                previous_extreme_val = max;
                                previous_extreme_pos = k;
                                state = FIND_MIN;
                            }
                        }else /* state == FIND_MIN */ {
                            min = src.at<uchar>(i,k);
                            while((min >= src.at<uchar>(i,k+1))&&(k<src.cols)){
                                k++;
                                if(edges.at<uchar>(i,k) > 0){
                                    found_edge = true;
                                    break;
                                }
                                min = src.at<uchar>(i,k);
                            }
                            if(!found_edge){
                                cur_oscillation = abs(min - previous_extreme_val);
                                if(cur_oscillation > max_oscillation) max_oscillation = cur_oscillation;
                                previous_extreme_val = min;
                                previous_extreme_pos = k;
                                state = FIND_MAX;
                            }
                        }
                    } while((cur_oscillation >= oscillation_threshold) && (found_edge == false));

                    c_end = previous_extreme_pos;
                    ring_index +=  max_oscillation * abs(c_start - c_end);
                }
            } /* if(edges.at<uchar>(i,j) > 0) */
        } /* for em j */
    } /* for em i*/


    if(edge_counter != 0){
        ring_index  = ring_index/edge_counter;
    }else{
        ring_index  = 0;
    }

    return ring_index;
}
double noise1Farias(const cv::Mat &src)
{
    double noise_index = 0.0;

    /// Filtro para separar o conteúdo da imagem da estimação do ruído
    cv::Mat filtered(src.rows - 1, src.cols - 1, src.type());
    filterRank(src,filtered);

    std::vector<double> blocks_variance;
    std::vector<double> subblocks_variance;

    int total_blocks = (filtered.rows/8)*(filtered.cols/8);

    for(int m = 0; m < ((int) (filtered.rows/8)); ++m){
        for(int n = 0; n < ((int) (filtered.cols/8)); ++n){
            /// Bloco de 8x8
            cv::Mat block = (filtered)(cv::Rect(8*n,8*m,8,8));

            /// Divide o bloco em 9 sub-blocos de 4x4
            subblocks_variance.clear();
            for(int i = 0; i < 3; ++i){
                for(int j = 0; j < 3; ++j){
                    cv::Mat subblock = (block)(cv::Rect(2*j,2*i,4,4));
                    cv::Scalar temp;
                    cv::Scalar std;
                    cv::meanStdDev(subblock,temp,std);
                    subblocks_variance.push_back(std[0]);
                }
            }

            /// Tira a média das 4 menores variâncias para ser a variância
            std::partial_sort(subblocks_variance.begin(),subblocks_variance.begin()+4,subblocks_variance.end());
            blocks_variance.push_back((subblocks_variance[0]+
                                       subblocks_variance[1]+
                                       subblocks_variance[2]+
                                       subblocks_variance[3])/4);
        }
    }

    int number_mean_blocks = int (total_blocks/3);
    std::partial_sort(blocks_variance.begin(),blocks_variance.begin() + (number_mean_blocks),blocks_variance.end());

    for(int i = 0; i < number_mean_blocks; ++i)
        noise_index += (double) blocks_variance.at(i);

    noise_index = noise_index/number_mean_blocks;
    return noise_index;

}
double noise2Farias(const cv::Mat &src,double algorithm_resolution)
{
    double noise_index = 0.0;

    /// Filtro para separar o conteúdo da imagem da estimação do ruído
    cv::Mat filtered(src.rows - 1, src.cols - 1, src.type());
    filterRank(src,filtered);

    std::vector<double> blocks_variance;
    std::vector<double> subblocks_variance;

    for(int m = 0; m < (filtered.rows/8); ++m){
        for(int n = 0; n < (filtered.cols/8); ++n){
            /// Bloco de 8x8
            cv::Mat block = (filtered)(cv::Rect(8*n,8*m,8,8));

            /// Divide o bloco em 9 sub-blocos de 4x4
            subblocks_variance.clear();
            for(int i = 0; i < 3; ++i){
                for(int j = 0; j < 3; ++j){
                    cv::Mat subblock = (block)(cv::Rect(2*j,2*i,4,4));
                    cv::Scalar temp;
                    cv::Scalar std;
                    cv::meanStdDev(subblock,temp,std);
                    subblocks_variance.push_back(std[0]);
                }
            }

            /// Tira a média das 4 menores variâncias para ser a variância
            std::partial_sort(subblocks_variance.begin(),subblocks_variance.begin()+4,subblocks_variance.end());
            blocks_variance.push_back((subblocks_variance[0]+
                                       subblocks_variance[1]+
                                       subblocks_variance[2]+
                                       subblocks_variance[3])/4);
        }
    }

    /// Algoritmo iterativo para estimar a variância
    /// Parâmetros iniciais
    double s_old = 0.0;
    double s_new = 0.0;
    int count = 0;
    double threshold = INFINITY;
    /// Cálculo iterativo do valor quadrático médio do histograma
    do
    {
        /// Cálculo do novo valor quadrático médio baseado no novo threshold
        s_new = 0.0;
        for(unsigned int i = 0; i < blocks_variance.size(); ++i){
            if(blocks_variance.at(i) <= threshold){ /// cut-off
                s_new += pow(blocks_variance.at(i),2);
                count++;
            }
        }
        s_new = s_new/count;

        /// Critério de parada
        if(fabs(s_new - s_old) < algorithm_resolution){
            noise_index = s_new;
            break;
        }

        /// Guarda o valor quadrático médio para critério de parada
        s_old = s_new;
        /// Computa novo valor de threshold de cut-off
        threshold = 1.5 * s_old;

    } while(true);

    return noise_index;
}

double contrastMean(const cv::Mat &src)
{
    cv::Scalar contrast_index;
    contrast_index = cv::mean(src);

    return contrast_index[0];
}

double contrastMichelson(const cv::Mat & src)
{
    double contrast_index;
    double min_luminance;
    double max_luminance;

    cv::minMaxLoc(src,&min_luminance,&max_luminance);
    if((min_luminance != 0) || (max_luminance != 0))
        contrast_index = (max_luminance - min_luminance)/(max_luminance + min_luminance);
    else
        contrast_index = 0.0;

    return contrast_index;
}

double contrastHess(const cv::Mat & src,cv::Mat & dest,OutputOptions out)
{
    double contrast_index = 0.0;

    cv::Mat src_w(src.rows,src.cols,CV_64FC1);
    windowHamming(src,src_w);

    cv::Mat fft_src(src.rows,src.cols,CV_64FC2);
    FFT(src_w,fft_src);

    double halfDC = sqrt(pow(fft_src.at<cv::Vec2d>(0,0)[0],2) + pow(fft_src.at<cv::Vec2d>(0,0)[1],2))/2;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){
            dest.at<double>(i,j) =  pow(fft_src.at<cv::Vec2d>(i,j)[0],2);
            dest.at<double>(i,j) += pow(fft_src.at<cv::Vec2d>(i,j)[1],2);
            dest.at<double>(i,j) =  sqrt(dest.at<double>(i,j));
            dest.at<double>(i,j) /= halfDC;
        }
    }

    if(out == OUT_AVERAGE){
        cv::Scalar mean;
        mean = cv::mean(dest);

        contrast_index = mean[0];
        return contrast_index;

    }else if(out == OUT_MEDIAN){
        cv::GaussianBlur(dest,dest,cv::Size(3,3),3.0);
        cv::Scalar mean;
        cv::mean(dest);

        contrast_index = mean[0];
        return contrast_index;

    }else{
        printf("Opcao invalida de saida [contrastHess] \n");
        exit(1);
    }
}

double textureStd(const cv::Mat & src)
{
    cv::Scalar texture_index;
    cv::Scalar temp;

    cv::meanStdDev(src,temp,texture_index);

    return texture_index[0];
}

double SSIM(const cv::Mat& src1,
            const cv::Mat& src2,
            const double K1,
            const double K2,
            const int L,
            const int downsamplewidth,
            const int gaussian_window,
            const double gaussian_sigma)
{

    const double C1 = (K1 * L) * (K1 * L); //6.5025 C1 = (K(1)*L)^2;
    const double C2 = (K2 * L) * (K2 * L); //58.5225 C2 = (K(2)*L)^2;

    int x = src1.cols; /** largura */
    int y = src1.rows; /** altura  */

    int rate_downsampling = std::max(1, int((std::min(x,y) / downsamplewidth) + 0.5));

    cv::Size size_L(x,y);
    cv::Size size(x/rate_downsampling,y/rate_downsampling);

    cv::Mat img1(size,CV_32FC1);
    cv::Mat img2(size,CV_32FC1);

    /// Converte 8 bit para 32 bit float
    cv::Mat img1_L(size_L,CV_32FC1);
    cv::Mat img2_L(size_L,CV_32FC1);

    src1.convertTo(img1_L,img1_L.depth());
    src2.convertTo(img2_L,img2_L.depth());

    /// Subamostragem
    cv::resize(img1_L,img1,cv::Size(img1.cols,img1.rows));
    cv::resize(img2_L,img2,cv::Size(img2.cols,img2.rows));

    /// Alocacao de buffers
    cv::Mat img1_sq(size,CV_32FC1);
    cv::Mat img2_sq(size,CV_32FC1);
    cv::Mat img1_img2(size,CV_32FC1);

    cv::pow(img1,2.0,img1_sq);
    cv::pow(img2,2.0,img2_sq);
    cv::multiply(img1,img2,img1_img2);

    /// Alocacao de sigma e mu
    cv::Mat mu1(size,CV_32FC1);
    cv::Mat mu2(size,CV_32FC1);
    cv::Mat mu1_sq(size,CV_32FC1);
    cv::Mat mu2_sq(size,CV_32FC1);
    cv::Mat mu1_mu2(size,CV_32FC1);

    cv::Mat sigma1_sq(size,CV_32FC1);
    cv::Mat sigma2_sq(size,CV_32FC1);
    cv::Mat sigma12(size,CV_32FC1);

    cv::Mat temp1(size,CV_32FC1);
    cv::Mat temp2(size,CV_32FC1);
    cv::Mat temp3(size,CV_32FC1);

    cv::Mat ssim_map(size,CV_32FC1);

    /// Computacao preliminar
    cv::GaussianBlur(img1,mu1,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::GaussianBlur(img1,mu2,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);

    cv::pow(mu1,2.0,mu1_sq);
    cv::pow(mu2,2.0,mu2_sq);
    cv::multiply(mu1,mu2,mu1_mu2);

    cv::GaussianBlur(img1_sq,sigma1_sq,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma1_sq,1.0,mu1_sq,-1.0,0.0,sigma1_sq);

    cv::GaussianBlur(img2_sq,sigma2_sq,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma2_sq,1.0,mu2_sq,-1.0,0.0,sigma2_sq);

    cv::GaussianBlur(img1_img2,sigma12,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma12,1.0,mu1_mu2,-1.0,0.0,sigma12);

    // (2*mu1_mu2 + C1)
    mu1_mu2.convertTo(temp1,temp1.depth(),2.0,C1);
    // (2*sigma12 + C2)
    sigma12.convertTo(temp2,temp2.depth(),2.0,C2);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    cv::multiply(temp1,temp2,temp3);

    // (mu1_sq + mu2_sq + C1)
    cv::add(mu1_sq,mu2_sq,temp1);
    cv::add(temp1,cv::Scalar(C1),temp1);

    // (sigma1_sq + sigma2_sq + C2)
    cv::add(sigma1_sq,sigma2_sq,temp2);
    cv::add(temp2,cv::Scalar(C2),temp2);

    // ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    cv::multiply(temp1,temp2,temp1);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    cv::divide(temp3,temp1,ssim_map);

    cv::Scalar index_scalar(cv::mean(ssim_map));
    return index_scalar.val[0];

}

