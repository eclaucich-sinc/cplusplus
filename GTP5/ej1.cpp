#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <typeinfo>

using namespace std;

class geoshape_t {
    public:
        virtual ~geoshape_t() = default;
        // Bounding box
        virtual void bbox(vector<double> &bb)=0;

        // Is x inside shape?
        virtual bool inside(const vector<double> &x)=0;
};

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
    vector<geoshape_t*> shapes;
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
        for (auto* shape : shapes) {
            if (!shape->inside(x)) return false;
        }
        return true;
    }
};

class union_t : public geoshape_t {
public:
    vector<geoshape_t*> shapes;
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
        for (auto* shape : shapes) {
            if (shape->inside(x)) return true;
        }
        return false;
    }
};

class difference_t : public geoshape_t {
public:
    vector<geoshape_t*> shapes;
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

    // 1. Verificar Cuadrado (Lado L=2, centrado en el origen)
    square_t sq(-1.0, -1.0, 1.0, 1.0);
    double sq_area_exact = 2.0 * 2.0;
    double sq_inertia_exact = (2.0 * pow(2.0, 3) / 12.0) + (2.0 * pow(2.0, 3) / 12.0); // I_x + I_y (Inercia)
    
    cout << "--- Cuadrado (L=2) ---" << endl;
    label(sq);
    cout << "Area (Num / Exacta): " << area(sq) << " / " << sq_area_exact << endl;
    vector<double> cg_sq = grav_center(sq);
    cout << "Centro de Gravedad: (" << cg_sq[0] << ", " << cg_sq[1] << ") / Exacto: (0.0000, 0.0000)" << endl;
    cout << "Inercia (Num / Exacta): " << inertia(sq) << " / " << sq_inertia_exact << endl << endl;

    // 2. Verificar Círculo (Radio R=2, centrado en el origen)
    circle_t circ(0.0, 0.0, 2.0);
    double pi = acos(-1.0);
    double circ_area_exact = pi * pow(2.0, 2);
    double circ_inertia_exact = pi * pow(2.0, 4) / 2.0;

    cout << "--- Circulo (R=2) ---" << endl;
    label(circ);
    cout << "Area (Num / Exacta): " << area(circ) << " / " << circ_area_exact << endl;
    vector<double> cg_circ = grav_center(circ);
    cout << "Centro de Gravedad: (" << cg_circ[0] << ", " << cg_circ[1] << ") / Exacto: (0.0000, 0.0000)" << endl;
    cout << "Inercia (Num / Exacta): " << inertia(circ) << " / " << circ_inertia_exact << endl << endl;

    // 3. Verificar Elipse (rx=2, ry=1, centrada en el origen)
    ellipse_t el(0.0, 0.0, 2.0, 1.0);
    double el_area_exact = pi * 2.0 * 1.0;
    // Inercia de la elipse = I_x + I_y = (pi * rx * ry^3 / 4) + (pi * ry * rx^3 / 4)
    double el_inertia_exact = (pi * 2.0 * pow(1.0, 3) / 4.0) + (pi * 1.0 * pow(2.0, 3) / 4.0);

    cout << "--- Elipse (rx=2, ry=1) ---" << endl;
    label(el);
    cout << "Area (Num / Exacta): " << area(el) << " / " << el_area_exact << endl;
    vector<double> cg_el = grav_center(el);
    cout << "Centro de Gravedad: (" << cg_el[0] << ", " << cg_el[1] << ") / Exacto: (0.0000, 0.0000)" << endl;
    cout << "Inercia (Num / Exacta): " << inertia(el) << " / " << el_inertia_exact << endl << endl;

    // 4. Pentágono regular (R=2, centrado en el origen)
    vector<double> pent_pts;
    double R = 2.0;
    for (int i = 0; i < 5; ++i) { // 5 vértices espaciados 72 grados
        pent_pts.push_back(R * cos(i * 2.0 * pi / 5.0));
        pent_pts.push_back(R * sin(i * 2.0 * pi / 5.0));
    }
    convex_polygon_t pent(pent_pts);
    
    // Fórmulas exactas para un polígono regular de N lados circunscrito en un círculo de radio R
    double pent_area_exact = (5.0 / 2.0) * pow(R, 2) * sin(2.0 * pi / 5.0);
    double pent_inertia_exact = pent_area_exact * pow(R, 2) / 6.0 * (2.0 + cos(2.0 * pi / 5.0));

    cout << "\n--- Pentagono Regular (R=2) ---" << endl;
    label(pent);
    cout << "Area (Num / Exacta): " << area(pent) << " / " << pent_area_exact << endl;
    vector<double> cg_pent = grav_center(pent);
    cout << "Centro de Gravedad: (" << cg_pent[0] << ", " << cg_pent[1] << ") / Exacto: (0.0000, 0.0000)" << endl;
    cout << "Inercia (Num / Exacta): " << inertia(pent) << " / " << pent_inertia_exact << endl;

    // 5. Operaciones booleanas
    square_t bsq(-2.0, -2.0, 2.0, 2.0); // Cuadrado de 4x4 centrado en el origen
    circle_t bcirc(0.0, 0.0, 1.0);      // Círculo de R=1 centrado en el origen (totalmente inscrito en el cuadrado)

    intersection_t inter;
    inter.shapes.push_back(&bsq);
    inter.shapes.push_back(&bcirc);
    
    union_t uni;
    uni.shapes.push_back(&bsq);
    uni.shapes.push_back(&bcirc);

    difference_t diff;
    diff.shapes.push_back(&bsq); // A este le restamos los siguientes
    diff.shapes.push_back(&bcirc);

    cout << "\n--- Interseccion (Circulo inscrito en Cuadrado) ---" << endl;
    label(inter);
    cout << "Area (Num / Exacta): " << area(inter) << " / " << (pi * 1.0) << " (Area del Circulo)" << endl;

    cout << "\n--- Union (Circulo inscrito en Cuadrado) ---" << endl;
    label(uni);
    cout << "Area (Num / Exacta): " << area(uni) << " / " << 16.0 << " (Area del Cuadrado)" << endl;

    cout << "\n--- Diferencia (Cuadrado menos Circulo) ---" << endl;
    label(diff);
    cout << "Area (Num / Exacta): " << area(diff) << " / " << (16.0 - pi * 1.0) << endl;

    return 0;
}
