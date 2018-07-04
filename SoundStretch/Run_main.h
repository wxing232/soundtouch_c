#ifndef Run_main_H
#define Run_main_H

#include <iostream>
#include <stdexcept>

#include<vector>
#include<string>

#include "Run_main.h"
#include "RunParameters.h"
#include "WavFile.h"
#include "SoundTouch.h"



using namespace soundtouch;
using namespace std;

class SoundStretch 
{
public:
	SoundStretch();

	~SoundStretch();

private:
	static const int BUFF_SIZE = 6720;
	static const int path_input_max = 50;
	static const int num_input_max = 10;
	static const int num_input_min = 4;
	static bool flag_sound_deal_end ;
	static void openFiles(WavInFile **inFile, WavOutFile **outFile, const RunParameters *params);

	static void setup(SoundTouch * pSoundTouch, const WavInFile * inFile, const RunParameters * params);

	static void process_sound(SoundTouch * pSoundTouch, WavInFile * inFile, WavOutFile * outFile);

	static void detectBPM(WavInFile * inFile, RunParameters * params);
	void Free_memory();
	static void get_line();

	static int nParams;
	static char ** paramStr;
	
public:

	void do_process();
	void sound_main();

};






//class RunParameters
//{
//private:
//	void throwIllegalParamExp(const string &str) const;
//	void throwLicense() const;
//	void parseSwitchParam(const string &str);
//	void checkLimits();
//	float parseSwitchValue(const string &str) const;
//
//public:
//	char  *inFileName;
//	char  *outFileName;
//	float tempoDelta;
//	float pitchDelta;
//	float rateDelta;
//	int   quick;
//	int   noAntiAlias;
//	float goalBPM;
//	bool  detectBPM;
//	bool  speech;
//
//	RunParameters(const int nParams, const char * const paramStr[]);
//};

#endif
