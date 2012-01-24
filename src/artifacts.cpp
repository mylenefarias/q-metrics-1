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
