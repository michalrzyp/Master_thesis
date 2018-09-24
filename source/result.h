#pragma once
#include <vector>
#include <thread>
#include "image.h"

#define M_PI 3.14159265359
#define FOV 1.04719755119 //60 degree  in rad
#define ROTATION_ANGEL 0.00174532925199// 0.1 degree

class result
{
	std::vector<int> num_of_column_per_image_;
	std::shared_ptr<bitmap_image> bmp_image_;
	std::shared_ptr<cv::Mat> jpg_image_;
	double correct_radius_;
	int width_result_, height_result_, width_per_1deg_, width_image_, height_image_;
public:
	result();
	result(int width_res, int height_res, int width_image, int height_image);
	~result();
	void create_image_bmp(int start_num, int finish_num) const;
	void create_image_jpg(int start_num, int finish_num) const;
	void select_columns(double x, double y, double r);
	std::thread thread_bmp(int start_num, int finish_num);
	std::thread thread_jpg(int start_num, int finish_num);
	double calculate_correct_radius(double r) const;
	int angle_to_num_of_column(double angle) const;
	void save_result_bmp() const;
	void save_result_jpg() const;
};

