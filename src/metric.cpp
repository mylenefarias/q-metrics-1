#include "metric.h"

#define packetLoss_LIMIAR (90/255);
void    packetLoss(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,QString fName)
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
    QFile        f_output;
    QTextStream  ts(&f_output);

    if(f_output.isOpen()) f_output.close();

    fName.remove(".yuv");
    fName.append("_packet_loss.txt");

    f_output.setFileName(fName);
    f_output.open(QIODevice::WriteOnly);

    ts << "Frame"
       << "\t"
       << "Perda de pacotes(sem contar bordas vizinhas)"
       << "\t"
       << "Perda de pacotes(contando bordas vizinhas)"
       << "\n";

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

        ts << frame_nr
           << "\t"
           << count
           << "\t"
           << count2
           << "\n";
    }

    FreeMatrix(cframeY,sizeX);
    FreeMatrix(dif,sizeX);
    FreeMatrix(dif2,sizeX);
    FreeMatrix(dif3,sizeX);
    f_output.close();

}

void    blockingVlachos(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,QString fName)
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

         QFile          f_output;
         QTextStream    ts(&f_output);

         if(f_output.isOpen()) f_output.close();

         fName.remove(".yuv");
         fName.append("_vlachos.txt");

         f_output.setFileName(fName);
         f_output.open(QIODevice::WriteOnly);

         cframeY = AllocateMatrix<uchar>(sizeX,sizeY);
         hWindow     = AllocateMatrix<double>(sizeX,sizeY);

         hammingWindow(hWindow,sizeY,sizeX);
         s1 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s2 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s3 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s4 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s5 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s6 = AllocateMatrix<uchar>(subsizeX,subsizeY);
         s7 = AllocateMatrix<uchar>(subsizeX,subsizeY);

         ts << "Frame \tBlocagem \n";

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

             ts << frame_nr
                << "\t"
                << block_metric
                << "\n";
         }

         FreeMatrix(cframeY,sizeX);
         FreeMatrix(s1,subsizeX);
         FreeMatrix(s2,subsizeX);
         FreeMatrix(s3,subsizeX);
         FreeMatrix(s4,subsizeX);
         FreeMatrix(s5,subsizeX);
         FreeMatrix(s6,subsizeX);
         FreeMatrix(s7,subsizeX);

         f_output.close();
}

void    blurringWinkler(int sizeX,int sizeY,int total_frame_nr,uchar * ybuf,int high,int low,float s,float (*norm)(float,float),QString fName)
{
    int i,j,k,x,y,pos;
    int index = 0;
    int frame_nr;

    double blur_index, blur_avg=0, blur_max=-1, blur_min=1000;

    uchar ** cframeY;
    uchar ** edges;

    cframeY = AllocateMatrix<uchar>(sizeX,sizeY);
    edges   = AllocateMatrix<uchar>(sizeX,sizeY);

    QFile        f_output;
    QTextStream  ts(&f_output);

    if(f_output.isOpen()) f_output.close();

    fName.remove(".yuv");
    fName.append("_blur.txt");

    f_output.setFileName(fName);
    f_output.open(QIODevice::WriteOnly);
    ts << "Frame\tBorragem\n";

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
        // write results for frame in file
        ts << frame_nr
           << "\t"
           << blur_index
           << "\n";
    }
    blur_avg /= total_frame_nr; // avg value for blur
    // Add avg, min and max values to the file
    ts << "Avg value = "
       << blur_avg
       << "\t Max value = "
       << blur_max
       << "\t Min value = "
       << blur_min
       << "\n";

    FreeMatrix(cframeY,sizeX);
    FreeMatrix(edges,sizeX);

    f_output.close();
}

double SSIM(cv::Mat& src1,
            cv::Mat& src2,
            const double K1,
            const double K2,
            const int L,
            const int downsamplewidth,
            const int gaussian_window,
            const double gaussian_sigma)
{

    const double C1 = (K1 * L) * (K1 * L); //6.5025 C1 = (K(1)*L)^2;
    const double C2 = (K2 * L) * (K2 * L); //58.5225 C2 = (K(2)*L)^2;

    int x = src1.cols; /** largura */
    int y = src1.rows; /** altura  */

    int rate_downsampling = std::max(1, int((std::min(x,y) / downsamplewidth) + 0.5));

    cv::Size size_L(x,y);
    cv::Size size(x/rate_downsampling,y/rate_downsampling);

    cv::Mat img1(size,CV_32FC1);
    cv::Mat img2(size,CV_32FC1);

    /// Converte 8 bit para 32 bit float
    cv::Mat img1_L(size_L,CV_32FC1);
    cv::Mat img2_L(size_L,CV_32FC1);

    src1.convertTo(img1_L,img1_L.depth());
    src2.convertTo(img2_L,img2_L.depth());

    /// Subamostragem
    cv::resize(img1_L,img1,cv::Size(img1.cols,img1.rows));
    cv::resize(img2_L,img2,cv::Size(img2.cols,img2.rows));

    /// Alocacao de buffers
    cv::Mat img1_sq(size,CV_32FC1);
    cv::Mat img2_sq(size,CV_32FC1);
    cv::Mat img1_img2(size,CV_32FC1);

    cv::pow(img1,2.0,img1_sq);
    cv::pow(img2,2.0,img2_sq);
    cv::multiply(img1,img2,img1_img2);

    /// Alocacao de sigma e mu
    cv::Mat mu1(size,CV_32FC1);
    cv::Mat mu2(size,CV_32FC1);
    cv::Mat mu1_sq(size,CV_32FC1);
    cv::Mat mu2_sq(size,CV_32FC1);
    cv::Mat mu1_mu2(size,CV_32FC1);

    cv::Mat sigma1_sq(size,CV_32FC1);
    cv::Mat sigma2_sq(size,CV_32FC1);
    cv::Mat sigma12(size,CV_32FC1);

    cv::Mat temp1(size,CV_32FC1);
    cv::Mat temp2(size,CV_32FC1);
    cv::Mat temp3(size,CV_32FC1);

    cv::Mat ssim_map(size,CV_32FC1);

    /// Computacao preliminar
    cv::GaussianBlur(img1,mu1,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::GaussianBlur(img1,mu2,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);

    cv::pow(mu1,2.0,mu1_sq);
    cv::pow(mu2,2.0,mu2_sq);
    cv::multiply(mu1,mu2,mu1_mu2);

    cv::GaussianBlur(img1_sq,sigma1_sq,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma1_sq,1.0,mu1_sq,-1.0,0.0,sigma1_sq);

    cv::GaussianBlur(img2_sq,sigma2_sq,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma2_sq,1.0,mu2_sq,-1.0,0.0,sigma2_sq);

    cv::GaussianBlur(img1_img2,sigma12,cv::Size(gaussian_window,gaussian_window),gaussian_sigma);
    cv::addWeighted(sigma12,1.0,mu1_mu2,-1.0,0.0,sigma12);

    // (2*mu1_mu2 + C1)
    mu1_mu2.convertTo(temp1,temp1.depth(),2.0,C1);
    // (2*sigma12 + C2)
    sigma12.convertTo(temp2,temp2.depth(),2.0,C2);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    cv::multiply(temp1,temp2,temp3);

    // (mu1_sq + mu2_sq + C1)
    cv::add(mu1_sq,mu2_sq,temp1);
    cv::add(temp1,cv::Scalar(C1),temp1);

    // (sigma1_sq + sigma2_sq + C2)
    cv::add(sigma1_sq,sigma2_sq,temp2);
    cv::add(temp2,cv::Scalar(C2),temp2);

    // ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    cv::multiply(temp1,temp2,temp1);

    // ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
    cv::divide(temp3,temp1,ssim_map);

    cv::Scalar index_scalar(cv::mean(ssim_map));
    return index_scalar.val[0];

}

