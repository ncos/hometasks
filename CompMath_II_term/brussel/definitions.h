#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <math.h>

using std::cout;
using std::endl;



class Color {
public:
    unsigned char r, g, b;
    double h, m, min;

    Color(double _max, double _median, double _min){
        r = 255;
        g = 255;
        b = 255;
        
        h = _max - _min;
        m = (_median - _min)/h;
        min = _min;
    };


    void set(double val) {
        double valn = (val - min)/h;
        //assert(valn >= 0);
        if (val < 0) val = -val;

        if (valn < m) {
            r = 0;
            g = 255 * valn;
            //b = 255 - g;
            b = 1;
        }
        else {
            r = 255 * valn;
            g = 255 - r;
            b = 0;
        }
        //std::cout<<int(r) <<" "<<int(g)<<" "<<int(b)<<"\n";

    }

    unsigned char get_r() {
        return r;// * 255 / (r + g + b);    
    }

    unsigned char get_g() {
        return g;// * 255 / (r + g + b);    
    }

    unsigned char get_b() {
        return b;// * 255 / (r + g + b);    
    }
};



static double min(double *image, int width, int height) {
    double m = image[0];
    for (int i = 0; i < width*height; ++i) {
        if (image[i] < m) m = image[i];
    }
    return m;
};

static double max(double *image, int width, int height) {
    double m = image[0];
    for (int i = 0; i < width*height; ++i) {
        if (image[i] > m) m = image[i];
    }
    return m;
};

static double avg(double *image, int width, int height) {
    double m = 0;
    int cnt = 0;
    for (int i = 0; i < width*height; ++i) {
        if (image[i] > 0) cnt++;
        m += image[i];
    }
    return m/cnt;
};



static void
writePPM(double *buf, int width, int height, const char *fn) {
    Color c(max(buf, width, height),
            avg(buf, width, height),
            min(buf, width, height));

    FILE *fp = fopen(fn, "wb");
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int i = 0; i < width*height; ++i) {
        c.set(buf[i]);
        fputc(c.get_r(), fp);
        fputc(c.get_g(), fp);
        fputc(c.get_b(), fp);
    }
    fclose(fp);
    printf("Wrote image file %s\n", fn);
};

class Data {
private:
    double* data_u;
    double* data_v;
    int M, N;

public:
    double dh, dt;

    Data (){}

    ~Data(){
        free(data_u);
        free(data_v);
    }

    void init(int m, int n, double _dh, double _dt){
        M = m;
        N = n;
        dh = _dh;
        dt = _dt;
        data_u = (double*) calloc(m*n, sizeof(double));
        data_v = (double*) calloc(m*n, sizeof(double));
    }

    void set_u(int m, int n, double x){
        *(data_u + N*m + n) = x;
    }

    void set_v(int m, int n, double x){
        *(data_v + N*m + n) = x;
    }

    double get_x(int m) {
        return (double)m*dh;
    }

    double get_u(int m, int n){
        if ((m >= M) || (n >= N)) return 0;
        return *(data_u + N*m + n);
    }

    double get_v(int m, int n){
        if ((m >= M) || (n >= N)) return 0;
        return *(data_v + N*m + n);
    }

    int getm(){
        return M;
    }

    int getn(){
        return N;
    }
};


