// #include <proportion_thresh.hpp>

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
        
        int threshold_int = sp::get_proportion_thresh(template_image, thresh_binar); //二值化模板图像

        #ifdef DEBUG
        std::cout<<"threshold_int="<<threshold_int<<std::endl;
        #endif
        
        cv::threshold(template_image, template_image, threshold_int, 255, CV_THRESH_BINARY); //二值化模板图像
        cv::medianBlur(template_image, template_image, 3); //中值滤波
		cv::resize(template_image, template_image, cv::Size(CLASSIFIER_IMAGEPART_COLS, CLASSIFIER_IMAGEPART_ROWS), (0,0), (0,0), CV_INTER_AREA); // 将模板图像的大小变成CLASSIFIER_IMAGEPART_COLS*CLASSIFIER_IMAGEPART_ROWS

        #ifdef DEBUG_CLASSIFIER
        std::cout << "读入" << template_image_names[i] << "号装甲板模板" << std::endl;
        #endif

   		template_images.push_back(template_image);
    }

    #ifdef SHOW_TEMPLATE_IMAGE
    for(int i=0;i<template_images.size();i++)
    {
        cv::imshow("img",template_images[i]);
        cv::waitKey(0);
    }
    #endif
}

classifierTrainer::~classifierTrainer()
{
    #ifdef DEBUG
    std::cout<<"ClassifierTrainer is being deleted"<<std::endl;
    #endif
}

void classifierTrainer::compare(std::string test_img_loc)
{
//--------------------------------------------------将文件名放入test_image_names容器----------------------------------------
    test_img_location = test_img_loc;
    cv::glob(test_img_location, test_image_names);

//--------------------------------------------------变量声明---------------------------------------------------------------

    std::vector<int> gain_list; //声明容器gain_list来放置每个图像的gain

    total = test_image_names.size();

    for(int i=0;i<test_image_names.size();i++) //循环遍历所有文件,开始分类
    {
//----------------------------------------------------预处理测试图像--------------------------------------------------
        #ifdef DEBUG
        std::cout<<std::endl<<test_image_names[i]<<std::endl;
        #endif

        cv::Mat test_image = imread(test_image_names[i],0);

        int threshold_int;
        threshold_int = sp::get_proportion_thresh(test_image, thresh_binar); //二值化测试图像
        
        #ifdef DEBUG
        std::cout<<"test threshold_int="<<threshold_int<<std::endl;
        #endif

        cv::threshold(test_image, test_image, threshold_int, 255, CV_THRESH_BINARY);

        // 将模板图像的大小变成CLASSIFIER_IMAGEPART_COLS*CLASSIFIER_IMAGEPART_ROWS
        cv::resize(test_image, test_image, cv::Size(CLASSIFIER_IMAGEPART_COLS, CLASSIFIER_IMAGEPART_ROWS), (0,0), (0,0), CV_INTER_AREA);

        #ifdef SHOW_CLASSIFIER_IMAGE
        cv::imshow("CLASSIFIER_IMAGE", test_image);
        #endif
//----------------------------------------------------预处理测试图像完成-----------------------------------------------

        #ifdef PRINT_CLASSIFIER_RUNTIME
        sp::timer timer_classifier; //建立计时器
        timer_classifier.reset(); // 开始计时
        #endif

//--------------------------------------------------------开始分类------------------------------------------------------------
        #ifdef DEBUG_CLASSIFIER
        std::cout << "开始分类" << std::endl;
        #endif

        int gain = 0; //初始化gain
    
//----------------------------------------------- 逐像素获取每个像素的gain并累积------------------------------------------------------------
        for(int template_count=0; template_count<template_images.size(); template_count++)
        {
            for(int i=0; i<CLASSIFIER_IMAGEPART_ROWS; i++)
            {
                //获取第i行首像素指针
                uchar* p_template_image = template_images[i].ptr<uchar>(i);
                uchar* p_test_image = test_image.ptr<uchar>(i);

                //对第i行的每个像素（Byte）进行操作
                for(int j=0; j<CLASSIFIER_IMAGEPART_COLS; j++)
                {
                    //用指针访问像素的方法（速度快）
                    if(p_template_image[j]==255 && p_test_image[j]==255)
                    {
                        gain += 3;
                    }
                    else if(p_template_image[j] != p_test_image[j])
                    {
                        gain -= 2;
                    }
                    else{}
                }
            }
        gain_list.push_back(gain); //将gain加入gain_list

        #ifdef DEBUG_CLASSIFIER
        std::cout << template_count << "号装甲板的gain是" << gain << std::endl; //打印gain
        #endif

        gain = 0; //重置gain
        }

        auto min = std::min_element(gain_list.begin(), gain_list.end());
        auto max = std::max_element(gain_list.begin(), gain_list.end());

        #ifdef DEBUG_CLASSIFIER
        std::cout << "这组图像的最小gain是" << *min << std::endl;
        std::cout << "这组图像的最大gain是" << *max << std::endl;
        #endif

//--------------------------------------获取当前时间作为imwrite文件名----------------------------------------
        std::string filePath;
        filePath.clear();
        sp::timer timer_now;
        long long int count_classifier_int(timer_now.getTimeStamp());
        std::string count_classifier_str = std::to_string(count_classifier_int);

        if(*max<1000)
        {
            #ifdef DEBUG_CLASSIFIER
            std::cout << "舍弃" << std::endl;
            #endif

            #ifdef CLASSIFIER_OUTPUT
            filePath = "../image/dst/negative/negative_1_"+count_classifier_str+".jpg";
            cv::imwrite(filePath, test_image);

            #ifdef DEBUG_CLASSIFIER
            std::cout << "输出negative图片成功" << std::endl;
            #endif

            #endif

            #ifdef PRINT_CLASSIFIER_RUNTIME
            std::cout << "> 一级分类器运行时间：" << timer_classifier.get() << "ms" << std::endl; //结束计时
            #endif

            gain_list.empty();
        }
        else
        {
            int maxGainArmor = (max_element(gain_list.begin(),gain_list.end()) - gain_list.begin()) + 1;

            #ifdef DEBUG_PRINT_ARMORNUM
            std::cout << "对应编号为" << maxGainArmor << "的装甲板" << std::endl;
            #endif

            #ifdef PRINT_CLASSIFIER_RUNTIME
            std::cout << "> 一级分类器运行时间：" << timer_classifier.get() << "ms" << std::endl; //结束计时
            #endif

            if(sp::ORB_classifier_isok(test_image) //使用ORB分类器
            )
            {
                #ifdef DEBUG_CLASSIFIER_ORB
                std::cout << "> 一级分类器接受到ORB返回的true" << std::endl; 
                #endif

                #ifdef CLASSIFIER_OUTPUT
                filePath = "../image/dst/positive/positive_"+count_classifier_str+".jpg";
                cv::imwrite(filePath, test_image);
                #ifdef DEBUG_CLASSIFIER
                std::cout << "输出positive图片成功" << std::endl;
                #endif
                #endif

                gain_list.empty();
            }
            else
            {
                #ifdef CLASSIFIER_OUTPUT
                filePath = "../image/dst/negative/negative_2_"+count_classifier_str+".jpg";
                cv::imwrite(filePath, test_image);
                #ifdef DEBUG_CLASSIFIER
                std::cout << "输出negative图片成功" << std::endl;
                #endif
                #endif
                
                #ifdef DEBUG_CLASSIFIER_ORB
                std::cout << "> 一级分类器接受到ORB返回的false" << std::endl; 
                #endif
                
                gain_list.empty();
            }
    }
    }  
}
