CXX = g++
CXXFLAGS = -Wall -O2 -I/usr/include/hdf5/serial
LDFLAGS = -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_cpp -lhdf5

all: ej1

ej1: ej1.cpp
	$(CXX) $(CXXFLAGS) ej1.cpp -o ej1 $(LDFLAGS)

clean:
	rm -f ej1 output.h5
