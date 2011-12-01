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

#include <aux.hpp>
#include <math.h>
#include <stdlib.h>
#include <fftw3.h>

#define EPSILON 2.2204e-16

void DHT(double * in, double * out,int s);

float   gauss2d(float x, float y, float s);
float   gauss(float x, float s);
float   mgauss(float x, float s);
float   dgauss(float x, float s);


/**
* @brief Aplica a transforma de Fourier na matriz de entrada
*
* @tparam T Tipo da matriz de entrada
* @param in Matriz de entrada
* @param out Transformada da matriz de entrada
* @param sx Tamanho x da matriz
* @param sy Tamanho y da matriz
*/
template <typename T>
        void   FFT(T ** in, fftw_complex * out,int sx,int sy)
{
    int i,j;
    fftw_complex * FFT_in;
    fftw_plan p;

    FFT_in  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sx * sy);

    for(i = 0; i < sx; ++i)
        for(j = 0; j < sy; ++j)
            FFT_in[i*sy + j][0] = in[i][j],
            FFT_in[i*sy + j][1] = 0;

    p = fftw_plan_dft_2d(sx,sy,FFT_in,out,FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(FFT_in);
}

/**
* @brief Aplica a transformada inversa de Fourier na matriz de entrada
*
* @tparam T Tipo da matriz de saida
* @param in Matriz de entrada
* @param out Matriz de saida
* @param sx Tamanho x da matriz
* @param sy Tamanho y da matriz
*/
template <typename T>
        void   IFFT(fftw_complex * in, T ** out,int sx,int sy)
{
    int i,j;
    fftw_complex * FFT_out;
    fftw_plan p;

    FFT_out  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sx * sy);

    p = fftw_plan_dft_2d(sx,sy,in,FFT_out,FFTW_BACKWARD,FFTW_ESTIMATE);
    fftw_execute(p);

    for(i = 0; i < sx; ++i)
        for(j = 0; j < sy; ++j)
            out[i][j] = FFT_out[i*sy + j][0];


    fftw_destroy_plan(p);
    fftw_free(FFT_out);
}

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
* @brief Cria uma janela de Hamming 
*
* @param Win Matriz de saida - guarda a janela de Hamming
* @param winLin Numero de linhas da janela de Hamming
* @param winCol Numero de colunas da janela de Hamming
*/
void   hammingWindow(double ** Win,int winLin,int winCol);

/** @todo: reescrever f. abaixo para manter consistencia */
/**
* @brief Classe para calcular o 1o e 2o momento (http://www.johndcook.com/standard_deviation.html)
*/
class RStatistics{
    public:
        RStatistics();

        void Clear();
/**
* @brief Coloca um valor dentro da pilha
* @param x valor x a ser colocado
*/
        void Push(double x);

/**
* @brief Retorna o numero de valores na pilha
*/
        int NumDataValues() const;
/**
* @brief Retorna a media aritmetica dos valores na pilha
*/
        double Mean() const;
/**
* @brief Retorna a variancia dos valores na pilha
*/
        double Variance() const;
/**
* @brief Retorna o desvio padrao
*/
        double StandardDeviation() const;

    private:
        int m_n;
        double m_oldM, m_newM, m_oldS, m_newS;
};



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

float   L2(float x,float y);
float   L1(float x,float y);
float   Loo(float x,float y);





#endif // MAT_HPP
