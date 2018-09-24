#include "image.h"
#include <string>
#include <sstream>


image::image()
{
  number_of_column_ = 0;
  width_ = 1080;
  height_ = 1920;
  cut_column_bmp_ = std::make_unique<bitmap_image>(1, height_);
  cut_column_jpg_.create(1920, 1, CV_8UC3);
  //cut_column_jpg_ = std::make_unique<cv::Mat>(1,height_,CV_8UC3);
}

image::image(const int w, const int h)
{
	number_of_column_ = 0;
	width_ = w;
	height_ = h;
	cut_column_bmp_ = std::make_unique<bitmap_image>(1, height_);
	cut_column_jpg_.create(1920, 1, CV_8UC3);
	//cut_column_jpg_ = std::make_unique<cv::Mat>(1, height_, CV_8UC3);
}


image::~image()
{
	jpeg_image_.release();
	cut_column_jpg_.release();
	bmp_image_.clear();
	cut_column_bmp_->clear();
}

void image::read_image_bmp(const int number)
{
	std::ostringstream oss;
	if (number >= 1 && number <= 9)  oss << "Poznan_Room_000" << number << "_1080x1920_cam_center.bmp";
	else if (number >= 10 && number <= 99)  oss << "Poznan_Room_00" << number << "_1080x1920_cam_center.bmp";
	else if (number >= 100 && number <= 999)  oss << "Poznan_Room_0" << number << "_1080x1920_cam_center.bmp";
	else if (number >= 1000 && number <= 3600)  oss << "Poznan_Room_" << number << "_1080x1920_cam_center.bmp";

	const auto file_name = oss.str();
	bmp_image_ = bitmap_image(file_name);
}

void image::read_image_jpeg(const int number)
{
	std::ostringstream oss;
	if (number >= 1 && number <= 9)  oss << "Poznan_Room_000" << number << "_1080x1920_cam_center.jpg";
	else if (number >= 10 && number <= 99)  oss << "Poznan_Room_00" << number << "_1080x1920_cam_center.jpg";
	else if (number >= 100 && number <= 999)  oss << "Poznan_Room_0" << number << "_1080x1920_cam_center.jpg";
	else if (number >= 1000 && number <= 3600)  oss << "Poznan_Room_" << number << "_1080x1920_cam_center.jpg";

	const auto file_name = oss.str();
	jpeg_image_ = cv::imread(file_name);

	if (jpeg_image_.empty())                      // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
	}
}


void image::cut_bmp_image_column(const int number) const
{
	for (auto j = 0; j < bmp_image_.height(); j++)
	{
		cut_column_bmp_->set_pixel(0, j, bmp_image_.get_pixel(number, j));
	}
}

void image::cut_jpg_image_column(const int number) const
{
	jpeg_image_.col(number).copyTo(cut_column_jpg_.col(0));
}

int image::get_height() const
{
	return height_;
}

int image::get_width() const
{
	return width_;
}

bitmap_image image::get_column() const
{
	auto column = *cut_column_bmp_;
	return column;
}

cv::Mat image::get_column_jpg() const
{
	return cut_column_jpg_;
}

