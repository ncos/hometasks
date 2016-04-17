#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
    return 0;
}

double alpha(int m, int n) {
    return 0.2;
}

double scheme(int m, int n, double h, double t, class Data& data){
    //return ( f_right(m, n) + alpha() * (data.get(m+1, n) - 2*data.get(m, n) + data.get(m - 1, n)) / (h*h) )*2*t + data.get(m, n);
    return data.get(m, n) - t*(alpha(m, n) * (data.get(m, n) - data.get(m - 1, n))/h - f_right(m, n));
}

static double f0(double x) {
    if(x < 0.2) return 1;
    if(x > 0.3) return 0;    
    return cos((x+0.2)*3.14/0.1);
};


int main(int argc, char **argv){
    int rank = 0, size = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double xmin = 0, xmax = 0, XMIN = 0.0, XMAX = 1.0, tmin = 0.0, tmax = 1.0;
    int height = 2000;
    int width = 2000;
    double h = (XMAX - XMIN)/width, t = (tmax - tmin)/height;

    int Nx = width;
    //int Nt = height;


    xmin = Nx / size * (rank);

    if (rank == size -1)
        xmax = Nx;
    else
        xmax = (Nx / size)*(rank + 1);
    int r = size-1;
    for (int i = (Nx % size-1); i > 0; i--){
        if (rank >= r)
            xmin++;
        if (rank >= r-1 && rank != size - 1)
            xmax++;
        r--;
    }

    Data tab;
    if (rank == 0){
        tab.init(width + 1, height);
        
        for (int i = 0; i < width; ++i) {

            tab.set(i, 0, f0(i*h));
        }
    }
    else
        tab.init((xmax - xmin + 1),(int)((tmax - tmin)/t));

    double starttime = 0, endtime = 0;
    starttime = MPI_Wtime();

    for (int n = 0; n < tab.getn()-1; n++){

        MPI_Status status;
        MPI_Request request;
        double buf = 0;
        if (n > 0 && rank > 0){
            MPI_Recv(&buf, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status );
            tab.set(0, n, buf);
        }
        for (int m = 1; m < tab.getm(); m++){
            tab.set(m, n +1, scheme(m, n, h, t, tab));
        }
        buf = tab.get(tab.getm()-1, n+1);
        if (rank < size-1){
            MPI_Isend(&buf, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &request);
        }

    }
        endtime = MPI_Wtime();
    if (rank > 0){
        int M = tab.getm();
        MPI_Send(&M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(tab.getdata(), tab.getn()*tab.getm(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

    }
    if (rank == 0 && size > 1){
        MPI_Status status;
        int dataM = 0;
        for (int i = 1; i < size; i++){
            MPI_Recv(&dataM, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(tab.get_addr_for_add(dataM), tab.getn()*dataM, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
        }

    }


    if (rank == 0){
        printf("Time [%i]: %lg c\n", rank, endtime-starttime);
    };

    //if (rank == 0){
    //    printf("t = %i:\n", tab.getn()-1);
    //    tab.print();
    //}


    if (rank == 0) {
        double *image = new double[width*height];
        for (int n = 0; n < height; n++){
            for (int m = 0; m < width; m++ )
                image[n*width + m] = tab.get(m, n);
        }

        std::cout << "Max: " << max(image, width, height) << " " 
                  << "Avg: " << avg(image, width, height) << " " 
                  << "Min: " << min(image, width, height) << "\n\n";

        writePPM(image, width, height, "convection.ppm");
    };

    MPI_Finalize();
    return 0;
}
