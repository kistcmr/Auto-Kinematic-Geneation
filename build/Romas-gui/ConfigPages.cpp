#include <QtWidgets>

#include "ConfigPages.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

BlockPage::BlockPage(QWidget *parent)
	: QWidget(parent)
	, changed_(false)
{
	QGroupBox *block_library_path_group = new QGroupBox(tr("Block Library Folder"));
	block_library_path_line_edit_ = new QLineEdit();
	block_library_path_push_button_ = new QPushButton("...");

	QHBoxLayout *block_library_path_layout = new QHBoxLayout();

	block_library_path_layout->addWidget(block_library_path_line_edit_);
	block_library_path_layout->addWidget(block_library_path_push_button_);

	block_library_path_group->setLayout(block_library_path_layout);

	QVBoxLayout *main_layout = new QVBoxLayout();

	main_layout->addWidget(block_library_path_group);
	main_layout->addStretch();

	setLayout(main_layout);

	connect(block_library_path_push_button_, SIGNAL(clicked()), this, SLOT(blockLibraryPathPushButtonClicked()));

	//QLabel *serverLabel = new QLabel(tr("Server:"));
	//QComboBox *serverCombo = new QComboBox;
	//serverCombo->addItem(tr("Qt (Australia)"));
	//serverCombo->addItem(tr("Qt (Germany)"));
	//serverCombo->addItem(tr("Qt (Norway)"));
	//serverCombo->addItem(tr("Qt (People's Republic of China)"));
	//serverCombo->addItem(tr("Qt (USA)"));

	//QHBoxLayout *serverLayout = new QHBoxLayout;
	//serverLayout->addWidget(serverLabel);
	//serverLayout->addWidget(serverCombo);

	//QVBoxLayout *configLayout = new QVBoxLayout;
	//configLayout->addLayout(serverLayout);
	//configGroup->setLayout(configLayout);

	//QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addWidget(configGroup);
	//mainLayout->addStretch(1);
	//setLayout(mainLayout);

}

void BlockPage::setBlockLibraryPath(QString path)
{
	block_library_path_line_edit_->setText(path);
}

QString BlockPage::blockLibraryPath(void)
{
	return block_library_path_line_edit_->text();
}

void BlockPage::blockLibraryPathPushButtonClicked(void)
{
	QString directory = QFileDialog::getExistingDirectory(this, QString("Path to block library folder"), QString(".\\"), QFileDialog::ShowDirsOnly);
	if (!directory.isEmpty())
	{
		block_library_path_line_edit_->setText(directory);
		changed_ = true;
	}
}


CompilePage::CompilePage(QWidget *parent)
	: QWidget(parent)
	, changed_(false)
{

	QGroupBox *vcvarsall_path_group = new QGroupBox(tr("vcvarsall.bat File"));
	vcvarsall_path_line_edit_ = new QLineEdit();
	vcvarsall_path_push_button_ = new QPushButton("...");

	QHBoxLayout *vcvarsall_path_layout = new QHBoxLayout();

	vcvarsall_path_layout->addWidget(vcvarsall_path_line_edit_);
	vcvarsall_path_layout->addWidget(vcvarsall_path_push_button_);

	vcvarsall_path_group->setLayout(vcvarsall_path_layout);

	QGroupBox *msbuild_path_group = new QGroupBox(tr("MSBuild.exe File"));
	msbuild_path_line_edit_ = new QLineEdit();
	msbuild_path_push_button_ = new QPushButton("...");
//	msbuild_path_push_button_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	QHBoxLayout *msbuild_path_layout = new QHBoxLayout();

	msbuild_path_layout->addWidget(msbuild_path_line_edit_);
	msbuild_path_layout->addWidget(msbuild_path_push_button_);

	msbuild_path_group->setLayout(msbuild_path_layout);

	QVBoxLayout *main_layout = new QVBoxLayout();

	main_layout->addWidget(vcvarsall_path_group);
	main_layout->addWidget(msbuild_path_group);
	main_layout->addStretch();

	setLayout(main_layout);

	connect(vcvarsall_path_push_button_, SIGNAL(clicked()), this, SLOT(vcvarsallPathPushButtonClicked()));
	connect(msbuild_path_push_button_, SIGNAL(clicked()), this, SLOT(msbuildPathPushButtonClicked()));

	//QGroupBox *updateGroup = new QGroupBox(tr("Package selection"));
	//QCheckBox *systemCheckBox = new QCheckBox(tr("Update system"));
	//QCheckBox *appsCheckBox = new QCheckBox(tr("Update applications"));
	//QCheckBox *docsCheckBox = new QCheckBox(tr("Update documentation"));

	//QGroupBox *packageGroup = new QGroupBox(tr("Existing packages"));

	//QListWidget *packageList = new QListWidget;
	//QListWidgetItem *qtItem = new QListWidgetItem(packageList);
	//qtItem->setText(tr("Qt"));
	//QListWidgetItem *qsaItem = new QListWidgetItem(packageList);
	//qsaItem->setText(tr("QSA"));
	//QListWidgetItem *teamBuilderItem = new QListWidgetItem(packageList);
	//teamBuilderItem->setText(tr("Teambuilder"));

	//QPushButton *startUpdateButton = new QPushButton(tr("Start update"));

	//QVBoxLayout *updateLayout = new QVBoxLayout;
	//updateLayout->addWidget(systemCheckBox);
	//updateLayout->addWidget(appsCheckBox);
	//updateLayout->addWidget(docsCheckBox);
	//updateGroup->setLayout(updateLayout);

	//QVBoxLayout *packageLayout = new QVBoxLayout;
	//packageLayout->addWidget(packageList);
	//packageGroup->setLayout(packageLayout);

	//QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addWidget(updateGroup);
	//mainLayout->addWidget(packageGroup);
	//mainLayout->addSpacing(12);
	//mainLayout->addWidget(startUpdateButton);
	//mainLayout->addStretch(1);
	//setLayout(mainLayout);
}

void CompilePage::setVcvarsallPath(QString path)
{
	vcvarsall_path_line_edit_->setText(path);
}

QString CompilePage::vcvarsallPath(void)
{
	return vcvarsall_path_line_edit_->text();
}

void CompilePage::setMsbuildPath(QString path)
{
	msbuild_path_line_edit_->setText(path);
}

QString CompilePage::msbuildPath(void)
{
	return msbuild_path_line_edit_->text();
}

void CompilePage::vcvarsallPathPushButtonClicked(void)
{
	QString absolute_path = QFileDialog::getOpenFileName(this, tr("Path to vcvarsall.bat"), vcvarsall_path_line_edit_->text(), tr("batch files(*.bat)"));

	if (!absolute_path.isEmpty())
	{
		vcvarsall_path_line_edit_->setText(absolute_path);
		changed_ = true;
	}
}

void CompilePage::msbuildPathPushButtonClicked(void)
{
	QString absolute_path = QFileDialog::getOpenFileName(this, tr("Path to MSBuild.exe"), msbuild_path_line_edit_->text(), tr("executable files(*.exe)"));

	if (!absolute_path.isEmpty())
	{
		msbuild_path_line_edit_->setText(absolute_path);
		changed_ = true;
	}
}

SimulationPage::SimulationPage(QWidget *parent)
	: QWidget(parent)
	, changed_(false)
{
	QGroupBox *block_library_path_group = new QGroupBox(tr("Block Library Folder"));
	block_library_path_line_edit_ = new QLineEdit();
	block_library_path_push_button_ = new QPushButton("...");

	QHBoxLayout *block_library_path_layout = new QHBoxLayout();

	block_library_path_layout->addWidget(block_library_path_line_edit_);
	block_library_path_layout->addWidget(block_library_path_push_button_);

	block_library_path_group->setLayout(block_library_path_layout);

	QVBoxLayout *main_layout = new QVBoxLayout();

	main_layout->addWidget(block_library_path_group);
	main_layout->addStretch();

	setLayout(main_layout);

	connect(block_library_path_push_button_, SIGNAL(clicked()), this, SLOT(blockLibraryPathPushButtonClicked()));

	//QLabel *serverLabel = new QLabel(tr("Server:"));
	//QComboBox *serverCombo = new QComboBox;
	//serverCombo->addItem(tr("Qt (Australia)"));
	//serverCombo->addItem(tr("Qt (Germany)"));
	//serverCombo->addItem(tr("Qt (Norway)"));
	//serverCombo->addItem(tr("Qt (People's Republic of China)"));
	//serverCombo->addItem(tr("Qt (USA)"));

	//QHBoxLayout *serverLayout = new QHBoxLayout;
	//serverLayout->addWidget(serverLabel);
	//serverLayout->addWidget(serverCombo);

	//QVBoxLayout *configLayout = new QVBoxLayout;
	//configLayout->addLayout(serverLayout);
	//configGroup->setLayout(configLayout);

	//QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addWidget(configGroup);
	//mainLayout->addStretch(1);
	//setLayout(mainLayout);

}

void SimulationPage::setBlockLibraryPath(QString path)
{
	block_library_path_line_edit_->setText(path);
}

QString SimulationPage::blockLibraryPath(void)
{
	return block_library_path_line_edit_->text();
}

void SimulationPage::blockLibraryPathPushButtonClicked(void)
{
	QString directory = QFileDialog::getExistingDirectory(this, QString("Path to block library folder"), QString(".\\"), QFileDialog::ShowDirsOnly);
	if (!directory.isEmpty())
	{
		block_library_path_line_edit_->setText(directory);
		changed_ = true;
	}
}
