#include <iostream>
#include <hdf5.h>

herr_t file_info(hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data) {
    std::cout << name << std::endl;
    return 0;
}

int main() {
    hid_t file_id = H5Fopen("input100.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) { std::cerr << "error input\n"; return 1; }
    std::cout << "input100.h5 keys:\n";
    H5Literate(file_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, file_info, NULL);
    H5Fclose(file_id);

    file_id = H5Fopen("outputref100.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) { std::cerr << "error output\n"; return 1; }
    std::cout << "\noutputref100.h5 keys:\n";
    H5Literate(file_id, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, file_info, NULL);
    H5Fclose(file_id);
    return 0;
}
