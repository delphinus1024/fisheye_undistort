#NAME = fish_eye_dist_grid
#NAME = fish_eye_udist_grid
NAME = fisheye_undistort
PROGRAM = $(NAME).exe
CXXFLAGS= -march=native -msse2 -O2

LDFLAGS=-lopencv_calib3d.dll -lopencv_core.dll -lopencv_features2d.dll -lopencv_flann.dll -lopencv_highgui.dll -lopencv_imgcodecs.dll -lopencv_imgproc.dll -lopencv_ml.dll -lopencv_objdetect.dll -lopencv_photo.dll -lopencv_shape.dll -lopencv_stitching.dll -lopencv_superres.dll -lopencv_video.dll -lopencv_videoio.dll -lopencv_videostab.dll -lm
OBJECTS=$(NAME).o

.SUFFIXES: .cpp .o

$(PROGRAM): $(OBJECTS)
	$(CXX)  $^  -o $(PROGRAM) $(LDFLAGS) 

.c.o:
	$(CXX) -c $(CXXFLAGS) $<

.PHONY:clean
clean:
	rm -f $(NAME).exe  *.o
