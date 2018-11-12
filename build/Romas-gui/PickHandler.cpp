#include "PickHandler.hpp"

#include <osg/io_utils>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/RayIntersector>
#include <osgViewer/Viewer>

#include <iostream>

PickHandler::~PickHandler()
{
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE &&
		ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		return false;
	}

	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (viewer)
	{
		//    osgUtil::LineSegmentIntersector* intersector
		//        = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, ea.getX(), ea.getY() );

		osgUtil::RayIntersector* intersector
			= new osgUtil::RayIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

		osgUtil::IntersectionVisitor iv(intersector);

		osg::Camera* camera = viewer->getCamera();
		if (!camera)
			return false;

		camera->accept(iv);

		if (!intersector->containsIntersections())
			return false;

		//   auto intersections = intersector->getIntersections();

		   //auto itr = intersections.begin();
	   //	itr->

		  //std::cout << "Got " << intersections.size() << " intersections:\n";
		  //for (auto&& intersection : intersections)
		  //{
			 // std::cout << "  - Local intersection point = " << intersection.localIntersectionPoint << "\n";
			 // std::string name = intersection.drawable->getName();
			 // const char * n = name.c_str();   
		  //}

		osgUtil::RayIntersector::Intersection intersection = intersector->getFirstIntersection();

		osg::NodePath& nodePath = intersection.nodePath;
		osg::Node *node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
		std::string name = node->getName();

		node->setName(name + std::string("p"));
		std::string name2 = node->getName();

//		parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
//		if (node) std::cout << "  Hits " << node->className() << " nodePath size " << nodePath.size() << std::endl;
//		toggleScribe(parent, node);

	}

	return true;
}
