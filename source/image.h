#pragma once
#include "bitmap_image.hpp"
#include <memory>
#include <opencv2/imgcodecs.hpp>

class image
{
	int width_, height_;
	int number_of_column_;
	bitmap_image bmp_image_;
	cv::Mat jpeg_image_;
	std::unique_ptr<bitmap_image> cut_column_bmp_;
	cv::Mat cut_column_jpg_;
public:
	image();
	image(int w, int h);
	~image();
	void read_image_bmp(int number);
	void read_image_jpeg(int number);
	void cut_bmp_image_column(int number) const;
	void cut_jpg_image_column(int number) const;
	int get_height() const;
	int get_width() const;
	bitmap_image get_column() const;
	cv::Mat get_column_jpg() const;
};

