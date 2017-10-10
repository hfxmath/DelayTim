#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

 void process();

double energy(const int16_t *src, const int16_t *des, const int len);
float cal_sum(int16_t *src, const int len);
float cal_power(int16_t *src, int16_t *src1, const int len);
void cal_corre_coef(int16_t *far_frame, int16_t *near_frame, const int len, double &coef);
void compl_delay(int16_t *Tfar, int16_t *Tnear, const int lenf, const int lenn);

double max_energy(int16_t *src, const int len);
double min_energy(int16_t *src, const int len);

bool ismute(double &a, double &b);
bool promote_mute(double &a, double &aold, double &b);
bool isvoice(double &a, double &b);
bool promote_voice(double &a, double &a_old, double &b);

bool time_modify(double *src, const int);
/*
//暂时废弃
void modify_time(std::vector<int> times, int &decay);*/
int res_time(int &delay);


