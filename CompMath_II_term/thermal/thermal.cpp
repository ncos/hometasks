#include <stdio.h>
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
        assert(valn <= 1.1 && valn >= 0);
        
        if (valn < m) {
            r = 0;
            g = 255 * valn;
            b = 255 - g;
        }
        else {
            r = 255 * valn;
            g = 255 - r;
            b = 0;
        }
        //std::cout<<int(r) <<" "<<int(g)<<" "<<int(b)<<"\n";

    }

    unsigned char get_r() {
        return r * 255 / (r + g + b);    
    }

    unsigned char get_g() {
        return g * 255 / (r + g + b);    
    }

    unsigned char get_b() {
        return b * 255 / (r + g + b);    
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
    double* data;
    int M, N;
public:
    Data (){}

    ~Data(){
        free(data);
    }

    void init(int m, int n){
        M = m;
        N = n;
        data = (double*) calloc(m*n, sizeof(double));
    }

    void set(int m, int n, double x){
        *(data + N*m + n) = x;
    }

    double get(int m, int n){
        if ((m >= M) || (n >= N)) return 0;
        return *(data + N*m + n);
    }
    int getm(){
        return M;
    }
    int getn(){
        return N;
    }
    double* getdata(){
        return data;
    }
    void adddata(double* d, int dataM){
        data =(double*) realloc(data, (M+dataM)*N*sizeof(double));
        for (int i = 0; i < dataM*N; i++)
            *(data + M*N + i) = d[N + i];
    }

    double* get_addr_for_add(int to_add){
        data = (double*)  realloc(data, (M + to_add - 1)*N*sizeof(double));
        double* ptr = (data + (M-1)*(N) );
        M += to_add - 1;
        return ptr;
    }

    void print(){
        for (int n = 0; n < N; n++){
            for (int m = 0; m < M; m++ )
                printf("%2lg ", this->get(m, n));
            printf("\n");
        }
    }

    void print_result(){
        printf("Result:\n");
        for (int m = 0; m < M; m++)
            printf("%2lg ", this->get(m, N-1));
        printf("\n");
    }
};

double f_right(int m, int n) {
    return 100;
}

double alpha(int m, int n) {
    return 10;
}

double scheme(int m, int n, double h, double t, class Data& data){
    return ( f_right(m, n) + alpha(m, n) * (data.get(m+1, n) - 2*data.get(m, n) + data.get(m - 1, n)) / (h*h) )*t + data.get(m, n);
    //return data.get(m, n) - t*(alpha(m, n) * (data.get(m, n) - data.get(m - 1, n))/h - f_right(m, n));
}

static double f0(double x) {
    return exp(-pow((x-0.5)*10, 2.0));
};


int main(int argc, char **argv){
    double XMIN = 0.0, XMAX = 1.0, TMIN = 0.0, TMAX = 1.0;
    int height = 200000;
    int width = 640;
    double dh = double(XMAX - XMIN)/double(width), dt = double(TMAX - TMIN)/double(height);
    std::cout << "dh: " << dh << " dt: " << dt << "\n";

    Data tab;
    tab.init(width, height);
        
    for (int m = 0; m < width; ++m) {
        tab.set(m, 0, f0(m * dh));
    }

    for (int n = 0; n < height - 1; ++n) {
        for (int m = 1; m < width - 1; ++m) {
            tab.set(m, n+1, scheme(m, n, dh, dt, tab));
        }
    }



    //tab.print();


    if (true) {
        int im_wdth = width;
        int im_hght = width;

        double *image = new double[im_wdth*im_hght];
        for (int n = 0; n < im_hght; n++){
            for (int m = 0; m < im_wdth; m++ )
                image[n*im_wdth + m] = tab.get(m*width/im_wdth, n*height/im_hght);
        }

        std::cout << "Max: " << max(image, im_wdth, im_hght) << " " 
                  << "Avg: " << avg(image, im_wdth, im_hght) << " " 
                  << "Min: " << min(image, im_wdth, im_hght) << "\n\n";

        writePPM(image, im_wdth, im_hght, "thermal.ppm");
    };

    return 0;
}
