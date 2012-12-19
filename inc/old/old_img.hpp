#ifndef OLD_IMG_HPP
#define OLD_IMG_HPP

#include <math.h>
#include <mat.hpp>
#include <old/old_aux.hpp>
#include <opencv/cv.h>

#include "global.h"

/**
* @brief Converte um buffer para uma matriz do OpenCV
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param ybuf  Buffer contendo os dados da imagem
*/
cv::Mat buffer2CVMat(int sizeX,int sizeY, uchar * ybuf);

/**
* @brief Faz subamostragem de uma imagem e retorna em [y]
*
* @tparam T Tipo das matrizes
* @param Y Imagem de entrada
* @param y Imagem de saida
* @param initX Inicio da posicao em x
* @param initY Inicio da posicao em y
* @param ratioX Razao da progressao aritmetica da amostragem em x
* @param ratioY Razao da progressao aritmetica da amostragem em y
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
*/
template <typename T>
void   downsample(T ** Y,T ** y,int initX,int initY,int ratioX,int ratioY,int sizeX, int sizeY)
{
    int i,j,i1=0,j1=0;
    for(i=initX; i< sizeX; i+=ratioX){
        j1 = 0;
        for(j=initY; j< sizeY; j+=ratioY){
            y[i1][j1]=Y[i][j];
            j1++;
        }
        i1++;
    }
}

/**
* @brief Cria uma janela de Hamming
*
* @param Win Matriz de saida - guarda a janela de Hamming
* @param winLin Numero de linhas da janela de Hamming
* @param winCol Numero de colunas da janela de Hamming
*/
void   hammingWindow(double ** Win,int winLin,int winCol);

/**
* @brief Acha o maximo da superficie da correlacao de 2 matrizes
*
* @tparam T Tipo das matrizes
* @param x Matriz de entrada 1
* @param y Matriz de entrada 2
* @param sx Tamanho x das matrizes de entrada
* @param sy Tamanho y das matrizes de entrada
*
* @return Maximo da superficie de correlacao
*/
template <typename T>
double Peak_Correlation(T ** x,T ** y,int sx,int sy)
{
    int i,j;
    double max,absc;

    fftw_complex * FTx,
            * FTy;
    double ** matrixCorr;


    FTx = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sx * sy);
    FTy = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sx * sy);
    matrixCorr = AllocateMatrix<double>(sx,sy);

    FFT<T>(x,FTx,sx,sy);
    FFT<T>(y,FTy,sx,sy);

    /* Conjuga a matriz FTx e multiplica termo a termo por FTy */
    /* Depois, divide pelo modulo (otimizar)                   */
    for(i = 0; i < sx*sy; ++i)
        FTx[i][1] = -1 * FTx[i][1],

                FTx[i][0] = FTx[i][0] * FTy[i][0],
                FTx[i][1] = FTx[i][1] * FTy[i][1],

                absc = sqrt((FTx[i][0]*FTx[i][0])+(FTx[i][1]*FTx[i][1])),
                FTx[i][0] = (FTx[i][0] / absc),
                FTx[i][1] = (FTx[i][1] / absc);

    IFFT<double>(FTx,matrixCorr,sx,sy);

    max = 0;
    for(i = 0; i < sx; ++i)
        for(j = 0; j < sy; ++j)
            if(matrixCorr[i][j] > max) max = matrixCorr[i][j];


    fftw_free(FTx);
    fftw_free(FTy);

    FreeMatrix<double>(matrixCorr,sx);
    return max;
}

/**
* @brief Aplica o filtro modificado de Laws de textura na horizontal usando T1
* (ref.: A Perceptually Relevant No-Reference Blockiness Metric Based on Local Image Characteristics)
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param yframe Frame contendo a luminancia, de tamanho (sizeX x sizeY)
* @param tframe Frame transformado de acordo com o coeficiente de visibilidade
*/
void    tfilterLawsH(int sizeX,int sizeY,uchar ** yframe,float ** tframe);
void    tfilterLawsH(int sizeX,int sizeY,uchar ** yframe,int ** tframe);

/**
* @brief Aplica o filtro modificado de Laws de textura na vertical usando T2
* (ref.: A Perceptually Relevant No-Reference Blockiness Metric Based on Local Image Characteristics)
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param yframe Frame contendo a luminancia, de tamanho (sizeX x sizeY)
* @param tframe Frame transformado de acordo com o coeficiente de visibilidade
*/
void    tfilterLawsV(int sizeX,int sizeY,uchar ** yframe,float ** tframe);
void    tfilterLawsV(int sizeX,int sizeY,uchar ** yframe,int ** tframe);

/**
* @brief Aplica o filtro de Hantao de luminancia na horizontal usando L1
* (ref.: A Perceptually Relevant No-Reference Blockiness Metric Based on Local Image Characteristics)
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param yframe Frame contendo a luminancia, de tamanho (sizeX x sizeY)
* @param lframe Frame transformado de acordo com o coeficiente de visibilidade
*/
void    lfilterHantaoH(int sizeX,int sizeY,uchar ** yframe,float **lframe);
void    lfilterHantaoH(int sizeX,int sizeY,uchar ** yframe,int **lframe);

/**
* @brief Aplica o filtro de Hantao de luminancia na vertical usando L2
* (ref.: A Perceptually Relevant No-Reference Blockiness Metric Based on Local Image Characteristics)
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param yframe Frame contendo a luminancia, de tamanho (sizeX x sizeY)
* @param lframe Frame transformado de acordo com o coeficiente de visibilidade
*/
void    lfilterHantaoV(int sizeX,int sizeY,uchar ** yframe,float **lframe);
void    lfilterHantaoV(int sizeX,int sizeY,uchar ** yframe,int **lframe);

/**
* @brief Detector de bordas usando o algoritmo de Canny baseado em Parker
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param yframe Frame contendo a luminancia, de tamanho (sizeX x sizeY)
* @param high   Valor maximo do threshold
* @param low    Valor minimo do threshold
* @param s      Desvio padrao do filtro gaussiano
* @param norm   Norma a ser utilizada (L1, L2, Loo, etc.)
* @param cframe Imagem do frame contendo as bordas
*/
void    cannyEdge(int sizeX,int sizeY,uchar ** yframe,int high,int low,float s,float (*norm)(float,float),uchar ** cframe);

int trace (int i, int j, int low, uchar ** im,uchar ** mag,uchar ** ori,int sizeX,int sizeY);
int range (int i, int j,int sizeX,int sizeY);

void    edgeDensity(int sizeX,int sizeY,uchar ** yframe,float ** tframe);

#endif // OLD_IMG_HPP
