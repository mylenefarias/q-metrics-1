#include "loader.h"

Loader::Loader(QString fN, int sX, int sY, int yuv)
    :fName(fN),sizeX(sX),sizeY(sY),format(yuv)
{
    qint64 length;

    if(fName != NULL)
    {
            if(file.isOpen())   file.close();
            file.setFileName(fName);
            file.open(QIODevice::ReadOnly);
    }

    if     (format == _400)     yuvbuffersize = sizeX*sizeY;
    else if(format == _420)     yuvbuffersize = 3*sizeX*sizeY/2;
    else if(format == _422)     yuvbuffersize = 2*sizeX*sizeY;
    else if(format == _444)     yuvbuffersize = 3*sizeX*sizeY;

    length = file.size();
    total_frame_nr = length/yuvbuffersize;

    ybuf   = new uchar[total_frame_nr*sizeX*sizeY];

    for(int frame_nr=0; frame_nr< total_frame_nr; ++frame_nr){
        file.seek(yuvbuffersize*frame_nr);
        file.read((char*)ybuf+((sizeX*sizeY)*frame_nr), sizeX*sizeY);
    }

}
Loader::~Loader()
{
      file.close();
}

/// @badcode:
/// https://www.securecoding.cert.org/confluence/display/seccode/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+file
#if 0
long MyGetFileSize ( FILE * hFile )
{
long lCurPos, lEndPos;
// Check for valid file pointer
if ( hFile == NULL )
{
return -1;
}
// Store current position
lCurPos = ftell ( hFile );
// Move to the end and get position
fseek ( hFile, 0, 2 );
lEndPos = ftell ( hFile );
// Restore the file pointer
fseek ( hFile, lCurPos, 0 );
return lEndPos;
}
#endif


void   Loader::callDebug() {

//    sizeDEBUG(sizeX,sizeY);
      frameDEBUG(sizeX,sizeY,ybuf);
      cvDEBUG(sizeX,sizeY,ybuf);
      LawsH_DEBUG(sizeX,sizeY,ybuf);
//    filterDEBUG(sizeX,sizeY,ybuf,"tfilterLawsH.txt",tfilterLawsH);
//    filterDEBUG(sizeX,sizeY,ybuf,"lfilterHantaoH.txt",lfilterHantaoH);
//    cannyEdgeDEBUG(sizeX,sizeY,ybuf,200,10,1,L2,"canny.txt");
//    edgeDensityDEBUG(sizeX,sizeY,ybuf,"edge.txt",200,10,1,L2);
}

void   Loader::callMetrics() {
    //blockingVlachos(sizeX,sizeY,total_frame_nr,ybuf,fName);
    //blurringWinkler(sizeX,sizeY,total_frame_nr,ybuf,200,10,1,L2,fName);
}


uchar* Loader::getYUVBuffer() const
{
        return yuvbuf;
}

int    Loader::getTotalFrameNr() const
{
        return this->total_frame_nr;
}

void   Loader::setSize(int sx, int sy)
{
        this->sizeX = sx;
        this->sizeY = sy;
}
