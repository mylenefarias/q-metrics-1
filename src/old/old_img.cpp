#include "old/old_img.hpp"

cv::Mat buffer2CVMat(int sizeX,int sizeY, uchar * ybuf)
{
    cv::Mat image(cv::Size(sizeX,sizeY), CV_8UC1, ybuf);
    return image;
}

void    hammingWindow(double ** Win,int winLin,int winCol){

    double ftmp;
    int i;
    double *winL, *winC;
    winL= new double[winLin];
    winC= new double[winCol];

    ftmp = M_PI/winLin;
    for (i=0; i<winLin; i++)
        winL[i] = (0.54 + 0.46*cos(ftmp*((double)i+0.5)));

    for (i=0; i<winCol; i++)
        winC[i] = (0.54 + 0.46*cos(ftmp*((double)i+0.5)));

    for(int Lin = 0; Lin < winLin; Lin++)
        for(int Col = 0; Col < winCol; Col++)
            Win[Lin][Col] = winL[Lin] * winC[Col];

    delete winL;
    delete winC;

}

/** @todo: Parametros para fazer regressao posteriormente: */
#define tfilterLawsH_THR   0.15
#define tfilterLawsH_ALPHA 5
void    tfilterLawsH(int sizeX,int sizeY,uchar ** yframe,float ** tframe){

    int i,j;

    /* Utiliza-se um frame aumentado para calcular o filtro espacialmente
       Aparentemente, melhor que usar varios branches:
    http://stackoverflow.com/questions/676709/fast-way-to-implement-2d-convolution-in-c */
    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    /** @todo Procurar maneira de otimizar juntando 2 loops */
    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            tframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            tframe[i-2][j-2] +=  2*augframe[i-2][j-1];
            tframe[i-2][j-2] += -2*augframe[i-2][j+1];
            tframe[i-2][j-2] += -1*augframe[i-2][j+2];
            tframe[i-2][j-2] +=  4*augframe[i-1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i-1][j-1];
            tframe[i-2][j-2] += -8*augframe[i-1][j+1];
            tframe[i-2][j-2] += -4*augframe[i-1][j+2];
            tframe[i-2][j-2] +=  6*augframe[i][j-2];
            tframe[i-2][j-2] +=  12*augframe[i][j-1];
            tframe[i-2][j-2] += -12*augframe[i][j+1];
            tframe[i-2][j-2] += -6*augframe[i][j+2];
            tframe[i-2][j-2] +=  4*augframe[i+1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i+1][j-1];
            tframe[i-2][j-2] += -8*augframe[i+1][j+1];
            tframe[i-2][j-2] += -4*augframe[i+1][j+2];
            tframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            tframe[i-2][j-2] +=  2*augframe[i+2][j-1];
            tframe[i-2][j-2] += -2*augframe[i+2][j+1];
            tframe[i-2][j-2] += -1*augframe[i+2][j+2];

            tframe[i-2][j-2] /= 48;
            tframe[i-2][j-2] =  (tframe[i-2][j-2] < tfilterLawsH_THR) ? 0 : tframe[i-2][j-2];

            tframe[i-2][j-2] = 1/powf(1 + tframe[i-2][j-2],tfilterLawsH_ALPHA);

        }
    }

    FreeMatrix(augframe,sizeX);
}
void    tfilterLawsH(int sizeX,int sizeY,uchar ** yframe,int ** tframe){

    int i,j;
    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    /** @todo Procurar maneira de otimizar juntando 2 loops */
    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            tframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            tframe[i-2][j-2] +=  2*augframe[i-2][j-1];
            tframe[i-2][j-2] += -2*augframe[i-2][j+1];
            tframe[i-2][j-2] += -1*augframe[i-2][j+2];
            tframe[i-2][j-2] +=  4*augframe[i-1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i-1][j-1];
            tframe[i-2][j-2] += -8*augframe[i-1][j+1];
            tframe[i-2][j-2] += -4*augframe[i-1][j+2];
            tframe[i-2][j-2] +=  6*augframe[i][j-2];
            tframe[i-2][j-2] +=  12*augframe[i][j-1];
            tframe[i-2][j-2] += -12*augframe[i][j+1];
            tframe[i-2][j-2] += -6*augframe[i][j+2];
            tframe[i-2][j-2] +=  4*augframe[i+1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i+1][j-1];
            tframe[i-2][j-2] += -8*augframe[i+1][j+1];
            tframe[i-2][j-2] += -4*augframe[i+1][j+2];
            tframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            tframe[i-2][j-2] +=  2*augframe[i+2][j-1];
            tframe[i-2][j-2] += -2*augframe[i+2][j+1];
            tframe[i-2][j-2] += -1*augframe[i+2][j+2];
        }
    }

    FreeMatrix(augframe,sizeX);
}

/** @todo: Parametros para fazer regressao posteriormente: */
#define tfilterLawsV_THR   0.15
#define tfilterLawsV_ALPHA 5
/** @note: Qualquer correcao em tfilterLawsH deve-se refletir aqui*/
void    tfilterLawsV(int sizeX,int sizeY,uchar ** yframe,float ** tframe){

    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            tframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            tframe[i-2][j-2] +=  4*augframe[i-2][j-1];
            tframe[i-2][j-2] +=  6*augframe[i-2][j];
            tframe[i-2][j-2] +=  4*augframe[i-2][j+1];
            tframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            tframe[i-2][j-2] +=  2*augframe[i-1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i-1][j-1];
            tframe[i-2][j-2] += 12*augframe[i-1][j];
            tframe[i-2][j-2] +=  8*augframe[i-1][j+1];
            tframe[i-2][j-2] +=  2*augframe[i-1][j+2];
            tframe[i-2][j-2] += -2*augframe[i+1][j-2];
            tframe[i-2][j-2] += -8*augframe[i+1][j-1];
            tframe[i-2][j-2] += -12*augframe[i+1][j];
            tframe[i-2][j-2] += -8*augframe[i+1][j+1];
            tframe[i-2][j-2] += -2*augframe[i+1][j+2];
            tframe[i-2][j-2] += -1*augframe[i+2][j-2];
            tframe[i-2][j-2] += -4*augframe[i+2][j-1];
            tframe[i-2][j-2] += -6*augframe[i+2][j];
            tframe[i-2][j-2] += -4*augframe[i+2][j+1];
            tframe[i-2][j-2] += -1*augframe[i+2][j+2];

            tframe[i-2][j-2] /= 48;
            tframe[i-2][j-2] =  (tframe[i-2][j-2] < tfilterLawsV_THR) ? 0 : tframe[i-2][j-2];

            tframe[i-2][j-2] = 1/powf(1 + tframe[i-2][j-2],tfilterLawsV_ALPHA);

        }
    }

    FreeMatrix(augframe,sizeX);
}
void    tfilterLawsV(int sizeX,int sizeY,uchar ** yframe,int ** tframe){

    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            tframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            tframe[i-2][j-2] +=  4*augframe[i-2][j-1];
            tframe[i-2][j-2] +=  6*augframe[i-2][j];
            tframe[i-2][j-2] +=  4*augframe[i-2][j+1];
            tframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            tframe[i-2][j-2] +=  2*augframe[i-1][j-2];
            tframe[i-2][j-2] +=  8*augframe[i-1][j-1];
            tframe[i-2][j-2] += 12*augframe[i-1][j];
            tframe[i-2][j-2] +=  8*augframe[i-1][j+1];
            tframe[i-2][j-2] +=  2*augframe[i-1][j+2];
            tframe[i-2][j-2] += -2*augframe[i+1][j-2];
            tframe[i-2][j-2] += -8*augframe[i+1][j-1];
            tframe[i-2][j-2] += -12*augframe[i+1][j];
            tframe[i-2][j-2] += -8*augframe[i+1][j+1];
            tframe[i-2][j-2] += -2*augframe[i+1][j+2];
            tframe[i-2][j-2] += -1*augframe[i+2][j-2];
            tframe[i-2][j-2] += -4*augframe[i+2][j-1];
            tframe[i-2][j-2] += -6*augframe[i+2][j];
            tframe[i-2][j-2] += -4*augframe[i+2][j+1];
            tframe[i-2][j-2] += -1*augframe[i+2][j+2];
        }
    }

    FreeMatrix(augframe,sizeX);
}

/** @todo: Parametros para fazer regressao posteriormente: */
/** @note: BETA2 = (1-BETA)/174 */
#define lfilterHantaoH_BETA2 0.001724138
/** @note: Qualquer correcao em tfilterLawsH deve-se refletir aqui*/
void    lfilterHantaoH(int sizeX,int sizeY,uchar ** yframe,float **lframe){
    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            lframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i-2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            lframe[i-2][j-2] +=  1*augframe[i-1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i-1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i-1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i][j-2];
            lframe[i-2][j-2] +=  2*augframe[i][j-1];
            lframe[i-2][j-2] +=  2*augframe[i][j+1];
            lframe[i-2][j-2] +=  1*augframe[i][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i+1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i+1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+2];

            lframe[i-2][j-2] /= 26;
            lframe[i-2][j-2] = ((lframe[i-2][j-2] >= 0) && (lframe[i-2][j-2] <= 81)) ? powf(lframe[i-2][j-2]/81,.5) :
                                                                          (lfilterHantaoH_BETA2*(81-lframe[i-2][j-2]))+1;

        }
    }

    FreeMatrix(augframe,sizeX);
}
void    lfilterHantaoH(int sizeX,int sizeY,uchar ** yframe,int **lframe){
    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            lframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i-2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            lframe[i-2][j-2] +=  1*augframe[i-1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i-1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i-1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i][j-2];
            lframe[i-2][j-2] +=  2*augframe[i][j-1];
            lframe[i-2][j-2] +=  2*augframe[i][j+1];
            lframe[i-2][j-2] +=  1*augframe[i][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i+1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i+1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+2];

            }
    }

    FreeMatrix(augframe,sizeX);
}

/** @todo: Parametros para fazer regressao posteriormente: */
/** @note: BETA2 = (1-BETA)/174 */
#define lfilterHantaoV_BETA2 0.001724138
/** @note: Qualquer correcao em lfilterHantaoH deve-se refletir aqui*/
void    lfilterHantaoV(int sizeX,int sizeY,uchar ** yframe,float **lframe){
    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            lframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i-2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            lframe[i-2][j-2] +=  1*augframe[i-1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i-1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i-1][j];
            lframe[i-2][j-2] +=  2*augframe[i-1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i+1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i+1][j];
            lframe[i-2][j-2] +=  2*augframe[i+1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+2];

            lframe[i-2][j-2] /= 26;
            lframe[i-2][j-2] = ((lframe[i-2][j-2] >= 0) && (lframe[i-2][j-2] <= 81)) ? powf(lframe[i-2][j-2]/81,.5) :
                                                                          (lfilterHantaoV_BETA2*(81-lframe[i-2][j-2]))+1;

        }
    }

    FreeMatrix(augframe,sizeX);
}
void    lfilterHantaoV(int sizeX,int sizeY,uchar ** yframe,int **lframe){
    int i,j;

    uchar ** augframe;
    augframe = AllocateMatrix<uchar>(sizeX+4,sizeY+4);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            augframe[i+2][j+2] = yframe[i][j];

    for(i = 2; i < sizeX+2; ++i){
        for(j = 2; j < sizeY+2; ++j){
            lframe[i-2][j-2]  =  1*augframe[i-2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i-2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-2][j+2];
            lframe[i-2][j-2] +=  1*augframe[i-1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i-1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i-1][j];
            lframe[i-2][j-2] +=  2*augframe[i-1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i-1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+1][j-2];
            lframe[i-2][j-2] +=  2*augframe[i+1][j-1];
            lframe[i-2][j-2] +=  2*augframe[i+1][j];
            lframe[i-2][j-2] +=  2*augframe[i+1][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+1][j+2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-2];
            lframe[i-2][j-2] +=  1*augframe[i+2][j-1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+1];
            lframe[i-2][j-2] +=  1*augframe[i+2][j+2];

        }
    }

    FreeMatrix(augframe,sizeX);
}

/** @todo: Parametros para fazer regressao posteriormente: */
#define cannyEdge_GAUSS_THR         0.005
#define cannyEdge_MAX_MASK_SIZE     20
#define cannyEdge_MAG_RESOLUTION    20.0
#define cannyEdge_ORI_RESOLUTION    40.0
void    cannyEdge(int sizeX,int sizeY,uchar ** yframe,int high,int low,float s,float (*norm)(float,float),uchar ** cframe){
    int i,j,k,I1,I2;
    int width = 1;

    float xx,yy,g1,g2,g3,g4,g,xc,yc;

    float ** smx, **smy;
    float ** dx, ** dy;
    uchar ** mag, ** ori;

    smx = AllocateMatrix<float>(sizeX,sizeY);
    smy = AllocateMatrix<float>(sizeX,sizeY);
    dx  = AllocateMatrix<float>(sizeX,sizeY);
    dy  = AllocateMatrix<float>(sizeX,sizeY);

    float x,y;

    float gau[cannyEdge_MAX_MASK_SIZE];
    float dgau[cannyEdge_MAX_MASK_SIZE];

    for(i = 0; i < cannyEdge_MAX_MASK_SIZE; ++i){
        gau[i] = mgauss( (float) i,s);
        if(gau[i] < cannyEdge_GAUSS_THR){
            width = i;
            break;
        }
        dgau[i] = dgauss( (float) i,s);
    }

    /* Aplica a convolucao da imagem com o filtro Gaussiano nas direcoes X e Y */
    for(i = 0; i < sizeX; ++i){
        for(j = 0; j < sizeY; ++j){
            x = gau[0]*yframe[i][j];
            y = gau[0]*yframe[i][j];
            for(k = 1; k < width; ++k){
                I1 = (i+k)%sizeX;
                I2 = (i-k+sizeX)%sizeX;
                y += gau[k]*yframe[I1][j] +
                     gau[k]*yframe[I2][j];
                I1 = (j+k)%sizeY;
                I2 = (j-k+sizeY)%sizeY;
                x += gau[k]*yframe[i][I1] +
                     gau[k]*yframe[i][I2];
            }
            smx[i][j] = x;
            smy[i][j] = y;
            }
        }

    /* Aplica a convolucao da imagem filtrada com o detector de borda (DoG) */
    /** @todo: Tentar juntar o loop seguinte com o loop acima para nao espancar a memoria RAM */
    for(i = 0; i < sizeX; ++i){
        for(j = 0; j < sizeY; ++j){
            x = 0.0;
            y = 0.0;
            for(k = 1; k < width; ++k){
                I1 = (i+k)%sizeX;
                I2 = (i-k+sizeX)%sizeX;
                y += -gau[k]*smy[I1][j] +
                      gau[k]*smy[I2][j];
                I1 = (j+k)%sizeY;
                I2 = (j-k+sizeY)%sizeY;
                x += gau[k]*smx[i][I1] +
                     gau[k]*smx[i][I2];
            }
            dx[i][j] = x;
            dy[i][j] = y;
        }
    }

    FreeMatrix<float>(smx, sizeX);
    FreeMatrix<float>(smy, sizeX);

    mag = AllocateMatrix<uchar>(sizeX,sizeY);
    ori = AllocateMatrix<uchar>(sizeX,sizeY);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            mag[i][j] = (uchar)((*norm)(dx[i][j],dy[i][j]) * cannyEdge_MAG_RESOLUTION);

    /* remocao dos pixels que nao sao maximos locais */

    for(i = 1; i < sizeX - 1; ++i){
        for(j = 1; j < sizeY - 1; ++j){

            mag[i][j] = 0;

            xc = dx[i][j];
            yc = dy[i][j];

            if((fabs(xc)<.01) && (fabs(yc)<.01)) continue;

            g = (*norm)(xc,yc);

            if(fabs(yc) > fabs(xc))
            {
                xx = fabs(xc)/fabs(yc);
                yy = 1.0;

                g2 = (*norm)(dx[i-1][j], dy[i-1][j]);
                g4 = (*norm)(dx[i+1][j], dy[i+1][j]);
                if (xc*yc > 0.0)
                {
                  g3 = (*norm)(dx[i+1][j+1], dy[i+1][j+1]);
                  g1 = (*norm)(dx[i-1][j-1], dy[i-1][j-1]);
                }else{
                  g3 = (*norm)(dx[i+1][j-1], dy[i+1][j-1]);
                  g1 = (*norm)(dx[i-1][j+1], dy[i-1][j+1]);
                }
            }else{
                xx = fabs(yc)/fabs(xc);
                yy = 1.0;

                g2 = (*norm)(dx[i][j+1], dy[i][j+1]);
                g4 = (*norm)(dx[i][j-1], dy[i][j-1]);
                if (xc*yc > 0.0)
                {
                  g3 = (*norm)(dx[i-1][j-1], dy[i-1][j-1]);
                  g1 = (*norm)(dx[i+1][j+1], dy[i+1][j+1]);
                }else{
                  g1 = (*norm)(dx[i-1][j+1], dy[i-1][j+1]);
                  g3 = (*norm)(dx[i+1][j-1], dy[i+1][j-1]);
                }
            }
            if ( (g > (xx*g1 + (yy-xx)*g2)) &&
                 (g > (xx*g3 + (yy-xx)*g4)) )
            {
                if (g*cannyEdge_MAG_RESOLUTION <= 255){
                  mag[i][j] = (uchar)(g*cannyEdge_MAG_RESOLUTION);
                }else{
                  mag[i][j] = 255;
                }
                ori[i][j] = atan2(yc, xc) * cannyEdge_ORI_RESOLUTION;
            }else{
                mag[i][j] = 0;
                ori[i][j] = 0;
            }
        }
    }

    FreeMatrix<float>(dx, sizeX);
    FreeMatrix<float>(dy, sizeX);

    /* Threshold com histerese */

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            if(mag[i][j] >= high) trace(i,j,low,cframe,mag,ori,sizeX,sizeY);

    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            if(cframe[i][j] == 0) cframe[i][j] = 255;
            else cframe[i][j] = 0;

}

int trace (int i, int j, int low, uchar ** im,uchar ** mag,uchar ** ori,int sizeX,int sizeY)
{
        int n,m;
        char flag = 0;

        if (im[i][j] == 0)
        {
          im[i][j] = 255;
          flag=0;
          for (n= -1; n<=1; n++)
          {
            for(m= -1; m<=1; m++)
            {
              if ((i==0) && (m==0)) continue;
              if (range(i+n, j+m,sizeX,sizeY) && mag[i+n][j+m] >= low)
                if (trace(i+n, j+m, low, im, mag, ori,sizeX,sizeY))
                {
                    flag=1;
                    break;
                }
            }
            if (flag) break;
          }
          return(1);
        }
        return(0);
}
int range (int i, int j,int sizeX,int sizeY)
{
    if((i<0) || (i>= sizeX)) return 0;
    if((j<0) || (j>= sizeY)) return 0;
    return 1;
}
/** @todo: Parametros para fazer regressao posteriormente: */
#define edgeDensity_STANDARD    720
void    edgeDensity(int sizeX,int sizeY,uchar ** yframe,float ** eframe){

    fftw_complex * FT_yframe, * FT_filter;
    float ** fgaussian;

    FT_yframe = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sizeX * sizeY);
    FT_filter = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * sizeX * sizeY);

    fgaussian = AllocateMatrix<float>(sizeX,sizeY);

    int i,j;
    for(i = 0; i < sizeX; ++i)
        for(j = 0; j < sizeY; ++j)
            fgaussian[i][j] = gauss2d(i,j,edgeDensity_STANDARD);

    FFT<uchar>(yframe,FT_yframe,sizeX,sizeY);
    FFT<float>(fgaussian,FT_filter,sizeX,sizeY);

    for(i = 0; i < sizeX*sizeY; ++i)
        FT_yframe[i][0] = FT_yframe[i][0] * FT_filter[i][0];

    /* Necessario normalizar? */

    IFFT<float>(FT_yframe,eframe,sizeX,sizeY);

    fftw_free(FT_yframe);
    fftw_free(FT_filter);
    FreeMatrix<float>(fgaussian,sizeX);


}
