#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>

#include <fftw3.h>

#include <opencv/ml.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "global.h"
#include "img.hpp"
#include "metric.h"
#include "debug.h"

using namespace std;

class Loader
{
        public:
                Loader (string,int,int,int);
                ~Loader();

                void    learnCodebook(string);
                void    callDebug();
                void    callMetrics();

                void    setSize(int, int);
                uchar*  getYUVBuffer() const;
                int     getTotalFrameNr() const;

                long    getFileSize(FILE *hFile);
private:

                FILE * file;
                string fName;

                vector<cv::Mat> frameY;

                int 	sizeX;          /// Numero de linhas  (altura)
                int 	sizeY;          /// Numero de colunas (largura)
                int 	frequency;      /// Frequencia do video
                int 	format;         /// Formato do video (444,422,420,400)
                int     total_frame_nr;	/// Numero total de frames

                uchar   *ybuf;          /// Buffer para a luminancia (Y)

                int	 yuvbuffersize;     /// Tamanho de *yuvbuf

};

#endif // LOADER_H
