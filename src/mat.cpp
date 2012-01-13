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
* @file mat.cpp
* @brief Funcoes e constantes matematicas
*/

#include <mat.hpp>

float   gauss2d(float x, float y, float s){
    float xx;
    if(s == 0) return 0.0;
    xx = (float) (1/(2*M_PI*s*s)) * exp((double) (-(x*x+y*y)/(2*s*s)));
    return xx;
}
float   gauss(float x, float s){
    float xx;
    if(s == 0) return 0.0;
    xx = (float) exp((double) ((-x*x)/(2*s*s)));
    return xx;
}
float   mgauss(float x, float s){
    float z;
    z = (gauss(x,s)+gauss(x+0.5,s)+gauss(x-0.5,s))/3.0;
    z = z/(M_PI*2.0*s*s);
    return z;
}
float   dgauss(float x,float s){
    return -x/(s*s)*gauss(x,s);
}
