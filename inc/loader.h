#ifndef LOADER_H
#define LOADER_H

#include <QFile>
#include <fftw3.h>

#include "global.h"
#include "img.hpp"
#include "metric.h"
#include "debug.h"

class Loader
{
        public:
                Loader (QString,int,int,int);
                ~Loader();

                void    callDebug();
                void    callMetrics();

                void    setSize(int, int);
                uchar*  getYUVBuffer() const;
                int     getTotalFrameNr() const;

        private:

                QFile	file;
                QString fName;

                int 	sizeX;          /// Numero de linhas  (altura)
                int 	sizeY;          /// Numero de colunas (largura)
                int 	frequency;      /// Frequencia do video
                int 	format;         /// Formato do video (444,422,420,400)
                int     total_frame_nr;	/// Numero total de frames

                uchar   *yuvbuf;	/// Buffer para o YUV
                uchar   *ybuf;          /// Buffer para a luminancia (Y)

                int	 yuvbuffersize;	/// Tamanho de *yuvbuf

};

#endif // LOADER_H
