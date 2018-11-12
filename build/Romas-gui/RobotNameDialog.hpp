#ifndef __ROBOT_NAME_DIALOG_HPP_
#define __ROBOT_NAME_DIALOG_HPP_

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <romas/romas.hpp>

//class QLineEidt;
class QDialogButtonBox;
class QRadioButton;
class QButtonGroup;
class QPushButton;

class RobotNameDialog : public QDialog
{
	Q_OBJECT

public:
	RobotNameDialog(void);

	QString robotName(void);
	QString projectDirectory(void);
	romas::Solver solver(void);
	QString define(void);

public slots :

	void directoryPushButtonClicked(void);

private :
	QLineEdit *name_line_edit_;
	QDialogButtonBox *ok_cancel_button_box_;
	QRadioButton *kinematic_radio_button_;
	QRadioButton *dynamic_radio_button_;
	QButtonGroup *radio_buttons_;
	QLineEdit *project_directory_line_edit_;
	QPushButton *project_directory_push_button_;
	QLineEdit *define_line_edit_;
};

#endif
