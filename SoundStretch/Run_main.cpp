#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <thread>


#include<vector>
#include<string>
#include<sstream> 

#include "Run_main.h"
#include "RunParameters.h"
#include "WavFile.h"
#include "SoundTouch.h"
#include "BPMDetect.h"


using namespace soundtouch;
using namespace std;

int SoundStretch::nParams = 0;
char ** SoundStretch::paramStr = new char*[num_input_max];
bool SoundStretch::flag_sound_deal_end = 0;

SoundStretch::SoundStretch()
{
	//for (int i = 0; i < num_input_max; i++)
	//{
	//	paramStr[i] = new char[path_input_max];
	//}
}
SoundStretch::~SoundStretch()
{
	for (int i = 0; i<num_input_max; i++)
	{
		delete[] paramStr[i];
	}
	delete[] paramStr;
	paramStr = NULL;
}
void SoundStretch::openFiles(WavInFile **inFile, WavOutFile **outFile, const RunParameters *params)
{
	int bits, samplerate, channels;

	// open input file...
	*inFile = new WavInFile(params->inFileName);

	// ... open output file with same sound parameters
	bits = (int)(*inFile)->getNumBits();
	samplerate = (int)(*inFile)->getSampleRate();
	channels = (int)(*inFile)->getNumChannels();


	*outFile = new WavOutFile(params->outFileName, samplerate, bits, channels);
}


void SoundStretch::setup(SoundTouch *pSoundTouch, const WavInFile *inFile, const RunParameters *params)
{
	int sampleRate;
	int channels;

	sampleRate = (int)inFile->getSampleRate();
	channels = (int)inFile->getNumChannels();
	pSoundTouch->setSampleRate(sampleRate);
	pSoundTouch->setChannels(channels);

	pSoundTouch->setTempoChange(params->tempoDelta);
	pSoundTouch->setPitchSemiTones(params->pitchDelta);
	pSoundTouch->setRateChange(params->rateDelta);

	pSoundTouch->setSetting(SETTING_USE_QUICKSEEK, params->quick);
	pSoundTouch->setSetting(SETTING_USE_AA_FILTER, !(params->noAntiAlias));

	//if (params->speech )
	//{
	// use settings for speech processing
	pSoundTouch->setSetting(SETTING_SEQUENCE_MS, 40);
	pSoundTouch->setSetting(SETTING_SEEKWINDOW_MS, 15);
	pSoundTouch->setSetting(SETTING_OVERLAP_MS, 8);
	//fprintf(stderr, "Tune processing parameters for speech processing.\n");
	//}

	// print processing information
	if (params->outFileName)
	{
		//#ifdef SOUNDTOUCH_INTEGER_SAMPLES
		//        fprintf(stderr, "Uses 16bit integer sample type in processing.\n\n");
		//#else
		//    #ifndef SOUNDTOUCH_FLOAT_SAMPLES
		//        #error "Sampletype not defined"
		//    #endif
		//        fprintf(stderr, "Uses 32bit floating point sample type in processing.\n\n");
		//#endif
		//        // print processing information only if outFileName given i.e. some processing will happen
		//        fprintf(stderr, "Processing the file with the following changes:\n");
		//        fprintf(stderr, "  tempo change = %+g %%\n", params->tempoDelta);
		//        fprintf(stderr, "  pitch change = %+g semitones\n", params->pitchDelta);
		//        fprintf(stderr, "  rate change  = %+g %%\n\n", params->rateDelta);
		//fprintf(stderr, "Working...");
		cerr << "Working...";
	}
	else
	{
		// outFileName not given
		//fprintf(stderr, "Warning: output file name missing, won't output anything.\n\n");
		cerr << "Warning: output file name missing, won't output anything.\n" << endl;
	}
}


void SoundStretch::process_sound(SoundTouch *pSoundTouch, WavInFile *inFile, WavOutFile *outFile)
{
	int nSamples;
	int nChannels;
	int buffSizeSamples;
	SAMPLETYPE sampleBuffer[BUFF_SIZE];

	if ((inFile == NULL) || (outFile == NULL)) return;  // nothing to do.

	nChannels = (int)inFile->getNumChannels();
	assert(nChannels > 0);
	buffSizeSamples = BUFF_SIZE / nChannels;

	// Process samples read from the input file
	while (inFile->eof() == 0)
	{
		int num;

		// Read a chunk of samples from the input file
		num = inFile->read(sampleBuffer, BUFF_SIZE);
		nSamples = num / (int)inFile->getNumChannels();

		// Feed the samples into SoundTouch processor
		pSoundTouch->putSamples(sampleBuffer, nSamples);

		// Read ready samples from SoundTouch processor & write them output file.
		// NOTES:
		// - 'receiveSamples' doesn't necessarily return any samples at all
		//   during some rounds!
		// - On the other hand, during some round 'receiveSamples' may have more
		//   ready samples than would fit into 'sampleBuffer', and for this reason 
		//   the 'receiveSamples' call is iterated for as many times as it
		//   outputs samples.
		do
		{
			nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
			outFile->write(sampleBuffer, nSamples * nChannels);
		} while (nSamples != 0);
	}

	// Now the input file is processed, yet 'flush' few last samples that are
	// hiding in the SoundTouch's internal processing pipeline.
	pSoundTouch->flush();
	do
	{
		nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
		outFile->write(sampleBuffer, nSamples * nChannels);
	} while (nSamples != 0);
}



// Detect BPM rate of inFile and adjust tempo setting accordingly if necessary
void SoundStretch::detectBPM(WavInFile *inFile, RunParameters *params)
{
	float bpmValue;
	int nChannels;
	BPMDetect bpm(inFile->getNumChannels(), inFile->getSampleRate());
	SAMPLETYPE sampleBuffer[BUFF_SIZE];

	// detect bpm rate
	cerr << "Detecting BPM rate..." << ends;

	nChannels = (int)inFile->getNumChannels();
	assert(BUFF_SIZE % nChannels == 0);

	// Process the 'inFile' in small blocks, repeat until whole file has 
	// been processed
	while (inFile->eof() == 0)
	{
		int num, samples;

		// Read sample data from input file
		num = inFile->read(sampleBuffer, BUFF_SIZE);

		// Enter the new samples to the bpm analyzer class
		samples = num / nChannels;
		bpm.inputSamples(sampleBuffer, samples);
	}

	// Now the whole song data has been analyzed. Read the resulting bpm.
	bpmValue = bpm.getBpm();
	
	cerr << "Done!" << endl;
	// rewind the file after bpm detection
	inFile->rewind();

	if (bpmValue > 0)
	{
		cerr << "Detected BPM rate "<< bpmValue << endl;
		//fprintf(stderr, "Detected BPM rate %.1f\n\n", bpmValue);
	}
	else
	{
		cerr << "\n Couldn't detect BPM rate.\n" << endl;
		return;
	}

	if (params->goalBPM > 0)
	{
		// adjust tempo to given bpm
		params->tempoDelta = (params->goalBPM / bpmValue - 1.0f) * 100.0f;
		//fprintf(stderr, "The file will be converted to %.1f BPM\n\n", params->goalBPM);
		cerr << "The file will be converted to " << params->goalBPM << "BPM" << endl;
	}
}
void SoundStretch::Free_memory()
{
	for (int i = 0; i<nParams; i++)
	{
		delete[] paramStr[i];
		paramStr[i] = "";
	}
	flag_sound_deal_end = 0;
}

void  SoundStretch::do_process()
{
	WavInFile *inFile;
	WavOutFile *outFile;
	RunParameters *params;
	SoundTouch soundTouch;
	try
	{
		// Parse command line parameters

		params = new RunParameters(nParams, paramStr);

		// Open input & output files
		openFiles(&inFile, &outFile, params);

		if (params->detectBPM == true)
		{
			// detect sound BPM (and adjust processing parameters
			//  accordingly if necessary)
			detectBPM(inFile, params);
		}

		// Setup the 'SoundTouch' object for processing the sound
		setup(&soundTouch, inFile, params);

		// clock_t cs = clock();    // for benchmarking processing duration
		// Process the sound
		process_sound(&soundTouch, inFile, outFile);
		// clock_t ce = clock();    // for benchmarking processing duration
		// printf("duration: %lf\n", (double)(ce-cs)/CLOCKS_PER_SEC);

		// Close WAV file handles & dispose of the objects
		delete inFile;
		delete outFile;
		delete params;
		cerr << "Done!" << endl;
	}
	catch (const runtime_error )
	{
		// An exception occurred during processing, display an error message
		//fprintf(stderr, "%s\n", e.what());
		cerr << "Error message : 指令错误" << endl;
	}
	Free_memory();
}

void SoundStretch::get_line()
{
	vector<string> inputstr;

	int flag = 1;
	while (flag)
	{
		flag = 0;
		string str1, str2;
		cout << "请输入指令:" << endl;
		getline(cin, str1);          //从屏幕读取一行字符并赋给str
		istringstream str_io(str1);      //创建istringstream对象并同时初始化，使其和字符串str绑定
		vector<string> inputsting;
		while (str_io >> str2)
		{
			inputsting.push_back(str2);
		}
		if (inputsting.size() > num_input_max)
		{
			flag = 1;
			cout << "指令过长，请重新输入：" << endl;
			vector<string>().swap(inputsting);
			str_io.clear();
			str_io.str("");
		}
		else if (inputsting.size() < num_input_min)
		{
			flag = 1;
			cout << "指令过短，请重新输入：" << endl;
			vector<string>().swap(inputsting);
			str_io.clear();
			str_io.str("");
		}
		else if (inputsting[0] == "out") exit(-1);
		else if (inputsting[0] != "soundtouch")
		{
			flag = 1;
			cout << "输入错误，请重新输入！" << endl;
			vector<string>().swap(inputsting);
			str_io.clear();
			str_io.str("");
		}
		else
		{
			flag = 0;
			inputstr = inputsting;
			vector<string>().swap(inputsting);
			str_io.clear();
			str_io.str("");
		}
	}
	if (flag_sound_deal_end)
	{
		cerr << "The last one is in progress! waiting !" << endl;
		while (flag_sound_deal_end);
	}
	for (int i = 0; i < (int)inputstr.size(); i++)
	{
		paramStr[i] = new char[inputstr[i].size()+1];
		strcpy(paramStr[i], inputstr[i].data());
	}
	for (int i = inputstr.size(); i < num_input_max; i++)
	{
		paramStr[i] = "";
	}
	nParams = inputstr.size();
	flag_sound_deal_end = 1;
}

void SoundStretch::sound_main()
{
	while (1)
	{
		get_line();
		thread doing(&SoundStretch::do_process, this );
		doing.join();
	}
}