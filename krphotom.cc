/*
ph.cc
15 nov 16

formula translation to c++
test numerical methods described in equations 1-7
print out a table of pH vrs RN

Values of RN from .1-2.8  produce pH from  6.37 to 8.39 depending on T and S

RN range   .8  - 2.6   Fig 3 range is 0.8 to 2.5 

pH range:  7.6 - 8.2
S  range: 30.0 - 36.2
T  range: 15C  - 30C    ~= 288K - 304K

288K = 14.85C
300K = 26.85C
303K = 29.85C
304K = 30.85C

eq 8 :calibration equation of RB to RN 
      RB absorbtion ratio of diy broadband spectrometer  
      RN absorbtion ratio  narrowband lab spectrometer)   
*/
#include <cstdio>
#include <stdio.h>
#include <math.h>

const char* Program = "ph.cc";

int main()
{
    printf ("Program: %s \n", Program);

    double pH, equConstant,R, e1,e2,e3, e23;
    double  T, S;  // temp and salinity

    // set values for S and T
    // for salinity S values from 20 to 40
    // for temperatures T from 278.15 K to 308.15 K
    S =  30;
    //S =  40;
    T =  280;
    T =  303;

    // RN is absorbance ratio
    double RN = 0.5;

    // eq 5
    e1 = -0.007762 + (4.5174 / pow(10.0,5.0)) * T;
    // e1 = -0.007762 + 4.5174 * T / pow(10.0,5.0) ;
    //e23 = -0.020813 + 2.60262 /  pow (10.0,4.0)*T + 1.0436 / pow (10.0,4.0) *(S-35);
    //e23 = -0.020813 + ((2.60262 / pow (10.0,4.0))*T) + ((1.0436 / pow (10.0,4.0)) * (S-35)) ; 
    e23 = -0.020813 + (2.60262 / pow (10.0,4.0))*T + (1.0436 / pow (10.0,4.0)) * (S-35) ; 
    printf ("e1  : %f\n", e1); 
    printf ("e23 : %f\n", e23); 

    // eq 7   equilibrium constant  equConstant
    double a,b,c,d;

    // look eq for a can produce wildly different results
    // a = -246.64209 + 0.315971 * S + 2.8855 + S*S/pow(10.0,4.0) ;
    // a = -246.64209 + 0.315971 * S + ((2.8855/pow (10.0,4.0)) * S* S) ;
    a = -246.64209 + 0.315971 * S + (2.8855/pow (10.0,4.0)) * S * S ;
    b= 7229.23864 - 7.098137* S - 0.057034 * S * S;
    c = 44.493382-0.052711 * S;
    d = 0.0781344;
    equConstant = a + b / T + c * log(T) - d*T;  // log is the natural log ln 

    printf (" a : %f\n", a);
    printf (" equConstant : %f\n", equConstant);

    // lets look at ph now
    pH = equConstant +  log10 (( RN - e1)/(1-RN*e23) );  // assume the COMMON LOG

    printf (" pH : %f\n", pH );

    // From Fig. 3. the calibration chart shows RN of 0.8 to 2.8 
    // print out table of RN to pH using equations 1-7

    /*
      RN = (1.1892) RB - 0.379
      RB = ( RN +0.379) / 1.1892
    */

    printf ("rb,rn,pH\n" );
    for ( double rn=0.2; rn <= 2.5; rn = rn + .01) {
         pH = equConstant +  log10 (( rn - e1)/(1-rn*e23) );  // assume the COMMON LOG
         double RB = ( rn +0.379) / 1.1892;
         printf ("%f,  %f, %f\n",RB, rn, pH );
    }

    return 0;
}
