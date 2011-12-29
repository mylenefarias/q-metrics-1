#include "debug.h"

void writeFrame(cv::Mat & src, string fName)
{
    cv::imwrite(fName,src);
    std::cout << "Frame escrito no arquivo" << fName << std::endl;
    return;
}
