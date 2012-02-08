#include "artifacts.h"

void blockingFrame(cv::Mat & src, int borderType)
{
    assert((src.rows % 8 == 0) && (src.cols % 8 == 0));

    /// Copia a matriz src no meio de outra matriz com bordas extendidas
    cv::Mat src_bordered(src.rows + 16, src.cols + 16, src.type());
    cv::copyMakeBorder(src,src_bordered,8,8,8,8,borderType);

    cv::Scalar inner_mean;
    cv::Scalar outer_mean;

    int rows_in_blocks = (src.rows/8);
    int cols_in_blocks = (src.cols/8);

    for(int i = 0; i < rows_in_blocks; ++i){
        for(int j = 0; j < cols_in_blocks; ++j){

            cv::Mat ROI = src(cv::Rect(8*j,8*i,8,8));

            cv::Mat inner_block = src_bordered(cv::Rect(8*(j+1),8*(i+1),8,8));
            cv::Mat outer_block = src_bordered(cv::Rect(8*j,8*i,24,24));

            inner_mean = cv::mean(inner_block);
            outer_mean = cv::mean(outer_block);

            ROI = ROI + abs((int)(inner_mean[0] - outer_mean[0]));
        }
    }
}

void blurringFrame(cv::Mat & src)
{
    cv::medianBlur(src,src,5);
}

void ringingFrame(cv::Mat & src, double echo_amplitude, RingingEchoDisplacement echo_displacement)
{
    assert(src.type() == CV_8UC1);
    assert((-30 <= echo_amplitude) && (echo_amplitude <= -1));
    assert((0 <= echo_displacement) && (echo_amplitude <= 2));

    cv::Mat filter_window = cv::Mat::zeros(13,13,CV_32FC1);

    double tap_sum = 175 + (2*echo_amplitude);
    double tap_filter1[] = {0,0,0,echo_amplitude/tap_sum,0,0,175/tap_sum,0,0,echo_amplitude/tap_sum,0,0,0};
    double tap_filter2[] = {0,0,echo_amplitude/tap_sum,0,0,0,175/tap_sum,0,0,0,echo_amplitude/tap_sum,0,0};
    double tap_filter3[] = {echo_amplitude/tap_sum,0,0,0,0,0,175/tap_sum,0,0,0,0,0,echo_amplitude/tap_sum};

    switch(echo_displacement)
    {
        case RINGING_375ns:
            for(int i = 0; i < 13; ++i)
                filter_window.row(6).at<float>(0,i) = tap_filter1[i];
            break;
        case RINGING_500ns:
            for(int i = 0; i < 13; ++i)
                 filter_window.row(6).at<float>(0,i) = tap_filter2[i];
            break;
        case RINGING_750ns:
            for(int i = 0; i < 13; ++i)
                filter_window.row(6).at<float>(0,i) = tap_filter3[i];
            break;
    }

    cv::Mat filter_window_t(13,13,CV_32FC1);
    cv::transpose(filter_window,filter_window_t);

    cv::Mat hor_filter(src.rows,src.cols,src.type());
    conv2D(src,hor_filter,filter_window,CONVOLUTION_SAME,cv::BORDER_REPLICATE);
    conv2D(hor_filter,src,filter_window_t);

}
