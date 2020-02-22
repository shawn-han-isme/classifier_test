#include <opencv2/opencv.hpp>
#include <classifier.hpp>
#include <proportion_thresh.hpp>
#include <timer.hpp>

#define DEBUG

int main()
{
    classifierTrainer myTest("../image/Template/*.png");
    sp::timer timer; //建立计时器
    timer.reset(); // 开始计时

    myTest.compare("../image/Test/*.png");
    
    std::cout << "> 分类器运行时间：" << timer.get() << "ms" << std::endl; //结束计时

    std::cout<<"total:"<<myTest.total<<std::endl;
    std::cout<<"good:"<<myTest.good<<std::endl;
    std::cout<<"bad:"<<myTest.bad<<std::endl;
    std::cout<<"positive率："<<std::setprecision(3)<<static_cast<double>(myTest.good*1.0/myTest.total)*100<<"%"<<std::endl;
    std::cout<<"negative率："<<std::setprecision(3)<<static_cast<double>(myTest.bad*1.0/myTest.total)*100<<"%"<<std::endl;
    std::cout<<"test for the negative"<<std::endl;

    return 0;
}
