#include "debug.h"

void    sizeDEBUG(int sizeX,int sizeY){
    printf("sizeX = %d \n",sizeX);
    printf("sizeY = %d \n",sizeY);
}
void    cvDEBUG(int sizeX,int sizeY,uchar * ybuf)
{
    FILE * dump;
    dump = fopen("cvtest.txt","w");

    cv::Mat img;
    img = buffer2CVMat(sizeX,sizeY,ybuf);

    cv::Mat It(cv::Size(sizeX,sizeY),CV_8UC1,cv::Scalar(0));
    analysisTexture(img,It);

    for(int i = 0; i < sizeY; ++i){
        for(int j = 0; j < sizeX; ++j){
            /// Prestar atencao na ordem dos indices no operador at
            fprintf(dump," %u ", It.at<uchar>(i,j));
        }
        fprintf(dump,"\n");
    }

    fclose(dump);
    return ;
}
void    LawsH_DEBUG(int sizeX,int sizeY,uchar * ybuf)
{
    FILE * dump;
    dump = fopen("lawsH.txt","w");

    cv::Mat img;
    img = buffer2CVMat(sizeX,sizeY,ybuf);

    cv::Mat F(cv::Size(sizeX,sizeY),CV_8UC1,cv::Scalar(0));
    filterLawsH(img,F,48);
    F = cv::abs(F);

    for(int i = 0; i < sizeY; ++i){
        for(int j = 0; j < sizeX; ++j){
            /// Prestar atencao na ordem dos indices no operador at
            fprintf(dump," %u ", F.at<uchar>(i,j));
        }
        fprintf(dump,"\n");
    }

    fclose(dump);
    return ;
}

void    frameDEBUG(int sizeX,int sizeY,uchar * ybuf){

    FILE * dump;
    dump = fopen("1frame.txt","w");

    int i,x,y;

    uchar ** frameY;
    frameY = AllocateMatrix<uchar>(sizeX,sizeY);

    x = 0;
    y = 0;

    for(i = 0; i < sizeX*sizeY; ++i){
        frameY[x][y] = *(ybuf+i);
        ++x;
        if(x == sizeX)
            x = 0, ++y;
    }

    for(y = 0; y < sizeY; ++y){
        for(x = 0; x < sizeX; ++x){
            fprintf(dump," %u ",frameY[x][y]);
        }
        fprintf(dump,"\n");
    }

    FreeMatrix(frameY,sizeX);
    fclose(dump);
}
void    filterDEBUG(int sizeX,int sizeY,uchar * ybuf,const char * dname,void(*filter)(int,int,uchar **,int **)){

    FILE * dump;
    dump = fopen(dname,"w");

    int i,x,y;

    uchar ** frameY;
    int ** TframeY;

    frameY = AllocateMatrix<uchar>(sizeX,sizeY);
    TframeY = AllocateMatrix<int>(sizeX,sizeY);

    x = 0,y = 0;

    for(i = 0; i < sizeX*sizeY; ++i){
        frameY[x][y] = *(ybuf+i);
        ++x;
        if(x == sizeX)
            x = 0, ++y;
    }

    (*filter)(sizeX,sizeY,frameY,TframeY);

    for(y = 0; y < sizeY; ++y){
        for(x = 0; x < sizeX; ++x){
            fprintf(dump," %d ",TframeY[x][y]);
        }

    }

    FreeMatrix(frameY,sizeX);
    FreeMatrix(TframeY,sizeX);

    fclose(dump);
}
void    cannyEdgeDEBUG(int sizeX,int sizeY,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),const char * dname){

    FILE * dump;
    dump = fopen(dname,"w");

    int i,x,y;

    uchar ** frameY;
    uchar ** frameE;

    frameY = AllocateMatrix<uchar>(sizeX,sizeY);
    frameE = AllocateMatrix<uchar>(sizeX,sizeY);

    x = 0, y = 0;

    for(i = 0; i < sizeX*sizeY; ++i){
        frameY[x][y] = *(ybuf+i);
        ++x;
        if(x == sizeX)
            x = 0, ++y;
    }

    cannyEdge(sizeX,sizeY,frameY,high,low,s,norm,frameE);

    for(y = 0; y < sizeY; ++y){
        for(x = 0; x < sizeX; ++x){
            fprintf(dump," %d ",frameE[x][y]);
        }
        fprintf(dump,"\n");
    }

    FreeMatrix(frameY,sizeX);
    FreeMatrix(frameE,sizeX);

    fclose(dump);
}
void    edgeDensityDEBUG(int sizeX,int sizeY,uchar * ybuf,const char * dname,int high,int low,float s,float (*norm)(float,float)){

    FILE * dump;
    dump = fopen(dname,"w");

    int i,x,y;

    uchar ** frameY;
    uchar ** frameE;
    float ** frameS;

    frameY = AllocateMatrix<uchar>(sizeX,sizeY);
    frameE = AllocateMatrix<uchar>(sizeX,sizeY);
    frameS = AllocateMatrix<float>(sizeX,sizeY);

    x = 0, y = 0;

    for(i = 0; i < sizeX*sizeY; ++i){
        frameY[x][y] = *(ybuf+i);
        ++x;
        if(x == sizeX)
            x = 0, ++y;
    }

    cannyEdge(sizeX,sizeY,frameY,high,low,s,norm,frameE);
    edgeDensity(sizeX,sizeY,frameE,frameS);

    for(y = 0; y < sizeY; ++y){
        for(x = 0; x < sizeX; ++x){
            fprintf(dump," %f ",frameS[x][y]);
        }
        fprintf(dump,"\n");
    }

    FreeMatrix(frameY,sizeX);
    FreeMatrix(frameE,sizeX);
    FreeMatrix(frameS,sizeX);

    fclose(dump);

}
