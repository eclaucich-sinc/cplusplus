#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <memory>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class geoshape_t {
    public:
        virtual ~geoshape_t() = default;
        virtual void bbox(vector<double> &bb)=0;
        virtual bool inside(const vector<double> &x)=0;
};

typedef shared_ptr<geoshape_t> gsp_t;

double area(geoshape_t &g, int N = 100) {
    vector<double> bb;
    g.bbox(bb);
    
    // Prevenir cajas inválidas
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

class square_t : public geoshape_t {
    double x_min, y_min, x_max, y_max;
public:
    square_t(double xmin, double ymin, double xmax, double ymax) 
        : x_min(xmin), y_min(ymin), x_max(xmax), y_max(ymax) {}
        
    void bbox(vector<double> &bb) override {
        bb = {x_min, y_min, x_max, y_max};
    }
    
    bool inside(const vector<double> &x) override {
        return (x[0] >= x_min && x[0] <= x_max && x[1] >= y_min && x[1] <= y_max);
    }
};

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

class ellipse_t : public geoshape_t {
    double cx, cy, rx, ry;
public:
    ellipse_t(double cx, double cy, double rx, double ry) 
        : cx(cx), cy(cy), rx(rx), ry(ry) {}
        
    void bbox(vector<double> &bb) override {
        bb = {cx - rx, cy - ry, cx + rx, cy + ry};
    }
    
    bool inside(const vector<double> &x) override {
        return pow((x[0] - cx) / rx, 2) + pow((x[1] - cy) / ry, 2) <= 1.0;
    }
};

class convex_polygon_t : public geoshape_t {
    vector<double> pts;
public:
    convex_polygon_t(const vector<double> &xj) : pts(xj) {}
    
    void bbox(vector<double> &bb) override {
        if (pts.empty()) return;
        bb = {pts[0], pts[1], pts[0], pts[1]};
        for (size_t i = 2; i < pts.size(); i += 2) { 
            if (pts[i] < bb[0]) bb[0] = pts[i];
            if (pts[i] > bb[2]) bb[2] = pts[i];
            if (pts[i+1] < bb[1]) bb[1] = pts[i+1];
            if (pts[i+1] > bb[3]) bb[3] = pts[i+1];
        }
    }
    
    bool inside(const vector<double> &x) override {
        if (pts.size() < 6) return false; 
        int sign = 0;
        size_t n = pts.size() / 2;
        for (size_t i = 0; i < n; ++i) {
            double x1 = pts[2*i];
            double y1 = pts[2*i + 1];
            double x2 = pts[(2*i + 2) % (2*n)];
            double y2 = pts[(2*i + 3) % (2*n)];
            
            double dx = x2 - x1;
            double dy = y2 - y1;
            double cross = dx * (x[1] - y1) - dy * (x[0] - x1);
            
            if (cross != 0) {
                int param_sign = (cross > 0) ? 1 : -1;
                if (sign == 0) sign = param_sign;
                else if (sign != param_sign) return false; 
            }
        }
        return true;
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

gsp_t parse_shape(const json& j) {
    if (!j.contains("type")) return nullptr;
    string type = j["type"];
    if (type == "circle") {
        return make_shared<circle_t>(j["xc"], j["yc"], j["R"]);
    } else if (type == "square") {
        double xc = j["xc"];
        double yc = j["yc"];
        double L = j["L"];
        return make_shared<square_t>(xc - L/2.0, yc - L/2.0, xc + L/2.0, yc + L/2.0);
    } else if (type == "ellipse") {
        return make_shared<ellipse_t>(j["cx"], j["cy"], j["rx"], j["ry"]);
    } else if (type == "convex_polygon") {
        vector<double> pts = j["pts"].get<vector<double>>();
        return make_shared<convex_polygon_t>(pts);
    } else if (type == "union") {
        auto u = make_shared<union_t>();
        if (j.contains("A")) u->shapes.push_back(parse_shape(j["A"]));
        if (j.contains("B")) u->shapes.push_back(parse_shape(j["B"]));
        return u;
    } else if (type == "intersection") {
        auto i = make_shared<intersection_t>();
        if (j.contains("A")) i->shapes.push_back(parse_shape(j["A"]));
        if (j.contains("B")) i->shapes.push_back(parse_shape(j["B"]));
        return i;
    } else if (type == "difference") {
        auto d = make_shared<difference_t>();
        if (j.contains("A")) d->shapes.push_back(parse_shape(j["A"]));
        if (j.contains("B")) d->shapes.push_back(parse_shape(j["B"]));
        return d;
    }
    return nullptr;
}

json checkgeo(json &in, int N=100) {
    vector<gsp_t> shapes;
    for (auto& item : in) {
        shapes.push_back(parse_shape(item));
    }

    json out = json::array();
    
    int n = shapes.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            json pair_res;
            pair_res["idA"] = i;
            pair_res["idB"] = j;
            pair_res["typeA"] = typeid(*shapes[i]).name();
            pair_res["typeB"] = typeid(*shapes[j]).name();
            
            double a_val = area(*shapes[i], N);
            double b_val = area(*shapes[j], N);
            pair_res["A"] = a_val;
            pair_res["B"] = b_val;
            
            // uniAB
            union_t uni;
            uni.shapes.push_back(shapes[i]);
            uni.shapes.push_back(shapes[j]);
            double uniAB = area(uni, N);
            pair_res["uniAB"] = uniAB;
            
            // intsAB
            intersection_t ints;
            ints.shapes.push_back(shapes[i]);
            ints.shapes.push_back(shapes[j]);
            double intsAB = area(ints, N);
            pair_res["intsAB"] = intsAB;
            
            // diffAB (A - B)
            difference_t diffAB_shape;
            diffAB_shape.shapes.push_back(shapes[i]);
            diffAB_shape.shapes.push_back(shapes[j]);
            double diffAB = area(diffAB_shape, N);
            pair_res["diffAB"] = diffAB;
            
            // diffBA (B - A)
            difference_t diffBA_shape;
            diffBA_shape.shapes.push_back(shapes[j]); // Primero B
            diffBA_shape.shapes.push_back(shapes[i]); // Luego A
            double diffBA = area(diffBA_shape, N);
            pair_res["diffBA"] = diffBA;
            
            // Error
            double err = std::abs(uniAB - (diffAB + diffBA + intsAB));
            pair_res["error"] = err;
            
            out.push_back(pair_res);
        }
    }
    
    return out;
}

int main(int argc, char* argv[]) {
    string filename = "input.json";
    if (argc > 1) filename = argv[1];

    ifstream ifs(filename);
    if (!ifs.is_open()) {
        cerr << "Error: No se pudo abrir " << filename << endl;
        return 1;
    }

    json in_json;
    ifs >> in_json;
    
    json out_json = checkgeo(in_json, 100);

    cout << out_json.dump(4) << endl;

    ofstream ofs("output_shared.json");
    if (ofs.is_open()) {
        ofs << out_json.dump(4) << endl;
        ofs.close();
        cout << "\n[Info] Resultados guardados exitosamente en 'output_shared.json'." << endl;
    } else {
        cerr << "\n[Error] No se pudo crear el archivo 'output_shared.json'." << endl;
    }

    return 0;
}
