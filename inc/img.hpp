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
#include <opencv/cv.h>

#include "global.h"

enum ConvolutionType {
  CONVOLUTION_FULL, /** Return the full convolution, including border */
  CONVOLUTION_SAME, /** Return only the part that corresponds to the original image */
  CONVOLUTION_VALID /** Return only the submatrix containing elements that were not influenced by the border */
};

void corr2D(const cv::Mat & src1,const cv::Mat & src2,cv::Mat & dest);
void conv2D(const cv::Mat &img, cv::Mat& dest, const cv::Mat& kernel, ConvolutionType ctype = CONVOLUTION_SAME, int btype = cv::BORDER_DEFAULT);

void downsample(const cv::Mat & src,
                cv::Mat & dest,
                const int initRow,
                const int initCol,
                const int ratioRow = 8,
                const int ratioCol = 8);

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


#endif // IMG_HPP
