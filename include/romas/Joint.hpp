#ifndef __ROMAS_JOINT__
#define __ROMAS_JOINT__

#include <romas/Block.hpp>
#include <romas/Assembly.hpp>
#include <screw/Lie.hpp>

namespace romas {

	class Joint
	{
	public :
		Joint(romas::AssembleInfo& assemble_info);
		virtual ~Joint();

		void setAssembleInfo(romas::AssembleInfo& assemble_info) { assemble_info_ = assemble_info; }
		romas::AssembleInfo assembleInfo(void) { return assemble_info_; }

		virtual screw::SE3d transition(void) = 0;
		virtual void setPosition(double q) = 0;
		virtual void setOffset(double offset) = 0;

		void setFrame(screw::SE3d parent_frame, screw::SE3d child_frame);

		void setM(int i, screw::SE3d M);


		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected :
		romas::AssembleInfo assemble_info_;

		screw::SE3d parent_frame_;
		screw::SE3d child_frame_;
		screw::SE3d M0_;
		screw::SE3d M1_;
	};


	class FixedJoint : public romas::Joint
	{
	public:
		FixedJoint(romas::AssembleInfo& assemble_info);
		virtual ~FixedJoint();

		virtual screw::SE3d transition(void);
		virtual void setPosition(double q);
		virtual void setOffset(double offset);

	};

	class RevoluteJoint : public romas::Joint
	{
	public :
		RevoluteJoint(romas::AssembleInfo& assemble_info);
		virtual ~RevoluteJoint();

		virtual screw::SE3d transition(void);
		virtual void setPosition(double q);
		virtual void setOffset(double offset);

	protected :
		double q_;
		double offset_;
		screw::se3d A_;
	};


	class PrismaticJoint : public romas::Joint
	{
	public :
		PrismaticJoint(romas::AssembleInfo& assemble_info);
		virtual ~PrismaticJoint();

		virtual screw::SE3d transition(void);
		virtual void setPosition(double q);
		virtual void setOffset(double offset);

	protected:
		double q_;
		double offset_;
		screw::se3d A_;
	};

}






#endif






