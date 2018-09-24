#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "result.h"
#include <memory>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
//#include <opencv2/highgui.hpp>

#define NUMBER_OF_IMAGE 3600
#define NUMBER_OF_COLUMN 1920
#define WIDTH 1080
#define HEIGHT 1920

void read_image(int number);

int main()
{
	int a, b, r;
	std::cout << "Enter the offset along the X-axis: " << std::endl;
	std::cin >> a;
	std::cout << "Enter the offset along the Y-axis: " << std::endl;
	std::cin >> b;
	std::cout << "Enter the radius: " << std::endl;
	std::cin >> r;

	const std::chrono::steady_clock::time_point start(std::chrono::steady_clock::now());

	std::shared_ptr<result> image_result(new result(NUMBER_OF_IMAGE, HEIGHT, WIDTH, HEIGHT));

	//image_result->select_columns(-0.6, 0.6, 2);
	image_result->select_columns(a, b, r);

	const int hardware_threads = std::thread::hardware_concurrency(); //hardware number of threads
	auto const number_of_threads = (hardware_threads == 0 ? 1 : hardware_threads / 2);//used number of threads
	const int number_of_image_per_thread = NUMBER_OF_IMAGE / number_of_threads; //number of photos per thread

	std::vector<std::thread> threads;

	threads.reserve(number_of_threads);

	for (auto i = 0; i < number_of_threads; i++)
	{
		//threads.push_back(image_result->thread_bmp(i*number_of_image_per_thread + 1, (i + 1)*number_of_image_per_thread));
		threads.push_back(image_result->thread_jpg(i*number_of_image_per_thread + 1, (i + 1)*number_of_image_per_thread));
	}
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	threads.clear();

	//image_result->save_result_bmp();
	image_result->save_result_jpg();

	const std::chrono::steady_clock::time_point end(std::chrono::steady_clock::now());
	std::cout << "Time is: " << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() << "s" << std::endl; // in seconds

	system("pause");
	return 0;
}

void read_image(const int number)
{
	/*cv::Mat neww;

	std::ostringstream oss;
	oss << "Poznan_Room_000" << 1 << "_1080x1920_cam_center.jpg";
	const auto file_name = oss.str();
	cv::Mat jpeg_image = cv::imread(file_name);

	neww.create(1920, 3600, CV_8UC3);

	for (auto i = 0; i < 3600; i++)
	{
		jpeg_image = cv::imread(file_name);
		jpeg_image.col(540).copyTo(neww.col((i)));
		//std::cout << i << std::endl;
		//cv::waitKey(50);
		//tmp->read_image_jpeg(i);
	}
	cv::imwrite("ResultImage.jpg", neww);
	*/
}
