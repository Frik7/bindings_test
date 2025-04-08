#include "pybind11/pybind11.h"
#include<cmath>
#include<algorithm>
#include<stdexcept>
#include<cstdlib>
#include<ctime>
#include<chrono>
#include<random>
using namespace std;

double GetOneGaussianBysummation(){
double result=0;

for(unsigned long j=0;j<12;j++)
result+=rand()/static_cast<double>(RAND_MAX);
result-=6.0;
return result;
}

double GetOneGaussianByBoxMuller(){
    double result;

    double x;
    double y;
    double sizeSquared;
    do{
    x=2.0*rand()/static_cast<double>(RAND_MAX)-1;
    y=2.0*rand()/static_cast<double>(RAND_MAX)-1;
    sizeSquared=x*x+y*y;
    }while(sizeSquared>=1.0);
    result=x*sqrt(-2*log(sizeSquared)/sizeSquared);
    return result;
}

double GetOneGaussianBySTL(){
    double result;
    static std::default_random_engine e;
    std::normal_distribution<double> d(0.,1.0);
    return d(e);
}

class PayOff{
    public:
    enum OptionType{call,put};
    PayOff(double Strike_, OptionType TheOptionsType_);
    double operator()(double Spot)const;
    private:
    double Strike;
    OptionType TheOptionsType;
};

PayOff::PayOff(double Strike_, OptionType TheOptionsType_)
:Strike(Strike_),TheOptionsType(TheOptionsType_)
{
   
}
double PayOff::operator()(double spot)const
{
    switch(TheOptionsType)
    {
        case call:
        return std::max<double>(spot-Strike,0.0);
        case put:
        return std::max<double>(Strike-spot,0.0);

        default:
        throw std::invalid_argument("unknown option type found.");
    }
    return 0.0;
}

double SimpleMonteCarlo2(const PayOff& thePayOff,
                        double Expiry,
                        double Spot,
                        double Vol,
                        double r,
                        unsigned long NumberOfPaths)

{
    double variance=Vol*Vol*Expiry;
    double rootVariance=sqrt(variance);
    double itoCorrection= -0.5*variance;

    double movedSpot =Spot*exp(r*Expiry+itoCorrection);
    double thisSpot;
    double runningSum=0;

    for(unsigned long i=0;i<NumberOfPaths;i++)
    {
        double thisGaussian= GetOneGaussianBySTL();
        thisSpot=movedSpot*exp(rootVariance*thisGaussian);
        double thisPayOff= thePayOff(thisSpot);//call operator
        runningSum+=thisPayOff;
    }

    double mean=runningSum/NumberOfPaths;
    mean*=exp(-r*Expiry);
    return mean;
}

tuple<double, double> MCOptionPricer(double Expiry,
         double Strike,
         double Spot,
         double Vol,
         double r,
         unsigned long NumberOfPaths)
{

        PayOff callPayOff(Strike,PayOff::call);
        PayOff putPayOff(Strike,PayOff::put);

        double resultCall = SimpleMonteCarlo2(callPayOff,
                                              Expiry,
                                              Spot,
                                              Vol,
                                              r,
                                              NumberOfPaths);
        
        double resultPut = SimpleMonteCarlo2(putPayOff,
                                            Expiry,
                                            Spot,
                                            Vol,
                                            r,
                                            NumberOfPaths);

        return tuple<double, double>{resultCall, resultPut};
}

PYBIND11_MODULE(MonteCarlo, m) {
        m.doc() = "Simple Monte Carlo Simulation for Option Pricing rturns the respective call and put prices";
    
        m.def("OptionPricer", &MCOptionPricer, "A function that calculates vanilla option prices by using Monte Carlo simulation",
              pybind11::arg("Expiry"),
              pybind11::arg("Strike"),
              pybind11::arg("Spot"),
              pybind11::arg("Vol"),
              pybind11::arg("r"),
              pybind11::arg("NumberOfPaths"));
    }