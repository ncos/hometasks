#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>     
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

typedef unsigned long long int uint64;
typedef long long int int64;

long double f(double *p) {
  //return sin(p[0]) + cos(p[1]) / sqrt(p[2] + p[3] + p[4] + 1) * pow (p[5] + p[6] + p[7] + p[8] + p[9], 5);
  return sin(p[0]) + cos(p[1]);// / sqrt(p[2] + p[3] + p[4] + 1) * pow (p[5] + p[6] + p[7] + p[8] + p[9], 5);
};


double integrate(std::vector<double> &xlow,
                 std::vector<double> &xhigh, uint64 npd)
{
  if (xlow.size() != xhigh.size()) {
    std::cerr << "unequal number of dimensions in xlow and xhigh\n";
    return NAN;
  }

  boost::mt19937 igen(42u);
  igen.seed(static_cast<unsigned int>(std::time(0)));
  boost::variate_generator<boost::mt19937, boost::uniform_real<> >
                  gen(igen, boost::uniform_real<>(0, 1));
  double *point = new double [xlow.size()];

  double mult = 1.0;
  for(uint64 i = 0; i < xlow.size(); ++i) {
    mult = mult * (xhigh[i] - xlow[i]);
  }

  double sigma = 0;
  for(uint64 i = 0; i < npd * xlow.size(); ++i) {
    for (uint64 j = 0; j < xlow.size(); ++j) {
      point[j] = gen() * (xhigh[j] - xlow[j]) + xlow[j];
      // std::cout << point[j] << " ";
    }
    // std::cout << std::endl;
    sigma += f(point);
    // std::cout << mult * sigma / (npd * xlow.size()) << ",\n";
  }

  delete [] point;
  return mult * sigma / (npd * xlow.size());
}

int main(void)
{
//  std::vector<double> xlow  { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};
//  std::vector<double> xhigh { 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0};
  std::vector<double> xlow  { 0.0,  0.0};
  std::vector<double> xhigh { 1.0,  1.0};


  std::cout << "I = " << integrate(xlow, xhigh, 1000000) << std::endl;

  return 0;
};

