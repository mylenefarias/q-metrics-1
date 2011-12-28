#include "old/old_metric.h"

#define measure_lines_v2_LIMIAR (90/255);
void    measure_lines_v2(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,string fName)
{
    int i,x,y,pos;
    int frame_nr;

    int count = 0;
    int count2 = 0;

    uchar  ** cframeY;
    uchar  ** dif;    /** Diferencas entre as bordas, sem limiar */
    uchar  ** dif2;   /** Diferencas entre as bordas consideradas significantes */
    uchar  ** dif3;   /** Diferencas entre as bordas contando os vizinhos */

    cframeY = AllocateMatrix<uchar>(sizeX,sizeY);
    dif     = AllocateMatrix<uchar>(sizeX,sizeY);
    dif2    = AllocateMatrix<uchar>(sizeX,sizeY);
    dif3    = AllocateMatrix<uchar>(sizeX,sizeY);

    int dif_inter;
    int dif_intra;

    /// Dump para o arquivo fName
    FILE * f_output;
    f_output = fopen(fName.c_str(),"w");

    fprintf(f_output,"Frame"
                     "\t"
                     "Perda de pacotes(sem contar bordas vizinhas)"
                     "\t"
                     "Perda de pacotes(contando bordas vizinhas)"
                     "\n");

    /// Processamento quadro a quadro
    for(frame_nr = 0; frame_nr < total_frame_nr; ++frame_nr){

        x = 0;
        y = 0;
        pos = frame_nr*sizeX*sizeY;

        for(i = 0; i < sizeX*sizeY; ++i){
            cframeY[x][y] = *(ybuf+pos+i);
            ++x;
            if(x == sizeX)
                x = 0, ++y;
        }

        /// Teste das bordas 16 x 16
        for(x = 0; x <= sizeX; x += 16){
            for(y = 0; y <= sizeY; y += 16){
                /// Linhas
                /// Índices dentro da imagem
                if((x > 0) && ((x + 15) < sizeX)){ // 14 ou 15?
                    /// Borda superior do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x][y+i]-cframeY[x-1][y+i]);
                        dif_intra += abs(cframeY[x][y+i]-cframeY[x+1][y+i]);    /// Do lado de dentro da borda
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x][y+i]   = dif_inter;
                        dif[x+1][y+i] = dif_intra;
                        /// Repensar esta estória...
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x][y+i] = 1;
                            count++;
                        }
                    }
                    /// Borda inferior do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+15][y+i]-cframeY[x+16][y+i]);
                        dif_intra += abs(cframeY[x+15][y+i]-cframeY[x+14][y+i]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+14][y+i] = dif_inter;
                        dif[x+15][y+i] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+15][y+i] = 1;
                            count++;
                        }
                    }
                }else if(x <= 0){
                    /// Borda inferior do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+15][y+i]-cframeY[x+16][y+i]);
                        dif_intra += abs(cframeY[x+15][y+i]-cframeY[x+14][y+i]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+14][y+i] = dif_inter;
                        dif[x+15][y+i] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+15][y+i] = 1;
                            count++;
                        }
                    }
                }else if((x+15) >= sizeX){
                    /// Borda superior do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x][y+i]-cframeY[x-1][y+i]);
                        dif_intra += abs(cframeY[x][y+i]-cframeY[x+1][y+i]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x][y+i]   = dif_inter;
                        dif[x+1][y+i] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x][y+i] = 1;
                            count++;
                        }
                    }
                }
                /// Colunas
                /// Índices dentro da imagem
                if((y > 0) && ((y+15) < sizeY)){
                    /// Borda esquerda do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+i][y]-cframeY[x+i][y-1]);
                        dif_intra += abs(cframeY[x+i][y]-cframeY[x+i][y+1]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+i][y]   = dif_inter;
                        dif[x+i][y+1] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+i][y] = 1;
                            count++;
                        }
                    }
                    /// Borda direita do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+i][y+15]-cframeY[x+i][y+14]);
                        dif_intra += abs(cframeY[x+i][y+15]-cframeY[x+i][y+16]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+i][y+15] = dif_inter;
                        dif[x+i][y+14] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+i][y+15] = 1;
                            count++;
                        }
                    }
                }else if(y <= 0){
                    /// Borda direita do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+i][y+15]-cframeY[x+i][y+14]);
                        dif_intra += abs(cframeY[x+i][y+15]-cframeY[x+i][y+16]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+i][y+15] = dif_inter;
                        dif[x+i][y+14] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+i][y+15] = 1;
                            count++;
                        }
                    }
                }else if((y+15) >= sizeY){
                    /// Borda esquerda do bloco
                    dif_inter = 0;
                    dif_intra = 0;
                    for(i = 0; i < 15 ; ++i){    // 14 ou 15?
                        dif_inter += abs(cframeY[x+i][y]-cframeY[x+i][y-1]);
                        dif_intra += abs(cframeY[x+i][y]-cframeY[x+i][y+1]);
                    }
                    for(i = 0; i < 15 ; ++i){
                        dif[x+i][y]   = dif_inter;
                        dif[x+i][y+1] = dif_intra;
                        if((dif_inter > (90/255)) && ((dif_inter/dif_intra) > 1)){ /** @todo substituir 90/255 por macro */
                            dif2[x+i][y] = 1;
                            count++;
                        }
                    }
                }
                // COUNT(i:i+15,j:j+15)=count;
            }
        }

        /// Reajuste das bordas para contar a vizinhanca
        for(x = 0; x <= sizeX; x += 16){
            for(y = 0; y <= sizeY; y += 16){
                count2 = 0; /// Contador de bordas

                /// Linhas e colunas centrais
                if((x > 0) && ((x+15) < sizeX) && (y > 0) && ((y+15) < sizeY)){
                    for(i = 0; i < 15; ++i){
                        /// Borda superior ou inferior do bloco anterior
                        if(dif2[x][y+i] || dif2[x-1][y+i]){
                            dif3[x][y+i] = 1;
                            count2++;
                        }
                        /// Borda superior ou inferior do bloco seguinte
                        if(dif2[x+15][y+i] || dif2[x+16][y+i]){
                            dif3[x+15][y+i] = 1;
                            count2++;
                        }
                        /// Borda da esquerda ou da direita do bloco anterior
                        if(dif2[x+i][y] || dif2[x+i][y-1]){
                            dif3[x+i][y] = 1;
                            count2++;
                        }
                        /// Borda da esquerda ou da direita do bloco seguinte
                        if(dif2[x+i][y+15] || dif2[x+i][y+16]){
                            dif3[x+i][y+15] = 1;
                            count2++;
                        }
                    }
                }else if((x == 0) && (y > 0) && ((y+15)<sizeY)){
                    for(i = 0; i < 15; ++i){
                        if(dif2[x+15][y+i] || dif2[x+16][y+i]){
                            dif3[x+15][y+i] = 1;
                            count2++;
                        }
                        if(dif2[x+i][y] || dif2[x+i][y-1]){
                            dif3[x+i][y] = 1;
                            count2++;
                        }
                        if(dif2[x+i][y+15] || dif2[x+i][y+16]){
                            dif3[x+i][y+15] = 1;
                            count2++;
                        }
                    }
                }else if(((x+15) >= sizeX) && (y > 0) && ((y+15)<sizeY)){
                    for(i = 0; i < 15; ++i){
                        if(dif2[x][y+i] || dif2[x-1][y+i]){
                            dif3[x][y+i] = 1;
                            count2++;
                        }
                        if(dif2[x+i][y] || dif2[x+i][y-1]){
                            dif3[x+i][y] = 1;
                            count2++;
                        }
                        if(dif2[x+i][y+15] || dif2[x+i][y+16]){
                            dif3[x+i][y+15] = 1;
                            count2++;
                        }
                    }
                }else if((x > 0) && ((x+15)<sizeX) && (y==0)){
                    if(dif2[x][y+i] || dif2[x-1][y+i]){
                        dif3[x][y+i] = 1;
                        count2++;
                    }
                    if(dif2[x+15][y+i] || dif2[x+16][y+i]){
                        dif3[x+15][y+i] = 1;
                        count2++;
                    }
                    if(dif2[x+i][y+15] || dif2[x+i][y+16]){
                        dif3[x+i][y+15] = 1;
                        count2++;
                    }
                }else if((x > 0) && ((x+15)<sizeX) && ((y+15)>=sizeY)){
                    if(dif2[x][y+i] || dif2[x-1][y+i]){
                        dif3[x][y+i] = 1;
                        count2++;
                    }
                    if(dif2[x+15][y+i] || dif2[x+16][y+i]){
                        dif3[x+15][y+i] = 1;
                        count2++;
                    }
                    if(dif2[x+i][y] || dif2[x+i][y-1]){
                        dif3[x+i][y] = 1;
                        count2++;
                    }
                }

            }
        }

        fprintf(f_output,"%d\t%d\t%d\n",frame_nr,count,count2);
    }

    FreeMatrix(cframeY,sizeX);
    FreeMatrix(dif,sizeX);
    FreeMatrix(dif2,sizeX);
    FreeMatrix(dif3,sizeX);
    fclose(f_output);

}

void    blockingVlachos(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,string fName)
{
         int i,x,y,pos;
         int frame_nr;


         uchar  ** cframeY;
         double ** hWindow;
         uchar  ** s1,** s2,** s3,** s4,
                ** s5,** s6,** s7;

         double inter_similarity;
         double intra_similarity;

         double block_metric;

         int subsizeX = sizeX/8;
         int subsizeY = sizeY/8;

         FILE * f_output;
         f_output = fopen(fName.c_str(),"w");

         fprintf(f_output,"Frame"
                          "\t"
                          "Blocagem"
                          "\n");

         cframeY = AllocateMatrix<uchar>(sizeX,sizeY);
         hWindow = AllocateMatrix<double>(sizeX,sizeY);

         hammingWindow(hWindow,sizeY,sizeX);
         s1 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s2 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s3 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s4 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s5 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s6 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s7 = AllocateMatrix<uchar>(subsizeX,subsizeY);

         for(frame_nr = 0; frame_nr < total_frame_nr; ++frame_nr){

             x = 0;
             y = 0;
             pos = frame_nr*sizeX*sizeY;

             for(i = 0; i < sizeX*sizeY; ++i){
                 cframeY[x][y] = *(ybuf+pos+i) * hWindow[x][y];
                 ++x;
                 if(x == sizeX)
                     x = 0, ++y;
             }

             inter_similarity = 0;
             intra_similarity = 0;

             downsample<uchar>(cframeY, s1,7,7,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s2,0,7,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s3,7,0,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s4,0,0,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s5,0,1,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s6,1,0,8,8,sizeX,sizeY);
             downsample<uchar>(cframeY, s7,1,1,8,8,sizeX,sizeY);

             intra_similarity += Peak_Correlation(s4,s5,subsizeX,subsizeY);
             intra_similarity += Peak_Correlation(s4,s6,subsizeX,subsizeY);
             intra_similarity += Peak_Correlation(s4,s7,subsizeX,subsizeY);

             inter_similarity += Peak_Correlation(s1,s2,subsizeX,subsizeY);
             inter_similarity += Peak_Correlation(s1,s3,subsizeX,subsizeY);
             inter_similarity += Peak_Correlation(s1,s4,subsizeX,subsizeY);

             if(inter_similarity != 0.0){
                 block_metric = (intra_similarity/inter_similarity);
             }else{
                 block_metric = 0;
             }

             fprintf(f_output,"%d\t%f\n",frame_nr,block_metric);
         }

         FreeMatrix(cframeY,sizeX);
         FreeMatrix(s1,subsizeX);
         FreeMatrix(s2,subsizeX);
         FreeMatrix(s3,subsizeX);
         FreeMatrix(s4,subsizeX);
         FreeMatrix(s5,subsizeX);
         FreeMatrix(s6,subsizeX);
         FreeMatrix(s7,subsizeX);

         fclose(f_output);
}

void    blurringWinkler(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),string fName)
{
    int i,j,k,x,y,pos;
    int index = 0;
    int frame_nr;

    double blur_index, blur_avg=0, blur_max=-1, blur_min=1000;

    uchar ** cframeY;
    uchar ** edges;

    cframeY = AllocateMatrix<uchar>(sizeX,sizeY);
    edges   = AllocateMatrix<uchar>(sizeX,sizeY);


    FILE * f_output;
    f_output = fopen(fName.c_str(),"w");

    fprintf(f_output,"Frame"
                     "\t"
                     "Borragem"
                     "\n");

    for(frame_nr = 0; frame_nr < total_frame_nr; ++frame_nr){

        blur_index = 0;
        x = 0;
        y = 0;
        pos = frame_nr*sizeX*sizeY;

        for(i = 0; i < sizeX*sizeY; ++i){
            cframeY[x][y] = *(ybuf+pos+i);
            ++x;
            if(x == sizeX)
                x = 0, ++y;
        }

        cannyEdge(sizeX,sizeY,cframeY,high,low,s,norm,edges);

        /* verificar ordem de alinhamento de sizeX e sizeY esta certa */
        int    p1,  p2;
        int    length = 0;
        double max, min;
        double dif, grad;
        double mean_length_right = 0;
        double mean_length_left  = 0;
        double mean_length       = 0;


        for(i = 0; i < sizeY; ++i){
            for(j = 0; j < sizeX; ++j){

                if(edges[i][j] > 0){

                    index++;

                    /* Lado direito da borda      */
                    /* Checa por maximo ou minimo */
                    dif  = cframeY[i][j+1] - cframeY[i][j];
                    grad = (dif > 0) ? 1 : -1;

                    if(grad > 0){

                        p2  = -1;
                        k   = j;
                        max = cframeY[i][k];
                        while((max<cframeY[i][k+1])&&(k<sizeX)){
                            max = cframeY[i][k++];
                            p2  = k;
                        }
                    }
                    else if(grad < 0)
                    {
                        p2  = -1;
                        k   = j;
                        min = cframeY[i][k];
                        while((min>cframeY[i][k+1])&&(k<sizeX)){
                            min = cframeY[i][k++];
                            p2  = k;
                        }
                    }

                    length             = abs(j - p2);
                    mean_length_right += length;

                    /* Lado esquerdo da borda     */
                    /* Checa por maximo ou minimo */
                    dif  = cframeY[i][j-1] - cframeY[i][j];
                    grad = (dif > 0) ? 1 : -1;

                    if(grad > 0){

                        p1  = -1;
                        k   = j;
                        max = cframeY[i][k];
                        while((max<cframeY[i][k-1])&&(k>1)){
                            max = cframeY[i][k--];
                            p1  = k;
                        }
                    }
                    else if(grad < 0)
                    {
                        p1  = -1;
                        k   = j;
                        min = cframeY[i][k];
                        while((min>cframeY[i][k-1])&&(k>1)){
                            min = cframeY[i][k--];
                            p1  = k;
                        }
                    }

                    length             = abs(j - p1);
                    mean_length_left += length;
                }
            }
        }

        mean_length = (mean_length_left/index + mean_length_right/index)/2;
        blur_index  = mean_length;

        blur_avg += blur_index;
        if (blur_index > blur_max)
                blur_max=blur_index;
        if (blur_index < blur_min)
                blur_min = blur_index;

        fprintf(f_output,"%d\t%f\n",frame_nr,blur_index);

    }

    /// Valor medio do indice de borragem
    blur_avg /= total_frame_nr;

    /// Coloca o valor minimo, medio e maximo no final do arquivo
    fprintf(f_output,"Avg value = "
                     "%f"
                     "\t Max value = "
                     "%f"
                     "\t Min value = "
                     "%f"
                     "\n",blur_avg,blur_max,blur_min);

    FreeMatrix(cframeY,sizeX);
    FreeMatrix(edges,sizeX);

    fclose(f_output);
}
