#include "RobotNameDialog.hpp"

#include <QtWidgets>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QGroupBox>

RobotNameDialog::RobotNameDialog(void)
	: QDialog(Q_NULLPTR)
{

	setWindowTitle(tr("Project Setting"));

	QGroupBox *name_group_box = new QGroupBox("Robot's Name");
	QHBoxLayout *name_layout = new QHBoxLayout();
	name_line_edit_ = new QLineEdit();
	name_layout->addWidget(name_line_edit_);
	name_group_box->setLayout(name_layout);

	QGroupBox *project_directory_group_box = new QGroupBox("Project's Folder");
	project_directory_line_edit_ = new QLineEdit();
	project_directory_push_button_ = new QPushButton("...");
//	directory_push_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	QHBoxLayout *project_directory_layout = new QHBoxLayout();
	project_directory_layout->addWidget(project_directory_line_edit_);
	project_directory_layout->addWidget(project_directory_push_button_);
	project_directory_group_box->setLayout(project_directory_layout);

	QGroupBox *solver_group_box = new QGroupBox("Solver");
	kinematic_radio_button_ = new QRadioButton("Kinematics");
	dynamic_radio_button_ = new QRadioButton("Dynamics");
	radio_buttons_ = new QButtonGroup();

	radio_buttons_->addButton(kinematic_radio_button_, 0);
	radio_buttons_->addButton(dynamic_radio_button_, 1);

	dynamic_radio_button_->setChecked(true);

	QHBoxLayout *solver_layout = new QHBoxLayout();
	solver_layout->addWidget(kinematic_radio_button_);
	solver_layout->addWidget(dynamic_radio_button_);

	solver_group_box->setLayout(solver_layout);

	QGroupBox *define_group_box = new QGroupBox("Custom Define");
	define_line_edit_ = new QLineEdit();
	QLabel *define_label = new QLabel("ex) DEFILE_EX1;DEFINE_EX2");

	QVBoxLayout *define_layout = new QVBoxLayout();
	define_layout->addWidget(define_line_edit_);
	define_layout->addWidget(define_label);

	define_group_box->setLayout(define_layout);

	ok_cancel_button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(name_group_box);
	layout->addWidget(project_directory_group_box);
	layout->addWidget(solver_group_box);
	layout->addWidget(define_group_box);
	layout->addWidget(ok_cancel_button_box_);

	setLayout(layout);
//	setMaximumSize(200, 200);

	connect(ok_cancel_button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ok_cancel_button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(project_directory_push_button_, SIGNAL(clicked()), this, SLOT(directoryPushButtonClicked()));
}

QString RobotNameDialog::robotName(void)
{
	return name_line_edit_->text();
}

QString RobotNameDialog::projectDirectory(void)
{
	return project_directory_line_edit_->text();
}

romas::Solver RobotNameDialog::solver(void)
{
	if (kinematic_radio_button_->isChecked())
		return romas::Solver::Kinematic;
	else
		return romas::Solver::Dynamic;

}

void RobotNameDialog::directoryPushButtonClicked(void)
{
	QString directory = QFileDialog::getExistingDirectory(this, QString("choose folder"), QString(".\\"), QFileDialog::ShowDirsOnly);
	project_directory_line_edit_->setText(directory);
}

QString RobotNameDialog::define(void)
{
	return define_line_edit_->text();
}
