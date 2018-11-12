#ifndef _ASSEMBLE_SPACE_WIDGET_
#define _ASSEMBLE_SPACE_WIDGET_

#include <QPoint>
#include <QOpenGLWidget>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include <osgGA/TrackballManipulator>
#include <osgText/String>
#include <osg/Quat>
#include <screw/Lie.hpp>
#include <romas/Block.hpp>
#include <QTimer>
#include <romas/Assembly.hpp>

#include "CentralWidget.hpp"
#include <romas/Config.hpp>

//#include <QTcpSocket>

namespace osgWidget {

	//! The subclass of osgViewer::CompositeViewer we use
	/*!
	 * This subclassing allows us to remove the annoying automatic
	 * setting of the CPU affinity to core 0 by osgViewer::ViewerBase,
	 * osgViewer::CompositeViewer's base class.
	 */
	class Viewer : public osgViewer::CompositeViewer
	{
	public:
		virtual void setUpThreading();
	};
}


class AssembleSpaceWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
//	AssembleSpaceWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	AssembleSpaceWidget(StateMachine *state_machine);
	virtual ~AssembleSpaceWidget();

	void moveCamera(romas::AXIS axis);

	bool addBlockInEmpty(romas::BlockInfo& block_info);
	void addBlockInNonEmpty(romas::BlockInfo block_info);
	void cancelAssemble(void);

	bool pickAssembleStep1(romas::Block *block, int joint_ID);
	bool pickAssembleStep2(romas::Block *block, int joint_ID);

	void finishAssemble(void);

	void getRotAxis(romas::AXIS& parent, romas::AXIS& child);
	romas::JointType getJointType(void);
	void getContactPointType(romas::ContactPointType& parent, romas::ContactPointType& child);

	void updateBlockCandidateByRotAxis(void);
	void setMatchedAxis(romas::AXIS parent_axis, romas::AXIS child_axis);
	void changeParentMatchedAxis(romas::AXIS axis);
	void changeChildMatchedAxis(romas::AXIS axis);

	// use axis1
	void updateBlockCandidateFrame1(void);
	// use axis1, axis2
	screw::SO3d updateBlockCandidateFrame2(void);

	void setRevoluteJoint(void);
	void setPrismaticJoint(void);

	void loadAssembly(std::string filename);
	void saveAssembly(std::string filename);
	void resetAssembly(void);

	void generateHeaderFile(std::string directory, std::string robot_name, romas::Solver solver, std::string define);

	void setConfig(romas::Config *config);

	bool simulator(void);

	QString getDifference(void);


public slots :
	void onPickTimerTimeout(void);

protected:
	virtual void paintEvent(QPaintEvent* paintEvent);
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual bool event(QEvent* event);

private:
	virtual void onHome();
	virtual void onResize(int width, int height);

	osgGA::EventQueue* getEventQueue() const;
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgWidget::Viewer> viewer_;
	osg::Camera* camera_;
	osgGA::TrackballManipulator* trackball_manipulator_;
	osgViewer::View* view_;
	
	QPoint selectionStart_;
	QPoint selectionEnd_;

	bool selectionActive_;
	bool selectionFinished_;

	void processSelection();

	osg::Group *scene_root_;
//	osg::ref_ptr<osg::Switch> switchNode;

	screw::SE3d T_block_candidate_;
	screw::vector3d p_block_candidate_;

	romas::AssembleInfo assemble_info_;
	romas::Assembly *assembly_;
	romas::Block *parent_block_;
	romas::Block *block_candidate_;

	CentralWidget *central_widget_;

	QTimer pick_timer_;
	std::vector<osg::Geode*> pick_nodes_;

	StateMachine *state_machine_;
	romas::Config *config_;

	bool now_simulating_;

//	QTcpSocket *socket_;
};

#endif
