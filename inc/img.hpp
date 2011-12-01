/* Copyright (C)
* 2010  
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file img.hpp
* @brief Funcoes para o processamento de imagens
*/

#ifndef IMG_HPP
#define IMG_HPP

#include <math.h>
#include <mat.hpp>
#include <aux.hpp>
#include <opencv/cv.h>

#include "global.h"

enum ConvolutionType {
  CONVOLUTION_FULL, /** Return the full convolution, including border */
  CONVOLUTION_SAME, /** Return only the part that corresponds to the original image */
  CONVOLUTION_VALID /** Return only the submatrix containing elements that were not influenced by the border */
};

void conv2D(const cv::Mat &img, cv::Mat& dest, const cv::Mat& kernel, ConvolutionType ctype = CONVOLUTION_SAME, int btype = cv::BORDER_DEFAULT);

/**
* @brief Converte um buffer para uma matriz do OpenCV
*
* @param sizeX Largura da imagem
* @param sizeY Altura da imagem
* @param ybuf  Buffer contendo os dados da imagem
*/
cv::Mat buffer2CVMat(int sizeX,int sizeY, uchar * ybuf);

/**
* @brief Retorna em dest a convolucao com o filtro de textura de Laws no sentido horizontal
*
* @param src   Matriz de origem
* @param dest  Matriz de destino (pode ser utilizado in-place)
* @param r     Amortecimento do filtro (?)
*/
void filterLawsH(cv::Mat & src,cv::Mat & dest,float r);

/**
* @brief Retorna em dest a convolucao com o filtro de textura de Laws no sentido vertical
*
* @param src   Matriz de origem
* @param dest  Matriz de destino (pode ser utilizado in-place)
* @param r     Amortecimento do filtro (?)
*/
void filterLawsV(cv::Mat & src,cv::Mat & dest,float r);

void filterHantaoH(cv::Mat & src,cv::Mat & dest);
void filterHantaoV(cv::Mat & src,cv::Mat & dest);
void analysisTexture(cv::Mat & src,cv::Mat & dest);
void analysisContrast(cv::Mat & src,cv::Mat & dest);


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

#endif // IMG_HPP
