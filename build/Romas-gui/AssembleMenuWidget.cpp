#include "AssembleMenuWidget.hpp"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QGroupBox>
#include "CentralWidget.hpp"

AssembleStep1Widget::AssembleStep1Widget(StateMachine* state_machine)
	: QWidget(Q_NULLPTR)
	, state_machine_(state_machine)
{
	char word[] = "[Step 1] 연결할 부모 블록의 조인트 후보를 선택하세요";
	QLabel *title = new QLabel(QString::fromLocal8Bit(word));
	title->setWordWrap(true);

	cancel_pushbutton_ = new QPushButton("Cancel");

	QHBoxLayout *pushbutton_layout = new QHBoxLayout();
	pushbutton_layout->addStretch();
	pushbutton_layout->addWidget(cancel_pushbutton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(title);
	layout->addLayout(pushbutton_layout);

	setLayout(layout);

	connect(cancel_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(cancelAssemble()));
}

AssembleStep2Widget::AssembleStep2Widget(StateMachine *state_machine)
	: QWidget(Q_NULLPTR)
	, state_machine_(state_machine)
{
	//	QLabel *label = new QLabel("Step 2 of 7 : Choose the Joint Box of the child Block to be connected");

	char word[] = "[Step 2] 연결할 자식블록의 조인트 박스를 선택해주세요";
	QLabel *title = new QLabel(QString::fromLocal8Bit(word));
	title->setWordWrap(true);

	previous_pushbutton_ = new QPushButton("< Previous");
	cancel_pushbutton_ = new QPushButton("Cancel");

	QHBoxLayout *pushbutton_layout = new QHBoxLayout();
	pushbutton_layout->addStretch();
	pushbutton_layout->addWidget(previous_pushbutton_);
	pushbutton_layout->addWidget(cancel_pushbutton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(title);
	layout->addLayout(pushbutton_layout);

	setLayout(layout);

	connect(previous_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(goPreviousAssemble()));
	connect(cancel_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(cancelAssemble()));
}

AssembleStep3Widget::AssembleStep3Widget(StateMachine *state_machine)
	: QWidget(Q_NULLPTR)
	, state_machine_(state_machine)
{
	//	QLabel *label = new QLabel("Step 4 of 7 : Axis Alignment 2 : ");
	char word[] = "[Step 3] 일치시킬 부모 Contact Point의 축과 자식 Contact Point의 축을 선택해 주세요";
	QLabel *title = new QLabel(QString::fromLocal8Bit(word));
	title->setWordWrap(true);

	previous_pushbutton_ = new QPushButton("< Previous");
	next_or_finish_pushbutton_ = new QPushButton("> Next");
	cancel_pushbutton_ = new QPushButton("Cancel");

	parent_axis_[romas::AXIS_PLUS_X] = new QRadioButton("X");
	parent_axis_[romas::AXIS_PLUS_Y] = new QRadioButton("Y");
	parent_axis_[romas::AXIS_PLUS_Z] = new QRadioButton("Z");
	parent_axis_[romas::AXIS_MINUS_X] = new QRadioButton("-X");
	parent_axis_[romas::AXIS_MINUS_Y] = new QRadioButton("-Y");
	parent_axis_[romas::AXIS_MINUS_Z] = new QRadioButton("-Z");

	child_axis_[romas::AXIS_PLUS_X] = new QRadioButton("X");
	child_axis_[romas::AXIS_PLUS_Y] = new QRadioButton("Y");
	child_axis_[romas::AXIS_PLUS_Z] = new QRadioButton("Z");
	child_axis_[romas::AXIS_MINUS_X] = new QRadioButton("-X");
	child_axis_[romas::AXIS_MINUS_Y] = new QRadioButton("-Y");
	child_axis_[romas::AXIS_MINUS_Z] = new QRadioButton("-Z");

	parent_axis_buttons_ = new QButtonGroup(this);
	child_axis_buttons_ = new QButtonGroup(this);

	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_PLUS_X], 0);
	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_PLUS_Y], 1);
	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_PLUS_Z], 2);
	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_MINUS_X], 3);
	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_MINUS_Y], 4);
	parent_axis_buttons_->addButton(parent_axis_[romas::AXIS_MINUS_Z], 5);

	child_axis_buttons_->addButton(child_axis_[romas::AXIS_PLUS_X], 0);
	child_axis_buttons_->addButton(child_axis_[romas::AXIS_PLUS_Y], 1);
	child_axis_buttons_->addButton(child_axis_[romas::AXIS_PLUS_Z], 2);
	child_axis_buttons_->addButton(child_axis_[romas::AXIS_MINUS_X], 3);
	child_axis_buttons_->addButton(child_axis_[romas::AXIS_MINUS_Y], 4);
	child_axis_buttons_->addButton(child_axis_[romas::AXIS_MINUS_Z], 5);

	QHBoxLayout *parent_axis_radiobuttons_layout = new QHBoxLayout();

	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_PLUS_X]);
	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_PLUS_Y]);
	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_PLUS_Z]);
	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_MINUS_X]);
	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_MINUS_Y]);
	parent_axis_radiobuttons_layout->addWidget(parent_axis_[romas::AXIS_MINUS_Z]);

	QGroupBox *parent_axis_groupbox = new QGroupBox("Parent Axis");
	parent_axis_groupbox->setLayout(parent_axis_radiobuttons_layout);

	QHBoxLayout *child_axis_radiobuttons_layout = new QHBoxLayout();

	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_PLUS_X]);
	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_PLUS_Y]);
	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_PLUS_Z]);
	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_MINUS_X]);
	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_MINUS_Y]);
	child_axis_radiobuttons_layout->addWidget(child_axis_[romas::AXIS_MINUS_Z]);

	QGroupBox *child_axis_groupbox = new QGroupBox("Child Axis");
	child_axis_groupbox->setLayout(child_axis_radiobuttons_layout);

	QHBoxLayout *pushbutton_layout = new QHBoxLayout();
	pushbutton_layout->addStretch();
	pushbutton_layout->addWidget(previous_pushbutton_);
	pushbutton_layout->addWidget(next_or_finish_pushbutton_);
	pushbutton_layout->addWidget(cancel_pushbutton_);

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(title, 0, 0, 1, 3);
	layout->addWidget(parent_axis_groupbox, 1, 0);
	layout->addWidget(child_axis_groupbox, 1, 1);
	layout->setColumnStretch(2, 1);
	layout->addLayout(pushbutton_layout, 2, 0, 1, 3);

	setLayout(layout);

	connect(previous_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(goPreviousAssemble()));
	connect(next_or_finish_pushbutton_, SIGNAL(clicked()), this, SLOT(nextOrFinishPushButtonClicked()));
	connect(cancel_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(cancelAssemble()));

	connect(parent_axis_[romas::AXIS_PLUS_X], SIGNAL(toggled(bool)), this, SLOT(parentAxisPlusXToggled(bool)));
	connect(parent_axis_[romas::AXIS_PLUS_Y], SIGNAL(toggled(bool)), this, SLOT(parentAxisPlusYToggled(bool)));
	connect(parent_axis_[romas::AXIS_PLUS_Z], SIGNAL(toggled(bool)), this, SLOT(parentAxisPlusZToggled(bool)));
	connect(parent_axis_[romas::AXIS_MINUS_X], SIGNAL(toggled(bool)), this, SLOT(parentAxisMinusXToggled(bool)));
	connect(parent_axis_[romas::AXIS_MINUS_Y], SIGNAL(toggled(bool)), this, SLOT(parentAxisMinusYToggled(bool)));
	connect(parent_axis_[romas::AXIS_MINUS_Z], SIGNAL(toggled(bool)), this, SLOT(parentAxisMinusZToggled(bool)));

	connect(child_axis_[romas::AXIS_PLUS_X], SIGNAL(toggled(bool)), this, SLOT(childAxisPlusXToggled(bool)));
	connect(child_axis_[romas::AXIS_PLUS_Y], SIGNAL(toggled(bool)), this, SLOT(childAxisPlusYToggled(bool)));
	connect(child_axis_[romas::AXIS_PLUS_Z], SIGNAL(toggled(bool)), this, SLOT(childAxisPlusZToggled(bool)));
	connect(child_axis_[romas::AXIS_MINUS_X], SIGNAL(toggled(bool)), this, SLOT(childAxisMinusXToggled(bool)));
	connect(child_axis_[romas::AXIS_MINUS_Y], SIGNAL(toggled(bool)), this, SLOT(childAxisMinusYToggled(bool)));
	connect(child_axis_[romas::AXIS_MINUS_Z], SIGNAL(toggled(bool)), this, SLOT(childAxisMinusZToggled(bool)));
}

void AssembleStep3Widget::initialize(void)
{
	for (int i = romas::AXIS_PLUS_X; i < romas::AXIS_NUM; i++)
	{
		parent_axis_[i]->setEnabled(true);
		child_axis_[i]->setEnabled(true);
	}

	parent_axis_[romas::AXIS_PLUS_X]->setChecked(true);
	child_axis_[romas::AXIS_PLUS_X]->setChecked(true);
}

void AssembleStep3Widget::setNextButton(void)
{
	next_or_finish_pushbutton_->setText("> Next");
}

void AssembleStep3Widget::setFinishButton(void)
{
	next_or_finish_pushbutton_->setText("Finish");
}

void AssembleStep3Widget::nextOrFinishPushButtonClicked(void)
{
	if (next_or_finish_pushbutton_->text() == QString("> Next"))
		state_machine_->goNextAssemble();
	else if (next_or_finish_pushbutton_->text() == QString("Finish"))
		state_machine_->finishAssemble();
}

void AssembleStep3Widget::setAxisDisabled(romas::AXIS parent, romas::AXIS child)
{
	for (int i = romas::AXIS_PLUS_X; i < romas::AXIS_NUM; i++)
	{
		parent_axis_[i]->setEnabled(true);
		child_axis_[i]->setEnabled(true);
	}

	switch (parent)
	{
	case romas::AXIS_PLUS_X:
	case romas::AXIS_MINUS_X:
		parent_axis_[romas::AXIS_PLUS_X]->setEnabled(false);
		parent_axis_[romas::AXIS_MINUS_X]->setEnabled(false);
		if (parent_axis_[romas::AXIS_PLUS_X]->isChecked())
		{
			parent_axis_[romas::AXIS_PLUS_X]->setChecked(false);
			parent_axis_[romas::AXIS_PLUS_Y]->setChecked(true);
		}
		else if (parent_axis_[romas::AXIS_MINUS_X]->isChecked())
		{
			parent_axis_[romas::AXIS_MINUS_X]->setChecked(false);
			parent_axis_[romas::AXIS_MINUS_Y]->setChecked(true);
		}
		break;
	case romas::AXIS_PLUS_Y:
	case romas::AXIS_MINUS_Y:
		parent_axis_[romas::AXIS_PLUS_Y]->setEnabled(false);
		parent_axis_[romas::AXIS_MINUS_Y]->setEnabled(false);
		if (parent_axis_[romas::AXIS_PLUS_Y]->isChecked())
		{
			parent_axis_[romas::AXIS_PLUS_Y]->setChecked(false);
			parent_axis_[romas::AXIS_PLUS_Z]->setChecked(true);
		}
		else if (parent_axis_[romas::AXIS_MINUS_Y]->isChecked())
		{
			parent_axis_[romas::AXIS_MINUS_Y]->setChecked(false);
			parent_axis_[romas::AXIS_MINUS_Z]->setChecked(true);
		}
		break;
	case romas::AXIS_PLUS_Z:
	case romas::AXIS_MINUS_Z:
		parent_axis_[romas::AXIS_PLUS_Z]->setEnabled(false);
		parent_axis_[romas::AXIS_MINUS_Z]->setEnabled(false);
		if (parent_axis_[romas::AXIS_PLUS_Z]->isChecked())
		{
			parent_axis_[romas::AXIS_PLUS_Z]->setChecked(false);
			parent_axis_[romas::AXIS_PLUS_X]->setChecked(true);
		}
		else if (parent_axis_[romas::AXIS_MINUS_Z]->isChecked())
		{
			parent_axis_[romas::AXIS_MINUS_Z]->setChecked(false);
			parent_axis_[romas::AXIS_MINUS_X]->setChecked(true);
		}
		break;
	}

	switch (child)
	{
	case romas::AXIS_PLUS_X:
	case romas::AXIS_MINUS_X:
		child_axis_[romas::AXIS_PLUS_X]->setEnabled(false);
		child_axis_[romas::AXIS_MINUS_X]->setEnabled(false);
		if (child_axis_[romas::AXIS_PLUS_X]->isChecked())
		{
			child_axis_[romas::AXIS_PLUS_X]->setChecked(false);
			child_axis_[romas::AXIS_PLUS_Y]->setChecked(true);
		}
		else if (child_axis_[romas::AXIS_MINUS_X]->isChecked())
		{
			child_axis_[romas::AXIS_MINUS_X]->setChecked(false);
			child_axis_[romas::AXIS_MINUS_Y]->setChecked(true);
		}
		break;
	case romas::AXIS_PLUS_Y:
	case romas::AXIS_MINUS_Y:
		child_axis_[romas::AXIS_PLUS_Y]->setEnabled(false);
		child_axis_[romas::AXIS_MINUS_Y]->setEnabled(false);
		if (child_axis_[romas::AXIS_PLUS_Y]->isChecked())
		{
			child_axis_[romas::AXIS_PLUS_Y]->setChecked(false);
			child_axis_[romas::AXIS_PLUS_Z]->setChecked(true);
		}
		else if (child_axis_[romas::AXIS_MINUS_Y]->isChecked())
		{
			child_axis_[romas::AXIS_MINUS_Y]->setChecked(false);
			child_axis_[romas::AXIS_MINUS_Z]->setChecked(true);
		}
		break;
	case romas::AXIS_PLUS_Z:
	case romas::AXIS_MINUS_Z:
		child_axis_[romas::AXIS_PLUS_Z]->setEnabled(false);
		child_axis_[romas::AXIS_MINUS_Z]->setEnabled(false);
		if (child_axis_[romas::AXIS_PLUS_Z]->isChecked())
		{
			child_axis_[romas::AXIS_PLUS_Z]->setChecked(false);
			child_axis_[romas::AXIS_PLUS_X]->setChecked(true);
		}
		else if (child_axis_[romas::AXIS_MINUS_Z]->isChecked())
		{
			child_axis_[romas::AXIS_MINUS_Z]->setChecked(false);
			child_axis_[romas::AXIS_MINUS_X]->setChecked(true);
		}
		break;
	}
}

void AssembleStep3Widget::parentAxisPlusXToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_PLUS_X);
}

void AssembleStep3Widget::parentAxisPlusYToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_PLUS_Y);
}

void AssembleStep3Widget::parentAxisPlusZToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_PLUS_Z);
}

void AssembleStep3Widget::parentAxisMinusXToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_MINUS_X);
}

void AssembleStep3Widget::parentAxisMinusYToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_MINUS_Y);
}

void AssembleStep3Widget::parentAxisMinusZToggled(bool checked)
{
	if (checked)
		state_machine_->changeParentMatchedAxis(romas::AXIS_MINUS_Z);
}

void AssembleStep3Widget::childAxisPlusXToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_PLUS_X);
}

void AssembleStep3Widget::childAxisPlusYToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_PLUS_Y);
}

void AssembleStep3Widget::childAxisPlusZToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_PLUS_Z);
}

void AssembleStep3Widget::childAxisMinusXToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_MINUS_X);
}

void AssembleStep3Widget::childAxisMinusYToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_MINUS_Y);
}

void AssembleStep3Widget::childAxisMinusZToggled(bool checked)
{
	if (checked)
		state_machine_->changeChildMatchedAxis(romas::AXIS_MINUS_Z);
}

void AssembleStep3Widget::choosedAxis(romas::AXIS& parent, romas::AXIS& child)
{
	if (parent_axis_[romas::AXIS_PLUS_X]->isChecked())
		parent = romas::AXIS_PLUS_X;
	else if (parent_axis_[romas::AXIS_PLUS_Y]->isChecked())
		parent = romas::AXIS_PLUS_Y;
	else if (parent_axis_[romas::AXIS_PLUS_Z]->isChecked())
		parent = romas::AXIS_PLUS_Z;
	else if (parent_axis_[romas::AXIS_MINUS_X]->isChecked())
		parent = romas::AXIS_MINUS_X;
	else if (parent_axis_[romas::AXIS_MINUS_Y]->isChecked())
		parent = romas::AXIS_MINUS_Y;
	else if (parent_axis_[romas::AXIS_MINUS_Z]->isChecked())
		parent = romas::AXIS_MINUS_Z;
	else
		parent = romas::AXIS_PLUS_X;

	if (child_axis_[romas::AXIS_PLUS_X]->isChecked())
		child = romas::AXIS_PLUS_X;
	else if (child_axis_[romas::AXIS_PLUS_Y]->isChecked())
		child = romas::AXIS_PLUS_Y;
	else if (child_axis_[romas::AXIS_PLUS_Z]->isChecked())
		child = romas::AXIS_PLUS_Z;
	else if (child_axis_[romas::AXIS_MINUS_X]->isChecked())
		child = romas::AXIS_MINUS_X;
	else if (child_axis_[romas::AXIS_MINUS_Y]->isChecked())
		child = romas::AXIS_MINUS_Y;
	else if (child_axis_[romas::AXIS_MINUS_Z]->isChecked())
		child = romas::AXIS_MINUS_Z;
	else
		child = romas::AXIS_PLUS_X;
}

AssembleStep4Widget::AssembleStep4Widget(StateMachine *state_machine)
	: QWidget(Q_NULLPTR)
	, state_machine_(state_machine)
{
	//	QLabel *label = new QLabel("Step 7 of 7 : Choose Movement");

	char word[] = "[Step 4] 운동 방식과 오프셋을 선택해주세요";
	QLabel *title = new QLabel(QString::fromLocal8Bit(word));
	title->setWordWrap(true);

	revolute_radiobutton_ = new QRadioButton("Revolute");
	prismatic_radiobutton_ = new QRadioButton("Prismatic");

	QButtonGroup *movement_type_button_group = new QButtonGroup(this);

	movement_type_button_group->addButton(revolute_radiobutton_, 0);
	movement_type_button_group->addButton(prismatic_radiobutton_, 1);

	QHBoxLayout *movement_type_radiobuttons_layout = new QHBoxLayout();

	movement_type_radiobuttons_layout->addWidget(revolute_radiobutton_);
	movement_type_radiobuttons_layout->addWidget(prismatic_radiobutton_);

	QGroupBox *movement_type_groupbox = new QGroupBox("Movement Type");
	movement_type_groupbox->setLayout(movement_type_radiobuttons_layout);

	difference_value_lineedit_ = new QLineEdit();
	difference_value_lineedit_->setMaximumWidth(50);
	difference_value_lineedit_->setAlignment(Qt::AlignRight);
	difference_value_lineedit_->setReadOnly(true);
	QLabel *difference_unit_label = new QLabel("degree");

	QHBoxLayout *difference_layout = new QHBoxLayout();
	difference_layout->addWidget(difference_value_lineedit_);
	difference_layout->addWidget(difference_unit_label);

	QGroupBox *difference_groupbox = new QGroupBox("Difference");
	difference_groupbox->setLayout(difference_layout);

	offset_value_lineedit_ = new QLineEdit();
	offset_value_lineedit_->setMaximumWidth(50);
	offset_value_lineedit_->setAlignment(Qt::AlignRight);
	offset_unit_label_ = new QLabel("degree");

	QHBoxLayout *offset_layout = new QHBoxLayout();
	offset_layout->addWidget(offset_value_lineedit_);
	offset_layout->addWidget(offset_unit_label_);

	QGroupBox *offset_groupbox = new QGroupBox("Offset");
	offset_groupbox->setLayout(offset_layout);

	previous_pushbutton_ = new QPushButton("< Previous");
	finish_pushbutton_ = new QPushButton("Finish");
	cancel_pushbutton_ = new QPushButton("Cancel");

	QHBoxLayout *pushbutton_layout = new QHBoxLayout();
	pushbutton_layout->addStretch();
	pushbutton_layout->addWidget(previous_pushbutton_);
	pushbutton_layout->addWidget(finish_pushbutton_);
	pushbutton_layout->addWidget(cancel_pushbutton_);

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(title, 0, 0, 1, 4);
	layout->addWidget(movement_type_groupbox, 1, 0);
	layout->addWidget(offset_groupbox, 1, 1);
	layout->addWidget(difference_groupbox, 1, 2);
	layout->setColumnStretch(3, 1);
	layout->addLayout(pushbutton_layout, 2, 0, 1, 4);

	setLayout(layout);

	connect(previous_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(goPreviousAssemble()));
	connect(finish_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(finishAssemble()));
	connect(cancel_pushbutton_, SIGNAL(clicked()), state_machine, SLOT(cancelAssemble()));

	connect(revolute_radiobutton_, SIGNAL(toggled(bool)), this, SLOT(revoluteToggled(bool)));
	connect(prismatic_radiobutton_, SIGNAL(toggled(bool)), this, SLOT(prismaticToggled(bool)));
	connect(offset_value_lineedit_, SIGNAL(editingFinished()), this, SLOT(offsetValueEditingFinished()));

	initialize();
}

void AssembleStep4Widget::initialize(void)
{
	revolute_radiobutton_->setChecked(true);
	offset_value_lineedit_->setText(0);

	offset_value_degree_ = QString("0");
	offset_value_mm_ = QString("0");

	offset_unit_label_->setText(QString("degree"));
	offset_value_lineedit_->setText(offset_value_degree_);
}

void AssembleStep4Widget::setOffsetUnitDegree(void)
{
	offset_unit_label_->setText(QString("degree"));
}

void AssembleStep4Widget::setOffsetUnitMm(void)
{
	offset_unit_label_->setText(QString("mm"));
}

void AssembleStep4Widget::revoluteToggled(bool checked)
{
	if (checked)
	{
		state_machine_->setRevoluteJoint();
	}
}

void AssembleStep4Widget::prismaticToggled(bool checked)
{
	if (checked)
	{
		state_machine_->setPrismaticJoint();

	}

}

void AssembleStep4Widget::offsetValueEditingFinished(void)
{

}

void AssembleStep4Widget::setDifference(QString difference)
{
	difference_value_lineedit_->setText(difference);
}

AssembleMenuWidget::AssembleMenuWidget(StateMachine *state_machine)
	: QStackedWidget(Q_NULLPTR)
{
	assemble_step1_widget_ = new AssembleStep1Widget(state_machine);
	assemble_step2_widget_ = new AssembleStep2Widget(state_machine);
	assemble_step3_widget_ = new AssembleStep3Widget(state_machine);
	assemble_step4_widget_ = new AssembleStep4Widget(state_machine);

	addWidget(assemble_step1_widget_);
	addWidget(assemble_step2_widget_);
	addWidget(assemble_step3_widget_);
	addWidget(assemble_step4_widget_);

	setCurrentStep(1);
}

void AssembleMenuWidget::setCurrentStep(int step)
{
	if ((step > 0) && (step < 8))
	{
		setCurrentIndex(step - 1);
	}
}

void AssembleMenuWidget::setStep3NextButton(void)
{
	assemble_step3_widget_->setNextButton();
}

void AssembleMenuWidget::setStep3FinishButton(void)
{
	assemble_step3_widget_->setFinishButton();
}

void AssembleMenuWidget::choosedMatchedAxis(romas::AXIS& parent, romas::AXIS& child)
{
	assemble_step3_widget_->choosedAxis(parent, child);
}

int AssembleMenuWidget::getChoosedJointType(void)
{
	return 0;
}

double AssembleMenuWidget::getChoosedOffset(void)
{
	return 0;
}

void AssembleMenuWidget::initialize(void)
{
	assemble_step3_widget_->initialize();
	assemble_step4_widget_->initialize();
}

void AssembleMenuWidget::setMatchedAxisDisabled(romas::AXIS parent, romas::AXIS child)
{
	assemble_step3_widget_->setAxisDisabled(parent, child);
}

void AssembleMenuWidget::setOffsetUnitDegree(void)
{
	assemble_step4_widget_->setOffsetUnitDegree();
}

void AssembleMenuWidget::setOffsetUnitMm(void)
{
	assemble_step4_widget_->setOffsetUnitMm();
}

void AssembleMenuWidget::setDifference(QString difference)
{
	assemble_step4_widget_->setDifference(difference);
}


