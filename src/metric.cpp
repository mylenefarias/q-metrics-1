#include "metric.h"

double   blockingVlachos(cv::Mat & src)
{
    double block_metric;

    double inter_similarity;
    double intra_similarity;

    int subrows = src.rows/8;
    int subcols = src.cols/8;

    cv::Mat src_w(src.rows,src.cols,CV_64FC1);
    windowHamming(src,src_w);

    cv::Mat s1(subrows,subcols,CV_64FC1);
    cv::Mat s2(subrows,subcols,CV_64FC1);
    cv::Mat s3(subrows,subcols,CV_64FC1);
    cv::Mat s4(subrows,subcols,CV_64FC1);
    cv::Mat s5(subrows,subcols,CV_64FC1);
    cv::Mat s6(subrows,subcols,CV_64FC1);
    cv::Mat s7(subrows,subcols,CV_64FC1);

    downsample(src_w,s1,7,7);
    downsample(src_w,s2,0,7);
    downsample(src_w,s3,7,0);
    downsample(src_w,s4,0,0);
    downsample(src_w,s5,0,1);
    downsample(src_w,s6,1,0);
    downsample(src_w,s7,1,1);

    intra_similarity = maxCorr2D(s4,s5) + maxCorr2D(s4,s6) + maxCorr2D(s4,s7);
    inter_similarity = maxCorr2D(s1,s2) + maxCorr2D(s1,s3) + maxCorr2D(s1,s4);

    if(inter_similarity != 0){
        block_metric = (intra_similarity/inter_similarity);
    }else{
        block_metric = 0;
    }

    return block_metric;
}

double  blurringWinkler(cv::Mat & src,double threshold1,double threshold2,int aperture_size,BlurWinklerType output_options)
{
    double blur_index = 0;
    double blur_min = -1;
    double blur_max = 1000;

    cv::Mat edges(src.rows,src.cols,CV_8UC1);
    cv::Canny(src,edges,threshold1,threshold2,aperture_size);

    int    index = 0;
    int    p1,  p2, k;
    int    length = 0;
    double max, min;
    double dif, grad;
    double mean_length_right = 0;
    double mean_length_left  = 0;
    double mean_length       = 0;

    for(int i = 0; i < src.rows; ++i){
        for(int j = 0; j < src.cols; ++j){

            if(edges.at<uchar>(i,j) > 0){
                index++;
                /** Lado direito da borda */
                dif  = src.at<uchar>(i,j+1) - src.at<uchar>(i,j);
                grad = (dif > 0) ? 1 : -1;

                if(grad > 0){
                    p2  = -1;
                    k   = j;
                    max = src.at<uchar>(i,k);
                    while((max < src.at<uchar>(i,k+1))&&(k<src.cols)){
                        max = src.at<uchar>(i,k++);
                        p2  = k;
                    }
                }else if(grad < 0)
                {
                    p2  = -1;
                    k   = j;
                    min = src.at<uchar>(i,k);
                    while((min > src.at<uchar>(i,k+1))&&(k<src.cols)){
                        min = src.at<uchar>(i,k++);
                        p2  = k;
                    }
                }

                length = abs(j - p2);
                mean_length_right += length;

                /** Lado esquerdo da borda */
                dif  = src.at<uchar>(i,j-1) - src.at<uchar>(i,j);
                grad = (dif > 0) ? 1 : -1;

                if(grad > 0){
                    p1  = -1;
                    k   = j;
                    max = src.at<uchar>(i,k);
                    while((max<src.at<uchar>(i,k-1))&&(k>1)){
                        max = src.at<uchar>(i,k--);
                        p1  = k;
                    }
                }else if(grad < 0)
                {
                    p1  = -1;
                    k   = j;
                    min = src.at<uchar>(i,k);
                    while((min>src.at<uchar>(i,k-1))&&(k>1)){
                        min = src.at<uchar>(i,k--);
                        p1  = k;
                    }
                }

                length = abs(j - p1);
                mean_length_left += length;
            }

        }
    }

    mean_length = (mean_length_left/index + mean_length_right/index)/2;
    blur_index  = mean_length;

    switch(output_options){
        case BLUR_INDEX:
            return blur_index;
        case BLUR_MIN:
        if(blur_index < blur_min)
            blur_min = blur_index;
            return blur_min;
        case BLUR_MAX:
        if(blur_index > blur_max)
            blur_max = blur_index;
            return blur_max;
        default:
            break;
    }
    return 0.0;
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

