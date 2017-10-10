#include <vector>
#include <algorithm>
#include "echo_time.h"
#include "helper_function.h"

#include <sstream>//≤‚ ‘”√
using namespace std;//≤‚ ‘”√


#define MAX_NUM 48000
#define  TMP_NUM 3840
#define  TMP_FAR_NUM 3840
static short gfar_data[MAX_NUM];
static short gnear_data[MAX_NUM];

static short far_T[TMP_FAR_NUM];
static short near_T[TMP_NUM];

std::vector<int> gdecay_time;

static int delay_time = 0;

void recvdata(short *src_far, short *src_near, const int nrOfSamples)
{
    memset(gfar_data, 0, nrOfSamples); 
	memset(gnear_data, 0, nrOfSamples);
    std::copy(src_far, src_far + nrOfSamples, gfar_data);
    std::copy(src_near, src_near + nrOfSamples, gnear_data);
	process();
}

void compl_delay(int16_t *Tfar, int16_t *Tnear,const int lenf, const int lenn)
{
#define NN 32
	double coff[3] = { 0 };

    bool flag = 1;
    int row = 0, col = 0;

    for (int i = 0; i < lenn - NN; i += NN)
    {
		double Egy_Near = energy(Tnear + i, Tnear + i, NN);
        {
            if (flag)
            {
				for (int j = 0; j < lenf - NN; j += NN)
				{
					if (i >= j)
					{
						/*static FILE *fp = NULL;
						fopen_s(&fp, "allcoff.txt", "a+");
						fprintf(fp, "i = %d \t j = %d \t cft1 = %.3f \t cft2 = %.3f \t cft3 = %.3f\n", i, j, coff[0], coff[1], coff[2]);;
						fclose(fp);
						fp = NULL;*/

						double Egy_Far = energy(Tfar + i, Tfar + i, NN);
						if (Egy_Far >= 17 && (Egy_Near > 15 && Egy_Far > 1.1 * Egy_Near))
						{
							cal_corre_coef(Tnear + i, Tfar + j, NN, coff[0]);
							cal_corre_coef(Tnear + i, Tfar + j + NN, NN, coff[1]);
							cal_corre_coef(Tnear + i, Tfar + j + 2 * NN, NN, coff[2]);

							/*static FILE *fp = NULL;
							fopen_s(&fp, "coff.txt", "a+");
							fprintf(fp, "i = %d \t j = %d \t cft1 = %.3f \t cft2 = %.3f \t cft3 = %.3f \t Efar = %.3f \t Enear = %.3f\n", i, j, coff[0], coff[1], coff[2], Egy_Far,Egy_Near);
							fclose(fp);
							fp = NULL;*/

							/*static FILE *fp = NULL;
							fopen_s(&fp, "coff1.txt", "a+");
							fprintf(fp, "i = %d \t j = %d \t cft1 = %.3f  \t cft2 = %.3f \t cft3 = %.3f\n", i, j, coff[0], coff[1], coff[2]);;
							fclose(fp);
							fp = NULL;*/


							bool status = time_modify(coff, 3);
							if (status)
							{
								row = i;
								col = j;
								cout << "--------- i =  " << row << " j =" << col << " diff = " << (i - j) << " time = " << (int)(abs(row - col) / 16) << endl;
								cout << "Tnear =" << energy(Tnear + i, Tnear + i, NN) << endl;
								cout << " Tfar = " << energy(Tfar + i, Tfar + i, NN) << endl;
								flag = 0;
								break;
							}
						}
					}
                }
            }
        }
    }
    delay_time = (int)(abs(row - col) / 16);

    if (delay_time == 0)
    {
		return;
    }
    if (delay_time >= 300)
    {
        delay_time = 80; 
    }

    gdecay_time.push_back(delay_time);

}


void process()
{
    const int SKIP = 160;

    int lenG = sizeof(gfar_data) / sizeof(gfar_data[0]);
	int lenF = sizeof(far_T) / sizeof(far_T[0]);
	int lenN = sizeof(near_T) / sizeof(near_T[0]);

    short FAR[SKIP];
    short NEAR[SKIP];

    static int mute_CNT = 0;
    static int cntFar = 0;
    static bool flag_ = false;
	static bool voice_flag = false;
    static bool discrete = false;

    for (int i = 0; i < lenG; i += SKIP)
    { 
        if (discrete)
        {	 
			mute_CNT = 0;	  
			cntFar = 0;
			flag_ = false; 
			voice_flag = false;
			discrete = false;
        }
        else
        {
            memset(FAR, 0, sizeof(FAR));
            memset(NEAR, 0, sizeof(NEAR));
            memset(far_T, 0, sizeof(far_T));
            memset(near_T, 0, sizeof(near_T));

            memcpy(FAR, gfar_data + i, sizeof(FAR));

            memcpy(NEAR, gnear_data + i, sizeof(NEAR));

            double Ef = energy(FAR, FAR, SKIP);
            double En = energy(NEAR, NEAR, SKIP);
			static double lastFar = 0;
			
			/*static FILE *fp = NULL;
			fopen_s(&fp, "far-near.txt", "a+");
			fprintf(fp, "far = %.3f \t near = %0.3f\n", Ef, En);
			fclose(fp);
			fp = NULL;*/

			/*static FILE *fp = NULL;
			fopen_s(&fp, "far.txt", "a+");
			fprintf(fp, "%.3f \n", Ef);
			fclose(fp);
			fp = NULL;
																						   `
			static FILE *fp1 = NULL;
			fopen_s(&fp1, "near.txt", "a+");
			fprintf(fp1, "far =%.3f\n",  En);
			fclose(fp1);
			fp1 = NULL;*/

            if (ismute(Ef, En))
            {
                ++mute_CNT;
                if (mute_CNT >= 20)
                {
                    voice_flag = true;
					continue;
                }
            }
			else if (promote_mute(Ef, lastFar, En))
			{
				flag_ = true;
				continue;
			}
            else
            {
				lastFar = Ef;
				mute_CNT = 0;
            }

            if (voice_flag && isvoice(Ef, En))
			{
                ++cntFar;
                if (cntFar >= 2)
				{
                    flag_ = true;
                }
            }
			/*else if (promote_voice(Ef,lastFar,En))
			{
				flag_ = true;
				continue;
			}*/
			else
			{
				//voice_flag = false;
				cntFar = 0;
            }
			
			if (flag_)
			{
				memcpy(far_T, gfar_data + i, sizeof(far_T));
				memcpy(near_T, gnear_data + i - 80, sizeof(near_T));
				
				string name;
				ostringstream stream;
				stream << i;
				name = stream.str();
				cout << i << "-----"<<i / 160<<endl;
				string tmp = "far" + name + ".pcm";
				string tmp1 = "near" + name + ".pcm";;

				const char *s1 = const_cast<char*>(tmp.c_str());
				const char *s2 = const_cast<char*>(tmp1.c_str());

				{
					FILE *FPtr_FAR = NULL;
					FILE *FPtr_NEAR = NULL;
					fopen_s(&FPtr_FAR,s1, "wb");
					fopen_s(&FPtr_NEAR, s2, "wb");
					fwrite(far_T, sizeof(int16_t), lenF, FPtr_FAR);
					fwrite(near_T, sizeof(int16_t), lenN, FPtr_NEAR);
					fclose(FPtr_FAR);
					fclose(FPtr_NEAR);
					remove(s1);
					remove(s2);
				}
				compl_delay(far_T, near_T, lenF,lenN);
				discrete = true;
				/*static FILE *fp = NULL;
				fopen_s(&fp, "time.txt", "a+");
				fprintf(fp, "far =%.3f \t  near = %.3f \t muteCnt = %d \t cntFar = %d \t time = %d \n", Ef, En, mute_CNT, cntFar, delay_time);
				fclose(fp);
				fp = NULL;*/
				
			}
        }
    }
}

void setCallBack(callBackDelay fun)
{
    fun = res_time;
}

int res_time(int &delay)
{
	/*modify_time(gdecay_time, delay_time);*/
	delay = delay_time;
	std::cout << "delay_time is :" << delay_time << std::endl;;
	return delay;
}


//≤‚ ‘”√
int aec_time_test()
{
	FILE *fp_far = NULL;
	FILE *fp_near = NULL;

	fopen_s(&fp_far, "play.pcm", "rb");
	
	fopen_s(&fp_near, "cap.pcm", "rb");

	do
	{
		if (!fp_far || !fp_near)
		{
			printf("WebRtcAecTest open file err \n");
			break;
		}
		while (true)
		{
			if (fread(gfar_data, sizeof(short), MAX_NUM, fp_far))
			{
				//std::cout << "enough" << std::endl;
				fread(gnear_data, sizeof(short), MAX_NUM, fp_near);

				process();
			}
			else
			{
				std::cout << "not enough" << std::endl;
				break;
			}
		}

	} while (0);

	fclose(fp_far);
	fclose(fp_near);
	return 0;
}

int main()
{
	aec_time_test();
	int dd = 0;
	res_time(dd);
	std::vector<int>::iterator itr = gdecay_time.begin();
	for (itr; itr != gdecay_time.end(); ++itr)
	{
		std::cout <<*itr << ",";
	}
	std::cout <<std::endl;
	std::cout << "over" << std::endl;
	return 0;
}