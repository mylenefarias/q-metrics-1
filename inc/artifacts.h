/**
* @file artifacts.h
* @brief Implementacao para sintetizar artefatos na imagem, cf.:
* \em{Detectability and Annoyance of Synthetic Blocky and Blurry Artifacts}
*/

#ifndef ARTIFACTS_H
#define ARTIFACTS_H

#include "img.hpp"


/// (ref.: Detectability and Annoyance of Synthetic Blocky and Blurry Artifacts)
void blockingFrame(cv::Mat & src, int borderType = cv::BORDER_DEFAULT);
void blockingFrame(const cv::Mat & src,cv::Mat & dst);
void blurringFrame(cv::Mat & src);
void blurringFrame(const cv::Mat & src,cv::Mat & dst);

/** Insere o artefato de ringing (Edge Busyness) de acordo com a especificao ITU-T,
*   "Principles of a reference impairment system for video",
*   Recommendation ITU-T P 930, August 1996.
*/
enum RingingEchoDisplacement{
  RINGING_375ns,
  RINGING_500ns,
  RINGING_750ns
};
void ringingFrame(cv::Mat & src, double echo_amplitude, RingingEchoDisplacement echo_displacement);

#endif // ARTIFACTS_H
