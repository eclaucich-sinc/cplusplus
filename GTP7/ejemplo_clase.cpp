#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "H5Cpp.h"

using namespace H5;
using namespace std;

void writeh5() {
  const int	 NX = 40000;
  const int	 NY = 6;
  const int	 RANK = 2;
  // Create a new file using the default property lists. 
  H5File file("w.h5",H5F_ACC_TRUNC);
    
  // Create the data space for the dataset.
  vector<hsize_t> dims = {NX,NY};
  DataSpace dataspace(RANK, dims.data());
  
  // Create the dataset.      
  DataSet dataset =
    file.createDataSet("/w",PredType::STD_I32BE, dataspace);
  
  vector<int> data(NX*NY);
  unsigned int checksum=0;
  for (int j = 0; j < NX; j++)
    for (int i = 0; i < NY; i++) {
      int z=10000+i * 6 + j + 1;
      data[j*NY+i] = z;
      checksum += z*z*z;
    }
  dataset.write(data.data(),PredType::NATIVE_INT);
  cout << "after write: checksum " << hex << checksum << endl;
}

void readh5() {
  H5::H5File h5file("w.h5",H5F_ACC_RDONLY);
  H5::DataSet dataset = h5file.openDataSet("/w");
  H5::DataSpace space = dataset.getSpace();
  int rank = space.getSimpleExtentNdims();

  vector<hsize_t> dims(rank);
  int ndims = space.getSimpleExtentDims(dims.data(),NULL);
  int sz = 1;
  for (int j=0; j<ndims; j++) sz *= dims[j];
  vector<int> w(sz);
  dataset.read(w.data(),H5::PredType::NATIVE_INT);
  h5file.close();
  unsigned int checksum=0;
  for (int j=0; j<sz; j++) 
    checksum += w[j]*w[j]*w[j];
  cout << "after read: checksum " << hex << checksum << endl;
}

int main (void) {
  writeh5();
  readh5();
  return 0;  // successfully terminated
}
