/* Copyright (C) 
* 2010 - 
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
* @file metric.h
* @brief Funcoes de metrica de qualidade de video
*/

#ifndef METRIC_H
#define METRIC_H

#include <stdio.h>
#include <iostream>

#include "global.h"
#include "mat.hpp"
#include "img.hpp"

enum    BlurWinklerOptions{
    BW_EDGE_CANNY,
    BW_EDGE_SOBEL,
    BW_EDGE_SCHARR
};


double  blockingVlachos(cv::Mat & src);

double  blurringWinkler(cv::Mat & src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
double  blurringWinklerV2(cv::Mat & src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
//(ref.: A No-Reference Image Blur Metric Based on the Cumulative Probability of Blur Detection (CPBD) )
double  blurringCPBD(cv::Mat & src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
//(ref.: The Blur Effect: Perception and Estimation with a New No-Reference Perceptual Blur Metric)
double  blurringPerceptual(cv::Mat & src);

double  packetLoss(cv::Mat & src);

/**
* @brief Calcula o SSIM entre duas imagens
* @param src1 Primeira imagem
* @param src2 Segunda imagem
*
* @return SSIM entre src1 e src2
*/
double SSIM(cv::Mat& src1,
            cv::Mat& src2,
            const double K1 = 0.01,
            const double K2 = 0.03,
            const int L     = 255,
            const int downsamplewidth = 256,
            const int gaussian_window = 11,
            const double gaussian_sigma = 1.5);


#endif // METRIC_H
