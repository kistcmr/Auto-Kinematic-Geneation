#include "AssembleSpaceWidget.hpp"
#include "PickHandler.hpp"
#include "MainWindow.hpp"

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <osgDB/WriteFile>
#include <osgDB/readFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osg/LineWidth>
#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QMessageBox>

#include <romas/ustring.hpp>

#include <screw/RevoluteJoint.hpp>

//#include <QTcpSocket>

#define WITH_SELECTION_PROCESSING 1

namespace
{

#ifdef WITH_SELECTION_PROCESSING
	QRect makeRectangle(const QPoint& first, const QPoint& second)
	{
		// Relative to the first point, the second point may be in either one of the
		// four quadrants of an Euclidean coordinate system.
		//
		// We enumerate them in counter-clockwise order, starting from the lower-right
		// quadrant that corresponds to the default case:
		//
		//            |
		//       (3)  |  (4)
		//            |
		//     -------|-------
		//            |
		//       (2)  |  (1)
		//            |

		if (second.x() >= first.x() && second.y() >= first.y())
			return QRect(first, second);
		else if (second.x() < first.x() && second.y() >= first.y())
			return QRect(QPoint(second.x(), first.y()), QPoint(first.x(), second.y()));
		else if (second.x() < first.x() && second.y() < first.y())
			return QRect(second, first);
		else if (second.x() >= first.x() && second.y() < first.y())
			return QRect(QPoint(first.x(), second.y()), QPoint(second.x(), first.y()));

		// Should never reach that point...
		return QRect();
	}
#endif

}

namespace osgWidget {
	void Viewer::setUpThreading()
	{
		if (_threadingModel == SingleThreaded) {
			if (_threadsRunning) {
				stopThreading();
			}
		}
		else {
			if (!_threadsRunning) {
				startThreading();
			}
		}
	}
}

AssembleSpaceWidget::AssembleSpaceWidget(StateMachine *state_machine)
	: QOpenGLWidget(Q_NULLPTR)
	, state_machine_(state_machine)
	, graphicsWindow_(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height()))
	, viewer_(new osgWidget::Viewer)
	, selectionActive_(false)
	, selectionFinished_(true)
	, pick_timer_(this)
{
	scene_root_ = new osg::Group();
	scene_root_->addChild(romas::createLineCoordinate(100));

	float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());

	camera_ = new osg::Camera;
	camera_->setViewport(0, 0, this->width(), this->height());
	camera_->setClearColor(osg::Vec4(0.9f, 0.9f, 0.9f, 1.f));
	camera_->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
	camera_->setGraphicsContext(graphicsWindow_);

	view_ = new osgViewer::View;
	view_->setCamera(camera_);
	view_->setSceneData(scene_root_);
	view_->addEventHandler(new osgViewer::StatsHandler);
	view_->addEventHandler(new PickHandler);

	trackball_manipulator_ = new osgGA::TrackballManipulator;
	trackball_manipulator_->setAllowThrow(false);
	trackball_manipulator_->setAutoComputeHomePosition(false);

	view_->setCameraManipulator(trackball_manipulator_);

	moveCamera(romas::AXIS_PLUS_X);

	viewer_->addView(view_);
	viewer_->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	viewer_->realize();

	// This ensures that the widget will receive keyboard events. This focus
	// policy is not set by default. The default, Qt::NoFocus, will result in
	// keyboard events that are ignored.
	this->setFocusPolicy(Qt::StrongFocus);
	this->setMinimumSize(100, 100);

	// Ensures that the widget receives mouse move events even though no
	// mouse button has been pressed. We require this in order to let the
	// graphics window switch viewports properly.
	this->setMouseTracking(true);

	connect(&pick_timer_, SIGNAL(timeout()), this, SLOT(onPickTimerTimeout()));

	block_candidate_ = nullptr;
	assembly_ = nullptr;

	T_block_candidate_ = screw::SE3d(screw::vector3d(0.2, .2, 0.5));
	p_block_candidate_ = screw::vector3d(0.2, .2, 0.5);

	config_ = nullptr;

	now_simulating_ = false;
}

AssembleSpaceWidget::~AssembleSpaceWidget()
{
}

void AssembleSpaceWidget::paintEvent(QPaintEvent* /* paintEvent */)
{
	this->makeCurrent();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	this->paintGL();

#ifdef WITH_SELECTION_PROCESSING
	if (selectionActive_ && !selectionFinished_)
	{
		painter.setPen(Qt::black);
		painter.setBrush(Qt::transparent);
		painter.drawRect(makeRectangle(selectionStart_, selectionEnd_));
	}
#endif

	painter.end();

	this->doneCurrent();
}

void AssembleSpaceWidget::paintGL()
{
	viewer_->frame();
}

void AssembleSpaceWidget::resizeGL(int width, int height)
{
	this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
	graphicsWindow_->resized(this->x(), this->y(), width, height);

	this->onResize(width, height);
}

void AssembleSpaceWidget::keyPressEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	if (event->key() == Qt::Key_S)
	{
#ifdef WITH_SELECTION_PROCESSING
		selectionActive_ = !selectionActive_;
#endif

		// Further processing is required for the statistics handler here, so we do
		// not return right away.
	}
	else if (event->key() == Qt::Key_D)
	{
		osgDB::writeNodeFile(*viewer_->getView(0)->getSceneData(), "/tmp/sceneGraph.osg");

		return;
	}
	//else if (event->key() == Qt::Key_H)
	//{
	//	static bool a = true;
	//	if (a)
	//	{
	//		switchNode->setAllChildrenOff();
	//		a = false;
	//	}
	//	else
	//	{
	//		switchNode->setAllChildrenOn();
	//		a = true;
	//	}


	//	this->onHome();
	//	return;
	//}

	this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void AssembleSpaceWidget::keyReleaseEvent(QKeyEvent* event)
{
	QString keyString = event->text();
	const char* keyData = keyString.toLocal8Bit().data();

	this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void AssembleSpaceWidget::mouseMoveEvent(QMouseEvent* event)
{
	// Note that we have to check the buttons mask in order to see whether the
	// left button has been pressed. A call to `button()` will only result in
	// `Qt::NoButton` for mouse move events.
	if (selectionActive_ && event->buttons() & Qt::LeftButton)
	{
		selectionEnd_ = event->pos();

		// Ensures that new paint events are created while the user moves the
		// mouse.
		this->update();
	}
	else
	{
		this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
			static_cast<float>(event->y()));
	}
}

void AssembleSpaceWidget::mousePressEvent(QMouseEvent* event)
{
	// Selection processing
	if (selectionActive_ && event->button() == Qt::LeftButton)
	{
		selectionStart_ = event->pos();
		selectionEnd_ = selectionStart_; // Deletes the old selection
		selectionFinished_ = false;           // As long as this is set, the rectangle will be drawn
	}

	// Normal processing
	else
	{
		// 1 = left mouse button
		// 2 = middle mouse button
		// 3 = right mouse button

		unsigned int button = 0;

		switch (event->button())
		{
		case Qt::LeftButton:
			button = 1;
			break;

		case Qt::MiddleButton:
			button = 2;
			break;

		case Qt::RightButton:
			button = 3;
			break;

		default:
			break;
		}

		this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()),
			static_cast<float>(event->y()),
			button);
	}
}

void AssembleSpaceWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// Selection processing: Store end position and obtain selected objects
	// through polytope intersection.
	if (selectionActive_ && event->button() == Qt::LeftButton)
	{
		selectionEnd_ = event->pos();
		selectionFinished_ = true; // Will force the painter to stop drawing the
								   // selection rectangle

		this->processSelection();
	}

	// Normal processing
	else
	{
		// 1 = left mouse button
		// 2 = middle mouse button
		// 3 = right mouse button

		unsigned int button = 0;

		switch (event->button())
		{
		case Qt::LeftButton:
			button = 1;
			break;

		case Qt::MiddleButton:
			button = 2;
			break;

		case Qt::RightButton:
			button = 3;
			break;

		default:
			break;
		}

		this->getEventQueue()->mouseButtonRelease(static_cast<float>(event->x()),
			static_cast<float>(event->y()),
			button);
	}
}

void AssembleSpaceWidget::wheelEvent(QWheelEvent* event)
{
	// Ignore wheel events as long as the selection is active.
	if (selectionActive_)
		return;

	event->accept();
	int delta = event->delta();

	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
		: osgGA::GUIEventAdapter::SCROLL_DOWN;

	this->getEventQueue()->mouseScroll(motion);
}

bool AssembleSpaceWidget::event(QEvent* event)
{
	bool handled = QOpenGLWidget::event(event);

	// This ensures that the OSG widget is always going to be repainted after the
	// user performed some interaction. Doing this in the event handler ensures
	// that we don't forget about some event and prevents duplicate code.
	switch (event->type())
	{
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
	case QEvent::Wheel:
		this->update();
		break;

	default:
		break;
	}

	return handled;
}

void AssembleSpaceWidget::onHome()
{
	osgViewer::ViewerBase::Views views;
	viewer_->getViews(views);

	for (std::size_t i = 0; i < views.size(); i++)
	{
		osgViewer::View* view = views.at(i);
		view->home();
	}
}

void AssembleSpaceWidget::onResize(int width, int height)
{
	camera_->setViewport(0, 0, width, height);
}

osgGA::EventQueue* AssembleSpaceWidget::getEventQueue() const
{
	osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();

	if (eventQueue)
		return eventQueue;
	else
		throw std::runtime_error("Unable to obtain valid event queue");
}

void AssembleSpaceWidget::processSelection()
{
#ifdef WITH_SELECTION_PROCESSING
	QRect selectionRectangle = makeRectangle(selectionStart_, selectionEnd_);
	int widgetHeight = this->height();

	double xMin = selectionRectangle.left();
	double xMax = selectionRectangle.right();
	double yMin = widgetHeight - selectionRectangle.bottom();
	double yMax = widgetHeight - selectionRectangle.top();

	osgUtil::PolytopeIntersector* polytopeIntersector
		= new osgUtil::PolytopeIntersector(osgUtil::PolytopeIntersector::WINDOW,
			xMin, yMin,
			xMax, yMax);

	// This limits the amount of intersections that are reported by the
	// polytope intersector. Using this setting, a single drawable will
	// appear at most once while calculating intersections. This is the
	// preferred and expected behaviour.
	polytopeIntersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE);

	osgUtil::IntersectionVisitor iv(polytopeIntersector);

	for (unsigned int viewIndex = 0; viewIndex < viewer_->getNumViews(); viewIndex++)
	{
		osgViewer::View* view = viewer_->getView(viewIndex);

		if (!view)
			throw std::runtime_error("Unable to obtain valid view for selection processing");

		camera_->accept(iv);

		if (!polytopeIntersector->containsIntersections())
			continue;

		auto intersections = polytopeIntersector->getIntersections();

		for (auto&& intersection : intersections)
			qDebug() << "Selected a drawable:" << QString::fromStdString(intersection.drawable->getName());
	}
#endif
}


void AssembleSpaceWidget::moveCamera(romas::AXIS axis)
{
	osg::Vec3d center;
	osg::Vec3d eye;
	osg::Vec3d up;

	switch (axis)
	{
	case romas::AXIS_PLUS_X :
		center.set(0.0, 0.0, 0.0);
		eye.set(2, 0, 0.);
		up.set(0., 0., 1.0);
		break;
	case romas::AXIS_MINUS_X :
		center.set(0.0, 0.0, 0.0);
		eye.set(-2, 0, 0.);
		up.set(0., 0., 1.0);
		break;
	case romas::AXIS_PLUS_Y :
		center.set(0.0, 0.0, 0.0);
		eye.set(0, 2, 0.);
		up.set(0., 0., 1.0);
		break;
	case romas::AXIS_MINUS_Y:
		center.set(0.0, 0.0, 0.0);
		eye.set(0, -2, 0.);
		up.set(0., 0., 1.0);
		break;
	case romas::AXIS_PLUS_Z:
		center.set(0.0, 0.0, 0.0);
		eye.set(0, 0, 2.);
		up.set(0., 1., 0.0);
		break;
	case romas::AXIS_MINUS_Z:
		center.set(0., 0., 0.);
		eye.set(0, 0, -2.);
		up.set(0., 1, 0.);
		break;
	}

	trackball_manipulator_->setHomePosition(eye, center, up);
	trackball_manipulator_->home(0);
	this->update();
}

bool AssembleSpaceWidget::addBlockInEmpty(romas::BlockInfo& block_info)
{
	if (assembly_)
		return false;

	assembly_ = new romas::Assembly(scene_root_);
	romas::Block *block = new romas::Block(block_info);

	assembly_->addBaseBlock(block);

	std::vector<osg::Geode*> joint_markers = block->jointMarkerGeodes();

	for (std::vector<osg::Geode*>::iterator itr = joint_markers.begin(); itr != joint_markers.end(); itr++)
	{
		pick_nodes_.push_back(*itr);
	}

	this->update();

	return true;
}

void AssembleSpaceWidget::addBlockInNonEmpty(romas::BlockInfo block_info)
{
	block_candidate_ = new romas::Block(block_info);

	std::vector<osg::Geode*> joint_markers = block_candidate_->jointMarkerGeodes();

	for (std::vector<osg::Geode*>::iterator itr = joint_markers.begin(); itr != joint_markers.end(); itr++)
	{
		pick_nodes_.push_back(*itr);
	}

	pick_timer_.start(200);
}

void AssembleSpaceWidget::onPickTimerTimeout(void)
{
//	for (std::vector<osg::Node*>::iterator itr = pick_nodes_.begin(); itr != pick_nodes_.end(); itr++)
	for (std::vector<osg::Geode*>::iterator itr = pick_nodes_.begin(); itr != pick_nodes_.end(); itr++)
	{
		std::string name = (*itr)->getName();
		if ((name.size() > 0) && (name.back() == 'p'))
		{
			name.pop_back();
			(*itr)->setName(name);

			std::vector<std::string> token;
			romas::tokenize(name, ";", token);
			if (token.size() == 2)
			{
				int block_assemble_id = romas::lexical_cast<int, std::string>(token[0]);
				int joint_id = romas::lexical_cast<int, std::string>(token[1]);

				if (block_assemble_id == -1)
					state_machine_->pickChild(block_candidate_, joint_id);
				else
				{
					romas::Block* block = assembly_->findBlock(block_assemble_id);
					state_machine_->pickParent(block, joint_id);
				}
			}

//			name = (*itr)->getName();
		}
	}
}

bool AssembleSpaceWidget::pickAssembleStep1(romas::Block *block, int joint_ID)
{
	parent_block_ = block;
	assemble_info_.parent_contact_point_id = joint_ID;
	assemble_info_.parent_contact_point_type = parent_block_->getContactPointType(joint_ID);
	assemble_info_.parent_rot_axis = parent_block_->getContactPointRotAxis(joint_ID);
	assemble_info_.parent_zero_axis = parent_block_->getContactPointZeroAxis(joint_ID);

	scene_root_->addChild(block_candidate_);
	block_candidate_->setFrame(T_block_candidate_);

	this->update();
	return true;
}

bool AssembleSpaceWidget::pickAssembleStep2(romas::Block *block, int joint_ID)
{
	if (!block)
		return false;

	if (parent_block_ == block)
		return false;

	if (block_candidate_ != block)
		return false;

	assemble_info_.child_contact_point_id = joint_ID;
	assemble_info_.child_contact_point_type = block->getContactPointType(joint_ID);
	assemble_info_.child_rot_axis = block->getContactPointRotAxis(joint_ID);
	assemble_info_.child_zero_axis = block->getContactPointZeroAxis(joint_ID);

	if ((assemble_info_.parent_contact_point_type == romas::ContactPointType::Assembly) &&
		(assemble_info_.child_contact_point_type == romas::ContactPointType::Assembly))
	{
		assemble_info_.joint_type = romas::JointType::Fixed;
		return true;
	}
	else if ((assemble_info_.parent_contact_point_type == romas::ContactPointType::Assembly) &&
		(assemble_info_.child_contact_point_type == romas::ContactPointType::Joint))
	{
		assemble_info_.joint_type = romas::JointType::Revolute;
		return true;
	}
	else if ((assemble_info_.parent_contact_point_type == romas::ContactPointType::Joint) &&
		(assemble_info_.child_contact_point_type == romas::ContactPointType::Assembly))
	{
		assemble_info_.joint_type = romas::JointType::Revolute;
		return true;
	}
	else
	{
		QMessageBox dialog(
			QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("Joint 속성끼리는 연결할 수 없습니다."),
			QMessageBox::StandardButton::Ok
		);
		dialog.exec();

		return false;
	}
}

void AssembleSpaceWidget::updateBlockCandidateByRotAxis(void)
{
//	assemble_info_.parent_rot_axis = parent_axis;
//	assemble_info_.child_rot_axis = child_axis;

	updateBlockCandidateFrame1();
}

void AssembleSpaceWidget::setMatchedAxis(romas::AXIS parent_axis, romas::AXIS child_axis)
{
	assemble_info_.parent_matched_axis = parent_axis;
	assemble_info_.child_matched_axis = child_axis;

	assemble_info_.R = updateBlockCandidateFrame2();
}

void AssembleSpaceWidget::changeParentMatchedAxis(romas::AXIS axis)
{
	assemble_info_.parent_matched_axis = axis;
	updateBlockCandidateFrame2();
}

void AssembleSpaceWidget::changeChildMatchedAxis(romas::AXIS axis)
{
	assemble_info_.child_matched_axis = axis;
	updateBlockCandidateFrame2();
}

void AssembleSpaceWidget::updateBlockCandidateFrame1(void)
{
	if (assemble_info_.parent_contact_point_id < 0)
		return;

	screw::SE3d T_g0 = parent_block_->frame();
	screw::SE3d T_01 = parent_block_->getContactPointFrame(assemble_info_.parent_contact_point_id);
	screw::SE3d T_g1 = T_g0 * T_01;

	screw::SO3d R_g1 = T_g1.rotation();

	screw::SO3d R_12 = romas::calculateRotation(assemble_info_.parent_rot_axis, assemble_info_.child_rot_axis);

	screw::SO3d R_g2 = R_g1 * R_12;

	screw::SE3d T_g2(R_g2, p_block_candidate_);
	screw::SE3d T_32 = block_candidate_->getContactPointLocalFrame(assemble_info_.child_contact_point_id);
	screw::SE3d T_g3 = T_g2 * T_32.inverse();
	this->update();
}

screw::SO3d AssembleSpaceWidget::updateBlockCandidateFrame2(void)
{
	if (assemble_info_.parent_contact_point_id < 0)
		return screw::SO3d();

//	screw::SE3d T_g0 = parent_block_->frame();
	screw::SE3d T_g1 = parent_block_->getContactPointFrame(assemble_info_.parent_contact_point_id);
//	screw::SE3d T_g1 = T_g0 * T_01;

	screw::SO3d R_g1 = T_g1.rotation();

	screw::SO3d R_12 = romas::calculateRotation(assemble_info_.parent_rot_axis, assemble_info_.child_rot_axis, assemble_info_.parent_matched_axis, assemble_info_.child_matched_axis);

	screw::SO3d R_g2 = R_g1 * R_12;

	screw::SE3d T_g2(R_g2, p_block_candidate_);
	screw::SE3d T_32 = block_candidate_->getContactPointLocalFrame(assemble_info_.child_contact_point_id);
	screw::SE3d T_g3 = T_g2 * T_32.inverse();
	block_candidate_->setFrame(T_g3);

	this->update();
	return R_12;
}

void AssembleSpaceWidget::cancelAssemble(void)
{
	pick_timer_.stop();

	scene_root_->removeChild(block_candidate_);
	block_candidate_ = nullptr;

	pick_nodes_.clear();

	std::vector<osg::Geode*> joint_markers = parent_block_->jointMarkerGeodes();

	for (std::vector<osg::Geode*>::iterator itr = joint_markers.begin(); itr != joint_markers.end(); itr++)
	{
		pick_nodes_.push_back(*itr);
	}

	this->update();
}

void AssembleSpaceWidget::finishAssemble(void)
{
	pick_timer_.stop();
	romas::Block *block = new romas::Block(block_candidate_->blockInfo());

	assembly_->addJointAndBlock(assemble_info_, block);

	assembly_->forwardKinematics();

	scene_root_->removeChild(block_candidate_);
	block_candidate_ = nullptr;

	pick_nodes_.clear();

	std::vector<osg::Geode*> joint_markers = block->jointMarkerGeodes();

	for (std::vector<osg::Geode*>::iterator itr = joint_markers.begin(); itr != joint_markers.end(); itr++)
	{
		pick_nodes_.push_back(*itr);
	}

	this->update();
}

void AssembleSpaceWidget::loadAssembly(std::string filename)
{
	if (assembly_)
		return;

	assembly_ = new romas::Assembly(scene_root_);

	assembly_->setBlockLibraryPath(config_->blockLibraryPath());
	assembly_->load(filename);
}

void AssembleSpaceWidget::saveAssembly(std::string filename)
{
	if (!assembly_)
		return;

	assembly_->save(filename);
}

void AssembleSpaceWidget::resetAssembly(void)
{
	if (!assembly_)
		return;

	delete assembly_;

	assembly_ = nullptr;

	pick_nodes_.clear();

	this->update();
}

void AssembleSpaceWidget::generateHeaderFile(std::string directory, std::string robot_name, romas::Solver solver, std::string define)
{
	if (!assembly_)
		return;

	assembly_->setVcvarsPath(config_->vcvarsallPath());
	assembly_->setMsbuildPath(config_->msbuildPath());
	assembly_->generateHeaderFile(directory, robot_name, solver, define);
}

void AssembleSpaceWidget::setRevoluteJoint(void)
{
	assemble_info_.joint_type = romas::JointType::Revolute;
}

void AssembleSpaceWidget::setPrismaticJoint(void)
{
	assemble_info_.joint_type = romas::JointType::Prismatic;
}

void AssembleSpaceWidget::getRotAxis(romas::AXIS& parent, romas::AXIS& child)
{
	parent = assemble_info_.parent_rot_axis;
	child = assemble_info_.child_rot_axis;
}

romas::JointType AssembleSpaceWidget::getJointType(void)
{
	return assemble_info_.joint_type;
}

void AssembleSpaceWidget::getContactPointType(romas::ContactPointType& parent, romas::ContactPointType& child)
{
	parent = assemble_info_.parent_contact_point_type;
	child = assemble_info_.child_contact_point_type;
}

void AssembleSpaceWidget::setConfig(romas::Config *config)
{
	config_ = config;
}

bool AssembleSpaceWidget::simulator(void)
{
	if (now_simulating_)
	{
//		socket_->


	}
	else {
//		socket_ = new QTcpSocket();
//		socket_->connectToHost()
	}

	return true;

}

QString AssembleSpaceWidget::getDifference(void)
{
	return QString("");
}

