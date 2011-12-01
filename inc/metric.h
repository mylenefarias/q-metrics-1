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

#include <qfile.h>
#include <qtextstream.h>

#include "global.h"
#include "mat.hpp"
#include "aux.hpp"
#include "img.hpp"

/**
* @brief Função que imprime no arquivo .txt a métrica de blocagem de Vlachos
* (ref.: Detection of blocking artifacts in compressed video)
* @param sizeX          Comprimento do vídeo
* @param sizeY          Largura do vídeo
* @param total_frame_nr Número de frames do vídeo
* @param ybuf           Buffer contendo a componente de luminância (Y) do vídeo
* @param fName          Nome do arquivo que contem o resultado da metrica [fName]_block.txt
*/
void   blockingVlachos(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,QString fName);

/**
* @brief Função que imprime no arquivo .txt a métrica de borragem de Winkler
* (ref.: A no-reference perceptual blur metric)
* @param sizeX          Comprimento do vídeo
* @param sizeY          Largura do vídeo
* @param total_frame_nr Número de frames do vídeo
* @param ybuf           Buffer contendo a componente de luminância (Y) do vídeo
* @param high           Valor maximo do threshold do detector de bordas
* @param low            Valor mínimo do threshold do detector de bordas
* @param s              Desvio padrão do filtro gaussiano
* @param fName          Nome do arquivo que contem o resultado da metrica [fName]_blur.txt
*/
void   blurringWinkler(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),QString fName);

/**
* @brief Função que imprime no arquivo .txt a métrica de perda de pacotes
* @param sizeX          Comprimento do vídeo
* @param sizeY          Largura do vídeo
* @param total_frame_nr Número de frames do vídeo
* @param ybuf           Buffer contendo a componente de luminância (Y) do vídeo
* @param fName          Nome do arquivo que contem o resultado da metrica [fName]_blur.txt
*/
void    packetLoss(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,QString fName);


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
