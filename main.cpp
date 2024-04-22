#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();
std::vector<int> hand_tlx;



std::string checkOrder (std::vector<int>& vec)
{
    bool ascending = true;
    bool descending = true;
    if (vec.size() < 3)
    {
        return "random";
    }
    else
    {
        for (size_t i = 1; i < vec.size(); i++)
        {
            if (vec[i-1] > vec[i])
                ascending = false;
            if (vec[i-1] < vec[i])
                descending = false;
            std::cout << "Top Left X axis = " << vec[i] << "\n";
        }

        if (ascending)
            return "ascending";
        else if (descending)
            return "descending";
        else
            return "random";
    }
    
}

[[nodiscard]] std::string checkHand (std::vector<int>& vec)
{
    if (vec.size() >= 8)
        return "Hand";
    else
        return "No Hand";
}

[[nodiscard]] std::string detectWave (cv::Mat& image, cv::CascadeClassifier& handCascade) 
{
    
    std::vector <cv::Rect> hands;
    
    handCascade.detectMultiScale (image, hands, 1.1, 4, 0, cv::Size(8, 8));
    bool handPresent = false;
    int tlx = 0;
    std::string result = "No Hand";
    
    if(!hands.empty())
    {
        if (hands.size() > 1)
        {

        }
        else
        {
            for (cv::Rect hand : hands)
            {
                cv::rectangle(image, hand.tl(), hand.br(), cv::Scalar(0,255,0), 3);
                cv::putText(image, "Hand", cv::Point(40, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1);
                std::cout << "Top Left X axis = " << hand.x << "\n";
                hand_tlx.push_back(hand.x);
            }
            start = std::chrono::high_resolution_clock::now();
            handPresent = true;
        }
        
    }
    else
    {
        auto current = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
        if (duration.count() >= 500)
        {
            handPresent = false;
            std::string& order = checkHand(hand_tlx);
            std::cout << order << "\n";
            hand_tlx.clear();
            result = order;
        }
        else
            cv::putText(image, "Hand", cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1);
    }
    hands.clear();

    cv::imshow("VideoFun", image);

    return result;
}

std::string getFileName()
{
    std::time_t currentTime = std::time(0);

    std::string timeString = std::ctime(&currentTime);

    std::cout << "Current Time is: " << timeString << std::endl;

    std::tm* localTime = std::localtime(&currentTime);

    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int seconds = localTime->tm_sec;

    std::ostringstream oss;
    oss << "VID" << year << (month < 10 ? "0" : "") << month
        << (day < 10 ? "0" : "") << day
        << (hour < 10 ? "0" : "") << hour
        << (minute < 10 ? "0" : "") << minute
        << (seconds < 10 ? "0" : "") << seconds
        << ".mp4";

    return oss.str();

}

int main ()
{
    //cv::VideoCapture cam(0);
    cv::VideoCapture cam ("http://192.168.4.1/mjpeg");
    if (!cam.isOpened())
    {
        std::cout << "Error!!! Unable to open Webcam!!!\n\n";
        system("pause");
        return -1;
    }

    cv::CascadeClassifier handCascade;
    handCascade.load(R"(d:\C++ Programing\Spy Camera System\Training Cascade Classifier 2\Cascade\cascade.xml)");

    cv::Size targetSize (240, 160);
    cv::Mat frame, frame1, gray;
    bool recording = false, canRecord = false, canStopRecord = false;
    cv::VideoWriter videoWriter;
    
    while (true)
    {
        auto current = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
        if (duration.count() >= 2000)
        {
            if(recording)
                canStopRecord = true;
            else
                canRecord = true;
        }

        cam.read(frame);
        cv::flip(frame, frame, 1);
        cv::resize(frame, frame1, targetSize);
        cv::cvtColor (frame1, gray, cv::COLOR_BGR2GRAY);
        
        std::string& result = detectWave(gray, handCascade);
        //std::cout << result << "\n";

        if (result == "Hand")
        {
            if(recording)
            {
                if(canStopRecord)
                {
                    videoWriter.release();
                    recording = false;
                    canStopRecord = false;
                }
            }
            else
            {
                std::string fileName = getFileName();
                int frameWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
                int frameHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
                videoWriter.open(fileName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frameWidth, frameHeight));
                if (!videoWriter.isOpened())
                    std::cout << "\n\n\nCould not open Video Writer!\n";
                else
                {
                    recording = true;
                    canRecord = false;
                }
            }
        }
        if (result == "No Hand")
        {

        }


        if (recording)
        {
            std::cout << "here\n";
            videoWriter.write(frame);
            cv::circle(frame, cv::Point(20, 50), 10, cv::Scalar(0, 0, 255), cv::FILLED);
            cv::circle(frame, cv::Point(20, 50), 15, cv::Scalar(0, 0, 255), 2);
            cv::putText(frame, "Recording", cv::Point(40, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1);
        }

        cv::imshow("Video", frame);

        cv::waitKey(1);
    }

    return 0;
}
