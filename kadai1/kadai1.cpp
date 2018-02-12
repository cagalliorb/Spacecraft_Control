#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>
#include <iomanip>
#include <iostream>
#define _USE_MATH_DEFINES

const double Ix = 1.9;
const double Iy = 1.6;
const double Iz = 2.0;
const double dt = 0.01;
const double OMEGA_S = 17.0 * 2.0 * M_PI / 60.0;  //[rad/s]*/
const double T_END = 100.0;
const int NORMALIZE_PEIRIOD = 100;

using namespace Eigen;
using namespace std;

Vector3d CalcFofOmega(Vector3d omega, Vector3d Mn, Vector3d Mc) {
    // d(omega)/dt = F というODEのfを計算
    Vector3d f;
    f << (Iy - Iz) / Ix * omega.y() * omega.z() + (Mn.x() + Mc.x()) / Ix,
	(Iz - Ix) / Iy * omega.z() * omega.x() + (Mn.y() + Mc.y()) / Iy,
	(Ix - Iy) / Iz * omega.x() * omega.y() + (Mn.z() + Mc.z()) / Iz;
    return f;
}

Vector4d CalcFofQuat(Vector3d omega, Vector4d quat) {
    // d(quat)/dt = Q*omegaというODEの右辺を計算
    MatrixXd Q(4, 3);
    Q << -quat(1) / 2, -quat(2) / 2, -quat(3) / 2,  // 1st column
	quat(0) / 2, -quat(3) / 2, quat(2) / 2,     // 2nd column
	quat(3) / 2, quat(0) / 2, -quat(1) / 2,     // 3rd column
	-quat(2) / 2, quat(1) / 2, quat(0) / 2;     // 4th column
    Vector4d q = Q * omega;
    return q;
}

void RK4(Vector3d &omega, Vector4d &quat, Vector3d Mn, Vector3d Mc, double t) {
    Vector3d k1 = CalcFofOmega(omega, Mn, Mc);
    Vector3d k2 = CalcFofOmega(omega + k1 * dt / 2.0, Mn, Mc);
    Vector3d k3 = CalcFofOmega(omega + k2 * dt / 2.0, Mn, Mc);
    Vector3d k4 = CalcFofOmega(omega + k3 * dt, Mn, Mc);

    Vector4d kq1 = CalcFofQuat(omega, quat);
    Vector4d kq2 = CalcFofQuat(omega, quat + kq1 * dt / 2.0);
    Vector4d kq3 = CalcFofQuat(omega, quat + kq2 * dt / 2.0);
    Vector4d kq4 = CalcFofQuat(omega, quat + kq3 * dt);

    omega += (k1 + 2.0 * k2 + 2.0 * k3 + k4) * dt / 6.0;
    quat += (kq1 + 2.0 * kq2 + 2.0 * kq3 + kq4) * dt / 6.0;
}

void file_write_omega(double t, Vector3d omega, const char *file_name) {
    FILE *fp;
    fp = fopen(file_name, "a");
    fprintf(fp, "%f,%f,%f,%f\n", t, omega.x(), omega.y(), omega.z());
    fclose(fp);
}
void file_write_quat(double t, Vector4d quat, const char *file_name) {
    FILE *fp;
    fp = fopen(file_name, "a");
    fprintf(fp, "%f,%f,%f,%f,%f\n", t, quat(0), quat(1), quat(2), quat(3));
    fclose(fp);
}

int main(void) {
    Vector3d omega(0.1, OMEGA_S + 0.1, 0);
    Vector4d quat(1.0, 0, 0, 0);
    Vector3d Mn(0, 0, 0);  // 外乱トルク
    Vector3d Mc(0, 0, 0);  // 制御トルク
    double t = 0.0;	// time initialize

    /*cout << "time\t"
	 << "omega_x\t"
	 << "omega_y\t"
	 << "omega_z" << endl;*/
    /*cout << "times\t"
	 << "quat_0\t"
	 << "quat_1\t"
	 << "quat_2\t"
	 << "quat_3" << endl;*/

    for (int i = 0; i <= int(T_END / dt); i++) {
	/*cout << t << "\t" << omega.x() << "\t" << omega.y() << "\t" <<
	   omega.z()
	     << endl;*/
	/*cout << t << "\t" << quat(0) << "\t" << quat(1) << "\t" << quat(2)
	     << "\t" << quat(3) << endl;*/
	file_write_omega(t, omega, "omega.csv");
	file_write_quat(t, quat, "quaternion.csv");
	if (i % NORMALIZE_PEIRIOD == 1) {
	    quat = quat.normalized();
	}
	RK4(omega, quat, Mn, Mc, t);
	t += dt;
    }
    return 0;
}
