#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <math.h>
#include "definitions.h"

using std::cout;
using std::endl;

double A = 1.0;
double B = 1.7;
double D1 = 0.0;
double D2 = 0.0;
double eps = 1;
double w = 20;
double XMIN = 0;
double XMAX = 1;
double TMIN = 0;
double TMAX = 20;


void apply_borders_u(Data& data) {
    int M = data.getm();
    int N = data.getn();
        
    for (int i = 0; i < M; ++i) {
        double x = data.get_x(i);
        data.set_u(i, 0, A*(1+eps*cos(w*x)));        
    };
     
    for (int j = 0; j < N; ++j) {
        data.set_u(1, j, data.get_u(0, j));
        data.set_u(M - 2, j, data.get_u(M - 1, j));
    }
}   

void apply_borders_v(Data& data) {
    int M = data.getm();
    int N = data.getn();
        
    for (int i = 0; i < M; ++i) {
        data.set_v(i, 0, A/B);        
    };
     
    for (int j = 0; j < N; ++j) {
        data.set_v(1, j, data.get_v(0, j));
        data.set_v(M - 2, j, data.get_v(M - 1, j));
    }
}   

double scheme_u(int m, int n, Data& data) {
    double dh = data.dh;
    double dt = data.dt;
    
    double rght_part = A-(B+1)*data.get_u(m, n)+pow(data.get_u(m, n), 2.0)*data.get_v(m, n);
    return (rght_part + D1*(data.get_u(m+1, n) - 2*data.get_u(m, n) + data.get_u(m - 1, n)) / (dh*dh))*dt + data.get_u(m, n); 
}

double scheme_v(int m, int n, Data& data) {
    double dh = data.dh;
    double dt = data.dt;
    
    double rght_part = B*data.get_u(m, n) - pow(data.get_u(m, n), 2.0)*data.get_v(m, n);
    return (rght_part + D2*(data.get_v(m+1, n) - 2*data.get_v(m, n) + data.get_v(m - 1, n)) / (dh*dh))*dt + data.get_v(m, n); 
}


int main(int argc, char **argv){
    int width = 100;
    double dh = double(XMAX - XMIN)/double(width);
    double gamma = (D2 == 0) ? 1 : D2;
    double dt = 2*dh*dh/(gamma*40);
    int height = ceil(double(TMAX - TMIN)/dt);
    std::cout << "dh: " << dh << " dt: " << dt << "\n";

    //double mew = A*sqrt(D1/D2);
    double mew = A;
    std::cout << "mew = " << mew << "\n\n";

    B = 1 + pow(mew, 2.0) - 0.5;

    Data tab;
    tab.init(width, height, dh, dt);
    apply_borders_u(tab);
    apply_borders_v(tab);

    for (int n = 0; n < height - 1; ++n) {
        for (int m = 1; m < width - 1; ++m) {
            tab.set_u(m, n+1, scheme_u(m, n, tab));
            tab.set_v(m, n+1, scheme_v(m, n, tab));
        }
        tab.set_u(0, n+1, tab.get_u(1, n+1));
        tab.set_u(width - 1, n+1, tab.get_u(width - 2, n+1));
        tab.set_v(0, n+1, tab.get_v(1, n+1));
        tab.set_v(width - 1, n+1, tab.get_v(width - 2, n+1));
    }

    int im_wdth = width;
    int im_hght = width;

    double *image_u = new double[im_wdth*im_hght];
    double *image_v = new double[im_wdth*im_hght];
    for (int n = 0; n < im_hght; n++){
        for (int m = 0; m < im_wdth; ++m) {
            image_u[n*im_wdth + m] = tab.get_u(m*width/im_wdth, n*height/im_hght);
            image_v[n*im_wdth + m] = tab.get_v(m*width/im_wdth, n*height/im_hght);
            //printf("%f\t", image_u[n*im_wdth + m]);
        }
        //printf("\n");
    }

    std::cout << "Max(u): " << max(image_u, im_wdth, im_hght) << " " 
              << "Avg(u): " << avg(image_u, im_wdth, im_hght) << " " 
              << "Min(u): " << min(image_u, im_wdth, im_hght) << "\n\n";

    std::cout << "Max(v): " << max(image_v, im_wdth, im_hght) << " " 
              << "Avg(v): " << avg(image_v, im_wdth, im_hght) << " " 
              << "Min(v): " << min(image_v, im_wdth, im_hght) << "\n\n";

    writePPM(image_u, im_wdth, im_hght, "brussel_u.ppm");
    writePPM(image_v, im_wdth, im_hght, "brussel_v.ppm");



    double max_u = 0, max_v = 0;
    double min_u = 255, min_v = 255;
    for (int m = 0; m < width; ++m) {
        if (max_u < tab.get_u(m, height - 1)) max_u = tab.get_u(m, height - 1);
        if (max_v < tab.get_v(m, height - 1)) max_v = tab.get_v(m, height - 1);
        if (min_u > tab.get_u(m, height - 1)) min_u = tab.get_u(m, height - 1);
        if (min_v > tab.get_v(m, height - 1)) min_v = tab.get_v(m, height - 1);
    }


    printf("max_u = %f\n", max_u);
    printf("max_v = %f\n", max_v);
    printf("min_u = %f\n", min_u);
    printf("min_v = %f\n", min_v);

    double *graph_u_x = new double[im_wdth*im_wdth];
    double *graph_v_x = new double[im_wdth*im_wdth];
    double *graph_u_v_x = new double[im_wdth*im_wdth];
    for (int m = 0; m < width; ++m) {
        //printf("%d; %f; %f\n", m, tab.get_u(m, height - 1), tab.get_v(m, height - 1));
        int nu = double(im_wdth)*(tab.get_u(m, height - 1) - min_u)/(max_u - min_u);
        int nv = double(im_wdth)*(tab.get_v(m, height - 1) - min_v)/(max_v - min_v);

        graph_u_x[nu*im_wdth + m] = 255;
        graph_v_x[nv*im_wdth + m] = 255;
    };

    for (int m = 0; m < width; ++m) {
        int nu = double(im_wdth)*(tab.get_u(m, height - 1) - min_u)/(max_u - min_u);
        int nv = double(im_wdth)*(tab.get_v(m, height - 1) - min_v)/(max_v - min_v);
        graph_u_v_x[nu*im_wdth + nv] = 255;
    };

    writePPM(graph_u_x, im_wdth, im_wdth, "ux.ppm");
    writePPM(graph_v_x, im_wdth, im_wdth, "vx.ppm");
    writePPM(graph_u_v_x, im_wdth, im_wdth, "uv_x.ppm");
  

    for (int n = 0; n < height; ++n) {
        if (max_u < tab.get_u(width/2, n)) max_u = tab.get_u(width/2, n);
        if (max_v < tab.get_v(width/2, n)) max_v = tab.get_v(width/2, n);
        if (min_u > tab.get_u(width/2, n)) min_u = tab.get_u(width/2, n);
        if (min_v > tab.get_v(width/2, n)) min_v = tab.get_v(width/2, n);
    }

    printf("max_u = %f\n", max_u);
    printf("max_v = %f\n", max_v);
    printf("min_u = %f\n", min_u);
    printf("min_v = %f\n", min_v);

    double *graph_u_t = new double[im_wdth*im_wdth];
    double *graph_v_t = new double[im_wdth*im_wdth];
    double *graph_u_v_t = new double[im_wdth*im_wdth];
    for (int n = 0; n < im_wdth; ++n) {
        //printf("%d; %f; %f\n", m, tab.get_u(m, height - 1), tab.get_v(m, height - 1));
        int nu = double(im_wdth)*(tab.get_u(im_wdth/2, n*height/im_wdth) - min_u)/(max_u - min_u);
        int nv = double(im_wdth)*(tab.get_v(im_wdth/2, n*height/im_wdth) - min_v)/(max_v - min_v);
        
        graph_u_t[nu*im_wdth + n] = 255;
        graph_v_t[nv*im_wdth + n] = 255;
    };

    for (int n = 0; n < height; ++n) {
        int nu = double(im_wdth)*(tab.get_u(width/2, n) - min_u)/(max_u - min_u);
        int nv = double(im_wdth)*(tab.get_v(width/2, n) - min_v)/(max_v - min_v);
        graph_u_v_t[nu*im_wdth + nv] = 255;
    };


    writePPM(graph_u_t, im_wdth, im_wdth, "ut.ppm");
    writePPM(graph_v_t, im_wdth, im_wdth, "vt.ppm");
    writePPM(graph_u_v_t, im_wdth, im_wdth, "uv_t.ppm");

    return 0;
}
