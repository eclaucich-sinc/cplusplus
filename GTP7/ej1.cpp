#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <memory>
#include <hdf5.h>
#include "H5Cpp.h"

using namespace std;
using namespace H5;

// Geometric Classes
class geoshape_t {
public:
    virtual ~geoshape_t() = default;
    virtual void bbox(vector<double> &bb) = 0;
    virtual bool inside(const vector<double> &x) = 0;
};

typedef shared_ptr<geoshape_t> gsp_t;

double area(geoshape_t &g, int N = 100) {
    vector<double> bb;
    g.bbox(bb);
    if(bb.size() < 4) return 0.0;
    double dx = (bb[2] - bb[0]) / N;
    double dy = (bb[3] - bb[1]) / N;
    if (dx <= 0 || dy <= 0) return 0.0;
    double a = 0.0;
    vector<double> pt(2);
    for (int i = 0; i < N; ++i) {
        pt[0] = bb[0] + (i + 0.5) * dx;
        for (int j = 0; j < N; ++j) {
            pt[1] = bb[1] + (j + 0.5) * dy;
            if (g.inside(pt)) {
                a += dx * dy;
            }
        }
    }
    return a;
}

class circle_t : public geoshape_t {
    double cx, cy, radius;
public:
    circle_t(double cx, double cy, double r) : cx(cx), cy(cy), radius(r) {}
    void bbox(vector<double> &bb) override {
        bb = {cx - radius, cy - radius, cx + radius, cy + radius};
    }
    bool inside(const vector<double> &x) override {
        return pow(x[0] - cx, 2) + pow(x[1] - cy, 2) <= radius * radius;
    }
};

class intersection_t : public geoshape_t {
public:
    vector<gsp_t> shapes;
    void bbox(vector<double> &bb) override {
        if (shapes.empty()) return;
        shapes[0]->bbox(bb);
        vector<double> temp_bb;
        for (size_t i = 1; i < shapes.size(); ++i) {
            shapes[i]->bbox(temp_bb);
            bb[0] = max(bb[0], temp_bb[0]);
            bb[1] = max(bb[1], temp_bb[1]);
            bb[2] = min(bb[2], temp_bb[2]);
            bb[3] = min(bb[3], temp_bb[3]);
        }
    }
    bool inside(const vector<double> &x) override {
        if (shapes.empty()) return false;
        for (const auto& shape : shapes) {
            if (!shape->inside(x)) return false;
        }
        return true;
    }
};

class union_t : public geoshape_t {
public:
    vector<gsp_t> shapes;
    void bbox(vector<double> &bb) override {
        if (shapes.empty()) return;
        shapes[0]->bbox(bb);
        vector<double> temp_bb;
        for (size_t i = 1; i < shapes.size(); ++i) {
            shapes[i]->bbox(temp_bb);
            bb[0] = min(bb[0], temp_bb[0]);
            bb[1] = min(bb[1], temp_bb[1]);
            bb[2] = max(bb[2], temp_bb[2]);
            bb[3] = max(bb[3], temp_bb[3]);
        }
    }
    bool inside(const vector<double> &x) override {
        for (const auto& shape : shapes) {
            if (shape->inside(x)) return true;
        }
        return false;
    }
};

class difference_t : public geoshape_t {
public:
    vector<gsp_t> shapes;
    void bbox(vector<double> &bb) override {
        if (shapes.empty()) return;
        shapes[0]->bbox(bb); 
    }
    bool inside(const vector<double> &x) override {
        if (shapes.empty() || !shapes[0]->inside(x)) return false;
        for (size_t i = 1; i < shapes.size(); ++i) {
            if (shapes[i]->inside(x)) return false;
        }
        return true;
    }
};

// HDF5 helpers using C++ API
vector<double> read_h5_dataset(const H5File& file, const string& dataset_name, size_t expected_size) {
    DataSet dataset = file.openDataSet(dataset_name);
    vector<double> data(expected_size);
    dataset.read(data.data(), PredType::NATIVE_DOUBLE);
    return data;
}

void write_h5_dataset(H5File& file, const string& dataset_name, const vector<double>& data) {
    hsize_t dims[1] = {data.size()};
    DataSpace dataspace(1, dims);
    DataSet dataset = file.createDataSet(dataset_name, PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(data.data(), PredType::NATIVE_DOUBLE);
}

void verify_error(const string& name, const vector<double>& calc, const vector<double>& ref) {
    if (calc.size() != ref.size()) {
        cerr << "Size mismatch for " << name << endl;
        return;
    }
    size_t N = calc.size();
    double sum_abs_err = 0;
    double sum_calc = 0, sum_ref = 0;
    double sum_sq_calc = 0, sum_sq_ref = 0;
    double sum_sqrt_calc = 0, sum_sqrt_ref = 0;

    for (size_t i = 0; i < N; ++i) {
        sum_abs_err += abs(calc[i] - ref[i]);
        
        sum_calc += calc[i];
        sum_ref += ref[i];
        
        sum_sq_calc += calc[i] * calc[i];
        sum_sq_ref += ref[i] * ref[i];
        
        sum_sqrt_calc += sqrt(abs(calc[i]));
        sum_sqrt_ref += sqrt(abs(ref[i]));
    }

    double mae = sum_abs_err / N;
    
    double mean_calc = sum_calc / N;
    double mean_ref = sum_ref / N;
    
    double mean_sq_calc = sum_sq_calc / N;
    double mean_sq_ref = sum_sq_ref / N;
    
    double mean_sqrt_calc = sum_sqrt_calc / N;
    double mean_sqrt_ref = sum_sqrt_ref / N;

    cout << "Estadísticas de error para " << name << ":\n";
    cout << "  Error Absoluto Medio: " << mae << "\n";
    cout << "  Media: calc = " << mean_calc << " | ref = " << mean_ref << "\n";
    cout << "  Media de los Cuadrados: calc = " << mean_sq_calc << " | ref = " << mean_sq_ref << "\n";
    cout << "  Media de las Raíces Cuadradas: calc = " << mean_sqrt_calc << " | ref = " << mean_sqrt_ref << "\n\n";
}

int main(int argc, char** argv) {
    string input_file = "input1000.h5";
    string ref_file = "outputref1000.h5";
    if (argc >= 3) {
        input_file = argv[1];
        ref_file = argv[2];
    }
    cout << "Leyendo de " << input_file << "...\n";

    H5File in_file(input_file, H5F_ACC_RDONLY);

    DataSet dset = in_file.openDataSet("R1");
    DataSpace dspace = dset.getSpace();
    int ndims = dspace.getSimpleExtentNdims();
    vector<hsize_t> dims(ndims);
    dspace.getSimpleExtentDims(dims.data(), NULL);
    
    size_t ncases = 1;
    for (int i=0; i<ndims; ++i) ncases *= dims[i];

    vector<double> R1 = read_h5_dataset(in_file, "R1", ncases);
    vector<double> R2 = read_h5_dataset(in_file, "R2", ncases);
    vector<double> x1 = read_h5_dataset(in_file, "x1", ncases * 2);
    vector<double> x2 = read_h5_dataset(in_file, "x2", ncases * 2);
    in_file.close();

    vector<double> areaA(ncases), areaB(ncases), diffAB(ncases), diffBA(ncases), intsAB(ncases), unionAB(ncases);

    cout << "Procesando " << ncases << " casos...\n";
    for (size_t i = 0; i < ncases; ++i) {
        auto A = make_shared<circle_t>(x1[2*i], x1[2*i+1], R1[i]);
        auto B = make_shared<circle_t>(x2[2*i], x2[2*i+1], R2[i]);
        
        areaA[i] = area(*A, 100);
        areaB[i] = area(*B, 100);
        
        auto u = make_shared<union_t>();
        u->shapes.push_back(A);
        u->shapes.push_back(B);
        unionAB[i] = area(*u, 100);
        
        auto inter = make_shared<intersection_t>();
        inter->shapes.push_back(A);
        inter->shapes.push_back(B);
        intsAB[i] = area(*inter, 100);
        
        auto dAB = make_shared<difference_t>();
        dAB->shapes.push_back(A);
        dAB->shapes.push_back(B);
        diffAB[i] = area(*dAB, 100);
        
        auto dBA = make_shared<difference_t>();
        dBA->shapes.push_back(B);
        dBA->shapes.push_back(A);
        diffBA[i] = area(*dBA, 100);
    }

    string output_file = "output.h5";
    H5File out_file(output_file, H5F_ACC_TRUNC);
    
    write_h5_dataset(out_file, "areaA", areaA);
    write_h5_dataset(out_file, "areaB", areaB);
    write_h5_dataset(out_file, "diffAB", diffAB);
    write_h5_dataset(out_file, "diffBA", diffBA);
    write_h5_dataset(out_file, "intsAB", intsAB);
    write_h5_dataset(out_file, "unionAB", unionAB);
    out_file.close();
    
    cout << "Resultados guardados en " << output_file << ".\n\n";

    // Verification
    cout << "Verificando contra " << ref_file << "...\n";
    H5File ref_in(ref_file, H5F_ACC_RDONLY);
    
    vector<double> r_areaA = read_h5_dataset(ref_in, "areaA", ncases);
    vector<double> r_areaB = read_h5_dataset(ref_in, "areaB", ncases);
    vector<double> r_diffAB = read_h5_dataset(ref_in, "diffAB", ncases);
    vector<double> r_diffBA = read_h5_dataset(ref_in, "diffBA", ncases);
    vector<double> r_intsAB = read_h5_dataset(ref_in, "intsAB", ncases);
    vector<double> r_unionAB = read_h5_dataset(ref_in, "unionAB", ncases);
    ref_in.close();

    verify_error("areaA", areaA, r_areaA);
    verify_error("areaB", areaB, r_areaB);
    verify_error("diffAB", diffAB, r_diffAB);
    verify_error("diffBA", diffBA, r_diffBA);
    verify_error("intsAB", intsAB, r_intsAB);
    verify_error("unionAB", unionAB, r_unionAB);

    return 0;
}