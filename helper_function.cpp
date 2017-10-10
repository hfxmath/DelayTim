#include "helper_function.h"


#define Eng_far 18
#define Eng_near 17
#define Eng_nearmin 13
#define NTN 160


double max_energy(int16_t *src, const int len)
{
	double tmp = 0;
	for (int i = 0; i < len; i+= NTN)
	{
		double sum = energy(src + i, src + i, NTN);
		if (tmp < sum)
		{
			tmp = sum;
		}

	}
	return tmp;
}

double min_energy(int16_t *src, const int len)
{
	double tmp = 65535;
	for (int i = 0; i < len; i += NTN)
	{
		double minus = energy(src + i, src + i, NTN);
		if (tmp > minus)
		{
			tmp = minus;
		}
	}
	return tmp;
}

double energy(const int16_t *src, const int16_t *des, const int len)
{
	int16_t i = 0;
	double sum = 0;
	for (i = 0; i < len; i++)
	{
		sum += src[i] * des[i];
	}
	return log(sum);
}

float cal_sum(int16_t *src, const int len)
{
	float tmp = 0;
	for (int i = 0; i < len; i++)
	{
		tmp += src[i];
	}
	return tmp;
}

float cal_power(int16_t *src, int16_t *src1, const int len)
{
	float tmp = 0;
	for (int i = 0; i < len; i++)
	{
		tmp += src[i] * src1[i];
	}
	return tmp;
}

void cal_corre_coef(int16_t *far_, int16_t *near_, const int len, double &coef)
{
	double coeff1 = 0;
	float mul_xy = cal_power(far_, near_, len);
	float sum_xy = cal_sum(far_, len) * cal_sum(near_, len);
	float fz = mul_xy - sum_xy / len;

	float mul_2x = cal_power(far_, far_, len);
	float sum_x = cal_sum(far_, len) * cal_sum(far_, len);
	float mul_2y = cal_power(near_, near_, len);
	float sum_y = cal_sum(near_, len) * cal_sum(near_, len);

	float fm = sqrt(mul_2x - sum_x / len) * sqrt(mul_2y - sum_y / len);

	if (fm == 0)
	{
		coeff1 = 0;
	}
	else
	{
		coeff1 = fz / fm;
	}
	coeff1 = coeff1 >= 0 ? coeff1 : (-coeff1);
	coef = coeff1;
}

bool ismute(double &a, double &b)
{
	return ((a < Eng_near && b < Eng_near) || (a < Eng_nearmin));
}

bool promote_mute(double &a, double &alast, double &b)
{
	bool fit_a_b = a < Eng_far && a > Eng_near && b > Eng_near && b < Eng_far;
	bool res2 = ((a > 1.05 * alast) && fit_a_b && a > 1.03 * b);
	if (res2)
	{
		return true;
	}
	return false;
}

bool isvoice(double &a, double &b)
{
	return (a > Eng_far && (b < Eng_near && b > Eng_nearmin));
}

bool promote_voice(double &a, double &a_old, double &b)
{
	double coff = 0.05;
	bool res1 = a > (1 + coff) * a_old && a_old > Eng_far && a_old< Eng_far + 2 && a < Eng_far + 2;
	bool res2 = a > (1 - coff) * a_old && a_old > Eng_far && a_old < Eng_far + 2 && a < Eng_far + 2;
	bool res3 = b < Eng_near && b > Eng_nearmin;
	if (res1 && res3)
	{
		return true;
	}
	else if (res2 && res3)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool time_modify(double *src, const int len)
{
	double minval = 1;
	for (int i = 0; i < len; ++i)
	{
		if (minval > src[i])
		{
			minval = src[i];
		}
	}
	if (src[0] >= .8 && minval >= 0.78)
	{
		return true;
	}
	return false;
}

/* //ÔÝÊ±·ÏÆú
void modify_time(std::vector<int> times, int &decay)
{
	if (!times.empty())
	{
		int max_value = *max_element(times.begin(), times.end());
		int min_value = *min_element(times.begin(), times.end());
		if (max_value == min_value)
		{
			decay = max_value;
		}
		else if ((min_value - 10 > max_value) && (max_value - 5 > min_value))
		{
			decay = (int)(0.5 * (max_value + min_value));
		}
		else
		{
			decay = (int)(max_value * .9 + min_value * .1);
		}
	}
	else
	{
		decay = 5;
	}
}*/

void zero_avrg(int16_t *src, const int len)
{
	int16_t avg = 0;
	for (int i = 0; i < len; ++i)
	{
		avg += (short)(src[i] / len);
	}
	for (int i = 0; i < len; ++i)
	{
		src[i] -= avg;
	}
}