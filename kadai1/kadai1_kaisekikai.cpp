#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>
#include <iomanip>
#include <iostream>
#define _USE_MATH_DEFINES

using namespace Eigen;
using namespace std;

const double Ix = 1.9;
const double Iy = 1.6;
const double Iz = 2.0;
const double dt = 0.01;
const double OMEGA_S = 17.0 * 2.0 * M_PI / 60.0;  //[rad/s]*/
const double T_END = 100.0;

const double omegax_init = 0.1;
const double omegay_init = OMEGA_S + 0.1;
const double omegaz_init = 0.0;

const double T = 1.0 / 2.0 *
		 (Ix * pow(omegax_init, 2.0) + Iy * pow(omegay_init, 2.0) +
		  Iz * pow(omegaz_init, 2.0));
const double L2 =
    pow(Ix, 2.0) * pow(omegax_init, 2.0) +
    pow(Iy, 2.0) * pow(omegay_init, 2.0) +
    pow(Iz, 2.0) * pow(omegaz_init, 2.0);  // squared angular momentum
const double k2 =
    (Iz - Ix) * (L2 - 2.0 * T * Iy) / ((Ix - Iy) * (2.0 * T * Iz - L2));  // k^2
const double dtau_dt = pow((2.0 * T * Iz - L2) * (Ix - Iy) / Ix / Iy / Iz, 0.5);
const double TAU_END = dtau_dt * T_END;

const double omegax_coeff = pow((2 * T * Iy - L2) / (Ix * (Iy - Ix)), 0.5);
const double omegay_coeff = pow((2 * T * Iz - L2) / (Iy * (Iz - Iy)), 0.5);
const double omegaz_coeff = pow((2 * T * Iy - L2) / (Iz * (Iy - Iz)), 0.5);

int main(void) {
    cout << "T:  " << T << endl;
    cout << "L2: " << L2 << endl;
    cout << "k: " << pow(k2, 0.5) << endl;
    cout << "dtau_dt; " << dtau_dt << endl;
    cout << "TAU_END: " << TAU_END << endl;
    cout << "omegax_coeff: " << omegax_coeff << endl;
    cout << "omegay_coeff: " << omegay_coeff << endl;
    cout << "omegaz_coeff: " << omegaz_coeff << endl;
    return 0;
}
