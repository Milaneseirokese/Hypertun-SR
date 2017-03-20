#include <iostream> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "sparse_stereo.hpp"
#include "delaunay_triangulation.hpp"
#include "disparity_interpolation.hpp"
#include "cost_evaluation.hpp"
#include "disparity_refinement.hpp"
#include "support_resampling.hpp"

struct parameters {
	// Occupancy grid size used for re-sampling
	int sz_occ;

	// Number of iterations the algorithm is allowed to run
	int n_iters;

	// Lower and upper threshold for validating disparities
	double t_lo;
	double t_hi;
};

void pipeline() {

	std::cout << "pipeline.cpp" << std::endl;

	//Load parameters
	parameters param;
	param.sz_occ = 32;
	param.n_iters = 1;
	param.t_lo = 1; // placeholder, verify optimal value
	param.t_hi = 5; // placeholder, verify optimal value

	// Load images
	cv::Mat I_l = cv::imread("../data/data_scene_flow/testing/image_2/000000_10.png");
	cv::Mat I_r = cv::imread("../data/data_scene_flow/testing/image_3/000000_10.png");
	// Display images
	//cv::imshow("Image Left", I_l);
	//cv::imshow("Image Right", I_r);
	//cv::waitKey(0);

	// Get image height and width
	int H = I_l.rows;
	int W = I_r.cols;

	// Initialize final disparity and associated cost
	cv::Mat D_f = cv::Mat(H, W, CV_64F, 0.0);
	cv::Mat C_f = cv::Mat(H, W, CV_64F, param.t_hi);

	// Declare other variables
	cv::Mat S; // set of N support points with valid depths, 3xN with [u,v,d]
	cv::Mat G; // graph (3D plane parameters?) from delaunay triangulation
	cv::Mat D; // dense piece-wise planar disparity
	cv::Mat C; // cost associated to D
	cv::Mat C_g; // cost associated with regions of good matches
	cv::Mat C_b; // cost associated with regions of bad matches

	sparse_stereo();
	float dummy_S[8][3] = {100, 100, 200, 200, 0, 0, 300, 300, 
						   100, 200, 100, 200, 0, 300, 0, 300,
						   500, 500, 500, 500, 200, 200, 200, 200};
	S = cv::Mat(3, 8, CV_32F, dummy_S);
	delaunay_triangulation(S, G, I_l);

	for (int i = 0; i < param.n_iters; ++i) {
		disparity_interpolation();
		cost_evaluation();
		disparity_refinement();
		if (i != param.n_iters) {
			support_resampling();
			//delaunay_triangulation(S, G, I_l);
		}
	}
	
	for (int i = 0; i < S.cols; ++i) {
		cv::circle(I_l, cv::Point(S.at<float>(0,i),S.at<float>(1,i)), 
			5, cv::Scalar(0,255,255),CV_FILLED, 8,0);
	}
	int k = 0;
	std::cout << G.rows/2 << std::endl;
	for (int i = 0; i < G.rows/2; ++i) {
		int i1 = G.at<int>(k++,0);
		int i2 = G.at<int>(k++,0);
		cv::Point p1(S.at<float>(0,i1), S.at<float>(1,i1));
		cv::Point p2(S.at<float>(0,i2), S.at<float>(1,i2));
		cv::line(I_l, p1, p2, cv::Scalar(0,255,255), 1, 8, 0);
		//std::cout << "drew line: " << i1 << ", " << i2 << std::endl;
	}
	cv::imshow("Image f", I_l);
	cv::waitKey(0);


}