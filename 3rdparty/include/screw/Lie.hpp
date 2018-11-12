#ifndef _SCREW_LIE_HPP_
#define _SCREW_LIE_HPP_

#include <screw/screw.hpp>
#include <iostream>

#define EIGEN_MATRIXBASE_PLUGIN <screw/EigenMatrixBasePlugin.hpp>
#include <Eigen/Dense>

namespace screw {

	template<typename Scalar> using vector3 = Eigen::Matrix<Scalar, 3, 1>;
	template<typename Scalar> using twist = Eigen::Matrix<Scalar, 6, 1>;
	template<typename Scalar> using wrench = Eigen::Matrix<Scalar, 6, 1>;

	template <typename Scalar> class se3;

	// skew-symmetric matrix representation of 3-dimensional vector
	// (omega0, omega1, omega2)
	// -----------------------------
	//   0     -omega2    omega1
	// omega2     0      -omega0
    //-omega1   omega0       0	

	template <typename Scalar>
	class so3 : public Eigen::Matrix <Scalar, 3, 3>
	{
	public :
		// z-axis, unit vector
		so3()
		{
			*this << 0, 1, 0,
					-1, 0, 0,
					 0, 0, 0;
		}

		so3(const Scalar& s)
		{
			*this << 0, -s,  s,
					 s,  0, -s,
					-s,  s,  0;
		}

		so3(const screw::Axis& axis)
		{
			switch (axis)
			{
			case screw::Axis::X :
				*this << 0, 0,  0,
  					     0, 0, -1,
					     0, 1,  0;
				break;
			case screw::Axis::Y :
				*this << 0, 0, 1,
					     0, 0, 0,
					    -1, 0, 0;
				break;
			case screw::Axis::Z:
				*this << 0, -1, 0,
					     1,  0, 0,
					     0,  0, 0;
				break;
			}
		}

		so3(const Scalar& omega0, const Scalar& omega1, const Scalar& omega2)
		{
			*this <<      0, -omega2,  omega1,
				     omega2,       0, -omega0,
				    -omega1,  omega0,       0;
		}

		so3(const screw::vector3<Scalar>& omega)
		{
			*this << 	0, -omega(2),  omega(1),
				 omega(2),         0, -omega(0),
				-omega(1), omega(0),          0;
		}

		so3(const so3<Scalar>& omega)
		{

		}

		template<typename OtherDerived>
		so3(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<Scalar, 3, 3>(other)
		{
		
		}

		template<typename OtherDerived>
		so3& operator=(const Eigen::MatrixBase <OtherDerived>& other)
		{
			this->Eigen::Matrix<Scalar, 3, 3>::operator=(other);
			return *this;
		}
	};

	template <typename Scalar>
	class SO3 : public Eigen::Matrix <Scalar, 3, 3>
	{
	public :
		SO3()
		{
			setIdentity();
		}

		template<typename OtherDerived>
		SO3(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<Scalar, 3, 3>(other)
		{

		}

		SO3(Scalar R0, Scalar R1, Scalar R2, Scalar R3, Scalar R4, Scalar R5, Scalar R6, Scalar R7, Scalar R8)
		{
			*this <<
				R0, R3, R6,
				R1, R4, R7,
				R2, R5, R8;
		}
		
		SO3(screw::vector3<Scalar> v1, screw::vector3<Scalar> v2, screw::vector3<Scalar> v3)
		{
			*this <<
				v1(0), v2(0), v3(0),
				v1(1), v2(1), v3(1),
				v1(2), v2(2), v3(2);
		}

		SO3(screw::RotationType type, double r1, double r2, double r3)
		{
			switch (type)
			{
			case screw::RotationType::ROLL_PITCH_YAW :
				rpy(r1, r2, r3);
				break;
			}
		}

		template<typename OtherDerived>
		SO3& operator=(const Eigen::MatrixBase <OtherDerived>& other)
		{
			this->Eigen::Matrix<Scalar, 3, 3>::operator=(other);
			return *this;
		}

		screw::SO3<Scalar> transpose() const
		{
			const Scalar *me = m_storage.data();
			return screw::SO3<Scalar>(me[0], me[3], me[6], me[1], me[4], me[7], me[2], me[5], me[8]);
		}

		screw::SO3<Scalar>& transposeInPlace()
		{
			this->Eigen::Matrix<Scalar, 3, 3>::transposeInPlace();
			return *this;
		}

		screw::SO3<Scalar>& inverse()
		{
			return transpose();			
		}

		screw::SO3<Scalar>& rpy(Scalar roll, Scalar pitch, Scalar yaw)
		{
			double sa, ca, sb, cb, sg, cg;

			sa = std::sin(roll);
			ca = std::cos(roll);
			sb = std::sin(pitch);
			cb = std::cos(pitch);
			sg = std::sin(yaw);
			cg = std::cos(yaw);

			*this << 
				cg * cb, cg * sb * sa - sg * ca, cg * sb * ca + sg * sa,
				sg * cb, sg * sb * sa + cg * ca, sg * sb * ca - cg * sa,
				-sb, cb * sa, cb * ca;

			return *this;
		}
	};

	template <typename Scalar>
	class SE3
	{

	public:
		SE3()
		{
			R_.setIdentity(3, 3);
			p_.setZero();
		}

		SE3(const screw::SO3<Scalar> &R, const screw::vector3<Scalar> &p)
		{
			R_ = R;
			p_ = p;
		}

		SE3(const screw::SO3<Scalar> &R)
		{
			R_ = R;
			p_.setZero();
		}

		SE3(const screw::vector3<Scalar> &p)
		{
			R_.setIdentity();
			p_ = p;
		}

		// column-order
		SE3(Scalar R0, Scalar R1, Scalar R2, Scalar R3, Scalar R4, Scalar R5, Scalar R6, Scalar R7, Scalar R8, Scalar p0, Scalar p1, Scalar p2)
		{
			R_ <<
				R0, R3, R6,
				R1, R4, R7,
				R2, R5, R8;

			p_ << p0, p1, p2;
		}

		Eigen::Matrix<Scalar, 4, 4> toEigenMatrix(void)
		{
			Eigen::Matrix<Scalar, 4, 4> M = Eigen::MatrixXd::Identity(4, 4);
			M.block(0, 0, 3, 3) = R_;
			M.block(0, 3, 3, 1) = p_;
			return  M;
		}

		//template<typename OtherDerived>
		//SE3(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<Scalar, 4, 4>(other)
		//{

		//}

		//template<typename OtherDerived>
		//SE3& operator=(const Eigen::MatrixBase <OtherDerived>& other)
		//{
		//	this->Eigen::Matrix<Scalar, 4, 4>::operator=(other);
		//	return *this;
		//}

		// T1, T2 and T3 are SE3
		// T3 = T1 * T2
		screw::SE3<Scalar> operator*(const screw::SE3<Scalar>& T) const
		{
			return screw::SE3<Scalar>(this->R_ * T.R_, this->R_ * T.p_ + this->p_);
		}

		// T is SE3, p1 and p2 are vector3
		// p2 = T * p1
		screw::vector3<Scalar> operator* (const screw::vector3<Scalar>& p) const
		{
			return this->R_*p + this->p_;			
		}
		
		screw::SE3<Scalar> operator/ (const screw::SE3<Scalar>& T) const
		{
			return screw::SE3<Scalar>(this->R_ * T.R_.transpose(), -this->R_ * T.R_.transpose() * T.p_ + this->p_);
		}

		// T1, T2 are SE3
		// T1 = T1 * T2
		//screw::SE3<Scalar>& operator*=(const screw::SE3<Scalar>& T)
		//{
		//	double x0, x1, x2;

		//	Scalar *me = m_storage.data();
		//	const Scalar *right = T.m_storage.data();

		//	me[12] +=
		//		me[0] * right[12] +
		//		me[4] * right[13] +
		//		me[8] * right[14];
		//	me[13] +=
		//		me[1] * right[12] +
		//		me[5] * right[13] +
		//		me[9] * right[14];
		//	me[14] +=
		//		me[2] * right[12] +
		//		me[6] * right[13] +
		//		me[10] * right[14];

		//	x0 =
		//		me[0] * right[0] +
		//		me[4] * right[1] +
		//		me[8] * right[2];
		//	x1 =
		//		me[0] * right[4] +
		//		me[4] * right[5] +
		//		me[8] * right[6];
		//	x2 =
		//		me[0] * right[8] +
		//		me[4] * right[9] +
		//		me[8] * right[10];

		//	me[0] = x0;
		//	me[4] = x1;
		//	me[8] = x2;

		//	x0 =
		//		me[1] * right[0] +
		//		me[5] * right[1] +
		//		me[9] * right[2];
		//	x1 =
		//		me[1] * right[4] +
		//		me[5] * right[5] +
		//		me[9] * right[6];
		//	x2 =
		//		me[1] * right[8] +
		//		me[5] * right[9] +
		//		me[9] * right[10];

		//	me[1] = x0;
		//	me[5] = x1;
		//	me[9] = x2;

		//	x0 =
		//		me[2] * right[0] +
		//		me[6] * right[1] +
		//		me[10] * right[2];
		//	x1 =
		//		me[2] * right[4] +
		//		me[6] * right[5] +
		//		me[10] * right[6];
		//	x2 =
		//		me[2] * right[8] +
		//		me[6] * right[9] +
		//		me[10] * right[10];

		//	me[2] = x0;
		//	me[6] = x1;
		//	me[10] = x2;

		//	return  *this;
		//}

		screw::SO3<Scalar> rotation(void)
		{
			return R_;
		}

		screw::vector3<Scalar> position(void)
		{
			return p_;
		}

		void setRotation(const screw::SO3<Scalar>& R)
		{
			R_ = R;
		}

		void setPosition(const screw::vector3<Scalar>& p)
		{
			p_ = p;
		}

		void translate(const screw::vector3<Scalar>& p)
		{
			p_ += p;
		}

		void exp(const screw::se3<Scalar>& S)
		{

		}

		screw::SE3<Scalar> inverse()
		{
			return screw::SE3<Scalar>(R_.transpose(), -R_.transpose() * p_);
			//Scalar *me = m_storage.data();

			//return screw::SE3<Scalar>(me[0], me[4], me[8], me[1], me[5], me[9], me[2], me[6], me[10],
			//	-me[0] * me[12] - me[1] * me[13] - me[2] * me[14],
			//	-me[4] * me[12] - me[5] * me[13] - me[6] * me[14],
			//	-me[8] * me[12] - me[9] * me[13] - me[10] * me[14]);
		}

		// omega of S is normalized.
		void exp(const screw::se3<Scalar>& S, double& theta)
		{
			// BUG : when w of W is zero
			static double st, ct, vt, ut, t0, t1, t2;

			const Scalar *R = R_.m_storage.data();
			const Scalar *p = p_.m_storage.data();
			const Scalar *algebra = S.m_storage.data();

			st = sin(theta);
			ct = cos(theta);
			vt = 1.0 - ct;
			ut = (theta - st) * (algebra[0] * algebra[3] + algebra[1] * algebra[4] + algebra[2] * algebra[5]);
			t0 = algebra[2] * st;
			t1 = algebra[1] * st;
			t2 = algebra[0] * st;

			R[0] = algebra[0] * algebra[0] * vt + ct;
			R[1] = algebra[0] * algebra[1] * vt + t0;
			R[2] = algebra[0] * algebra[2] * vt - t1;
			R[3] = algebra[0] * algebra[1] * vt - t0;
			R[4] = algebra[1] * algebra[1] * vt + ct;
			R[5] = algebra[1] * algebra[2] * vt + t2;
			R[6] = algebra[0] * algebra[2] * vt + t1;
			R[7] = algebra[1] * algebra[2] * vt - t2;
			R[8] = algebra[2] * algebra[2] * vt + ct;
			p[0] = st * algebra[3] + ut * algebra[0] + vt * (algebra[1] * algebra[5] - algebra[2] * algebra[4]);
			p[1] = st * algebra[4] + ut * algebra[1] + vt * (algebra[2] * algebra[3] - algebra[0] * algebra[5]);
			p[2] = st * algebra[5] + ut * algebra[2] + vt * (algebra[0] * algebra[4] - algebra[1] * algebra[3]);

			return;
		}


//		friend std::ostream& operator<< (std::ostream &os, const screw::SE3<Scalar> &T);	// std::ostream standard output

	public:
		screw::SO3<Scalar> R_;
		screw::vector3<Scalar> p_;

	};

	template<typename Scalar>
	screw::se3<Scalar> Ad(screw::SE3<Scalar> T, const screw::se3<Scalar>& s)
	{
		screw::se3<Scalar> vr;
		vr.Ad(T, s);
		return vr;
	}


	// skew-symmetric matrix representation
	// (omega0, omega1, omega2, v0, v1, v2)
	// -----------------------------
	//   0     -omega3    omega2    v0
	// omega3     0      -omega1    v1
	//-omega2   omega1       0	    v2
	//   0        0          0       0

	template <typename Scalar>
	class se3 : public Eigen::Matrix <Scalar, 4, 4>
	{
	public:
		se3()
		{ 
			setZero();
		}

		se3(Scalar omega0, Scalar omega1, Scalar omega2, Scalar v0, Scalar v1, Scalar v2)
		{
			*this <<
				0, -omega2, omega1, v0,
				omega2, 0, -omega0, v1,
				-omega1, omega0, 0, v2,
				0, 0, 0, 0;
		}

		se3(const screw::se3<Scalar>& S)
		{

		}

		se3(const screw::vector3<Scalar>& omega, const screw::vector3<Scalar>& v)
		{
			block(3, 0, 3, 1) = v;
		}

		se3(const screw::so3<Scalar>& omega, const screw::vector3<Scalar>& v)
		{
			block(0, 0, 3, 3) = omega;
			block(3, 0, 3, 1) = v;
		}

		template<typename OtherDerived>
		se3(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<Scalar, 4, 4>(other)
		{

		}

		template<typename OtherDerived>
		se3& operator=(const Eigen::MatrixBase <OtherDerived>& other)
		{
			this->Eigen::Matrix<Scalar, 4, 4>::operator=(other);
			return *this;
		}

		void Ad(screw::SE3<Scalar> &T, screw::se3<Scalar> &S)
		{
			Scalar omega[3];
			Scalar v[3];
			Scalar S_omega[3];
			Scalar S_v[3];
			S_omega[0] = S(2, 1);
			S_omega[1] = S(0, 2);
			S_omega[2] = S(1, 0);
			S_v[0] = S(0, 3);
			S_v[1] = S(1, 3);
			S_v[2] = S(2, 3);

			const screw::SO3<Scalar> R = T.rotation();
			const screw::vector3<Scalar> p = T.position();

			omega[0] =
				R(0, 0) * S_omega[0] +
				R(0, 1) * S_omega[1] +
				R(0, 2) * S_omega[2];
			omega[1] =
				R(1, 0) * S_omega[0] +
				R(1, 1) * S_omega[1] +
				R(1, 2) * S_omega[2];
			omega[2] =
				R(2, 0) * S_omega[0] +
				R(2, 1) * S_omega[1] +
				R(2, 2) * S_omega[2];
			v[0] =
				p[1] * omega[2] -
				p[2] * omega[1] +
				R(0, 0) * S_v[0] +
				R(0, 1) * S_v[1] +
				R(0, 2) * S_v[2];
			v[1] =
				p[2] * omega[0] -
				p[0] * omega[2] +
				R(1, 0) * S_v[0] +
				R(1, 1) * S_v[1] +
				R(1, 2) * S_v[2];
			v[2] =
				p[0] * omega[1] -
				p[1] * omega[0] +
				R(2, 0) * S_v[0] +
				R(2, 1) * S_v[1] +
				R(2, 2) * S_v[2];

			*this <<
				0, -omega[2], omega[1], v[0],
				omega[2], 0, -omega[0], v[1],
				-omega[1], omega[0], 0, v[2],
				0, 0, 0, 0;

		}


	};

	//template <typename Scalar>
	//class Inertia : public Eigen::Matrix<Scalar, 6, 6>
	//{
	//public :
	//	Inertia(void)
	//	{
	//		setZero();
	//	}
	//	Inertia(Scalar mass, Scalar Ixx, Scalar Iyy, Scalar Izz)
	//	{

	//	}
	//	Inertia(Scalar mass, Scalar Ixx, Scalar Iyy, Scalar Izz, Scalar Ixy, Scalar Ixz, Scalar Iyz, Scalar r0, Scalar r1, Scalar r2)
	//	{

	//	}

	//};


	// omega of S is normalized.
	template<typename Scalar> 
	screw::SE3<Scalar> exp(const screw::se3<Scalar>& S, Scalar theta)
	{
		// BUG : when w of W is zero
		double st, ct, vt, ut, t0, t1, t2;
		Scalar R[9], p[3];

//		const Scalar *algebra = S.m_storage.data();
		Scalar algebra[6];

		algebra[0] = S(2, 1);
		algebra[1] = S(0, 2);
		algebra[2] = S(1, 0);
		algebra[3] = S(0, 3);
		algebra[4] = S(1, 3);
		algebra[5] = S(2, 3);

		st = sin(theta);
		ct = cos(theta);
		vt = 1.0 - ct;
		ut = (theta - st) * (algebra[0] * algebra[3] + algebra[1] * algebra[4] + algebra[2] * algebra[5]);
		t0 = algebra[2] * st;
		t1 = algebra[1] * st;
		t2 = algebra[0] * st;

		R[0] = algebra[0] * algebra[0] * vt + ct;
		R[1] = algebra[0] * algebra[1] * vt + t0;
		R[2] = algebra[0] * algebra[2] * vt - t1;
		R[3] = algebra[0] * algebra[1] * vt - t0;
		R[4] = algebra[1] * algebra[1] * vt + ct;
		R[5] = algebra[1] * algebra[2] * vt + t2;
		R[6] = algebra[0] * algebra[2] * vt + t1;
		R[7] = algebra[1] * algebra[2] * vt - t2;
		R[8] = algebra[2] * algebra[2] * vt + ct;
		p[0] = st * algebra[3] + ut * algebra[0] + vt * (algebra[1] * algebra[5] - algebra[2] * algebra[4]);
		p[1] = st * algebra[4] + ut * algebra[1] + vt * (algebra[2] * algebra[3] - algebra[0] * algebra[5]);
		p[2] = st * algebra[5] + ut * algebra[2] + vt * (algebra[0] * algebra[4] - algebra[1] * algebra[3]);

		return screw::SE3<Scalar>(R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8], p[0], p[1], p[2]);
	}


	// 3-dimensional vector
	typedef vector3<double> vector3d;
	typedef vector3<float> vector3f;

	// 6-dimensional vector
	typedef twist<double> twistd;
	typedef twist<float> twistf;
	typedef wrench<double> wrenchd;
	typedef wrench<float> wrenchf;

	// 3x3 matrix
	typedef so3<double> so3d;
	typedef so3<float> so3f;
	typedef SO3<double> SO3d;
	typedef SO3<float> SO3f;

	// 4x4 matrix
	typedef SE3<double> SE3d;
	typedef SE3<float> SE3f;
	typedef se3<double> se3d;
	typedef se3<float> se3f;
}


template <typename Scalar>
inline std::ostream& screw::operator<<(std::ostream& os, screw::SE3<Scalar>& T)
{
	//const Scalar *R = T.R_.m_storage.data();
	//const Scalar *p = T.p_.m_storage.data();

	//os << R[0] << " " << R[3] << " " << R[6] << " " << p[0] << std::endl
	//	<< R[1] << " " << R[4] << " " << R[7] << " " << p[1] << std::endl
	//	<< R[2] << " " << R[5] << " " << R[8] << " " << p[2] << std::endl
	//	<< "0 0 0 1" << std::endl;
//	os << T.toEigenMatrix();

	os << T.rotation() << std::endl;
	os << T.position() << std::endl;
	return os;
}



#endif
