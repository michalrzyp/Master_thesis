#include <iostream>
#include <vector>
#include <cstdlib>
#include "result.h"
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

#define NUMBER_OF_IMAGE 3600
#define NUMBER_OF_COLUMN 1920
#define WIDTH 1080
#define HEIGHT 1920

int main()
{
	/*double a, b, r;
	std::cout << "Enter the offset along the X-axis: " << std::endl;
	std::cin >> a;
	std::cout << "Enter the offset along the Y-axis: " << std::endl;
	std::cin >> b;
	std::cout << "Enter the radius: " << std::endl;
	std::cin >> r;
	*/
	const auto start(std::chrono::steady_clock::now());
	const auto size_array = 2;

	std::array<double, size_array> input{};
	std::ifstream in("input.txt");
	if (in.is_open())
	{
		for (auto i = 0; i < size_array; i++)
		{
			in >> input[i];
		}
		in.close();
	}
	else
	{
		std::cout << "Unable to open file";
		system("pause");
		return 0;
	}
	//for_each(begin(input), end(input), [](int i) {std::cout << i << " "; });

	std::vector<std::thread> threads;
	const auto hardware_threads = std::thread::hardware_concurrency(); //sprzetowa liczba watkow
	auto const number_of_threads = hardware_threads == 0 ? 1 : hardware_threads / 2;//uzywana liczba watkow
	const auto number_of_image_per_thread = NUMBER_OF_IMAGE / number_of_threads; //liczba zdjec na watek

	for (auto number = 0; number < (size_array >> 1); number++)
	{
		std::shared_ptr<result> image_result(new result(NUMBER_OF_IMAGE, HEIGHT, WIDTH, HEIGHT, number_of_image_per_thread));
		image_result->select_columns(input[(number << 1)], input[(number << 1) + 1], 2);
		std::cout << "Image number : " << number + 1 << std::endl;

		/*threads.reserve(number_of_threads);

		for (auto i = 0; i < number_of_threads; i++)
		{
			//threads.push_back(image_result->thread_bmp(i*number_of_image_per_thread + 1, (i + 1)*number_of_image_per_thread));
			threads.push_back(image_result->thread_jpg(i*number_of_image_per_thread + 1, (i + 1)*number_of_image_per_thread));
			//threads.push_back(std::thread(read_image, 45));
		}
		std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		threads.clear();
		*/
		cv::parallel_for_(cv::Range(0, number_of_threads), *image_result, number_of_threads);
		image_result->save_result_jpg(number);
	}
	const auto end(std::chrono::steady_clock::now());
	std::cout << "Time is: " << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() << "s" << std::endl; // in seconds

	system("pause");
	return 0;
}
