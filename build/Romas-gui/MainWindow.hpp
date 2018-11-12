#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <QtWidgets/QMainWindow>
#include <romas/Block.hpp>

#include "AssembleMenuWidget.hpp"
#include "AssembleSpaceWidget.hpp"
#include <romas/Config.hpp>

class QWidget;
class QAction;
class QToolBar;
class QSplitter;
class MainMenuWidget;
class SubMenuWidget;
class CentralWidget;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void readSettings(void);
	void writeSettings(void);

private slots:
	void cameraRotateTriggeredEvent();
	void cameraPanTriggeredEvent();
	void cameraZoomTriggeredEvent();
	void cameraPlusXTriggeredEvent();
	void cameraMinusXTriggeredEvent();
	void cameraPlusYTriggeredEvent();
	void cameraMinusYTriggeredEvent();
	void cameraPlusZTriggeredEvent();
	void cameraMinusZTriggeredEvent();
	void configTriggeredEvent();

	void loadBlocksClickedEvent(void);
	void saveAssemblyClickedEvent(void);
	void loadAssemblyClickedEvent(void);
	void resetAssemblyClickedEvent(void);
	void headerFileClicked(void);
	void simulatorClicked(void);
	void blockCandidateDoubleClickedEvent(QListWidgetItem *item);

private:
	
	void _createToolBars(void);
	void _createActions(void);

	MainMenuWidget* main_menu_widget_;  // left
	CentralWidget *central_widget_; // middle
	SubMenuWidget* sub_menu_widget_; // right

	QSplitter *splitter_;

	QAction *camera_rotate_action_;
	QAction *camera_zoom_action_;
	QAction *camera_pan_action_;
	QAction *camera_plus_x_action_;
	QAction *camera_minus_x_action_;
	QAction *camera_plus_y_action_;
	QAction *camera_minus_y_action_;
	QAction *camera_plus_z_action_;
	QAction *camera_minus_z_action_;
	QAction *config_action_;

	QToolBar *camera_toolbar_;

	romas::BlockInfoSet block_info_set_;

	romas::Block *block_;

	romas::Config config_;
};


#endif