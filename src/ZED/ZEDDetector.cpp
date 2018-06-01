#include "Visualizer.h"
#include "..\YOLO_Header.h"
#include "..\network.h" // Need first include C++ version of network to exclude the c version of parser.h
extern "C" {
#include  "..\option_list.h"
#include "..\parser.h"
#include "..\image.h"
#include "..\image_utils.h"
#include "..\region_layer.h"
#include "..\utils.h"
}

#include <stdio.h>

namespace ZEDDetector
{
	void InitizeYOLO()
	{
		char *datacfg = "data/coco.data";
		char *cfgfile = "yolo.cfg";
		char *weightfile = "yolo.weights";
		char *filename = "";
		float thresh = 0.3;

		read_data_cfg(datacfg);
		list *options = read_data_cfg(datacfg);
		char *name_list = option_find_str(options, "names", "data/names.list");
		char **names = get_labels(name_list);

		image **alphabet = load_alphabet();
		network net = parse_network_cfg(cfgfile);
		if (weightfile) {
			load_weights(&net, weightfile);
		}
		set_batch_network(&net, 1);
		srand(2222222);
		//char buff[256];
		//char *input = buff;
		//int j;
		float nms = .4;

	}


	image ConvertOpenCVImage(IplImage & img,int w,int h)
	{
		image out = ipl_to_image(&img);
		if ((h && w) && (h != out.h || w != out.w)) {
			image resized = resize_image(out, w, h);
			free_image(out);
			out = resized;
		}
		return out;
	}

	void DetectOneFrame(IplImage & input,network & net,float thresh,float nms,char ** names,image ** alphabet)
	{
		clock_t time;

		image im = ConvertOpenCVImage(input, 0, 0);
		image sized = resize_image(im, net.w, net.h);
		layer l = net.layers[net.n - 1];

		box *boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
		float **probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
		for (int j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float *));

		float *X = sized.data;
		time = clock();
		network_predict(net, X);
		printf("%s: Predicted in %f seconds.\n", input, sec(clock() - time));
		get_region_boxes(l, 1, 1, thresh, probs, boxes, 0, 0);
		if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);
		draw_detections(im, l.w*l.h*l.n, thresh, boxes, probs, names, alphabet, l.classes);
	}
}


#ifdef __cplusplus
extern "C" {
#endif



	void Detector_ZED(int argc, char ** argv)
	{

		ZEDDetector::InitizeYOLO();

		char* new_argv[2];
		new_argv[0] = argv[0];
		new_argv[1] = argv[2];
		Initialize_ZED(2, new_argv);
		

		IplImage one_frame = GetOneFrame();
		printf("%d %d", one_frame.height, one_frame.width);


		system("pause");
	}

#ifdef __cplusplus
}
#endif