#ifndef _SCREW_SYSTEM_HPP_
#define _SCREW_SYSTEM_HPP_

#include <screw/screw.hpp>
#include <screw/Lie.hpp>
#include <string>

namespace screw {

	class System {

	public:
		System();
		virtual ~System();

		const int& id(void) const { return id_; }
		const std::string& name(void) const { return name_; }
		const bool& fixed(void) const { return fixed_; }
		screw::Link* baseLink(void) const { return base_link_; }
		const screw::SE3d frame(void) const;

		void setID(const int id);
		void setName(std::string name) { name_ = name; }
		void setFixed(bool fixed) { fixed_ = fixed; }
		void setFrame(screw::SE3d frame);

		//		virtual void change_support(int support) { support_ = support; }
		//		void set_support_joint(Joint* joint) { support_joint_ = joint; }
		//		void set_support_link(Link* link) { support_link_ = link; }
		//		void set_do_movement(bool yesno);
		//		void set_render_geometry(bool yesno) { render_geometry_ = yesno; }
		//		void go_kinematics(double &timestep);
		//		void step_kinematics(int step);

		//		void go_dynamics(double &timestep);
		//

		void setBase(Link* link) { base_link_ = link; };

		//		virtual void initialize(void);

		virtual bool build(void);

		//		virtual void control(double &timestep);
		////		virtual void render(void);
		//		void clear_force(void);
		////		void control2(double &timestep);
		//
		//		rmcp::dvector calc_com(void);
		//
		//		void clear_state(void);
		//		bool load(std::string filename);	// mvm file load
		//		void unload(void);
		//		bool save(std::string filename);	// mvm file save

		//		void update_vcollide(VCollide *vc);

		screw::Joint* findJoint(const int joint_id);
		screw::Link* findLink(const int link_id);

		//Links_t::iterator begin_link(void) { return link_members_.begin(); }
				//Links_t::iterator end_link(void) { return link_members_.end(); }

		virtual void forward_kinematics(void);
				//virtual void inverse_dynamics(void);
				//virtual void forward_dynamics(void);
				//void integrate_dynamics_euler(const double& timestep);
				//void integrate_dynamics_rk4(const double& timestep);
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW


	protected:
		screw::Link *base_link_;
		bool fixed_;

		int id_;
		std::string name_;

		screw::LinkPointers link_members_;
		screw::JointPointers joint_members_;

		unsigned int dof_;

		void forward_kinematics_r(Link* link, const screw::SE3d& T);
		//void forward_kinematics_b(Joint* joint, const screw::SE3d& T);
		///* parent joint of current link */
		//void inverse_dynamics_1st(void);
		//void inverse_dynamics_2nd(void);
		//void inverse_dynamics_3rd(void);
		//void forward_dynamics_1st(void);
		//void forward_dynamics_2nd(void);
		//void forward_dynamics_3rd(void);
		//void forward_dynamics_4th(void);
		//void inverse_dynamics_1st_r(Link* link, const screw::SE3d& Vl, const screw::SE3d& dVl);
		//screw::SE3d& inverse_dynamics_2nd_r(Link* link);
		//void forward_dynamics_1st_r(Link* link, const screw::SE3d& Vl);
		//void forward_dynamics_2nd_r(Link* link);
		//void forward_dynamics_3rd_r(Link* link);
	};

} // namespace screw

#endif
