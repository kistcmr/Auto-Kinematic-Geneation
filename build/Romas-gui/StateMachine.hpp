#ifndef __STATE_MACHINE__
#define __STATE_MACHINE__

#include <QWidget>
#include <QObject>

#include <romas/Block.hpp>

class AssembleMenuWidget;
class AssembleSpaceWidget;
class MainWindow;

enum class State : int
{
	Empty,
	NonEmpty,
	AssembleStep1,
	AssembleStep2,
	AssembleStep3,
	AssembleStep4,
	AssembleStep5,
	AssembleStep6,
	AssembleStep7,
};

class StateMachine : public QObject
{
	Q_OBJECT

public :
	StateMachine();
	void initialize(AssembleSpaceWidget *assemble_space_widget, AssembleMenuWidget *assemble_menu_widget);

public slots :

	void addBlock(romas::BlockInfo& block_info);
	void cancelAssemble(void);
	void goNextAssemble(void);
	void goPreviousAssemble(void);
	void finishAssemble(void);
	void pickParent(romas::Block *block, int joint_ID);
	void pickChild(romas::Block *block, int joint_ID);
	void changeParentMatchedAxis(romas::AXIS axis);
	void changeChildMatchedAxis(romas::AXIS axis);
	void setRevoluteJoint(void);
	void setPrismaticJoint(void);

	void loadAssembly(void);
	void saveAssembly(void);
	void resetAssembly(void);
	void headerFile(void);
	void simulator(void);

private :
	AssembleSpaceWidget *assemble_space_widget_;
	AssembleMenuWidget *assemble_menu_widget_;
	State current_state_;
};

#endif