#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>

#include <fftw3.h>

#include <opencv/ml.h>

// Esses cabecalhos sao necessarios para a leitura do tamanho
// do arquivo a partir do file descriptor:
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

                uchar*  getYUVBuffer() const;
                int     getTotalFrameNr() const;

                long    getFileSize(FILE *hFile);

                void    writeCodebook(string fCodebook, float DMOS, int frames_in_word, int word_sizeX, int word_sizeY);
                double  predictMOS(string fCodebook, int K, int frames_in_word, int word_sizeX, int word_sizeY);

                void    callDebug();
                void    callMetrics();
                void    showFrame(int i);

private:

                FILE * file;
                string fName;

                vector<cv::Mat> frameY;

                int 	sizeX;          /// Numero de linhas  (altura)
                int 	sizeY;          /// Numero de colunas (largura)
                int 	format;         /// Formato do video (444,422,420,400)
                int     total_frame_nr;	/// Numero total de frames

                double  pool_frame(vector<double> v);
                int     count_lines(FILE *codebook);
};

#endif // LOADER_H
