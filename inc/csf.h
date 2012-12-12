/**
* @file csf.h
* @brief Funcoes de sensitividade de contraste
*/

#ifndef CSF_H
#define CSF_H

#include "mat.hpp"

/**
* @brief Computa o peso dos erros com a freq. espacial
* @ref   "Objective Picture Quality Scale (PQS) for Image Coding"
*
* @param u  Frequencias espaciais horizontal
* @param v  Frequencias espaciais verticais
*
* @return Resposta em frequencia
*/
double CSFFreqResponse(double u,double v);

#endif // CSF_H
