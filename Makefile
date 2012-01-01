#############################################################################
# Makefile for building qmetrics
#############################################################################

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2 -Wall -W $(DEFINES) `pkg-config --cflags opencv fftw3`
CXXFLAGS      = -pipe -O2 -Wall -W $(DEFINES) `pkg-config --cflags opencv fftw3`
INCPATH       = -Iinc
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  `pkg-config --libs fftw3 opencv`

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = src/main.cpp \
		src/mat.cpp \
		src/img.cpp \
		src/metric.cpp \
		src/debug.cpp \
		src/loader.cpp \
		src/csf.cpp 
OBJECTS       = main.o \
		mat.o \
		img.o \
		metric.o \
		debug.o \
		loader.o \
		csf.o
DESTDIR       = 
TARGET        = qmetrics

first: all

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

main.o: src/main.cpp inc/loader.h \
		inc/global.h \
		inc/img.hpp \
		inc/mat.hpp \
		inc/metric.h \
		inc/debug.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o src/main.cpp

mat.o: src/mat.cpp inc/mat.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mat.o src/mat.cpp

img.o: src/img.cpp inc/img.hpp \
		inc/mat.hpp \
		inc/global.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o img.o src/img.cpp

metric.o: src/metric.cpp inc/metric.h \
		inc/global.h \
		inc/mat.hpp \
		inc/img.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o metric.o src/metric.cpp

debug.o: src/debug.cpp inc/debug.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o debug.o src/debug.cpp

loader.o: src/loader.cpp inc/loader.h \
		inc/global.h \
		inc/img.hpp \
		inc/mat.hpp \
		inc/metric.h \
		inc/debug.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o loader.o src/loader.cpp

csf.o: src/csf.cpp inc/csf.h \
		inc/mat.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o csf.o src/csf.cpp
	
clean: 
	rm -f $(TARGET) $(OBJECTS) *~

