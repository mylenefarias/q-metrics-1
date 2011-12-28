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
* @file old_aux.hpp
* @brief Funcoes auxiliares de alocacao e desalocacao de matrizes
*/
#ifndef OLD_AUX_HPP
#define OLD_AUX_HPP

/**
* @brief Libera a memoria alocada para uma matriz
*
* @tparam T Tipo da matriz
* @param matrix Matriz bidimensional
* @param nr Numero de linhas da matriz
*/
template <typename T>
        void FreeMatrix(T ** matrix,int nr)
{
    for(int i=0;i<nr;++i){
        delete [] matrix[i];
    }
    delete [] matrix;
}


/**
* @brief Aloca na memoria espaco para uma matriz (igual a calloc - tudo comeca limpo com 0)
*
* @tparam T Tipo da matriz
* @param nr Numero de linhas
* @param nc Numero de colunas
*
* @return Matriz bidimensional
*/
template <typename T>
        T ** AllocateMatrix(int nr,int nc)
{
    int i, j;
    T **matrix;

    matrix = new T*[nr];
    for (i=0;i<nr;++i) {
            matrix[i] = new T[nc];
            for (j = 0; j < nc; ++j){
                    matrix[i][j] = 0;
            }
    }
    return matrix;
}

#endif // OLD_AUX_HPP
