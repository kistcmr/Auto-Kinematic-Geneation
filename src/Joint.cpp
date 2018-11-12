
#include <romas/Joint.hpp>

romas::Joint::Joint(romas::AssembleInfo& assemble_info) 
	: assemble_info_(assemble_info)
{

};

romas::Joint::~Joint() 
{


};

void romas::Joint::setFrame(screw::SE3d parent_frame, screw::SE3d child_frame)
{
	parent_frame_ = parent_frame;
	child_frame_ = child_frame;
}

void romas::Joint::setM(int i, screw::SE3d M)
{
	if (i == 0) M0_ = M;
	else if(i == 1) M1_ = M;
}


romas::FixedJoint::FixedJoint(romas::AssembleInfo& assemble_info)
	: Joint(assemble_info)
{

}

romas::FixedJoint::~FixedJoint()
{

}

screw::SE3d romas::FixedJoint::transition(void)
{
	screw::SE3d T = child_frame_.inverse();

	return parent_frame_ * M0_ * M1_ * T;
}

void romas::FixedJoint::setPosition(double q)
{

}

void romas::FixedJoint::setOffset(double offset)
{

}

romas::RevoluteJoint::RevoluteJoint(romas::AssembleInfo& assemble_info)
	: Joint(assemble_info)
	, q_(0)
	, offset_(0)
{
//	A_.set(screw::vector3d(0, 0, 1), screw::vector3d(0, 0, 0));
	A_ = screw::se3d(0, 0, 1, 0, 0, 0);
}

romas::RevoluteJoint::~RevoluteJoint()
{

}

screw::SE3d romas::RevoluteJoint::transition(void)
{
	screw::SE3d T = child_frame_.inverse();
	screw::SE3d trans = screw::exp(A_, q_);

	return parent_frame_ * M0_ * trans * M1_ * T;
}

void romas::RevoluteJoint::setPosition(double q)
{
	q_ = q;
}

void romas::RevoluteJoint::setOffset(double offset)
{
	offset_ = offset;
}

romas::PrismaticJoint::PrismaticJoint(romas::AssembleInfo& assemble_info)
	: Joint(assemble_info)
	, q_(0)
	, offset_(0)
{
//	A_.set(screw::vector3d(0, 0, 0), screw::vector3d(0, 0, 1));
	A_ = screw::se3d(0, 0, 0, 0, 0, 1);
}

romas::PrismaticJoint::~PrismaticJoint()
{

}

screw::SE3d romas::PrismaticJoint::transition(void)
{
	screw::SE3d T = child_frame_.inverse();
	screw::SE3d trans = screw::exp(A_, q_);

	return parent_frame_ * M0_ * trans * M1_ * T;

}

void romas::PrismaticJoint::setPosition(double q)
{
	q_ = q;
}

void romas::PrismaticJoint::setOffset(double offset)
{
	offset_ = offset;
}
