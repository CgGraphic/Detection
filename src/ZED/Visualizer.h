#pragma once
#include "opencv2/imgproc/imgproc_c.h"
#ifdef __cplusplus

extern "C"
{
#endif

	int visializer(int argc, char **argv);

	int Initialize_ZED(int argc, char ** argv);

	IplImage GetOneFrame();

	void Detector_ZED(int argc, char ** argv);

#ifdef __cplusplus
}
#endif
