#include <img.hpp>

cv::Mat buffer2CVMat(int sizeX,int sizeY, uchar * ybuf)
{
    cv::Mat image(cv::Size(sizeX,sizeY), CV_8UC1, ybuf);
    return image;
}

void conv2D(const cv::Mat &img, cv::Mat& dest, const cv::Mat& kernel, ConvolutionType ctype, int btype) {

  cv::Mat source = img;
  cv::Mat flip_kernel(kernel.rows,kernel.cols,kernel.type());
  cv::flip(kernel,flip_kernel,-1);

  if(CONVOLUTION_FULL == ctype) {
    source = cv::Mat();
    const int additionalRows = kernel.rows-1, additionalCols = kernel.cols-1;
    cv::copyMakeBorder(img, source, (additionalRows+1)/2, additionalRows/2, (additionalCols+1)/2, additionalCols/2, cv::BORDER_CONSTANT, cv::Scalar(0));
  }

  cv::Point anchor(kernel.cols - kernel.cols/2 - 1, kernel.rows - kernel.rows/2 - 1);
  //int borderMode = cv::BORDER_CONSTANT;
  int borderMode = btype;

  cv::filter2D(source, dest, img.depth(), flip_kernel, anchor, 0, borderMode);

  if(CONVOLUTION_VALID == ctype) {
    dest = dest.colRange((kernel.cols-1)/2, dest.cols - kernel.cols/2)
               .rowRange((kernel.rows-1)/2, dest.rows - kernel.rows/2);
  }
}

void filterLawsH(cv::Mat & src,cv::Mat & dest,float r=48.0)
{
    conv2D(src,dest,(cv::Mat_<float>(5,5) <<
                    1/r,2/r ,0,-2/r ,-1/r,
                    4/r,8/r ,0,-8/r ,-4/r,
                    6/r,12/r,0,-12/r,-6/r,
                    4/r,8/r ,0,-8/r ,-4/r,
                    1/r,2/r ,0,-2/r ,-1/r));
    return ;
}


void filterLawsV(cv::Mat & src,cv::Mat & dest,float r=48.0)
{
    conv2D(src,dest,(cv::Mat_<float>(5,5) <<
                    1/r, 4/r,  6/r, 4/r, 1/r,
                    2/r, 8/r, 12/r, 8/r, 2/r,
                    0  , 0  ,  0  , 0  , 0  ,
                   -2/r,-8/r,-12/r,-8/r,-2/r,
                   -1/r,-4/r, -6/r,-4/r,-1/r));
    return ;
}

void filterHantaoH(cv::Mat & src,cv::Mat & dest)
{
    conv2D(src,dest,(cv::Mat_<float>(5,5) <<
                    1,1,0,1,1,
                    1,2,0,2,1,
                    1,2,0,2,1,
                    1,2,0,2,1,
                    1,1,0,1,1));
    return ;
}

void filterHantaoV(cv::Mat & src,cv::Mat & dest)
{
    conv2D(src,dest,(cv::Mat_<float>(5,5) <<
                    1,1,1,1,1,
                    1,2,2,2,1,
                    0,0,0,0,0,
                    1,2,2,2,1,
                    1,1,1,1,1));
    return ;
}

/// Não é desejado utilizar essa funcao in-place - adequar um const no src
/// @todo Está com resultado diferente do MATLAB
void analysisTexture(cv::Mat & src,cv::Mat & dest)
{
    cv::Mat t1(src.rows,src.cols,src.type());
    cv::Mat t2(src.rows,src.cols,src.type());

    /// O padrão do OpenCV já faz padding simétrico (cv::BORDER_DEFAULT = cv::BORDER_REFLECT_101)
    filterLawsH(src,t1);
    filterLawsV(src,t2);

    t1 = cv::abs(t1);
    t2 = cv::abs(t2);

    for(int i = 0; i < src.rows; ++i)
        for(int j = 0; j < src.cols; ++j)
            if((t1.at<uchar>(i,j) > 0.15) || (t2.at<uchar>(i,j) > 0.15))
              dest.at<uchar>(i,j) = t1.at<uchar>(i+5,j+5) + t2.at<uchar>(i+5,j+5);

    return ;
}

/// Não é desejado utilizar essa funcao in-place - adequar um const no src
void analysisContrast(cv::Mat & src,cv::Mat & dest)
{
    cv::Scalar mean;
    cv::Scalar std;

    cv::Mat c1(src.rows,src.cols,src.type());
    cv::Mat c2(src.rows+33,src.cols+33,src.type());

    /// Filtra a imagem utilizando a funcao de sensitividade de contraste (CSF) no domínio espacial
    conv2D(src,c1,(cv::Mat_<float>(33,33) <<
                  4.1243e-08,3.2397e-08,1.1371e-08,-2.0774e-08,-6.0455e-08,-1.1145e-07,-1.8157e-07,-2.6422e-07,-3.301e-07,-3.4356e-07,-2.856e-07,-1.6191e-07,-6.9515e-10,1.4841e-07,2.343e-07,2.5095e-07,2.4572e-07,2.5095e-07,2.343e-07,1.4841e-07,-6.9515e-10,-1.6191e-07,-2.856e-07,-3.4356e-07,-3.301e-07,-2.6422e-07,-1.8157e-07,-1.1145e-07,-6.0455e-08,-2.0774e-08,1.1371e-08,3.2397e-08,4.1243e-08,
                  3.2397e-08,2.2705e-08,9.2313e-09,-9.8996e-09,-4.9927e-08,-1.1915e-07,-1.9687e-07,-2.4966e-07,-2.6984e-07,-2.7684e-07,-2.7219e-07,-2.0553e-07,1.1891e-08,4.6019e-07,1.1197e-06,1.766e-06,2.0425e-06,1.766e-06,1.1197e-06,4.6019e-07,1.1891e-08,-2.0553e-07,-2.7219e-07,-2.7684e-07,-2.6984e-07,-2.4966e-07,-1.9687e-07,-1.1915e-07,-4.9927e-08,-9.8996e-09,9.2313e-09,2.2705e-08,3.2397e-08,
                  1.1371e-08,9.2313e-09,7.1241e-09,-1.0239e-08,-5.5309e-08,-1.0363e-07,-1.2772e-07,-1.5906e-07,-2.5775e-07,-4.0892e-07,-4.9915e-07,-4.3263e-07,-2.7198e-07,-2.4771e-07,-5.5069e-07,-1.0417e-06,-1.2882e-06,-1.0417e-06,-5.5069e-07,-2.4771e-07,-2.7198e-07,-4.3263e-07,-4.9915e-07,-4.0892e-07,-2.5775e-07,-1.5906e-07,-1.2772e-07,-1.0363e-07,-5.5309e-08,-1.0239e-08,7.1241e-09,9.2313e-09,1.1371e-08,
                 -2.0774e-08,-9.8996e-09,-1.0239e-08,-3.2372e-08,-4.0323e-08,-1.2704e-08,-3.3299e-08,-2.0036e-07,-4.3824e-07,-5.3327e-07,-3.8192e-07,-7.8348e-08,3.1609e-07,9.8584e-07,2.1171e-06,3.3751e-06,3.9459e-06,3.3751e-06,2.1171e-06,9.8584e-07,3.1609e-07,-7.8348e-08,-3.8192e-07,-5.3327e-07,-4.3824e-07,-2.0036e-07,-3.3299e-08,-1.2704e-08,-4.0323e-08,-3.2372e-08,-1.0239e-08,-9.8996e-09,-2.0774e-08,
                 -6.0455e-08,-4.9927e-08,-5.5309e-08,-4.0323e-08,3.1377e-08,4.4257e-08,-1.2201e-07,-2.9598e-07,-2.008e-07,5.2816e-08,2.2358e-08,-4.6194e-07,-1.0754e-06,-1.5256e-06,-1.945e-06,-2.4459e-06,-2.7024e-06,-2.4459e-06,-1.945e-06,-1.5256e-06,-1.0754e-06,-4.6194e-07,2.2358e-08,5.2816e-08,-2.008e-07,-2.9598e-07,-1.2201e-07,4.4257e-08,3.1377e-08,-4.0323e-08,-5.5309e-08,-4.9927e-08,-6.0455e-08,
                 -1.1145e-07,-1.1915e-07,-1.0363e-07,-1.2704e-08,4.4257e-08,-8.2062e-08,-1.3261e-07,2.2502e-07,5.1969e-07,-1.0007e-07,-1.4557e-06,-2.2171e-06,-1.2838e-06,1.2671e-06,4.519e-06,7.2838e-06,8.3876e-06,7.2838e-06,4.519e-06,1.2671e-06,-1.2838e-06,-2.2171e-06,-1.4557e-06,-1.0007e-07,5.1969e-07,2.2502e-07,-1.3261e-07,-8.2062e-08,4.4257e-08,-1.2704e-08,-1.0363e-07,-1.1915e-07,-1.1145e-07,
                 -1.8157e-07,-1.9687e-07,-1.2772e-07,-3.3299e-08,-1.2201e-07,-1.3261e-07,4.0958e-07,7.6089e-07,-3.1224e-07,-1.9059e-06,-1.7349e-06,3.1369e-07,1.6616e-06,1.5685e-07,-3.5292e-06,-6.9366e-06,-8.2415e-06,-6.9366e-06,-3.5292e-06,1.5685e-07,1.6616e-06,3.1369e-07,-1.7349e-06,-1.9059e-06,-3.1224e-07,7.6089e-07,4.0958e-07,-1.3261e-07,-1.2201e-07,-3.3299e-08,-1.2772e-07,-1.9687e-07,-1.8157e-07,
                 -2.6422e-07,-2.4966e-07,-1.5906e-07,-2.0036e-07,-2.9598e-07,2.2502e-07,7.6089e-07,-3.6682e-07,-1.5158e-06,8.6586e-07,4.6284e-06,3.7091e-06,-2.1285e-06,-5.4078e-06,4.0555e-07,1.175e-05,1.7539e-05,1.175e-05,4.0555e-07,-5.4078e-06,-2.1285e-06,3.7091e-06,4.6284e-06,8.6586e-07,-1.5158e-06,-3.6682e-07,7.6089e-07,2.2502e-07,-2.9598e-07,-2.0036e-07,-1.5906e-07,-2.4966e-07,-2.6422e-07,
                 -3.301e-07,-2.6984e-07,-2.5775e-07,-4.3824e-07,-2.008e-07,5.1969e-07,-3.1224e-07,-1.5158e-06,2.2473e-06,6.8127e-06,4.3871e-07,-1.3554e-05,-1.9554e-05,-1.1949e-05,4.8718e-07,7.2008e-06,8.2578e-06,7.2008e-06,4.8718e-07,-1.1949e-05,-1.9554e-05,-1.3554e-05,4.3871e-07,6.8127e-06,2.2473e-06,-1.5158e-06,-3.1224e-07,5.1969e-07,-2.008e-07,-4.3824e-07,-2.5775e-07,-2.6984e-07,-3.301e-07,
                 -3.4356e-07,-2.7684e-07,-4.0892e-07,-5.3327e-07,5.2816e-08,-1.0007e-07,-1.9059e-06,8.6586e-07,6.8127e-06,-1.638e-06,-1.6904e-05,-5.9626e-06,2.2956e-05,3.319e-05,1.5353e-05,-1.1122e-05,-2.2544e-05,-1.1122e-05,1.5353e-05,3.319e-05,2.2956e-05,-5.9626e-06,-1.6904e-05,-1.638e-06,6.8127e-06,8.6586e-07,-1.9059e-06,-1.0007e-07,5.2816e-08,-5.3327e-07,-4.0892e-07,-2.7684e-07,-3.4356e-07,
                 -2.856e-07,-2.7219e-07,-4.9915e-07,-3.8192e-07,2.2358e-08,-1.4557e-06,-1.7349e-06,4.6284e-06,4.3871e-07,-1.6904e-05,8.1807e-06,6.1551e-05,3.4991e-05,-5.5282e-05,-0.00010312,-5.199e-05,5.4491e-06,-5.199e-05,-0.00010312,-5.5282e-05,3.4991e-05,6.1551e-05,8.1807e-06,-1.6904e-05,4.3871e-07,4.6284e-06,-1.7349e-06,-1.4557e-06,2.2358e-08,-3.8192e-07,-4.9915e-07,-2.7219e-07,-2.856e-07,
                 -1.6191e-07,-2.0553e-07,-4.3263e-07,-7.8348e-08,-4.6194e-07,-2.2171e-06,3.1369e-07,3.7091e-06,-1.3554e-05,-5.9626e-06,6.1551e-05,1.2939e-05,-0.00017577,-0.00022694,-0.00022987,-0.00011755,5.7546e-05,-0.00011755,-0.00022987,-0.00022694,-0.00017577,1.2939e-05,6.1551e-05,-5.9626e-06,-1.3554e-05,3.7091e-06,3.1369e-07,-2.2171e-06,-4.6194e-07,-7.8348e-08,-4.3263e-07,-2.0553e-07,-1.6191e-07,
                 -6.9515e-10,1.1891e-08,-2.7198e-07,3.1609e-07,-1.0754e-06,-1.2838e-06,1.6616e-06,-2.1285e-06,-1.9554e-05,2.2956e-05,3.4991e-05,-0.00017577,-0.00014129,0.00014561,-0.00092767,-0.0022122,-0.0022718,-0.0022122,-0.00092767,0.00014561,-0.00014129,-0.00017577,3.4991e-05,2.2956e-05,-1.9554e-05,-2.1285e-06,1.6616e-06,-1.2838e-06,-1.0754e-06,3.1609e-07,-2.7198e-07,1.1891e-08,-6.9515e-10,
                  1.4841e-07,4.6019e-07,-2.4771e-07,9.8584e-07,-1.5256e-06,1.2671e-06,1.5685e-07,-5.4078e-06,-1.1949e-05,3.319e-05,-5.5282e-05,-0.00022694,0.00014561,-0.00034885,-0.0043642,-0.00615,-0.0045863,-0.00615,-0.0043642,-0.00034885,0.00014561,-0.00022694,-5.5282e-05,3.319e-05,-1.1949e-05,-5.4078e-06,1.5685e-07,1.2671e-06,-1.5256e-06,9.8584e-07,-2.4771e-07,4.6019e-07,1.4841e-07,
                  2.343e-07,1.1197e-06,-5.5069e-07,2.1171e-06,-1.945e-06,4.519e-06,-3.5292e-06,4.0555e-07,4.8718e-07,1.5353e-05,-0.00010312,-0.00022987,-0.00092767,-0.0043642,-0.0079703,0.002361,0.014145,0.002361,-0.0079703,-0.0043642,-0.00092767,-0.00022987,-0.00010312,1.5353e-05,4.8718e-07,4.0555e-07,-3.5292e-06,4.519e-06,-1.945e-06,2.1171e-06,-5.5069e-07,1.1197e-06,2.343e-07,
                  2.5095e-07,1.766e-06,-1.0417e-06,3.3751e-06,-2.4459e-06,7.2838e-06,-6.9366e-06,1.175e-05,7.2008e-06,-1.1122e-05,-5.199e-05,-0.00011755,-0.0022122,-0.00615,0.002361,0.043915,0.076061,0.043915,0.002361,-0.00615,-0.0022122,-0.00011755,-5.199e-05,-1.1122e-05,7.2008e-06,1.175e-05,-6.9366e-06,7.2838e-06,-2.4459e-06,3.3751e-06,-1.0417e-06,1.766e-06,2.5095e-07,
                  2.4572e-07,2.0425e-06,-1.2882e-06,3.9459e-06,-2.7024e-06,8.3876e-06,-8.2415e-06,1.7539e-05,8.2578e-06,-2.2544e-05,5.4491e-06,5.7546e-05,-0.0022718,-0.0045863,0.014145,0.076061,0.12044,0.076061,0.014145,-0.0045863,-0.0022718,5.7546e-05,5.4491e-06,-2.2544e-05,8.2578e-06,1.7539e-05,-8.2415e-06,8.3876e-06,-2.7024e-06,3.9459e-06,-1.2882e-06,2.0425e-06,2.4572e-07,
                  2.5095e-07,1.766e-06,-1.0417e-06,3.3751e-06,-2.4459e-06,7.2838e-06,-6.9366e-06,1.175e-05,7.2008e-06,-1.1122e-05,-5.199e-05,-0.00011755,-0.0022122,-0.00615,0.002361,0.043915,0.076061,0.043915,0.002361,-0.00615,-0.0022122,-0.00011755,-5.199e-05,-1.1122e-05,7.2008e-06,1.175e-05,-6.9366e-06,7.2838e-06,-2.4459e-06,3.3751e-06,-1.0417e-06,1.766e-06,2.5095e-07,
                  2.343e-07,1.1197e-06,-5.5069e-07,2.1171e-06,-1.945e-06,4.519e-06,-3.5292e-06,4.0555e-07,4.8718e-07,1.5353e-05,-0.00010312,-0.00022987,-0.00092767,-0.0043642,-0.0079703,0.002361,0.014145,0.002361,-0.0079703,-0.0043642,-0.00092767,-0.00022987,-0.00010312,1.5353e-05,4.8718e-07,4.0555e-07,-3.5292e-06,4.519e-06,-1.945e-06,2.1171e-06,-5.5069e-07,1.1197e-06,2.343e-07,
                  1.4841e-07,4.6019e-07,-2.4771e-07,9.8584e-07,-1.5256e-06,1.2671e-06,1.5685e-07,-5.4078e-06,-1.1949e-05,3.319e-05,-5.5282e-05,-0.00022694,0.00014561,-0.00034885,-0.0043642,-0.00615,-0.0045863,-0.00615,-0.0043642,-0.00034885,0.00014561,-0.00022694,-5.5282e-05,3.319e-05,-1.1949e-05,-5.4078e-06,1.5685e-07,1.2671e-06,-1.5256e-06,9.8584e-07,-2.4771e-07,4.6019e-07,1.4841e-07,
                 -6.9515e-10,1.1891e-08,-2.7198e-07,3.1609e-07,-1.0754e-06,-1.2838e-06,1.6616e-06,-2.1285e-06,-1.9554e-05,2.2956e-05,3.4991e-05,-0.00017577,-0.00014129,0.00014561,-0.00092767,-0.0022122,-0.0022718,-0.0022122,-0.00092767,0.00014561,-0.00014129,-0.00017577,3.4991e-05,2.2956e-05,-1.9554e-05,-2.1285e-06,1.6616e-06,-1.2838e-06,-1.0754e-06,3.1609e-07,-2.7198e-07,1.1891e-08,-6.9515e-10,
                 -1.6191e-07,-2.0553e-07,-4.3263e-07,-7.8348e-08,-4.6194e-07,-2.2171e-06,3.1369e-07,3.7091e-06,-1.3554e-05,-5.9626e-06,6.1551e-05,1.2939e-05,-0.00017577,-0.00022694,-0.00022987,-0.00011755,5.7546e-05,-0.00011755,-0.00022987,-0.00022694,-0.00017577,1.2939e-05,6.1551e-05,-5.9626e-06,-1.3554e-05,3.7091e-06,3.1369e-07,-2.2171e-06,-4.6194e-07,-7.8348e-08,-4.3263e-07,-2.0553e-07,-1.6191e-07,
                 -2.856e-07,-2.7219e-07,-4.9915e-07,-3.8192e-07,2.2358e-08,-1.4557e-06,-1.7349e-06,4.6284e-06,4.3871e-07,-1.6904e-05,8.1807e-06,6.1551e-05,3.4991e-05,-5.5282e-05,-0.00010312,-5.199e-05,5.4491e-06,-5.199e-05,-0.00010312,-5.5282e-05,3.4991e-05,6.1551e-05,8.1807e-06,-1.6904e-05,4.3871e-07,4.6284e-06,-1.7349e-06,-1.4557e-06,2.2358e-08,-3.8192e-07,-4.9915e-07,-2.7219e-07,-2.856e-07,
                 -3.4356e-07,-2.7684e-07,-4.0892e-07,-5.3327e-07,5.2816e-08,-1.0007e-07,-1.9059e-06,8.6586e-07,6.8127e-06,-1.638e-06,-1.6904e-05,-5.9626e-06,2.2956e-05,3.319e-05,1.5353e-05,-1.1122e-05,-2.2544e-05,-1.1122e-05,1.5353e-05,3.319e-05,2.2956e-05,-5.9626e-06,-1.6904e-05,-1.638e-06,6.8127e-06,8.6586e-07,-1.9059e-06,-1.0007e-07,5.2816e-08,-5.3327e-07,-4.0892e-07,-2.7684e-07,-3.4356e-07,
                 -3.301e-07,-2.6984e-07,-2.5775e-07,-4.3824e-07,-2.008e-07,5.1969e-07,-3.1224e-07,-1.5158e-06,2.2473e-06,6.8127e-06,4.3871e-07,-1.3554e-05,-1.9554e-05,-1.1949e-05,4.8718e-07,7.2008e-06,8.2578e-06,7.2008e-06,4.8718e-07,-1.1949e-05,-1.9554e-05,-1.3554e-05,4.3871e-07,6.8127e-06,2.2473e-06,-1.5158e-06,-3.1224e-07,5.1969e-07,-2.008e-07,-4.3824e-07,-2.5775e-07,-2.6984e-07,-3.301e-07,
                 -2.6422e-07,-2.4966e-07,-1.5906e-07,-2.0036e-07,-2.9598e-07,2.2502e-07,7.6089e-07,-3.6682e-07,-1.5158e-06,8.6586e-07,4.6284e-06,3.7091e-06,-2.1285e-06,-5.4078e-06,4.0555e-07,1.175e-05,1.7539e-05,1.175e-05,4.0555e-07,-5.4078e-06,-2.1285e-06,3.7091e-06,4.6284e-06,8.6586e-07,-1.5158e-06,-3.6682e-07,7.6089e-07,2.2502e-07,-2.9598e-07,-2.0036e-07,-1.5906e-07,-2.4966e-07,-2.6422e-07,
                 -1.8157e-07,-1.9687e-07,-1.2772e-07,-3.3299e-08,-1.2201e-07,-1.3261e-07,4.0958e-07,7.6089e-07,-3.1224e-07,-1.9059e-06,-1.7349e-06,3.1369e-07,1.6616e-06,1.5685e-07,-3.5292e-06,-6.9366e-06,-8.2415e-06,-6.9366e-06,-3.5292e-06,1.5685e-07,1.6616e-06,3.1369e-07,-1.7349e-06,-1.9059e-06,-3.1224e-07,7.6089e-07,4.0958e-07,-1.3261e-07,-1.2201e-07,-3.3299e-08,-1.2772e-07,-1.9687e-07,-1.8157e-07,
                 -1.1145e-07,-1.1915e-07,-1.0363e-07,-1.2704e-08,4.4257e-08,-8.2062e-08,-1.3261e-07,2.2502e-07,5.1969e-07,-1.0007e-07,-1.4557e-06,-2.2171e-06,-1.2838e-06,1.2671e-06,4.519e-06,7.2838e-06,8.3876e-06,7.2838e-06,4.519e-06,1.2671e-06,-1.2838e-06,-2.2171e-06,-1.4557e-06,-1.0007e-07,5.1969e-07,2.2502e-07,-1.3261e-07,-8.2062e-08,4.4257e-08,-1.2704e-08,-1.0363e-07,-1.1915e-07,-1.1145e-07,
                 -6.0455e-08,-4.9927e-08,-5.5309e-08,-4.0323e-08,3.1377e-08,4.4257e-08,-1.2201e-07,-2.9598e-07,-2.008e-07,5.2816e-08,2.2358e-08,-4.6194e-07,-1.0754e-06,-1.5256e-06,-1.945e-06,-2.4459e-06,-2.7024e-06,-2.4459e-06,-1.945e-06,-1.5256e-06,-1.0754e-06,-4.6194e-07,2.2358e-08,5.2816e-08,-2.008e-07,-2.9598e-07,-1.2201e-07,4.4257e-08,3.1377e-08,-4.0323e-08,-5.5309e-08,-4.9927e-08,-6.0455e-08,
                 -2.0774e-08,-9.8996e-09,-1.0239e-08,-3.2372e-08,-4.0323e-08,-1.2704e-08,-3.3299e-08,-2.0036e-07,-4.3824e-07,-5.3327e-07,-3.8192e-07,-7.8348e-08,3.1609e-07,9.8584e-07,2.1171e-06,3.3751e-06,3.9459e-06,3.3751e-06,2.1171e-06,9.8584e-07,3.1609e-07,-7.8348e-08,-3.8192e-07,-5.3327e-07,-4.3824e-07,-2.0036e-07,-3.3299e-08,-1.2704e-08,-4.0323e-08,-3.2372e-08,-1.0239e-08,-9.8996e-09,-2.0774e-08,
                  1.1371e-08,9.2313e-09,7.1241e-09,-1.0239e-08,-5.5309e-08,-1.0363e-07,-1.2772e-07,-1.5906e-07,-2.5775e-07,-4.0892e-07,-4.9915e-07,-4.3263e-07,-2.7198e-07,-2.4771e-07,-5.5069e-07,-1.0417e-06,-1.2882e-06,-1.0417e-06,-5.5069e-07,-2.4771e-07,-2.7198e-07,-4.3263e-07,-4.9915e-07,-4.0892e-07,-2.5775e-07,-1.5906e-07,-1.2772e-07,-1.0363e-07,-5.5309e-08,-1.0239e-08,7.1241e-09,9.2313e-09,1.1371e-08,
                  3.2397e-08,2.2705e-08,9.2313e-09,-9.8996e-09,-4.9927e-08,-1.1915e-07,-1.9687e-07,-2.4966e-07,-2.6984e-07,-2.7684e-07,-2.7219e-07,-2.0553e-07,1.1891e-08,4.6019e-07,1.1197e-06,1.766e-06,2.0425e-06,1.766e-06,1.1197e-06,4.6019e-07,1.1891e-08,-2.0553e-07,-2.7219e-07,-2.7684e-07,-2.6984e-07,-2.4966e-07,-1.9687e-07,-1.1915e-07,-4.9927e-08,-9.8996e-09,9.2313e-09,2.2705e-08,3.2397e-08,
                  4.1243e-08,3.2397e-08,1.1371e-08,-2.0774e-08,-6.0455e-08,-1.1145e-07,-1.8157e-07,-2.6422e-07,-3.301e-07,-3.4356e-07,-2.856e-07,-1.6191e-07,-6.9515e-10,1.4841e-07,2.343e-07,2.5095e-07,2.4572e-07,2.5095e-07,2.343e-07,1.4841e-07,-6.9515e-10,-1.6191e-07,-2.856e-07,-3.4356e-07,-3.301e-07,-2.6422e-07,-1.8157e-07,-1.1145e-07,-6.0455e-08,-2.0774e-08,1.1371e-08,3.2397e-08,4.1243e-08));

    cv::copyMakeBorder(c1,c2,33,33,33,33,cv::BORDER_DEFAULT);

    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            cv::Mat block_ij(c2, cv::Rect(i, j, 67, 67)); /* Tamanho do bloco: 33 + 33 + 1 */
            cv::meanStdDev(block_ij,mean,std);

            dest.at<float>(i,j) = std[0]/mean[0];
        }
    }

    return ;
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
/* Algoritmo do MATLAB (edit edge):
if strcmp(method,'canny')
  % Magic numbers
  GaussianDieOff = .0001;
  PercentOfPixelsNotEdges = .7; % Used for selecting thresholds
  ThresholdRatio = .4;          % Low thresh is this fraction of the high.

  % Design the filters - a gaussian and its derivative

  pw = 1:30; % possible widths
  ssq = sigma^2;
  width = find(exp(-(pw.*pw)/(2*ssq))>GaussianDieOff,1,'last');
  if isempty(width)
    width = 1;  % the user entered a really small sigma
  end

  t = (-width:width);
  gau = exp(-(t.*t)/(2*ssq))/(2*pi*ssq);     % the gaussian 1D filter

  % Find the directional derivative of 2D Gaussian (along X-axis)
  % Since the result is symmetric along X, we can get the derivative along
  % Y-axis simply by transposing the result for X direction.
  [x,y]=meshgrid(-width:width,-width:width);
  dgau2D=-x.*exp(-(x.*x+y.*y)/(2*ssq))/(pi*ssq);

  % Convolve the filters with the image in each direction
  % The canny edge detector first requires convolution with
  % 2D gaussian, and then with the derivitave of a gaussian.
  % Since gaussian filter is separable, for smoothing, we can use
  % two 1D convolutions in order to achieve the effect of convolving
  % with 2D Gaussian.  We convolve along rows and then columns.

  %smooth the image out
  aSmooth=imfilter(a,gau,'conv','replicate');   % run the filter across rows
  aSmooth=imfilter(aSmooth,gau','conv','replicate'); % and then across columns

  %apply directional derivatives
  ax = imfilter(aSmooth, dgau2D, 'conv','replicate');
  ay = imfilter(aSmooth, dgau2D', 'conv','replicate');

  mag = sqrt((ax.*ax) + (ay.*ay));
  magmax = max(mag(:));
  if magmax>0
    mag = mag / magmax;   % normalize
  end

  % Select the thresholds
  if isempty(thresh)
    counts=imhist(mag, 64);
    highThresh = find(cumsum(counts) > PercentOfPixelsNotEdges*m*n,...
                      1,'first') / 64;
    lowThresh = ThresholdRatio*highThresh;
    thresh = [lowThresh highThresh];
  elseif length(thresh)==1
    highThresh = thresh;
    if thresh>=1
      eid = sprintf('Images:%s:thresholdMustBeLessThanOne', mfilename);
      msg = 'The threshold must be less than 1.';
      error(eid,'%s',msg);
    end
    lowThresh = ThresholdRatio*thresh;
    thresh = [lowThresh highThresh];
  elseif length(thresh)==2
    lowThresh = thresh(1);
    highThresh = thresh(2);
    if (lowThresh >= highThresh) || (highThresh >= 1)
      eid = sprintf('Images:%s:thresholdOutOfRange', mfilename);
      msg = 'Thresh must be [low high], where low < high < 1.';
      error(eid,'%s',msg);
    end
  end

  % The next step is to do the non-maximum suppression.
  % We will accrue indices which specify ON pixels in strong edgemap
  % The array e will become the weak edge map.
  idxStrong = [];
  for dir = 1:4
    idxLocalMax = cannyFindLocalMaxima(dir,ax,ay,mag);
    idxWeak = idxLocalMax(mag(idxLocalMax) > lowThresh);
    e(idxWeak)=1;
    idxStrong = [idxStrong; idxWeak(mag(idxWeak) > highThresh)]; %#ok<AGROW>
  end

  if ~isempty(idxStrong) % result is all zeros if idxStrong is empty
    rstrong = rem(idxStrong-1, m)+1;
    cstrong = floor((idxStrong-1)/m)+1;
    e = bwselect(e, cstrong, rstrong, 8);
    e = bwmorph(e, 'thin', 1);  % Thin double (or triple) pixel wide contours
  end

elseif any(strcmp(method, {'log','marr-hildreth','zerocross'}))
  rr = 2:m-1; cc=2:n-1;

  % We don't use image blocks here
  if isempty(H),
    fsize = ceil(sigma*3) * 2 + 1;  % choose an odd fsize > 6*sigma;
    op = fspecial('log',fsize,sigma);
  else
    op = H;
  end

  op = op - sum(op(:))/numel(op); % make the op to sum to zero
  b = imfilter(a,op,'replicate');

  if isempty(thresh)
    thresh = .75*mean2(abs(b));
  end

  % Look for the zero crossings:  +-, -+ and their transposes
  % We arbitrarily choose the edge to be the negative point
  [rx,cx] = find( b(rr,cc) < 0 & b(rr,cc+1) > 0 ...
                  & abs( b(rr,cc)-b(rr,cc+1) ) > thresh );   % [- +]
  e((rx+1) + cx*m) = 1;
  [rx,cx] = find( b(rr,cc-1) > 0 & b(rr,cc) < 0 ...
                  & abs( b(rr,cc-1)-b(rr,cc) ) > thresh );   % [+ -]
  e((rx+1) + cx*m) = 1;
  [rx,cx] = find( b(rr,cc) < 0 & b(rr+1,cc) > 0 ...
                  & abs( b(rr,cc)-b(rr+1,cc) ) > thresh);   % [- +]'
  e((rx+1) + cx*m) = 1;
  [rx,cx] = find( b(rr-1,cc) > 0 & b(rr,cc) < 0 ...
                  & abs( b(rr-1,cc)-b(rr,cc) ) > thresh);   % [+ -]'
  e((rx+1) + cx*m) = 1;

  % Most likely this covers all of the cases.   Just check to see if there
  % are any points where the LoG was precisely zero:
  [rz,cz] = find( b(rr,cc)==0 );
  if ~isempty(rz)
    % Look for the zero crossings: +0-, -0+ and their transposes
    % The edge lies on the Zero point
    zero = (rz+1) + cz*m;   % Linear index for zero points
    zz = (b(zero-1) < 0 & b(zero+1) > 0 ...
              & abs( b(zero-1)-b(zero+1) ) > 2*thresh);     % [- 0 +]'
    e(zero(zz)) = 1;
    zz = (b(zero-1) > 0 & b(zero+1) < 0 ...
              & abs( b(zero-1)-b(zero+1) ) > 2*thresh);     % [+ 0 -]'
    e(zero(zz)) = 1;
    zz = (b(zero-m) < 0 & b(zero+m) > 0 ...
              & abs( b(zero-m)-b(zero+m) ) > 2*thresh);     % [- 0 +]
    e(zero(zz)) = 1;
    zz = (b(zero-m) > 0 & b(zero+m) < 0 ...
              & abs( b(zero-m)-b(zero+m) ) > 2*thresh);     % [+ 0 -]
    e(zero(zz)) = 1;
  end

else  % one of the easy methods (roberts,sobel,prewitt)

  if strcmp(method,'sobel')
    op = fspecial('sobel')/8; % Sobel approximation to derivative
    x_mask = op'; % gradient in the X direction
    y_mask = op;

    scale = 4; % for calculating the automatic threshold
    offset = [0 0 0 0]; % offsets used in the computation of the threshold

  elseif strcmp(method,'prewitt')
    op = fspecial('prewitt')/6; % Prewitt approximation to derivative
    x_mask = op';
    y_mask = op;

    scale = 4;
    offset = [0 0 0 0];

  elseif strcmp(method, 'roberts')
    x_mask = [1 0; 0 -1]/2; % Roberts approximation to diagonal derivative
    y_mask = [0 1;-1  0]/2;

    scale = 6;
    offset = [-1 1 1 -1];

  else

    eid = sprintf('Images:%s:invalidEdgeDetectionMethod', mfilename);
    msg = sprintf('%s %s',method, 'is not a valid method.' );
    error(eid,'%s',msg);
  end

  % compute the gradient in x and y direction
  bx = imfilter(a,x_mask,'replicate');
  by = imfilter(a,y_mask,'replicate');

  if (nargout > 2) % if gradients are requested
    gv_45  = bx;
    gh_135 = by;
  end

  % compute the magnitude
  b = kx*bx.*bx + ky*by.*by;

  % determine the threshold; see page 514 of "Digital Imaging Processing" by
  % William K. Pratt
  if isempty(thresh), % Determine cutoff based on RMS estimate of noise
                      % Mean of the magnitude squared image is a
                      % value that's roughly proportional to SNR
    cutoff = scale*mean2(b);
    thresh = sqrt(cutoff);
  else                % Use relative tolerance specified by the user
    cutoff = (thresh).^2;
  end

  if thinning
      e = computeEdgesWithThinning(b,bx,by,kx,ky,offset,cutoff);
  else
      e = b > cutoff;
  end

end

if nargout==0,
  imshow(e);
else
  eout = e;
end
*/

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
