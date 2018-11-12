#ifndef _SCEW_FIXED_JOINT_HPP_
#define _SCREW_FIXED_JOINT_HPP_

#include <screw/Joint.hpp>

namespace screw {
		
	class FixedJoint : public screw::Joint
	{

	protected:
		screw::SE3d S_;
		screw::SE3d A_;
		screw::SE3d Jl_;
		screw::SE3d dJl_;		// time derivative of Spatial Jacobian

		double invOmega_;	
		
	public:
		FixedJoint();
		virtual ~FixedJoint();
		
		virtual	screw::JointType type(void) const { return screw::JointType::FIXED; }
		virtual unsigned int dof(void) const { return 0; } 

		virtual bool build(screw::System* system, screw::LinkPointers& link_members, screw::JointPointers& joint_members);

//		virtual void render(void); 

//		virtual void make_controller_pid(void);

		//virtual void set_axis(rmcp::dvector axis, size_t i = 0);
		//virtual void set_angle(double q, size_t i = 0);
		//virtual void set_offset(double qi, size_t i = 0);
		//virtual void set_velocity(double dq, size_t i = 0);
		//virtual void set_acceleration(double ddq, size_t i = 0);
		//virtual void set_torque(double tau, size_t i = 0);
		//virtual rmcp::dvector axis(size_t i = 0) const;
		//virtual double& angle(size_t i = 0);
		//virtual double& offset(size_t i = 0);
		//virtual double& velocity(size_t i = 0);
		//virtual double& acceleration(size_t i = 0);
		//virtual double& torque(size_t i = 0);

		//virtual screw::SE3d& get_S(void) { return S_; };

		//// kinematics, dynamics
		virtual void updateTransition(void);
		//virtual void update_local_velocity(void);
		//virtual void update_local_acceleration(void);
		//virtual void update_torque(const screw::SE3d& Fl);
		//virtual void update_ainertia_product(const rmcp::dainertia& Jh);
		//virtual void update_jcb(const rmcp::dainertia& Jh, const screw::SE3d& C,
		//	const screw::SE3d& B);
		//virtual void update_bias_force(void);
		//virtual void update_inverse_omega(const rmcp::dainertia& Jh);
		//virtual void update_angular_acceleration(const screw::SE3d& dVl,
		//	const rmcp::dainertia& Jh);
	private:

	};

} // namespace screw

#endif
