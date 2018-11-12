#ifndef _SCREW_LINK_HPP_
#define _SCREW_LINK_HPP_

#include <screw/Lie.hpp>
#include <string>

namespace screw {

	class Link {

	public:
		Link();
		virtual ~Link();

		const int &id(void) const { return id_; }
		screw::System* system(void) const { return system_; }
		std::string name(void) const { return name_; }
		screw::SE3d frame(void) const { return frame_; }

		inline bool isBase(void) const;
		double mass(void) const { return mass_; }
		screw::vector3d centerOfMass(void) const { return com_; }
		//rmcp::dinertia& inertia(void) { return J_; }
		//void get_inertia(double I[]) { J_.get_inertia(I); }

		screw::Joint* parentJoint(void) const { return parent_joint_; }

		//const screw::SE3d& local_velocity(void) const { return Vl_; }
		//const screw::SE3d& local_acceleration(void) const { return dVl_; }
		//screw::SE3d global_velocity(void) { return rmcp::rotate(frame_, Vl_); }
		//screw::SE3d global_velocity_at(rmcp::dvector& pos) { return rmcp::rotate(frame_, Ad(-pos, Vl_)); } 

		void setID(const int id);
		void setName(std::string name) { name_ = name; }
		void setMass(double mass) { mass_ = mass;/*  J_.set_mass(mass);*/	}
		void setCenterOfMass(screw::vector3d com);
		void setFrame(screw::SE3d frame) { frame_ = frame; }
		//		void set_inertia(double Ixx, double Iyy, double Izz, double Ixy,
//		   		double Ixz, double Iyz);
//		void set_inertia(rmcp::dinertia& J);
//		void set_local_velocity(screw::SE3d Vl) { Vl_ = Vl; }
//		void set_local_acceleration(screw::SE3d dVl) { dVl_ = dVl; }

		bool build(screw::System* system, screw::LinkPointers& links, screw::JointPointers& joints);
		void connect(screw::Direction dir, screw::Joint* joint, const screw::SE3d& T);

		screw::JointPointers::iterator beginChildJoints(void) { return child_joints_.begin(); }
		screw::JointPointers::iterator endChildJoints(void) { return child_joints_.end(); }

//
//		screw::SE3d gravity_force(void);
//		screw::SE3d external_force(void);
//		
//		void add(Force* force);
//		void clear_force(void);
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	protected:
		screw::System* system_;	///< belonged system
		screw::Joint* parent_joint_;	///< parent joint
		screw::JointPointers child_joints_;	///< child joints
		//		Forces_t forces_;	///< external forces

		int id_;
		std::string name_;

		screw::SE3d frame_;		///< global frame		
		double mass_;						///< mass
		screw::vector3d com_;		///< center of mass
		//		rmcp::dinertia J_; // inertia
		//screw::SE3d Vl_; // local velocity
	//screw::SE3d dVl_; // local acceleration
	//screw::SE3d Fl_; // local force
	//rmcp::dainertia Jh_; // articulated inertia
	//screw::SE3d B_;// bias force
	//screw::SE3d C_;

	};


} // namespace screw

#endif 

