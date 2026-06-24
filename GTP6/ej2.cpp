#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <memory>

using namespace std;

class geoshape_t {
    public:
        virtual ~geoshape_t() = default;
        // Bounding box
        virtual void bbox(vector<double> &bb)=0;

        // Is x inside shape?
        virtual bool inside(const vector<double> &x)=0;
};

typedef unique_ptr<geoshape_t> gsp_t;

void label(geoshape_t &g) {
    cout << "Nombre del objeto (RTTI): " << typeid(g).name() << endl;
}

// Polimorfismo en acción: Estas funciones genéricas no saben qué figura están
// recibiendo, solo saben que responde a bbox() y a inside(). 
// Podemos usar integración numérica (grilla 2D) para calcular las propiedades.

double area(geoshape_t &g) {
    vector<double> bb;
    g.bbox(bb);
    
    int N = 500; // Resolución de nuestra grilla de integración
    double dx = (bb[2] - bb[0]) / N;
    double dy = (bb[3] - bb[1]) / N;
    
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

vector<double> grav_center(geoshape_t &g) {
    vector<double> bb;
    g.bbox(bb);
    
    int N = 500;
    double dx = (bb[2] - bb[0]) / N;
    double dy = (bb[3] - bb[1]) / N;
    
    double a = 0.0, cx = 0.0, cy = 0.0;
    vector<double> pt(2);
    for (int i = 0; i < N; ++i) {
        pt[0] = bb[0] + (i + 0.5) * dx;
        for (int j = 0; j < N; ++j) {
            pt[1] = bb[1] + (j + 0.5) * dy;
            if (g.inside(pt)) {
                double dA = dx * dy;
                a += dA;
                cx += pt[0] * dA;
                cy += pt[1] * dA;
            }
        }
    }
    return {cx / a, cy / a};
}

double inertia(geoshape_t &g) {
    vector<double> cg = grav_center(g);
    vector<double> bb;
    g.bbox(bb);
    
    int N = 500;
    double dx = (bb[2] - bb[0]) / N;
    double dy = (bb[3] - bb[1]) / N;
    
    double I = 0.0;
    vector<double> pt(2);
    for (int i = 0; i < N; ++i) {
        pt[0] = bb[0] + (i + 0.5) * dx;
        for (int j = 0; j < N; ++j) {
            pt[1] = bb[1] + (j + 0.5) * dy;
            if (g.inside(pt)) {
                double r2 = pow(pt[0] - cg[0], 2) + pow(pt[1] - cg[1], 2);
                I += r2 * (dx * dy);
            }
        }
    }
    return I;
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
        for (size_t i = 2; i < pts.size(); i += 2) { // Iterar sobre los puntos x, y
            if (pts[i] < bb[0]) bb[0] = pts[i];
            if (pts[i] > bb[2]) bb[2] = pts[i];
            if (pts[i+1] < bb[1]) bb[1] = pts[i+1];
            if (pts[i+1] > bb[3]) bb[3] = pts[i+1];
        }
    }
    
    bool inside(const vector<double> &x) override {
        if (pts.size() < 6) return false; // Todo polígono debe tener al menos 3 vértices
        int sign = 0;
        size_t n = pts.size() / 2;
        // Comprobar hacia qué lado de todas las aristas dirigidas cae el punto mediante un producto cruz
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
                else if (sign != param_sign) return false; // El punto no está en el mismo semiplano
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
        shapes[0]->bbox(bb); // La caja nunca será más grande que la primera figura
    }
    bool inside(const vector<double> &x) override {
        if (shapes.empty() || !shapes[0]->inside(x)) return false;
        for (size_t i = 1; i < shapes.size(); ++i) {
            if (shapes[i]->inside(x)) return false;
        }
        return true;
    }
};

int main() {
    cout << fixed << setprecision(4);

    cout << "\n--- Pruebas de Operaciones Booleanas con Circulos ---" << endl;
    circle_t C1(0.0, 0.0, 1.0);
    circle_t C2(1.0, 1.0, 1.0);

    cout << "Area(C1) (Num / Exacta): " << area(C1) << " / " << 3.14159265 << endl;
    cout << "Area(C2) (Num / Exacta): " << area(C2) << " / " << 3.14159265 << endl;

    intersection_t int_c1_c1;
    int_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    int_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    cout << "Area(intersection(C1, C1)) (Num / Exacta): " << area(int_c1_c1) << " / " << 3.14159265 << endl;

    union_t uni_c1_c1;
    uni_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    uni_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    cout << "Area(union(C1, C1)) (Num / Exacta): " << area(uni_c1_c1) << " / " << 3.14159265 << endl;

    difference_t diff_c1_c1;
    diff_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    diff_c1_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    cout << "Area(difference(C1, C1)) (Num / Exacta): " << area(diff_c1_c1) << " / " << 0.0 << endl;

    intersection_t int_c1_c2;
    int_c1_c2.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    int_c1_c2.shapes.push_back(make_unique<circle_t>(1.0, 1.0, 1.0));
    cout << "Area(intersection(C1, C2)) (Num / Exacta): " << area(int_c1_c2) << " / " << (3.14159265 / 2.0 - 1.0) << endl;

    union_t uni_c1_c2;
    uni_c1_c2.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    uni_c1_c2.shapes.push_back(make_unique<circle_t>(1.0, 1.0, 1.0));
    cout << "Area(union(C1, C2)) (Num / Exacta): " << area(uni_c1_c2) << " / " << (3.0 * 3.14159265 / 2.0 + 1.0) << endl;

    difference_t diff_c1_c2;
    diff_c1_c2.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    diff_c1_c2.shapes.push_back(make_unique<circle_t>(1.0, 1.0, 1.0));
    cout << "Area(difference(C1, C2)) (Num / Exacta): " << area(diff_c1_c2) << " / " << (3.14159265 / 2.0 + 1.0) << endl;

    difference_t diff_c2_c1;
    diff_c2_c1.shapes.push_back(make_unique<circle_t>(1.0, 1.0, 1.0));
    diff_c2_c1.shapes.push_back(make_unique<circle_t>(0.0, 0.0, 1.0));
    cout << "Area(difference(C2, C1)) (Num / Exacta): " << area(diff_c2_c1) << " / " << (3.14159265 / 2.0 + 1.0) << endl;

    // 6. Relacion de conjuntos
    cout << "\n--- Verificacion Relacion de Conjuntos ---" << endl;
    // A = circle_t(0.3, 0.3, 2.0), B = circle_t(1.0, 0.7, 3.0)
    
    union_t uni_A_B;
    uni_A_B.shapes.push_back(make_unique<circle_t>(0.3, 0.3, 2.0));
    uni_A_B.shapes.push_back(make_unique<circle_t>(1.0, 0.7, 3.0));
    double area_uni_A_B = area(uni_A_B);

    difference_t diff_A_B;
    diff_A_B.shapes.push_back(make_unique<circle_t>(0.3, 0.3, 2.0));
    diff_A_B.shapes.push_back(make_unique<circle_t>(1.0, 0.7, 3.0));
    double area_diff_A_B = area(diff_A_B);

    difference_t diff_B_A;
    diff_B_A.shapes.push_back(make_unique<circle_t>(1.0, 0.7, 3.0));
    diff_B_A.shapes.push_back(make_unique<circle_t>(0.3, 0.3, 2.0));
    double area_diff_B_A = area(diff_B_A);

    intersection_t int_A_B;
    int_A_B.shapes.push_back(make_unique<circle_t>(0.3, 0.3, 2.0));
    int_A_B.shapes.push_back(make_unique<circle_t>(1.0, 0.7, 3.0));
    double area_int_A_B = area(int_A_B);

    cout << "Area(A U B) = " << area_uni_A_B << endl;
    cout << "Area(A-B) + Area(B-A) + Area(A int B) = " 
         << area_diff_A_B << " + " << area_diff_B_A << " + " << area_int_A_B 
         << " = " << (area_diff_A_B + area_diff_B_A + area_int_A_B) << endl;

    return 0;
}
