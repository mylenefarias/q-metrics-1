#ifndef OLD_METRIC_H
#define OLD_METRIC_H

#include <stdio.h>

#include "global.h"
#include "mat.hpp"
#include "old/old_img.hpp"

/**
* @brief Função que imprime no arquivo .txt a métrica de blocagem de Vlachos
* (ref.: Detection of blocking artifacts in compressed video)
* @param sizeX          Comprimento do vídeo
* @param sizeY          Largura do vídeo
* @param total_frame_nr Número de frames do vídeo
* @param ybuf           Buffer contendo a componente de luminância (Y) do vídeo
* @param fName          Nome do arquivo que contem o resultado da metrica [fName]_block.txt
*/
void   blockingVlachos(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,string fName);

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
void   blurringWinkler(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),string fName);

/**
* @brief Função que imprime no arquivo a medida de measure_lines_v2.m
* @param sizeX          Comprimento do vídeo
* @param sizeY          Largura do vídeo
* @param total_frame_nr Número de frames do vídeo
* @param ybuf           Buffer contendo a componente de luminância (Y) do vídeo
* @param fName          Nome do arquivo que contem o resultado da metrica
*/
void    measure_lines_v2(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,string fName);

#endif // OLD_METRIC_H
