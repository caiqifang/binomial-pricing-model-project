EXECUTABLE := main

CC_FILES   := main.cpp model_s.cpp

all: $(EXECUTABLE) $(REFERENCE)

###########################################################

OBJDIR=objs
CXX=g++ -m64
CXXFLAGS=-O3 -Wall -g
LDFLAGS=-L/usr/local/depot/cuda-7.5/lib64/ -lcudart
NVCC=nvcc -arch=sm_20
NVCCFLAGS=-O3 -m64
PATH=$(shell pwd)


OBJS=$(OBJDIR)/main.o $(OBJDIR)/model_s.o
.PHONY: dirs clean

default: $(EXECUTABLE)

dirs:
		mkdir -p $(OBJDIR)/

clean:
		rm -rf $(OBJDIR) $(EXECUTABLE)

$(EXECUTABLE): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o:%.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@ -I$(PATH)

