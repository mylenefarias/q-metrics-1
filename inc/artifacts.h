/**
* @file artifacts.h
* @brief Implementacao para sintetizar artefatos na imagem, cf.:
* \em{Detectability and Annoyance of Synthetic Blocky and Blurry Artifacts}
*/

#ifndef ARTIFACTS_H
#define ARTIFACTS_H

#include "img.hpp"

void blockingFrame(cv::Mat & src, int borderType = cv::BORDER_DEFAULT);
void blockingFrame(const cv::Mat & src,cv::Mat & dst);
void blurringFrame(cv::Mat & src);
void blurringFrame(const cv::Mat & src,cv::Mat & dst);

#endif // ARTIFACTS_H
