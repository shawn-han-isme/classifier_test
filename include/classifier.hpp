#include <proportion_thresh.hpp>
#define CLASSIFIER_IMAGEPART_ROWS 100
#define CLASSIFIER_IMAGEPART_COLS 120

#define DEBUG


class classifierTrainer
{
private:
    /* data */
public:
    std::string template_img_location;
    std::vector<cv::String> template_image_names;
    std::vector<cv::Mat> template_images;

    std::string test_img_location;
    std::vector<cv::String> test_image_names;

	double thresh_binar = 0.3; //二值化取thresh_binar最亮部分
    int total, good, bad;

    classifierTrainer(std::string template_img_location);
    ~classifierTrainer();
    void compare(std::string test_img_loc);
};

classifierTrainer::classifierTrainer(std::string template_img_loc):template_img_location(template_img_loc)
{
    #ifdef DEBUG
    std::cout<<"ClassifierTrainer is being created"<<std::endl;
    #endif

    cv::glob(template_img_location, template_image_names); //将文件名放入template_image_names容器
    for(int i=0;i<template_image_names.size();i++) //循环遍历所有文件
    {
        cv::Mat template_image = imread(template_image_names[i],0);
        if(!template_image.data)//检测图片是否读取成功
        {        
            std::cout<<"读取第"<<i+1<<"张图片错误，请确定目录下是否存在该图片"<<std::endl;
        }
        
        int threshold_int = sp::get_proportion_thresh(template_image, template_image, 255, thresh_binar); //二值化模板图像

        #ifdef DEBUG
        std::cout<<"threshold_int="<<threshold_int<<std::endl;
        #endif
        
        cv::threshold(template_image, template_image, threshold_int, 255, CV_THRESH_BINARY); //二值化模板图像
        cv::medianBlur(template_image, template_image, 3); //中值滤波
		cv::resize(template_image, template_image, cv::Size(CLASSIFIER_IMAGEPART_COLS, CLASSIFIER_IMAGEPART_ROWS), (0,0), (0,0), CV_INTER_AREA); // 将模板图像的大小变成CLASSIFIER_IMAGEPART_COLS*CLASSIFIER_IMAGEPART_ROWS

   		template_images.push_back(template_image);
    }
    for(int i=0;i<template_images.size();i++)
    {
        cv::imshow("img",template_images[i]);
        cv::waitKey(0);
    }
}

classifierTrainer::~classifierTrainer()
{
    #ifdef DEBUG
    std::cout<<"ClassifierTrainer is being deleted"<<std::endl;
    #endif
}

void classifierTrainer::compare(std::string test_img_loc)
{
    test_img_location = test_img_loc;
    cv::glob(test_img_location, test_image_names); //将文件名放入test_image_names容器
    total = test_image_names.size();

    for(int i=0;i<test_image_names.size();i++) //循环遍历所有文件
    {
        cv::Mat test_image = imread(test_image_names[i],0);

        int threshold_int;
		threshold_int = sp::get_proportion_thresh(test_image, test_image, 255, thresh_binar); //二值化模板图像
        
        #ifdef DEBUG
        std::cout<<"threshold_int="<<threshold_int<<std::endl;
        #endif

        cv::threshold(test_image, test_image, threshold_int, 255, CV_THRESH_BINARY);

        // 将模板图像的大小变成CLASSIFIER_IMAGEPART_COLS*CLASSIFIER_IMAGEPART_ROWS
		cv::resize(test_image, test_image, cv::Size(CLASSIFIER_IMAGEPART_COLS, CLASSIFIER_IMAGEPART_ROWS), (0,0), (0,0), CV_INTER_AREA);
    

    }
}
