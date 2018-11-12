#ifndef __PICK_HANDLER_HPP__
#define __PICK_HANDLER_HPP__

#include <osgGA/GUIEventHandler>

class PickHandler : public osgGA::GUIEventHandler
{
public:
	virtual ~PickHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

#endif
