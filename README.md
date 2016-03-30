# fisheye_undistort

Undistort photo taken by fisheye lens.

## Description

fisheye_undistort is command line tool that removes distortion of photograph caused by fisheye lens characteristics.
This program is based on lensprofile_creator_cameramodel.pdf document provided by Adobe. You can find this pdf by googling.
You can use the same parameters as lens profile used by Camera Raw or Photoshop LR.

Before

<img src="https://raw.githubusercontent.com/delphinus1024/fisheye_undistort/master/sample.png" style="width: 600px;"/>

After

<img src="https://raw.githubusercontent.com/delphinus1024/fisheye_undistort/master/result.png" style="width: 600px;"/>

## Features

- Remove fisheye lens distortion.
- Lens profile parameters can be specified by arguments.

## Requirement

- OpenCV 3.0.0 or above is preferable.
- Checked with win7 32bit + msys2 + gcc

## Usage

$ fisheye_undistort.exe [params] image result

        -?, -h, --help, --usage (value:true)
                show help
        -C, -c
                Clip to maximum rect
        --FX, --fx (value:0.421427)
                FocalLengthX
        --FY, --fy (value:0.421427)
                FocalLengthY
        --IX, --ix (value:0.500484)
                ImageXCenter
        --IY, --iy (value:0.507854)
                ImageYCenter
        --R1, --r1 (value:-0.020758)
                Radial Distortion Param1
        --R2, --r2 (value:-0.003717)
                Radial Distortion Param2
        -S, -s
                show image when finish
        -X, -x (value:2.0)
                enlarge factor

        image
                input image file
        result (value:result.tif)
                result image file


	
## Installation

1. Modify Makefile according to your OpenCV inludes and libs environment.
2. make

## Author

delphinus1024

## License

[MIT](https://raw.githubusercontent.com/delphinus1024/fisheye_undistort/master/LICENSE.txt)

