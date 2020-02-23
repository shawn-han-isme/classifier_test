#define DEBUG_CLASSIFIER
#define DEBUG_PRINT_ARMORNUM
#define DEBUG_CLASSIFIER_ORB

// #define FRAME_BY_FRAME
// #define SHOW_TEMPLATE_IMAGE
#define SHOW_CLASSIFIER_IMAGE
#define DRAW_IMAGE_FEATURE_MATCH

#define PRINT_CLASSIFIER_RUNTIME

#define CLASSIFIER_OUTPUT

#define CLASSIFIER_IMAGEPART_ROWS 100
#define CLASSIFIER_IMAGEPART_COLS 120

#include <opencv2/opencv.hpp>
#include <get_proportion_thresh.hpp>
#include <timer.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <classifier.hpp>

int main()
{
    classifierTrainer myTest("../image/Template/*.png");
    sp::timer timer; //建立计时器
    timer.reset(); // 开始计时

    myTest.compare("../image/Test/*.png");
    
    std::cout<<std::endl;
    std::cout<<"> myTest.compare运行时间："<<timer.get()<<"ms"<< std::endl; //结束计时
    std::cout<<"total:"<<myTest.total<<std::endl;
    std::cout<<"good:"<<myTest.good<<std::endl;
    std::cout<<"bad:"<<myTest.bad<<std::endl;
    std::cout<<"positive："<<std::setprecision(3)<<static_cast<double>(myTest.good*1.0/myTest.total)*100<<"%"<<std::endl;
    std::cout<<"negative："<<std::setprecision(3)<<static_cast<double>(myTest.bad*1.0/myTest.total)*100<<"%"<<std::endl;
    std::cout<<"test for the negative"<<std::endl;

    return 0;
}
