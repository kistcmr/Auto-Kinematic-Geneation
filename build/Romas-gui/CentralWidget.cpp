#include "CentralWidget.hpp"
#include "AssembleMenuWidget.hpp"
#include "AssembleSpaceWidget.hpp"
#include <QSplitter>
#include <QHBoxLayout>
#include <QMessagebox>

CentralWidget::CentralWidget(MainWindow *main_window)
	: QWidget(Q_NULLPTR)
	, main_window_(main_window)
{
	assemble_space_widget_ = new AssembleSpaceWidget(&state_machine_);

	assemble_menu_widget_ = new AssembleMenuWidget(&state_machine_);

	state_machine_.initialize(assemble_space_widget_, assemble_menu_widget_);

	QSplitter *splitter = new QSplitter(Qt::Vertical);

	splitter->addWidget(assemble_space_widget_);
	splitter->addWidget(assemble_menu_widget_);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(splitter);

	setLayout(layout);

	assemble_menu_widget_->setVisible(false);
}

void CentralWidget::moveCamera(romas::AXIS axis)
{
	assemble_space_widget_->moveCamera(axis);
}

void CentralWidget::addBlock(romas::BlockInfo& block_info)
{
	state_machine_.addBlock(block_info);
}

void CentralWidget::saveAssembly()
{
	state_machine_.saveAssembly();
	
}

void CentralWidget::loadAssembly()
{
	state_machine_.loadAssembly();
}

void CentralWidget::resetAssembly()
{
	state_machine_.resetAssembly();
}

void CentralWidget::headerFile(void)
{
	state_machine_.headerFile();


}

void CentralWidget::setConfig(romas::Config *config)
{
	assemble_space_widget_->setConfig(config);
}

void CentralWidget::simulatorClicked(void)
{
	state_machine_.simulator();
}
