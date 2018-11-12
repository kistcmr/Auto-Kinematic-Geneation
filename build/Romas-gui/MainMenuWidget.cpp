
#include "MainMenuWidget.hpp"
#include "MainWindow.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>

MainMenuWidget::MainMenuWidget(MainWindow *main_window)
	: main_window_(main_window)
{

	load_blocks_pushbutton_ = new QPushButton("Load Blocks");
//	load_blocks_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	save_assembly_pushbutton_ = new QPushButton("Save Assembly");
//	save_assembly_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	load_assembly_pushbutton_ = new QPushButton("Load Assembly");
//	load_assembly_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	header_file_pushbutton_ = new QPushButton("Generate/Compile Code");
//	header_file_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	simulator_pushbutton_ = new QPushButton("Simulator");
//	simulator_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	block_generator_pushbutton_ = new QPushButton("Block Generator");
//	block_generator_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	reset_assembly_pushbutton_ = new QPushButton("Reset Assembly");
//	reset_assemble_pushbutton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	delete_pushbutton_ = new QPushButton("Delete");
	edit_pushbutton_ = new QPushButton("Edit");
	target_pushbutton_ = new QPushButton("Target");
	reference_pushbutton_ = new QPushButton("Reference");


	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(load_blocks_pushbutton_);
	layout->addWidget(save_assembly_pushbutton_);
	layout->addWidget(load_assembly_pushbutton_);
	layout->addWidget(reset_assembly_pushbutton_);
	layout->addWidget(header_file_pushbutton_);
	layout->addWidget(simulator_pushbutton_);
	layout->addWidget(block_generator_pushbutton_);

	layout->addStretch();

	layout->addWidget(delete_pushbutton_);
	layout->addWidget(edit_pushbutton_);
	layout->addWidget(target_pushbutton_);
	layout->addWidget(reference_pushbutton_);

	setBlockHandlerVisible(false);

	setLayout(layout);
	
	connect(load_blocks_pushbutton_, SIGNAL(clicked()), main_window_, SLOT(loadBlocksClickedEvent()));
	connect(save_assembly_pushbutton_, SIGNAL(clicked()), main_window_, SLOT(saveAssemblyClickedEvent()));
	connect(load_assembly_pushbutton_, SIGNAL(clicked()), main_window, SLOT(loadAssemblyClickedEvent()));
	connect(reset_assembly_pushbutton_, SIGNAL(clicked()), main_window, SLOT(resetAssemblyClickedEvent()));
	connect(header_file_pushbutton_, SIGNAL(clicked()), main_window_, SLOT(headerFileClicked()));
	connect(simulator_pushbutton_, SIGNAL(clicked()), main_window_, SLOT(simulatorClicked()));
	connect(block_generator_pushbutton_, SIGNAL(clicked()), this, SLOT(blockGeneratorClicked()));
}

void MainMenuWidget::simulatorClicked(void)
{

}

void MainMenuWidget::blockGeneratorClicked(void)
{

}


const bool MainMenuWidget::isBlockHandlerVisible(void)
{
	return delete_pushbutton_->isVisible();
}

void MainMenuWidget::setBlockHandlerVisible(bool visible)
{
	delete_pushbutton_->setVisible(visible);
	edit_pushbutton_->setVisible(visible);
	target_pushbutton_->setVisible(visible);
	reference_pushbutton_->setVisible(visible);
}
