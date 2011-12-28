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
* @file old_debug.h
* @brief Testes e dumps de funcoes
*/
#ifndef OLD_DEBUG_H
#define OLD_DEBUG_H

#include "global.h"
#include "old/old_aux.hpp"
#include "old/old_img.hpp"

#include <stdio.h>

/**
* @brief Coloca o primeiro frame do video no arquivo 1frame.txt
*
* @param sizeX  Tamanho X da imagem (largura)
* @param sizeY  Tamanho Y da imagem (altura)
* @param ybuf   Buffer da componente de luminancia
*/
void    frameDEBUG(int sizeX,int sizeY,uchar * ybuf);

/**
* @brief Coloca a imagem filtrada por [filter] no arquivo [dname]
*
* @param sizeX  Tamanho X da imagem (largura)
* @param sizeY  Tamanho Y da imagem (altura)
* @param ybuf   Buffer da componente de luminancia
* @param dname  Nome do arquivo
* @param filter Filtro utilizado
*/
void    filterDEBUG(int sizeX,int sizeY,uchar * ybuf,const char * dname,void(*filter)(int,int,uchar **,int **));

/**
* @brief Coloca a imagem resultante do detector de borda no arquivo [dname]
*
* @param sizeX  Tamanho X da imagem (largura)
* @param sizeY  Tamanho Y da imagem (altura)
* @param ybuf   Buffer da componente de luminancia
* @param high   Threshold maximo
* @param low    Threshold minimo
* @param s      Desvio Padrao da Gaussiana
* @param norm   Norma utilizada
* @param dname  Nome do arquivo
*/
void    cannyEdgeDEBUG(int sizeX,int sizeY,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),const char * dname);

/**
* @brief Coloca a imagem resultante da densidade de bordas (Experimento 4) no arquivo [dname]
*
* @param sizeX  Tamanho X da imagem (largura)
* @param sizeY  Tamanho Y da imagem (altura)
* @param ybuf   Buffer da componente de luminancia
* @param high   Threshold maximo
* @param low    Threshold minimo
* @param s      Desvio Padrao da Gaussiana
* @param norm   Norma utilizada
* @param dname  Nome do arquivo
*/
void    edgeDensityDEBUG(int sizeX,int sizeY,uchar * ybuf,const char * dname,int high,int low,float s,float (*norm)(float,float));

#endif // OLD_DEBUG_H
