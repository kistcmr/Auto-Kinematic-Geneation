#ifndef __ASSEMBLY_MENU_WIDGET__
#define __ASSEMBLY_MENU_WIDGET__

#include <QWidget>
#include <romas/romas.hpp>
#include <QStackedWidget>
#include "CentralWidget.hpp"

class QPushButton;
class QStackedWidget;
class QRadioButton;
class QButtonGroup;
class QLineEdit;
class CentralWidget;
class QLabel;

class AssembleStep1Widget : public QWidget
{
	Q_OBJECT

public:
	AssembleStep1Widget(StateMachine* state_machine);

private:
	QPushButton *cancel_pushbutton_;
	StateMachine *state_machine_;
};

class AssembleStep2Widget : public QWidget
{
	Q_OBJECT

public:
	AssembleStep2Widget(StateMachine *state_machine);

private:
	QPushButton *cancel_pushbutton_;
	QPushButton *previous_pushbutton_;
	StateMachine *state_machine_;
};

//class AssembleStep3Widget : public QWidget
//{
//	Q_OBJECT
//
//public:
//	AssembleStep3Widget(StateMachine *state_machine);
//	void initialize(void);
//
//	void choosedAxis(romas::AXIS& parent, romas::AXIS& child);
//
//	void setUseAxis(romas::AXIS parent, romas::AXIS child);
//
//public slots :
//	void parentAxisPlusXToggled(bool checked);
//	void parentAxisPlusYToggled(bool checked);
//	void parentAxisPlusZToggled(bool checked);
//	void parentAxisMinusXToggled(bool checked);
//	void parentAxisMinusYToggled(bool checked);
//	void parentAxisMinusZToggled(bool checked);
//
//	void childAxisPlusXToggled(bool checked);
//	void childAxisPlusYToggled(bool checked);
//	void childAxisPlusZToggled(bool checked);
//	void childAxisMinusXToggled(bool checked);
//	void childAxisMinusYToggled(bool checked);
//	void childAxisMinusZToggled(bool checked);
//
//private:
//	QPushButton *cancel_pushbutton_;
//	QPushButton *previous_pushbutton_;
//	QPushButton *next_pushbutton_;
//
//	QRadioButton *parent_axis_[romas::AXIS_NUM];
//	QRadioButton *child_axis_[romas::AXIS_NUM];
//	QButtonGroup *parent_axis_buttons_;
//	QButtonGroup *child_axis_buttons_;
//
//	StateMachine *state_machine_;
//};

class AssembleStep3Widget : public QWidget
{
	Q_OBJECT

public:
	AssembleStep3Widget(StateMachine *state_machine);
	void initialize(void);
	void choosedAxis(romas::AXIS& parent, romas::AXIS& child);
	void setAxisDisabled(romas::AXIS parent, romas::AXIS child);
	void setNextButton(void);
	void setFinishButton(void);

public slots :
	void parentAxisPlusXToggled(bool checked);
	void parentAxisPlusYToggled(bool checked);
	void parentAxisPlusZToggled(bool checked);
	void parentAxisMinusXToggled(bool checked);
	void parentAxisMinusYToggled(bool checked);
	void parentAxisMinusZToggled(bool checked);

	void childAxisPlusXToggled(bool checked);
	void childAxisPlusYToggled(bool checked);
	void childAxisPlusZToggled(bool checked);
	void childAxisMinusXToggled(bool checked);
	void childAxisMinusYToggled(bool checked);
	void childAxisMinusZToggled(bool checked);

	void nextOrFinishPushButtonClicked(void);

private:
	StateMachine *state_machine_;

	QPushButton *cancel_pushbutton_;
	QPushButton *previous_pushbutton_;
	QPushButton *next_or_finish_pushbutton_;

	QRadioButton *parent_axis_[romas::AXIS_NUM];
	QRadioButton *child_axis_[romas::AXIS_NUM];
	QButtonGroup *parent_axis_buttons_;
	QButtonGroup *child_axis_buttons_;
};

//class AssembleStep5Widget : public QWidget
//{
//	Q_OBJECT
//
//public:
//	AssembleStep5Widget(StateMachine *state_machine);
//	void initialize(void);
//
//	romas::AXIS choosedAxis(void);
//	void setUseAxis(romas::AXIS axis);
//
//public slots:
//	void zRotationAxisPlusXToggled(bool checked);
//	void zRotationAxisPlusYToggled(bool checked);
//	void zRotationAxisPlusZToggled(bool checked);
//	void zRotationAxisMinusXToggled(bool checked);
//	void zRotationAxisMinusYToggled(bool checked);
//	void zRotationAxisMinusZToggled(bool checked);
//
//private:
//	StateMachine *state_machine_;
//	QPushButton *cancel_pushbutton_;
//	QPushButton *previous_pushbutton_;
//	QPushButton *next_pushbutton_;
//
//	QRadioButton *z_rotation_axis_[romas::AXIS_NUM];
//};
//
//class AssembleStep6Widget : public QWidget
//{
//	Q_OBJECT
//
//public:
//	AssembleStep6Widget(StateMachine *state_machine);
//	void initialize(void);
//	void setAxisDisabled(romas::AXIS axis);
//	romas::AXIS choosedAxis(void);
//
//public slots:
//	void xRotationAxisPlusXToggled(bool checked);
//	void xRotationAxisPlusYToggled(bool checked);
//	void xRotationAxisPlusZToggled(bool checked);
//	void xRotationAxisMinusXToggled(bool checked);
//	void xRotationAxisMinusYToggled(bool checked);
//	void xRotationAxisMinusZToggled(bool checked);
//
//private:
//	StateMachine *state_machine_;
//	QPushButton *cancel_pushbutton_;
//	QPushButton *previous_pushbutton_;
//	QPushButton *next_pushbutton_;
//
//	QRadioButton *x_rotation_axis_[romas::AXIS_NUM];
//};

class AssembleStep4Widget : public QWidget
{
	Q_OBJECT

public:
	AssembleStep4Widget(StateMachine *state_machine);
	void initialize(void);

	void setOffsetUnitDegree(void);
	void setOffsetUnitMm(void);

	void setDifference(QString difference);

public slots:
	void revoluteToggled(bool checked);
	void prismaticToggled(bool checked);
	void offsetValueEditingFinished(void);

private:
	StateMachine *state_machine_;
	QPushButton *cancel_pushbutton_;
	QPushButton *previous_pushbutton_;
	QPushButton *finish_pushbutton_;

	QRadioButton *revolute_radiobutton_;
	QRadioButton *prismatic_radiobutton_;

	QLineEdit* offset_value_lineedit_;
	QLabel* offset_unit_label_;

	QLineEdit* difference_value_lineedit_;

	QString offset_value_degree_;
	QString offset_value_mm_;

};

class AssembleMenuWidget : public QStackedWidget
{
	Q_OBJECT

public :
	AssembleMenuWidget(StateMachine* state_machine);

	void setCurrentStep(int step);
	void initialize(void);

	void setStep3NextButton(void);
	void setStep3FinishButton(void);

	void choosedMatchedAxis(romas::AXIS& parent, romas::AXIS& child);

	int getChoosedJointType(void);
	double getChoosedOffset(void);

	void setMatchedAxisDisabled(romas::AXIS parent, romas::AXIS child);

	void setOffsetUnitDegree(void);
	void setOffsetUnitMm(void);

	void setDifference(QString difference);

private :
	AssembleStep1Widget* assemble_step1_widget_;
	AssembleStep2Widget* assemble_step2_widget_;
	AssembleStep3Widget* assemble_step3_widget_;
	AssembleStep4Widget* assemble_step4_widget_;
	//AssembleStep5Widget* assemble_step5_widget_;
	//AssembleStep6Widget* assemble_step6_widget_;
	//AssembleStep7Widget* assemble_step7_widget_;
};

#endif

/*
step0 : �ƹ� �޼��� ����.
step1 : ������ �θ����� ����Ʈ �ڽ��� �������ּ��� [cancel]
step2 : ������ �ڽĺ���� ����Ʈ �ڽ��� �������ּ��� [back/cancel]
//step3 : �� ��ġ��Ű�� 1 : ������ �� �θ� ����Ʈ ��� �ڽ� ����Ʈ ���� ������ �ּ��� [set/back/cancel]
step4 : �� ��ġ��Ű�� 2 : �������� ��������, 2��°�� ��ġ��ų �θ� ����Ʈ ��� �ڽ� ����Ʈ ���� ������ �ּ��� [set/back/cancel]
//step5 : �߰��� ���� ȸ����z�� �������ּ���. [set/back/cancel] 
//step6 : z���� �������� �߰��� ���� ȸ���� x�� ������ �ּ���. [set/back/cancel]
step7 : ������ �������ּ���. [set/back/cancel]
*/