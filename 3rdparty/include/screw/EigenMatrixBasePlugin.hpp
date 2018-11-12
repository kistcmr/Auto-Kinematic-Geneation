
inline void set(Eigen::Matrix<Scalar, 3, 1> angular, Eigen::Matrix<Scalar, 3, 1> linear)
{
	this->block(0, 0, 3, 1) = angular;
	this->block(3, 0, 3, 1) = linear;
}




