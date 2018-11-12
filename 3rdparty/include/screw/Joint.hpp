#ifndef _SCREW_JOINT_HPP_
#define _SCREW_JOINT_HPP_

#include <screw/screw.hpp>
#include <screw/Lie.hpp>

namespace screw {

	class Joint {
		friend class Link;
		friend class System;

	public:
		Joint();
		virtual ~Joint();
		
		const int& id(void) const { return id_; }
		virtual size_t dof(void) const = 0;
		virtual	screw::JointType type(void) const = 0;	
		screw::System* system(void) const { return system_; }
		const std::string& name(void) const { return name_; }

		screw::Link* parentLink(void) const { return parent_link_; }
		screw::Link* childLink(void) const { return child_link_; }
		const screw::SE3d& parentFrame(void) const { return parent_frame_; }
		const screw::SE3d& childFrame(void) const { return child_frame_; }

		void setID(const int id);
		void setName(std::string name) { name_ = name; }

		virtual bool build(screw::System* system, screw::LinkPointers& link_members, screw::JointPointers& joint_members);

		virtual void setAxis(screw::vector3d axis, size_t i = 0) = 0;
		virtual void setPosition(double q, size_t i = 0) = 0;
		//virtual void set_offset(double offset, size_t i = 0) = 0;
		//virtual void set_velocity(double dq, size_t i = 0) = 0;
		//virtual void set_acceleration(double ddq, size_t i = 0) = 0;
		//virtual void set_torque(double tau, size_t i = 0) = 0;
		virtual screw::vector3d axis(size_t i = 0) = 0;
		virtual double position(size_t i = 0) = 0;
		//virtual double& offset(size_t i = 0) = 0;
		//virtual double& velocity(size_t i = 0) = 0;
		//virtual double& acceleration(size_t i = 0) = 0;
		//virtual double& torque(size_t i = 0) = 0;

//		virtual screw::SE3d& get_S(void) = 0;
		screw::SE3d& transition(void) { return transition_; }

		//// kinematics, dynamics
		virtual void updateTransition(void) = 0;
		//virtual void update_local_velocity(void) = 0;
		//virtual void update_local_acceleration(void) = 0;
		//virtual void update_torque(const screw::SE3d& Fl) = 0;
		//virtual void update_ainertia_product(const rmcp::dainertia& Jh) = 0;
		//virtual void update_jcb(const rmcp::dainertia& Jh, const screw::SE3d& C,
		//	const screw::SE3d& B) = 0;
		//virtual void update_bias_force(void) = 0;
		//virtual void update_inverse_omega(const rmcp::dainertia& Jh) = 0;
		//virtual void update_angular_acceleration(const screw::SE3d& dVl,
		//	const rmcp::dainertia& Jh) = 0;
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:
		screw::System* system_;
		screw::Link* parent_link_;
		screw::Link* child_link_;
		screw::SE3d parent_frame_;
		screw::SE3d child_frame_;

		screw::SE3d transition_;
		screw::SE3d M_;
		screw::SE3d Vl_;
		screw::SE3d dVl_;
		screw::SE3d JCB_;
		screw::SE3d B_;
		//		rmcp::dainertia JhS_;

		int id_;
		std::string name_;

		// just called only by Link class
		void _connectParent(screw::Link* link, const screw::SE3d T);
		void _connectChild(screw::Link* link, const screw::SE3d T);

	};

} // namespace screw

#endif
