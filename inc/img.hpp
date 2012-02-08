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
#include <assert.h>

#include <mat.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "global.h"

enum ConvolutionType {
  CONVOLUTION_FULL, /** Return the full convolution, including border */
  CONVOLUTION_SAME, /** Return only the part that corresponds to the original image */
  CONVOLUTION_VALID /** Return only the submatrix containing elements that were not influenced by the border */
};

/**
 * Retorna o pico na superficie de correlacao entre duas matrizes
 */
double maxCorr2D(const cv::Mat & src1,const cv::Mat & src2);

/**
 * Coloca em \param{dest} o resultado da convolucao de \param{img}
 * com \param{kernel}.
 * Equivalente ao uso da funcao conv2 do MATLAB.
 */
void conv2D(const cv::Mat &img, cv::Mat& dest, const cv::Mat& kernel, ConvolutionType ctype = CONVOLUTION_SAME, int btype = cv::BORDER_DEFAULT);

void downsample(const cv::Mat & src,
                cv::Mat & dest,
                const int initRow,
                const int initCol,
                const int ratioRow = 8,
                const int ratioCol = 8);

void FFT(const cv::Mat & src,cv::Mat & dest);
void IFFT(const cv::Mat & src,cv::Mat & dest);

void windowHamming(const cv::Mat & src, cv::Mat &dest);


/**
* @brief Retorna em dest a convolucao com o filtro de textura de Laws no sentido horizontal
*
* @param src   Matriz de origem
* @param dest  Matriz de destino
* @param r     Amortecimento do filtro
*/
void filterLawsH(const cv::Mat &src,cv::Mat & dest,float r=48.0);

/**
* @brief Retorna em dest a convolucao com o filtro de textura de Laws no sentido vertical
*
* @param src   Matriz de origem
* @param dest  Matriz de destino
* @param r     Amortecimento do filtro
*
*/
void filterLawsV(const cv::Mat &src,cv::Mat & dest,float r=48.0);

void filterHantaoH(const cv::Mat &src,cv::Mat & dest);
void filterHantaoV(const cv::Mat &src,cv::Mat & dest);
void analysisTexture(const cv::Mat & src,cv::Mat & dest);
void analysisContrast(const cv::Mat &src,cv::Mat & dest);

/**
 * Filtra a imagem em src para estimar melhor o ruído na imagem, colocando
 * a imagem filtrada em dest.
 * Ref.:\sa{Estimation of image noise variance}
 */
void filterRank(const cv::Mat &src,cv::Mat & dest);



#endif // IMG_HPP
