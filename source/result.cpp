#include "result.h"
#include "bitmap_image.hpp"
#include <memory>
#include <sstream>
#include "opencv2/imgproc.hpp"


result::result() : correct_radius_(0)
{
  width_result_ = 3600;
  height_result_ = 1920;
  width_image_ = 1920;
  height_image_ = 1080;
  width_per_1deg_ = nearbyint(width_image_ / 360);
  correct_radius_ = 0;
  bmp_image_ = std::make_shared<bitmap_image>(width_result_, height_result_);
  jpg_image_ = std::make_shared<cv::Mat>(height_result_, width_result_, CV_8UC3);
  number_of_image_per_thread_ = 1;
}

result::result(const int width_res, const int height_res, const int width_image, const int height_image, const int number_of_threads)
{
  width_result_ = width_res;
  height_result_ = height_res;
  width_image_ = width_image;
  height_image_ = height_image;
  width_per_1deg_ = nearbyint(width_image_ / 360);
  correct_radius_ = 0;
  bmp_image_ = std::make_shared<bitmap_image>(width_result_, height_result_);
  jpg_image_ = std::make_shared<cv::Mat>(height_result_, width_result_, CV_8UC3);
  number_of_image_per_thread_ = number_of_threads;
}

result::~result()
{
  num_of_column_per_image_.clear();
  num_of_image_.clear();
}

void result::create_image_bmp(const int start_num, const int finish_num) const
{
  std::unique_ptr<image> tmp(new image);

  for (auto i = start_num; i <= finish_num; i++)
  {
    tmp->read_image_bmp(i);
    tmp->cut_bmp_image_column(num_of_column_per_image_[i - 1]);
    bmp_image_->copy_from(tmp->get_column(), i, 0);
  }
}

void result::create_image_jpg(const int start_num, const int finish_num) const
{
  std::unique_ptr<image> tmp(new image);

  for (auto i = start_num; i <= finish_num; i++)
  {
    tmp->read_image_jpeg(i);
    tmp->cut_jpg_image_column(num_of_column_per_image_[i - 1]);
    tmp->get_column_jpg().col(0).copyTo(jpg_image_->col(i - 1));
  }
}

void result::select_columns(const double x, const double y, const double r)
{
  double gamma = -M_PI;
  double u, v;
  double alpha = 0, beta = 0;
  double cos_gamma = 0, sin_gamma = 0, tan_gamma = 0, sqrt_temp = 0;
  double tan_x_tan = 0, cos_x_cos = 0;
  const double rad_to_deg_const = 180 / M_PI;
  const double half_pi = M_PI / 2;
  const double x_square = x * x, y_square = y * y, xy = x * y;
  correct_radius_ = calculate_correct_radius(r);
  if (x_square + y_square <= correct_radius_)
  {
    auto count = 0;
    while (gamma <= M_PI)
    {
      cos_gamma = cos(gamma);
      tan_gamma = tan(gamma);
      sin_gamma = sin(gamma);
      tan_x_tan = tan_gamma * tan_gamma;
      cos_x_cos = cos_gamma * cos_gamma;
      sqrt_temp = sqrt(x_square - y_square + 2 * xy*tan_gamma + ((-x + r)*(x + r) / cos_x_cos));

      if (gamma >= (-half_pi) && gamma <= (half_pi))
      {
        u = cos_x_cos * ((-y * tan_gamma) + (x*tan_x_tan) + sqrt_temp);
        v = cos_gamma * ((y*cos_gamma) + (sin_gamma*(-x + sqrt_temp)));
      }
      else
      {
        u = (-cos_x_cos)*((y*tan_gamma) - (x*tan_x_tan) + sqrt_temp);
        v = (-cos_gamma)*((-y * cos_gamma) + (sin_gamma*(x + sqrt_temp)));
      }

	  alpha = atan2(v,u);
      beta = ((alpha - gamma) * rad_to_deg_const);
	  num_of_image_.push_back(angle_to_num_of_image(alpha * rad_to_deg_const));
      num_of_column_per_image_.push_back(angle_to_num_of_column((beta)));
	 // std::cout << "Number: " << count << " alpha: " << angle_to_num_of_image(alpha * rad_to_deg_const) << std::endl;
     // << " beta: " << nearbyint(beta) << " gamma: " << nearbyint(gamma * rad_to_deg_const) << std::endl;
     //std::cout << "Number: " << count << " NoC " << angle_to_num_of_column((beta)) << std::endl;
	 //std::cout << "Number: " << count << " NoP " << angle_to_num_of_image((alpha*rad_to_deg_const)) << std::endl;
      gamma += ROTATION_ANGEL;

      count++;
    }
  }
}

std::thread result::thread_bmp(const int start_num, const int finish_num)
{
  return std::thread([=] { create_image_bmp(start_num, finish_num); });
}

std::thread result::thread_jpg(const int start_num, const int finish_num)
{
  return std::thread([=] { create_image_jpg(start_num, finish_num); });
}

double result::calculate_correct_radius(const double r) const
{
  return r*cos((M_PI / 2) - (FOV / 2));
}

int result::angle_to_num_of_column(const double angle) const
{
	auto tmp = (angle);
	auto direction_sign = (tmp < 0) ? -1 : 1;
	if (tmp > 180)
	{
		tmp = tmp - 360;
		direction_sign = -1;
	}
	else if (tmp < -180)
	{
		tmp = tmp + 360;
		direction_sign = 1;
	}
	else {}
	return ((width_image_ >> 1) + nearbyint(abs(tmp) * width_per_1deg_ * direction_sign));
}

int result::angle_to_num_of_image(const double angle)
{
	auto tmp = (angle);

	tmp = 1801 + (angle * 10);

	return tmp;
}

void result::save_result_bmp() const
{
  std::ostringstream oss;
  oss << "ResultImage.bmp";

  const auto file_name = oss.str();
  bmp_image_->save_image(file_name);

  oss.clear();
  oss.seekp(0);
}

void result::save_result_jpg(const int i) const
{

	std::ostringstream oss;
	oss << "ResultImage" << i << ".jpg";

	cv::Mat resized;
	const auto file_name = oss.str();

	resize(*jpg_image_, resized, cv::Size(3600, 738));

	const cv::Mat result(1800, 3600, CV_8UC3, cv::Scalar(0, 0, 0));
	resized.copyTo(result(cv::Rect(0, 532, resized.cols, resized.rows)));
	imwrite(file_name, result);

	oss.clear();
	oss.seekp(0);

}

void result::operator()(const cv::Range& range) const
{
	for (auto r = range.start; r < range.end; ++r)
	{
		std::unique_ptr<image> tmp(new image);

		for (auto i = (r*number_of_image_per_thread_) + 1; i <= (r + 1)*number_of_image_per_thread_; i++)
		{
			tmp->read_image_jpeg(num_of_image_[i - 1]);
			tmp->cut_jpg_image_column(num_of_column_per_image_[i - 1]);
			tmp->get_column_jpg().col(0).copyTo(jpg_image_->col(i - 1));
		}
	}
}
