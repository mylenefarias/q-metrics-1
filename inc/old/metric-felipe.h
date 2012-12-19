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

/// Resume a feature de saida para um escalar
enum    OutputOptions{
    OUT_AVERAGE,
    OUT_MEDIAN
};

/// Escolher o detector de bordas apropriado
enum    BlurWinklerOptions{
    BW_EDGE_CANNY,
    BW_EDGE_SOBEL,
    BW_EDGE_SCHARR,
    BW_EDGE_BILATERAL
};

/// (ref.: Detection of blocking artifacts in compressed video)
double  blockingVlachos(const cv::Mat &src);
/// (ref.: No-Reference perceptual quality assessment of jpeg compressed)
/// Implementacao em MATLAB:
/// https://ece.uwaterloo.ca/~z70wang/research/nr_jpeg_quality/jpeg_quality_score.m
double  blockingWang(const cv::Mat & src);

/// (ref.: Perceptual blur and ringing metrics: Application to JPEG2000)
double  blurringWinkler(const cv::Mat &src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
double  blurringWinklerV2(const cv::Mat &src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
/// (ref.: A No-Reference Image Blur Metric Based on the Cumulative Probability of Blur Detection (CPBD) )
double  blurringCPBD(const cv::Mat & src,
                        BlurWinklerOptions options = BW_EDGE_CANNY,
                        double threshold1 = 10,
                        double threshold2 = 200,
                        int aperture_size = 3);
/// (ref.: The Blur Effect: Perception and Estimation with a New No-Reference Perceptual Blur Metric)
double  blurringPerceptual(const cv::Mat & src);

double  packetLoss(const cv::Mat &src);
/// (ref.: PERCEPTUAL NO-REFERENCE PACKET-LOSS METRIC)
double packetLossImpairments(const cv::Mat &src,cv::Mat &frameEDGES,double threshold1,double threshold2,
                             double threshold3,double alpha,double beta, double *a,double *b,double *c,double *d);
/// (ref.: Evaluation of packet loss impairment on streaming video)
double packetLossHuaXiaRui(const cv::Mat & src);
/// (ref.: NO-REFERENCE METRICS FOR VIDEO STREAMING APPLICATIONS)
double packetLossBabu(const cv::Mat & src);

/// (ref.: No-Reference and Reduced Reference Video Quality Metrics: New Contributions)
double  ringing1Farias(const cv::Mat &src);
double  ringing2Farias(const cv::Mat &src,
                       BlurWinklerOptions options = BW_EDGE_CANNY,
                       double threshold1 = 10,
                       double threshold2 = 200,
                       int aperture_size = 3,
                       int oscillation_threshold = 15);
double  noise1Farias(const cv::Mat &src);
double  noise2Farias(const cv::Mat &src, double algorithm_resolution = 0.001);

double contrastMean(const cv::Mat &src);
double contrastMichelson(const cv::Mat & src);
double contrastHess(const cv::Mat & src,cv::Mat & dest,OutputOptions out = OUT_AVERAGE);

double textureStd(const cv::Mat &src);

/**
* @brief Calcula o SSIM entre duas imagens
* @param src1 Primeira imagem
* @param src2 Segunda imagem
*
* @return SSIM entre src1 e src2
*/
double SSIM(const cv::Mat &src1,
            const cv::Mat &src2,
            const double K1 = 0.01,
            const double K2 = 0.03,
            const int L     = 255,
            const int downsamplewidth = 256,
            const int gaussian_window = 11,
            const double gaussian_sigma = 1.5);


#endif // METRIC_H
