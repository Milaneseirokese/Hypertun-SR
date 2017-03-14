#ifndef COST_EVALUATION_HPP
#define COST_EVALUATION_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat cost_evaluation(cv::Mat I_l, cv::Mat I_r, cv::Mat D_it);

#endif // COST_EVALUATION_HPP