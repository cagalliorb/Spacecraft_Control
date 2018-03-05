#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <unsupported/Eigen/MatrixFunctions>
#define _USE_MATH_DEFINES

using namespace Eigen;
using namespace std;

const double Ix = 1.9;
const double Iy = 1.6;
const double Iz = 2.0;
const double dt = 0.01;
const double OMEGA_S = 17.0 * 2.0 * M_PI / 60.0;  //[rad/s]
const double T_END = 100.0;
const double OBSERVE_INTERVAL = 1.0;
const double sigma_w = 0.01;  //状態ノイズの標準偏差[Nm]
const double sigma_v = 0.01;  //観測ノイズの標準偏差[Nm]

/*メルセンヌツイスタ乱数生成*/
mt19937 mt(1);
normal_distribution<> w_noise(0.0, 0.01);
normal_distribution<> v_noise(0.0, 0.01);
/*0~2の値をとる,DCM列ベクトル指定用一様乱数*/
uniform_int_distribution<> rand012(0, 2);

Vector3d CalcFofOmega(Vector3d omega, Vector3d Mn, Vector3d Mc) {
    // d(omega)/dt = F というODEのfを計算
    Vector3d f;
    f << (Iy - Iz) / Ix * omega.y() * omega.z() +
	     (Mn.x() + Mc.x()) / Ix,  // 1st column
	(Iz - Ix) / Iy * omega.z() * omega.x() +
	    (Mn.y() + Mc.y()) / Iy,  // 2nd column
	(Ix - Iy) / Iz * omega.x() * omega.y() +
	    (Mn.z() + Mc.z()) / Iz;  // 3rd column
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
    /*RK4メイン関数*/
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

MatrixXd CalcA(Vector3d omega, Vector4d quat) {
    MatrixXd A(7, 7);
    A << 0.0, -omega.x() / 2.0, -omega.y() / 2.0, -omega.z() / 2.0,
	-quat(1) / 2.0, -quat(2) / 2.0, -quat(3) / 2.0,  // 1st column
	omega.x() / 2.0, 0.0, omega.z() / 2.0, -omega.y() / 2.0, quat(0) / 2.0,
	-quat(3) / 2.0, quat(2) / 2.0,  // 2nd column
	omega.y() / 2.0, -omega.z() / 2.0, 0.0, omega.x() / 2.0, quat(3) / 2.0,
	quat(0) / 2.0, -quat(1) / 2.0,  // 3rd column
	omega.z() / 2.0, omega.y() / 2.0, -omega.x() / 2.0, 0.0, -quat(2) / 2.0,
	quat(1) / 2.0, quat(0) / 2.0,  // 4th column
	0.0, 0.0, 0.0, 0.0, 0.0, (Iy - Iz) / Ix * omega.z(),
	(Iy - Iz) / Ix * omega.y(),  // 5th column
	0.0, 0.0, 0.0, 0.0, (Iz - Ix) / Iy * omega.z(), 0,
	(Iz - Ix) / Iy * omega.x(),  // 6th column
	0.0, 0.0, 0.0, 0.0, (Ix - Iy) / Iz * omega.y(),
	(Ix - Iy) / Iz * omega.x(),
	0.0;  // 7th column
    return A;
}

MatrixXd CalcB(Vector3d omega, Vector4d quat) {
    MatrixXd B(7, 3);
    B << 0.0, 0.0, 0.0,      // 1st column
	0.0, 0.0, 0.0,       // 2nd column
	0.0, 0.0, 0.0,       // 3rd column
	0.0, 0.0, 0.0,       // 4th column
	1.0 / Ix, 0.0, 0.0,  // 5th column
	0.0, 1.0 / Iy, 0.0,  // 6th column
	0.0, 0.0, 1.0 / Iz;  // 7th column
    return B;
}

MatrixXd CalcH(Vector4d quat, int DCM_index) {
    MatrixXd H(3, 7);
    double q0 = quat(0);
    double q1 = quat(1);
    double q2 = quat(2);
    double q3 = quat(3);

    switch (DCM_index) {
	case 0:
	    H << 2.0 * q0, 2.0 * q1, -2.0 * q2, -2.0 * q3, 0.0, 0.0,
		0.0,  // 1st column
		2.0 * q3, 2.0 * q2, 2.0 * q1, 2.0 * q0, 0.0, 0.0,
		0.0,  // 2nd column
		-2.0 * q2, 2.0 * q3, -2.0 * q0, 2.0 * q1, 0.0, 0.0,
		0.0;  // 3rd column
	    break;
	case 1:
	    H << -2.0 * q3, 2.0 * q2, 2.0 * q1, -2.0 * q0, 0.0, 0.0,
		0.0,  // 1st column
		2.0 * q0, -2.0 * q1, 2.0 * q2, -2.0 * q3, 0.0, 0.0,
		0.0,  // 2nd column
		2.0 * q1, 2.0 * q0, 2.0 * q3, 2.0 * q2, 0.0, 0.0,
		0.0;  // 3rd column
	    break;
	case 2:
	    H << 2.0 * q2, 2.0 * q3, 2.0 * q0, 2.0 * q1, 0.0, 0.0,
		0.0,  // 1st column
		-2.0 * q1, -2.0 * q0, 2.0 * q3, 2.0 * q2, 0.0, 0.0,
		0.0,  // 2nd column
		2.0 * q0, -2.0 * q1, -2.0 * q2, 2.0 * q3, 0.0, 0.0,
		0.0;  // 3rd column
	    break;
	default:
	    exit(1);
    }

    return H;
}

Matrix3d CalcDCM(Vector4d quat) {
    /*EigenにもDCMを取得する
    関数があるが,Quaternionの定義が
    違う説があってめんどくさいので
    自作する*/
    Matrix3d DCM(3, 3);
    double q0 = quat(0);
    double q1 = quat(1);
    double q2 = quat(2);
    double q3 = quat(3);
    DCM << q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3, 2.0 * (q1 * q2 - q0 * q3),
	2.0 * (q1 * q3 + q0 * q2),  // 1st column
	2.0 * (q1 * q2 + q0 * q3), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3,
	2.0 * (q2 * q3 - q0 * q1),  // 2nd column
	2.0 * (q1 * q3 - q0 * q2), 2.0 * (q2 * q3 - q0 * q1),
	q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;  // 3rd column
    return DCM;
}

void file_write_omega(double t, Vector3d omega_true, Vector3d omega_est,
		      MatrixXd P, const char *file_name) {
    FILE *fp;
    fp = fopen(file_name, "a");
    double omega_x_error = omega_est.x() - omega_true.x();
    double omega_y_error = omega_est.y() - omega_true.y();
    double omega_z_error = omega_est.z() - omega_true.z();
    fprintf(fp, "%f,,%f,%f,%f,,%f,%f,%f,,%f,%f,%f,,%f,%f,%f,%f,%f,%f\n", t,
	    omega_true.x(), omega_true.y(), omega_true.z(), omega_est.x(),
	    omega_est.y(), omega_est.z(), omega_x_error, omega_y_error,
	    omega_z_error, pow(P(4, 4), 0.5), -pow(P(4, 4), 0.5),
	    pow(P(5, 5), 0.5), -pow(P(5, 5), 0.5), pow(P(6, 6), 0.5),
	    -pow(P(6, 6), 0.5));
    fclose(fp);
}

void file_write_quat(double t, Vector4d quat_true, Vector4d quat_est,
		     MatrixXd P, const char *file_name) {
    FILE *fp;
    fp = fopen(file_name, "a");
    double q0_error = quat_est(0) - quat_true(0);
    double q1_error = quat_est(1) - quat_true(1);
    double q2_error = quat_est(2) - quat_true(2);
    double q3_error = quat_est(3) - quat_true(3);
    fprintf(
	fp,
	"%f,,%f,%f,%f,%f,,%f,%f,%f,%f,,%f,%f,%f,%f,,%f,%f,%f,%f,%f,%f,%f,%f\n",
	t, quat_true(0), quat_true(1), quat_true(2), quat_true(3), quat_est(0),
	quat_est(1), quat_est(2), quat_est(3), q0_error, q1_error, q2_error,
	q3_error, pow(P(0, 0), 0.5), -pow(P(0, 0), 0.5), pow(P(1, 1), 0.5),
	-pow(P(1, 1), 0.5), pow(P(2, 2), 0.5), -pow(P(2, 2), 0.5),
	pow(P(3, 3), 0.5), -pow(P(3, 3), 0.5));
    fclose(fp);
}
int main(void) {
    Vector3d omega_true(0.1, OMEGA_S + 0.1, 0.0);  //真値 initialize
    Vector4d quat_true(1.0, 0.0, 0.0, 0.0);	//真値 initialize
    Vector3d omega_est(0.2, OMEGA_S + 0.2, 0.1);   // 推定値 initialize
    Vector4d quat_est(1.0 + 0.1, 0.1, 0.1, 0.1);   //推定値 initialize

    quat_true = quat_true.normalized();
    quat_est = quat_est.normalized();
    Vector3d Mn(0, 0, 0);  //なにもない時の外乱トルク
    Vector3d Mc(0, 0, 0);  // 何もない時の制御トルク
    Vector3d y_true, y_est, y_obs;
    VectorXd x;
    Matrix3d DCM_true, DCM_est;
    Matrix3d Q = sigma_w * sigma_w * Matrix3d::Identity();
    Matrix3d R = sigma_v * sigma_v * Matrix3d::Identity();
    MatrixXd A, B, Phi, Gamma, M, H, K;
    MatrixXd P = MatrixXd::Identity(7, 7);  // P initialize
    double t = 0.0;			    // time initialize

    for (int i = 0; i <= int(T_END / dt); i++) {
	Vector3d Mn_noise(w_noise(mt), w_noise(mt),
			  w_noise(mt));  // 外乱ノイズ
	RK4(omega_true, quat_true, Mn_noise, Mc, t);
	RK4(omega_est, quat_est, Mn, Mc, t);

	A = CalcA(omega_est, quat_est);
	B = CalcB(omega_est, quat_est);
	Phi = (A * dt).exp();
	Gamma = A.inverse() * (Phi - MatrixXd::Identity(7, 7)) * B;
	M = Phi * P * Phi.transpose() + Gamma * Q * Gamma.transpose();
	P = M;

	/*ここで観測*/
	if (i % int(OBSERVE_INTERVAL / dt) == 0) {
	    DCM_true = CalcDCM(quat_true);
	    DCM_est = CalcDCM(quat_est);
	    int DCM_index = rand012(mt);
	    y_true = DCM_true.col(0);
	    y_est = DCM_est.col(0);
	    y_obs = y_true;
	    for (int i = 0; i < 2; i++) {
		y_obs(i) += v_noise(mt);  //観測
	    }

	    H = CalcH(quat_est, 0);
	    P = M - M * H.transpose() * (H * M * H.transpose() + R).inverse() *
			H * M;
	    K = P * H.transpose() * R.inverse();
	    x = K * (y_obs - y_est);
	    for (int i = 0; i < 3; i++) {
		omega_est(i) += x(4 + i);
	    }
	    for (int i = 0; i < 4; i++) {
		quat_est(i) += x(i);
	    }
	}
	file_write_omega(t, omega_true, omega_est, P, "omega.csv");
	file_write_quat(t, quat_true, quat_est, P, "quat.csv");
	quat_true = quat_true.normalized();
	quat_est = quat_est.normalized();
	cout << omega_est.x() << omega_est.y() << omega_est.z() << quat_est(0)
	     << quat_est(1) << quat_est(2) << quat_est(3) << "\n"
	     << endl;
	t += dt;
    }
    return 0;
}
