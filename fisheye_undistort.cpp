// fisheye undistortion

#include <math.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

// Distortion params
double FocalLengthX;
double FocalLengthY;
double ImageXCenter;
double ImageYCenter;
double RadialDistortParam1;
double RadialDistortParam2;

// multiply by Dmax
double FocalLengthXD;
double FocalLengthYD;
double ImageXCenterD;
double ImageYCenterD;

typedef struct {
	double du;
	double dv;
	double ud;
	double vd;
} coords;

typedef struct {
	int src_width;
	int src_height;
	int tar_width;
	int tar_height;
} im_size;

double	sEnlage = 2.;	// enlarge factor default

int init(double pWidth,double pHeight) {
	
	// multiply by Dmax

	if(pWidth > pHeight) {	
		FocalLengthXD = pWidth * FocalLengthX;
		FocalLengthYD = pWidth * FocalLengthY;
	} else {
		FocalLengthXD = pHeight * FocalLengthX;
		FocalLengthYD = pHeight * FocalLengthY;
	}
	
	ImageXCenterD = pWidth * ImageXCenter;
	ImageYCenterD = pHeight * ImageYCenter;
	
	std::cout <<
		"FocalLengthXD=" << FocalLengthXD << "," << 
		"FocalLengthYD=" << FocalLengthYD << "," << 
		"ImageXCenterD=" << ImageXCenterD << "," << 
		"ImageYCenterD=" << ImageYCenterD << std::endl;
		
	
	return 0;
}

coords distort_point(double pU,double pV) {
	coords ret;
	double du=pU;
	double dv=pV;
	
	double r = sqrt(du * du + dv * dv);

	ret.du = du;
	ret.dv = dv;
	
	if(r == 0.) {
		ret.ud = 0.;
		ret.vd = 0.;
		return ret;
	}
	
	double ff = sqrt(FocalLengthXD * FocalLengthYD);
	double theta = atan2(r,ff);
	double theta3 = theta * theta * theta;
	double theta5 = theta3 * theta * theta;
	double factor = (theta + RadialDistortParam1 * theta3 + RadialDistortParam2 * theta5) / r;
	const double ud = factor * FocalLengthXD * du;
	const double vd = factor * FocalLengthYD * dv;

	ret.ud = ud;
	ret.vd = vd;

	return ret;
} 

int calc_remap(const im_size &isize,cv::Mat &src,cv::Mat &canvas,cv::Mat &map_x,cv::Mat &map_y,cv::Rect &org_rect,cv::Rect &max_rect) {
	
	int canvas_centerx = isize.tar_width/2;
	int canvas_centery = isize.tar_height/2;
	int xorg = canvas_centerx - static_cast<int>(ImageXCenterD);
	int yorg = canvas_centery - static_cast<int>(ImageYCenterD);
	
	assert(xorg >= 0 && yorg >= 0);
	
	std::cout << "(canvas_centerx,canvas_centery)=" << canvas_centerx << "," << canvas_centery << std::endl;
	std::cout << "(xorg,yorg)=" << xorg << "," << yorg << std::endl;
	
	org_rect = cv::Rect(xorg, yorg, isize.src_width, isize.src_height);
	
	cv::Mat src_region(canvas, org_rect);
	src.copyTo(src_region);
	
	for(int y = 0;y < isize.tar_height;++y) {
		for(int x = 0;x < isize.tar_width;++x) {
			coords co = distort_point(static_cast<double>(x - canvas_centerx) ,static_cast<double>(y - canvas_centery));
			map_x.at<float>(y,x) = static_cast<float>(co.ud + canvas_centerx);
			map_y.at<float>(y,x) = static_cast<float>(co.vd + canvas_centery);
		}
	}
		
	// find maximum rectangle area
	for(int x = canvas_centerx;;--x) { // find left edge
		coords co = distort_point(static_cast<double>(x - canvas_centerx) ,0.);
		if(co.ud < -ImageXCenterD) {
			max_rect.x = x + 1;
			break;
		}
	}

	for(int x = canvas_centerx;;++x) { // find right edge
		coords co = distort_point(static_cast<double>(x - canvas_centerx) ,0.);
		if(co.ud > (src.cols - ImageXCenterD)) {
			max_rect.width = x - 1 - max_rect.x;
			break;
		}
	}
	
	for(int y = canvas_centery;;--y) { // find top edge
		coords co = distort_point(0., static_cast<double>(y - canvas_centery) );
		if(co.vd < -ImageYCenterD) {
			max_rect.y = y + 1;
			break;
		}
	}

	for(int y = canvas_centery;;++y) { // find bottom edge
		coords co = distort_point(0., static_cast<double>(y - canvas_centery) );
		if(co.vd > (src.rows - ImageYCenterD)) {
			max_rect.height = y - 1 - max_rect.y;
			break;
		}
	}
	
	
	std::cout << "maxrect:" << max_rect << std::endl;
	
	return 0;
}

// For default parameter, using that of Canon EF 15mm fisheye.
const cv::String keys =
	"{help h usage ? |          | show help                }"
	"{@image         |          | input image file         }"
    "{@result        |result.tif| result image file        }"
	"{X x            |      2.0 | enlarge factor           }"
    "{S s            |          | show image when finish   }"
    "{C c            |          | Clip to maximum rect     }"
    "{FX fx          | 0.421427 | FocalLengthX             }"
    "{FY fy          | 0.421427 | FocalLengthY             }"
    "{IX ix          | 0.500484 | ImageXCenter             }"
    "{IY iy          | 0.507854 | ImageYCenter             }"
    "{R1 r1          |-0.020758 | Radial Distortion Param1 }"
    "{R2 r2          |-0.003717 | Radial Distortion Param2 }"
	;

int main(int argc,char *argv[]) {
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Fisheye camera calibration sample.");

    bool should_show = parser.has("S") ? true: false;
    bool do_clip = parser.has("C") ? true: false;
    sEnlage = parser.get<double>("X");
    cv::String imagefile = parser.get<cv::String>("@image");
    cv::String resultfile = parser.get<cv::String>("@result");
    sEnlage = parser.get<double>("X");
    FocalLengthX = parser.get<double>("FX");
    FocalLengthY = parser.get<double>("FY");
    ImageXCenter = parser.get<double>("IX");
    ImageYCenter = parser.get<double>("IY");
    RadialDistortParam1 = parser.get<double>("R1");
    RadialDistortParam2 = parser.get<double>("R2");

    if (parser.has("h") || !parser.check() || imagefile.empty() || resultfile.empty()) {
        parser.printMessage();
        return -1;
    }
	
	CV_Assert(sEnlage >= 1.);
	CV_Assert(FocalLengthX > 0.);
	CV_Assert(FocalLengthY > 0.);
	CV_Assert(ImageXCenter > 0.);
	CV_Assert(ImageYCenter > 0.);
	
    cv::Mat src = cv::imread(imagefile);
	cv::Mat canvas = cv::Mat::zeros(src.rows * static_cast<int>(sEnlage),src.cols * static_cast<int>(sEnlage), CV_8UC3);
	cv::Mat tar = cv::Mat::zeros(canvas.size(), CV_8UC3);
	cv::Mat map_x = cv::Mat::zeros(canvas.size(), CV_32FC1);
	cv::Mat map_y = cv::Mat::zeros(canvas.size(), CV_32FC1);

    std::cout << "Processing " << imagefile << "..." << std::endl;
    std::cout << "source image size:" << src.cols << "," << src.rows << std::endl;
	
	im_size isize;
	
	isize.src_width = src.cols,isize.src_height = src.rows;
	isize.tar_width = tar.cols,isize.tar_height = tar.rows;
	
	std::cout << "source image size:" << isize.src_width << "," << isize.src_height << std::endl;
	std::cout << "target image size:" << isize.tar_width << "," << isize.tar_height << std::endl;
	std::cout << "Enlarge factor:" << sEnlage << std::endl;
	
	init(isize.src_width , isize.src_height);
	
	cv::Rect org_rect,max_rect;
	
	calc_remap(isize,src,canvas,map_x,map_y,org_rect,max_rect);
	cv::remap(canvas, tar, map_x, map_y, CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0, 0));
	
	//cv::rectangle(tar, cv::Point(org_rect.x,org_rect.y), cv::Point(org_rect.x + org_rect.width,org_rect.y + org_rect.height), 
	//	cv::Scalar(0,0,200), 4, 4);
	//cv::rectangle(tar, max_rect,cv::Scalar(0,200,0), 4, 4);
	
	if(max_rect.x < 0) max_rect.x = 0;
	if(max_rect.y < 0) max_rect.y = 0;
	if((max_rect.width - max_rect.x) > tar.cols) max_rect.width = tar.cols - max_rect.x;
	if((max_rect.height - max_rect.y) > tar.rows) max_rect.height = tar.cols - max_rect.y;
	
	cv::Mat clip;
	if(do_clip) 
	    clip = tar(max_rect);
    else
        clip = tar;
	
	imwrite(resultfile, clip);
	
	if(should_show) {
        cv::namedWindow("result", cv::WINDOW_NORMAL );
        cv::imshow("result",clip);
        cv::waitKey();
    }
	
	
	return 0;
}
