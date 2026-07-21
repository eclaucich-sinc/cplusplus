#include <iostream>
#include <hdf5.h>
int main() {
    hid_t file_id = H5Fopen("input100.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) { std::cerr << "error opening file\n"; return 1; }
    H5Fclose(file_id);
    std::cout << "success\n";
    return 0;
}
