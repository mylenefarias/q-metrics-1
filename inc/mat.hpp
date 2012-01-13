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
* @file mat.hpp
* @brief Funcoes e constantes matematicas 
*/

#ifndef MAT_HPP
#define MAT_HPP

#include <math.h>
#include <stdlib.h>
#include <fftw3.h>

#include <vector>

#define EPSILON 2.2204e-16

float   gauss2d(float x, float y, float s);
float   gauss(float x, float s);
float   mgauss(float x, float s);
float   dgauss(float x, float s);

template <typename T>
    T mean(std::vector<T> const & v)
{
    T s;
    s = 0;
    for(unsigned int i = 0; i < v.size(); ++i)
        s += v.at(i);
    s /= v.size();
    return s;
}


/** @todo: Transportar essa funcao para img.hpp/img.cpp */
/* Fez-se o benchmark entre os dois algoritmos para o filtro de mediana 1d:
 * [1] http://www.sergejusz.com/engineering_tips/median_filter/medflt_bcc.zip
 *     Baseado no algoritmo de Huang para filtro de mediana 2D
 *
 * [2] http://www.physics.ox.ac.uk/users/littlem/software/fastmedfilt1d.zip
 *     Baseado no selection sort, implementacao original de Nicolas Devillard
 *
 * Aparentemente para uma janela grande(~ acima de 20) [1] se sai melhor que [2];
 * Nota-se que [1] funciona in-situ e portanto usou-se [2] para usar no detector
 * de grid.
 */
/**
* @brief Usada para calcular o filtro da mediana com selection sort
*
* @tparam T Tipo do vetor
* @param arr Vetor de entrada
* @param n Tamanho do vetor
*
* @return Vetor de saida
*/
template <typename T>
    T quickSelect(T arr[], int n)
{
    #define SWAP(a,b) { register double t=(a);(a)=(b);(b)=t; }
    long low, high;
    long median;
    long middle, ll, hh;

    low = 0;
    high = n-1;
    median = (low + high) / 2;
    for (;;)
    {
        /* One element only */
        if (high <= low)
            return arr[median];

        /* Two elements only */
        if (high == low + 1)
        {
            if (arr[low] > arr[high])
                SWAP(arr[low], arr[high]);
            return arr[median];
        }

        /* Find median of low, middle and high items; swap to low position */
        middle = (low + high) / 2;
        if (arr[middle] > arr[high])
            SWAP(arr[middle], arr[high])
        if (arr[low] > arr[high])
            SWAP(arr[low], arr[high])
        if (arr[middle] > arr[low])
            SWAP(arr[middle], arr[low])

        /* Swap low item (now in position middle) into position (low+1) */
        SWAP(arr[middle], arr[low+1])

        /* Work from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;
        for (;;)
        {
            do
                ll++;
            while (arr[low] > arr[ll]);
            do
                hh--;
            while (arr[hh] > arr[low]);

            if (hh < ll)
                break;

            SWAP(arr[ll], arr[hh])
        }

        /* Swap middle item (in position low) back into correct position */
        SWAP(arr[low], arr[hh])

        /* Reset active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
    #undef SWAP
}


/**
* @brief Filtro da mediana 1D
*
* @tparam T Tipo da matriz
* @param x Sinal de entrada
* @param m Buffer para o sinal de saida
* @param xic Condicoes iniciais
* @param xfc Condicoes finais
* @param N Tamanho do vetor de entrada
* @param W Tamanho da Janela
* @param W2 W2 satisfaz W = 2*W2+1
*
* @return Matriz de saida
*/
template <typename T>
    void medianFilter(T * x,T * m,T * xic,T * xfc,int N,int W,int W2)
{
    long i, k, idx;
    double *w;
    w = (double *) calloc(W, sizeof(double));   /* Allocate sliding window */

    for (i = 0; i < N; i ++)
    {
        /* Fill up the sliding window */
        for (k = 0; k < W; k++)
        {
            idx = i - W2 + k;

            if (idx < 0)
            {
                /* Need to get values from the initial condition vector */
                w[k] = xic[W2 + idx];
            }
            else if (idx >= N)
            {
                /* Need to get values from the final condition vector */
                w[k] = xfc[idx - N];
            }
            else
            {
                w[k] = x[idx];
            }
        }

        /* Select the median of the sliding window */
        m[i] = quickSelect(w, W);
    }

    /* Clean up */
    free(w);
}

#endif // MAT_HPP
