#include "MainWindow.hpp"
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include "MainMenuWidget.hpp"
#include "SubMenuWidget.hpp"
#include "AssembleMenuWidget.hpp"
#include "AssembleSpaceWidget.hpp"
#include "CentralWidget.hpp"
#include <QAction>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QSettings>
#include "ConfigDialog.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(800, 600);
	setWindowTitle("Romas(RObot Module ASsembler)");

	main_menu_widget_ = new MainMenuWidget(this);
	central_widget_ = new CentralWidget(this);
	sub_menu_widget_ = new SubMenuWidget(this);

	splitter_ = new QSplitter(Qt::Horizontal);

	splitter_->addWidget(main_menu_widget_);
	splitter_->addWidget(central_widget_);
	splitter_->addWidget(sub_menu_widget_);

	setCentralWidget(splitter_);

	_createActions();
	_createToolBars();

	block_ = nullptr;

	readSettings();

	config_.load();
	central_widget_->setConfig(&config_);
}

MainWindow::~MainWindow()
{
	writeSettings();
}

void MainWindow::_createActions(void)
{
	camera_rotate_action_ = new QAction("&Rotate", this);
	camera_rotate_action_->setIcon(QIcon(":/MainWindow/Resources/camera_rotate.png"));
	camera_rotate_action_->setCheckable(true);

	connect(camera_rotate_action_, SIGNAL(triggered()), this, SLOT(cameraRotateTriggeredEvent()));

	camera_zoom_action_ = new QAction("&Zoom", this);
	camera_zoom_action_->setIcon(QIcon(":/MainWindow/Resources/camera_zoom.png"));
	camera_zoom_action_->setCheckable(true);

	connect(camera_zoom_action_, SIGNAL(triggered()), this, SLOT(cameraZoomTriggeredEvent()));

	camera_pan_action_ = new QAction("&Pan", this);
	camera_pan_action_->setIcon(QIcon(":/MainWindow/Resources/camera_pan.png"));
	camera_pan_action_->setCheckable(true);

	connect(camera_pan_action_, SIGNAL(triggered()), this, SLOT(cameraPanTriggeredEvent()));

	camera_plus_x_action_ = new QAction("&X+", this);
	camera_plus_x_action_->setIcon(QIcon(":/MainWindow/Resources/camera_xplus.png"));
	connect(camera_plus_x_action_, SIGNAL(triggered()), this, SLOT(cameraPlusXTriggeredEvent()));

	camera_minus_x_action_ = new QAction("&X-", this);
	camera_minus_x_action_->setIcon(QIcon(":/MainWindow/Resources/camera_xminus.png"));
	connect(camera_minus_x_action_, SIGNAL(triggered()), this, SLOT(cameraMinusXTriggeredEvent()));

	camera_plus_y_action_ = new QAction("&Y+", this);
	camera_plus_y_action_->setIcon(QIcon(":/MainWindow/Resources/camera_yplus.png"));
	connect(camera_plus_y_action_, SIGNAL(triggered()), this, SLOT(cameraPlusYTriggeredEvent()));

	camera_minus_y_action_ = new QAction("&Y-", this);
	camera_minus_y_action_->setIcon(QIcon(":/MainWindow/Resources/camera_yminus.png"));
	connect(camera_minus_y_action_, SIGNAL(triggered()), this, SLOT(cameraMinusYTriggeredEvent()));

	camera_plus_z_action_ = new QAction("&Z+", this);
	camera_plus_z_action_->setIcon(QIcon(":/MainWindow/Resources/camera_zplus.png"));
	connect(camera_plus_z_action_, SIGNAL(triggered()), this, SLOT(cameraPlusZTriggeredEvent()));

	camera_minus_z_action_ = new QAction("&Z-", this);
	camera_minus_z_action_->setIcon(QIcon(":/MainWindow/Resources/camera_zminus.png"));
	connect(camera_minus_z_action_, SIGNAL(triggered()), this, SLOT(cameraMinusZTriggeredEvent()));

	config_action_ = new QAction("&Config", this);
	config_action_->setIcon(QIcon(":/MainWindow/Resources/config.png"));
	connect(config_action_, SIGNAL(triggered()), this, SLOT(configTriggeredEvent()));

}

void MainWindow::cameraRotateTriggeredEvent(void)
{
//	camera_rotate_action_->setChecked(!camera_rotate_action_->isChecked());

//	bool b = camera_rotate_action_->isChecked();
//	camera_rotate_action_->setChecked(!b);
}

void MainWindow::cameraPanTriggeredEvent(void)
{

}

void MainWindow::cameraZoomTriggeredEvent(void)
{

}

void MainWindow::cameraPlusXTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_PLUS_X);
}

void MainWindow::cameraMinusXTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_MINUS_X);
}

void MainWindow::cameraPlusYTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_PLUS_Y);
}

void MainWindow::cameraMinusYTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_MINUS_Y);
}

void MainWindow::cameraPlusZTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_PLUS_Z);
}

void MainWindow::cameraMinusZTriggeredEvent(void)
{
	central_widget_->moveCamera(romas::AXIS_MINUS_Z);
}

void MainWindow::configTriggeredEvent()
{
	ConfigDialog dialog(&config_);
	dialog.exec();
}


void MainWindow::_createToolBars(void)
{
	camera_toolbar_ = addToolBar(tr("&Camera"));
//	camera_toolbar_->addAction(camera_rotate_action_);
//	camera_toolbar_->addAction(camera_pan_action_);
//	camera_toolbar_->addAction(camera_zoom_action_);
	camera_toolbar_->addAction(camera_plus_x_action_);
	camera_toolbar_->addAction(camera_minus_x_action_);
	camera_toolbar_->addAction(camera_plus_y_action_);
	camera_toolbar_->addAction(camera_minus_y_action_);
	camera_toolbar_->addAction(camera_plus_z_action_);
	camera_toolbar_->addAction(camera_minus_z_action_);
	camera_toolbar_->addAction(config_action_);

	camera_toolbar_->setIconSize(QSize(32, 32));
//	camera_toolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void MainWindow::loadBlocksClickedEvent(void)
{
//	sub_menu_widget_->setBlockSelectedVisible(!sub_menu_widget_->isBlockSelectedVisible());

	QString absolute_file_path = QFileDialog::getOpenFileName(this, tr("Load Blocks"), QString::fromStdString(config_.blockLibraryPath()), tr("Blocks files (*.xml)"));

	if (!absolute_file_path.isEmpty())
	{
		QFileInfo fi(absolute_file_path);
		QString filename = fi.fileName();
		QString absolute_path = fi.absolutePath() + QString("/");

		if (block_info_set_.load(absolute_path.toStdString(), filename.toStdString()))
		{
			for (auto itr = block_info_set_.begin();
				itr != block_info_set_.end();
				itr++)
			{
				sub_menu_widget_->addBlockCandidate(*itr);
			}
		}
	}
}

void MainWindow::saveAssemblyClickedEvent(void)
{
	central_widget_->saveAssembly();
}

void MainWindow::loadAssemblyClickedEvent(void)
{
	central_widget_->loadAssembly();
}

void MainWindow::resetAssemblyClickedEvent(void)
{
	central_widget_->resetAssembly();
}

void MainWindow::blockCandidateDoubleClickedEvent(QListWidgetItem *item)
{
	QMessageBox dialog(QMessageBox::Icon::Question, QString::fromLocal8Bit("Question"), QString::fromLocal8Bit("선택된 Block을 사용하시겠습니까?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
	if (QMessageBox::No == dialog.exec())
		return;

	BlockCandidate *block_candidate = reinterpret_cast<BlockCandidate*>(item);

	central_widget_->addBlock(block_candidate->getBlockInfo());
}

void MainWindow::readSettings(void)
{
	QSettings settings("KIMM", "Romas");

	settings.beginGroup("mainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	splitter_->restoreState(settings.value("splitter").toByteArray());
	settings.endGroup();
}

void MainWindow::writeSettings(void)
{
	QSettings settings("KIMM", "Romas");

	settings.beginGroup("mainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("splitter", splitter_->saveState());
	settings.endGroup();
}

void MainWindow::headerFileClicked(void)
{
	central_widget_->headerFile();
}

void MainWindow::simulatorClicked(void)
{
	central_widget_->simulatorClicked();
}
