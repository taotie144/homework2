#include "opencv2/opencv.hpp"
using namespace cv;
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#define PI 3.141592653
//#define IMG_SHOW
#define MY_OK 1
#define MY_FAIL -1
#define M 10000
int ustc_Find_Circles_By_Difference(
	Mat colorImg,
	int min_radius,
	int max_radius,
	int min_center_dist,
	int min_radius_dist,
	int max_circle_diff,
	int* x,
	int* y,
	int* radius,
	int* circle_cnt,
	int max_circle){
	if (NULL == colorImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}

	int tempx[M] = { 0 };
	int tempy[M] = { 0 };
	int tempr[M] = { 0 };
	float tempdelta[M] = { 0 };
	int num = 0;
	int width = colorImg.cols;
	int height = colorImg.rows;

	float sinnew[360];
	float cosnew[360];

	for (int angle = 0; angle < 360; angle++)
	{
		sinnew[angle] = (float)sin((float)angle*PI / 180.0f);
		cosnew[angle] = (float)cos((float)angle*PI / 180.0f);
	}

	vector<Mat> mv;
	split(colorImg, mv);
	for (int i = min_radius; i < height - min_radius; i++)
	{
		for (int j = min_radius; j < width - min_radius; j++)
		{
			for (int r = min_radius; r <= max_radius; r++)
			{
				float color_r_out0 = 0;
				float color_r_out1 = 0;
				float color_r_out2 = 0;
				float color_r_in0 = 0;
				float color_r_in1 = 0;
				float color_r_in2 = 0;
				int count_out = 0;
				int count_in = 0;
				for (int theta = 0; theta < 360; theta++){
					int out_y = i + (r + 5)*sinnew[theta];
					int out_x = j + (r + 5)*cosnew[theta];
					int temp_out = out_y*width + out_x;
					if (out_y > 0 && out_x > 0 && out_y < height&&out_x < width)
					{
						color_r_out0 = color_r_out0 + mv[0].data[temp_out];
						color_r_out1 = color_r_out1 + mv[1].data[temp_out];
						color_r_out2 = color_r_out2 + mv[2].data[temp_out];
						count_out++;
					}
					int in_y = i + (r - 5)*sinnew[theta];
					int in_x = j + (r - 5)*cosnew[theta];
					int temp_in = in_y*width + in_x;
					if (in_y > 0 && in_x > 0 && in_y < height&&in_x < width)
					{
						color_r_in0 = color_r_in0 + mv[0].data[temp_in];
						color_r_in1 = color_r_in1 + mv[1].data[temp_in];
						color_r_in2 = color_r_in2 + mv[2].data[temp_in];
						count_in++;
					}

				}
				float mean0_out = color_r_out0 / count_out;
				float mean0_in = color_r_in0 / count_in;
				float mean1_out = color_r_out1 / count_out;
				float mean1_in = color_r_in1 / count_in;
				float mean2_out = color_r_out2 / count_out;
				float mean2_in = color_r_in2 / count_in;
				float delta0 = abs(mean0_out - mean0_in);
				float delta1 = abs(mean1_out - mean1_in);
				float delta2 = abs(mean2_out - mean2_in);
				if ((delta0 > max_circle_diff) || (delta1 > max_circle_diff) || (delta2 > max_circle_diff)){
					int flag = 0;
					for (int n = 0; n < num; n++){
						if ((abs(tempx[n] - j) < min_center_dist && abs(tempy[n] - i) < min_center_dist && abs(tempr[n] - r) < min_radius_dist)) {
							flag = 1;
							break;
						}
					}
					if (flag == 0){
						tempx[num] = j;
						tempy[num] = i;
						tempr[num] = r;

						if (delta0 > delta1){
							if (delta0 > delta2) tempdelta[num] = delta0;
							else tempdelta[num] = delta2;
						}
						else{
							if (delta1>delta2)  tempdelta[num] = delta1;
							else tempdelta[num] = delta2;
						}num++;
					}

				}

			}

		}
	}

	if (num <= max_circle){
		*circle_cnt = num;
		if (num == 0)  return MY_OK;
		for (int i = 0; i < num; i++){
			x[i] = tempx[i];
			y[i] = tempy[i];
			radius[i] = tempr[i];
		}
		return MY_OK;
	}
	*circle_cnt = max_circle;
	int temporder[M] = { 0 };
	for (int i = 0; i < num; i++){
		temporder[i] = i;
	}
	for (int i = 1; i <= max_circle; i++) {
		for (int j = num - 1; j >= i; j--){
			if (tempdelta[j] > tempdelta[j - 1]) {
				int temp = temporder[j];
				temporder[j] = temporder[j - 1];
				temporder[j - 1] = temp;
				float temp_delta = tempdelta[j];
				tempdelta[j] = tempdelta[j - 1];
				tempdelta[j - 1] = temp_delta;
			}
		}

	}
	for (int i = 0; i < max_circle; i++){
		int order = temporder[i];
		x[i] = tempx[order];
		y[i] = tempy[order];
		radius[i] = tempr[order];
	}

	
	return  MY_OK;
}
