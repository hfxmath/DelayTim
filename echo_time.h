#pragma once


/*
#ifdef __cplusplus  
extern "C" {
#endif  

	typedef int(*recvFarData)(short*, const int&);
	typedef int(*recvNearData)(short*, const int&);


	static inline void recvdata_far(short *far, const int nrOfSamples);
	static inline void recvdata_near(short *near, const int nrOfSamples);

	typedef int(*callBackDelay)(int&);
	inline void setCallBack(callBackDelay fun);

#ifdef __cplusplus  
}
#endif 
*/

//typedef int(*recvdata)(short*, short*,const int&);

inline void recvdata(short *far, short *near, const int nrOfSamples);

typedef int(*callBackDelay)(int&);
inline void setCallBack(callBackDelay fun);