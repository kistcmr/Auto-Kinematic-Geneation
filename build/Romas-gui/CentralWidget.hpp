#ifndef __CENTRAL_WIDGET__
#define __CENTRAL_WIDGET__

#include <QWidget>
#include "StateMachine.hpp"
#include <romas/Block.hpp>

class AssembleMenuWidget;
class AssembleSpaceWidget;
class MainWindow;

class CentralWidget : public QWidget
{
	Q_OBJECT

public:
	CentralWidget(MainWindow *main_window);

	void moveCamera(romas::AXIS axis);
	void addBlock(romas::BlockInfo& block_info);

	void loadAssembly(void);
	void saveAssembly(void);
	void resetAssembly(void);
	void headerFile(void);
	void simulatorClicked(void);

	void setConfig(romas::Config *config);

private :
	AssembleMenuWidget *assemble_menu_widget_;
	AssembleSpaceWidget *assemble_space_widget_;
	MainWindow *main_window_;
	StateMachine state_machine_;
};

#endif