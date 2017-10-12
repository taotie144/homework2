#include "iostream"
#include "opencv2/opencv.hpp"
#include <time.h>
#include<math.h>
using namespace cv;
using namespace std;
//#define IMG_SHOW
#define MY_OK 1
#define MY_FAIL -1
#define MAX_NUMBER 10000
#define PI 3.1415926


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
	int max_circle)
{
	if (NULL == colorImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}

	vector <Mat> mv;
	split(colorImg, mv);

	float sinvalue[360];
	float cosvalue[360];
	for (int angle = 0; angle < 360; angle++)
	{
		sinvalue[angle] = (float)sin((float)angle*PI / 180.0f);
		cosvalue[angle] = (float)cos((float)angle*PI / 180.0f);
	}

	int width = colorImg.cols;
	int height = colorImg.rows;


	int circle_x[MAX_NUMBER];
	memset(circle_x, 0, sizeof(int)*MAX_NUMBER);
	int circle_y[MAX_NUMBER];
	memset(circle_y, 0, sizeof(int)*MAX_NUMBER);
	int circle_radius[MAX_NUMBER];
	memset(circle_radius, 0, sizeof(int)*MAX_NUMBER);
	float circle_diff[MAX_NUMBER];
	memset(circle_diff, 0, sizeof(float)*MAX_NUMBER);
	int count_circle = 0;

	int rsin[360];
	int rcos[360];

	int sin5[360];
	int cos5[360];
	for (int r = min_radius; r <= max_radius; r++)
	{
		for (int the = 0; the < 360; the++)
		{

			rsin[the] = (int)r*sinvalue[the];
			rcos[the] = (int)r*cosvalue[the];
			sin5[the] = (int)5 * sinvalue[the];
			cos5[the] = (int)5 * cosvalue[the];

		}
		for (int i = r; i < height - r; i++)
		{
			for (int j = r; j < width - r; j++)
			{

				float sum_color_r_out = 0;
				float sum_color_g_out = 0;
				float sum_color_b_out = 0;
				float mean_color_r_out = 0;
				float mean_color_g_out = 0;
				float mean_color_b_out = 0;
				float sum_color_r_in = 0;
				float sum_color_g_in = 0;
				float sum_color_b_in = 0;
				float mean_color_r_in = 0;
				float mean_color_g_in = 0;
				float mean_color_b_in = 0;
				float color_r_out = 0;
				float color_g_out = 0;
				float color_b_out = 0;
				float color_r_in = 0;
				float color_g_in = 0;
				float color_b_in = 0;
				float count = 0;
				float mean_color_r = 0;
				float mean_color_g = 0;
				float mean_color_b = 0;

				for (int theta = 0; theta < 360; theta++)
				{

					int y_now = i + rsin[theta];
					int x_now = j + rcos[theta];
					int y_out = y_now + sin5[theta];
					int x_out = x_now + cos5[theta];
					int y_in = y_now - sin5[theta];
					int x_in = x_now - cos5[theta];

					int temp_out = y_out*width + x_out;
					int temp_in = y_in*width + x_in;

					color_r_in = mv[0].data[temp_in];
					color_g_in = mv[1].data[temp_in];
					color_b_in = mv[2].data[temp_in];

					sum_color_r_in += color_r_in;
					sum_color_g_in += color_g_in;
					sum_color_b_in += color_b_in;
					if (y_out >  0 && x_out > 0 && x_out < (width) && y_out < (height))
					{
						color_r_out = mv[0].data[temp_out];
						color_g_out = mv[1].data[temp_out];
						color_b_out = mv[2].data[temp_out];

						sum_color_r_out += color_r_out;
						sum_color_g_out += color_g_out;
						sum_color_b_out += color_b_out;

						count++;
					}

				}
				mean_color_r_out = sum_color_r_out / count;
				mean_color_g_out = sum_color_g_out / count;
				mean_color_b_out = sum_color_b_out / count;
				mean_color_r_in = sum_color_r_in / 360.0f;
				mean_color_g_in = sum_color_g_in / 360.0f;
				mean_color_b_in = sum_color_b_in / 360.0f;

				mean_color_r = abs(mean_color_r_out - mean_color_r_in);
				mean_color_g = abs(mean_color_g_out - mean_color_g_in);
				mean_color_b = abs(mean_color_b_out - mean_color_b_in);



				float max_diff = mean_color_r;
				if (mean_color_g > mean_color_r&&mean_color_g > mean_color_b)
				{
					max_diff = mean_color_g;
				}
				if (mean_color_b > mean_color_r&&mean_color_b > mean_color_g)
				{
					max_diff = mean_color_b;
				}

				if (max_diff > max_circle_diff)
				{
					int found_circle_sum = 0;
					for (int k2 = 0; k2 < count_circle; k2++)
					{
						if (abs(i - circle_y[k2]) <= min_center_dist &&abs(j - circle_x[k2]) <= min_center_dist  && abs(r - circle_radius[k2]) <= min_radius_dist)
						{
							found_circle_sum = 1;

							if (max_diff > circle_diff[k2])
							{
								circle_x[k2] = j;
								circle_y[k2] = i;
								circle_radius[k2] = r;
								circle_diff[k2] = max_diff;


							}
							break;
						}
					}
					if (found_circle_sum == 0)
					{
						circle_x[count_circle] = j;
						circle_y[count_circle] = i;
						circle_radius[count_circle] = r;
						circle_diff[count_circle] = max_diff;
						count_circle++;

					}

				}




			}
		}
	}



	if (count_circle <= max_circle)
	{
		*circle_cnt = count_circle;
		for (int ii = 0; ii < count_circle; ii++)
		{
			*x = circle_x[ii];
			*y = circle_y[ii];
			*radius = circle_radius[ii];
			x++;
			y++;
			radius++;
		}
	}
	else if (count_circle > max_circle)
	{

		float temp_diff[MAX_NUMBER];
		*circle_cnt = max_circle;
		for (int temp_count = 0; temp_count < count_circle; temp_count++)
		{
			temp_diff[temp_count] = circle_diff[temp_count];
		}
		for (int temp_count1 = 1; temp_count1 < max_circle + 1; temp_count1++)
		{
			for (int temp_count2 = count_circle; temp_count2 >= temp_count1; temp_count2--)
			{
				if (temp_diff[temp_count2] > temp_diff[temp_count2 - 1])
				{
					float temp_change = temp_diff[temp_count2 - 1];
					temp_diff[temp_count2 - 1] = temp_diff[temp_count2];
					temp_diff[temp_count2] = temp_change;

				}
			}

		}
		for (int temp_count = 0; temp_count < count_circle; temp_count++)
		{
			if (circle_diff[temp_count] >= temp_diff[max_circle - 1])
			{
				*x = circle_x[temp_count];
				*y = circle_y[temp_count];
				*radius = circle_radius[temp_count];
				x++;
				y++;
				radius++;
			}
		}

	}

	return MY_OK;


}
