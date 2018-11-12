#include "StateMachine.hpp"
#include "CentralWidget.hpp"
#include "AssembleMenuWidget.hpp"
#include "AssembleSpaceWidget.hpp"
#include <QSplitter>
#include <QHBoxLayout>
#include <QMessagebox>
#include <QInputDialog>
#include <QFileDialog>
#include "RobotNameDialog.hpp"

StateMachine::StateMachine()
	: assemble_space_widget_(nullptr)
	, assemble_menu_widget_(nullptr)
{

}

void StateMachine::initialize(AssembleSpaceWidget *assemble_space_widget, AssembleMenuWidget *assemble_menu_widget)
{
	assemble_space_widget_ = assemble_space_widget;
	assemble_menu_widget_ = assemble_menu_widget;
	current_state_ = State::Empty;
}

void StateMachine::cancelAssemble(void)
{
	QMessageBox dialog(
		QMessageBox::Icon::Question,
		QString::fromLocal8Bit("Question"),
		QString::fromLocal8Bit("������ ����Ͻðڽ��ϱ�?"),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

	if (QMessageBox::No == dialog.exec())
		return;

	assemble_menu_widget_->setVisible(false);
	assemble_space_widget_->cancelAssemble();
	current_state_ = State::NonEmpty;
}

void StateMachine::goNextAssemble(void)
{
	romas::AXIS parent_axis;
	romas::AXIS child_axis;
	romas::AXIS axis;

	romas::JointType joint_type;

	romas::ContactPointType parent_type, child_type;

	romas::AXIS parent_rot_axis, child_rot_axis;

	switch (current_state_)
	{
	default :
		break;
	case State::AssembleStep3:
		current_state_ = State::AssembleStep4;
		assemble_menu_widget_->setCurrentStep(4);
		assemble_menu_widget_->choosedMatchedAxis(parent_axis, child_axis);
		assemble_space_widget_->setMatchedAxis(parent_axis, child_axis);
		assemble_space_widget_->getContactPointType(parent_type, child_type);


		QString difference = assemble_space_widget_->getDifference();

		assemble_menu_widget_->setDifference(difference);

		break;
	}
}

void StateMachine::goPreviousAssemble(void)
{
	switch (current_state_)
	{
	case State::AssembleStep2:
		assemble_menu_widget_->setCurrentStep(1);
		current_state_ = State::AssembleStep1;
		break;
	case State::AssembleStep3:
		assemble_menu_widget_->setCurrentStep(2);
		current_state_ = State::AssembleStep2;
		break;
	case State::AssembleStep4:
		assemble_menu_widget_->setCurrentStep(3);
		current_state_ = State::AssembleStep3;
		break;
	}
}

void StateMachine::pickParent(romas::Block *block, int joint_ID)
{
	switch (current_state_)
	{
	case State::AssembleStep1:
	{
		if (!assemble_space_widget_->pickAssembleStep1(block, joint_ID))
			break;
		assemble_menu_widget_->setCurrentStep(2);
		current_state_ = State::AssembleStep2;
		break;
	}
	}

}
void StateMachine::pickChild(romas::Block *block, int joint_ID)
{
	switch (current_state_)
	{
	case State::AssembleStep2:
	{
		if (!assemble_space_widget_->pickAssembleStep2(block, joint_ID))
			break;

		current_state_ = State::AssembleStep3;
		assemble_menu_widget_->setCurrentStep(3);

		assemble_space_widget_->updateBlockCandidateByRotAxis();
		romas::AXIS parent_rot_axis, child_rot_axis;
		assemble_space_widget_->getRotAxis(parent_rot_axis, child_rot_axis);
		assemble_menu_widget_->setMatchedAxisDisabled(parent_rot_axis, child_rot_axis);

		romas::JointType joint_type = assemble_space_widget_->getJointType();
		if (joint_type == romas::JointType::Fixed)
			assemble_menu_widget_->setStep3FinishButton();
		else
			assemble_menu_widget_->setStep3NextButton();

		break;
	}
	}
}

void StateMachine::addBlock(romas::BlockInfo& block_info)
{
	switch (current_state_)
	{
	case State::Empty:
	{
		if (!block_info.isBase())
		{
			QMessageBox dialog(
				QMessageBox::Icon::Warning,
				QString::fromLocal8Bit("Warning"),
				QString::fromLocal8Bit("ó�� �����ϴ� Block�� Base �Ӽ��� ������ �־�� �մϴ�."),
				QMessageBox::StandardButton::Ok
			);
			dialog.exec();
		}
		else
		{
			assemble_space_widget_->addBlockInEmpty(block_info);
			current_state_ = State::NonEmpty;
		}
	}
	break;
	case State::NonEmpty:
		if (block_info.isBase())
		{
			QMessageBox dialog(
				QMessageBox::Icon::Warning,
				QString::fromLocal8Bit("Warning"),
				QString::fromLocal8Bit("Base �Ӽ��� ���� Block�� �̹� ����ϰ� �ֽ��ϴ�."),
				QMessageBox::StandardButton::Ok
			);
			dialog.exec();
		}
		else
		{
			current_state_ = State::AssembleStep1;
			assemble_space_widget_->addBlockInNonEmpty(block_info);
			assemble_menu_widget_->initialize();
			assemble_menu_widget_->setCurrentStep(1);
			assemble_menu_widget_->setVisible(true);
		}
		break;
	default :
	case State::AssembleStep1:
	case State::AssembleStep2:
	case State::AssembleStep3:
	case State::AssembleStep4:
	case State::AssembleStep5:
	case State::AssembleStep6:
	case State::AssembleStep7:
		{
			QMessageBox dialog(QMessageBox::Icon::Warning,
				QString::fromLocal8Bit("Warning"),
				QString::fromLocal8Bit("���� �߿��� Block�� �߰��� �� �����ϴ�."),
				QMessageBox::StandardButton::Ok);
			dialog.exec();
			break;
		}
	}
}

void StateMachine::finishAssemble(void)
{
	switch (current_state_)
	{
	case State::AssembleStep4 :
	case State::AssembleStep7 :
	{
		QMessageBox dialog(
			QMessageBox::Icon::Question,
			QString::fromLocal8Bit("Question"),
			QString::fromLocal8Bit("�����Ͻðڽ��ϱ�?"),
			QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
		);

		if (dialog.exec() == QMessageBox::StandardButton::No)
			return;

		assemble_space_widget_->finishAssemble();
		current_state_ = State::NonEmpty;
		assemble_menu_widget_->setVisible(false);
		break;
	}
	}
}

void StateMachine::changeParentMatchedAxis(romas::AXIS axis)
{
	if (!assemble_space_widget_)
		return;

	assemble_space_widget_->changeParentMatchedAxis(axis);
}

void StateMachine::changeChildMatchedAxis(romas::AXIS axis)
{
	if (!assemble_space_widget_)
		return;

	assemble_space_widget_->changeChildMatchedAxis(axis);
}

void StateMachine::setRevoluteJoint(void)
{
	if (!assemble_space_widget_)
		return;

	assemble_space_widget_->setRevoluteJoint();
}

void StateMachine::setPrismaticJoint(void)
{
	if (!assemble_space_widget_)
		return;

	assemble_space_widget_->setPrismaticJoint();
}

void StateMachine::loadAssembly(void)
{
	switch (current_state_)
	{
	case State::Empty:
	{
		QString filename = QFileDialog::getOpenFileName(assemble_space_widget_, tr("Load Assembly"), ".", tr("Assembly files (*.xml)"));
		if (!filename.isEmpty())
		{
			assemble_space_widget_->loadAssembly(filename.toStdString());
		}
		break;
	}
	case State::NonEmpty:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("�̹� �κ��� �ֽ��ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	default:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("���� �߿��� �κ��� �ҷ��� �� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}

	}

}

void StateMachine::saveAssembly(void)
{
	switch (current_state_)
	{
	case State::NonEmpty:
	{
		QString filename = QFileDialog::getSaveFileName(assemble_space_widget_, tr("Save Assembly"), ".", tr("Assembly files (*.xml)"));
		if (!filename.isEmpty())
		{
			assemble_space_widget_->saveAssembly(filename.toStdString());
		}
		break;
	}
	case State::Empty:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("������ �κ��� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	default:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("���� �߿��� �κ��� ������ �� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}

	}

}

void StateMachine::resetAssembly(void)
{
	switch (current_state_)
	{
	case State::NonEmpty:
	{
		QMessageBox dialog(
			QMessageBox::Icon::Question,
			QString::fromLocal8Bit("Question"),
			QString::fromLocal8Bit("�κ��� Reset�Ͻðڽ��ϱ�? Yes�ϸ� ���� ���̴� �κ��� ��� ������ϴ�."),
			QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
		);

		if (dialog.exec() == QMessageBox::StandardButton::No)
			return;

		assemble_space_widget_->resetAssembly();
		current_state_ = State::Empty;

		break;
	}
	case State::Empty:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("Reset�� �κ��� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	default:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("���� �߿��� �κ��� Reset�� �� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}

	}

}

void StateMachine::headerFile(void)
{
	switch (current_state_)
	{
	case State::NonEmpty :
	{
		bool ok;
		RobotNameDialog dialog;
		if (dialog.exec() == QDialog::Rejected)
			return;

		QString robot_name = dialog.robotName();
		if (robot_name.isEmpty())
		{
			QMessageBox dialog(QMessageBox::Icon::Warning,
				QString::fromLocal8Bit("Warning"),
				QString::fromLocal8Bit("�κ� �̸��� �������� �ʾҽ��ϴ�."),
				QMessageBox::StandardButton::Ok);
			dialog.exec();
			return;
		}
		QString directory = dialog.projectDirectory();
		if (directory.isEmpty())
		{
			QMessageBox dialog(QMessageBox::Icon::Warning,
				QString::fromLocal8Bit("Warning"),
				QString::fromLocal8Bit("�ڵ带 ������ ������ �������� �ʾҽ��ϴ�."),
				QMessageBox::StandardButton::Ok);
			dialog.exec();
			return;
		}

		romas::Solver solver = dialog.solver();
		QString define = dialog.define();
		assemble_space_widget_->generateHeaderFile(directory.toStdString(), robot_name.toStdString(), solver, define.toStdString());

		break;
	}
	case State::Empty :
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("�ڵ带 ������ �κ��� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	default :
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("���� �߿��� �ڵ带 ���� �� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}

	}
}

void StateMachine::simulator(void)
{
	switch (current_state_)
	{
	case State::NonEmpty:
	{
//		assemble_space_widget_->


		break;
	}
	case State::Empty:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("�ùķ��̼��� �κ��� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	default:
	{
		QMessageBox dialog(QMessageBox::Icon::Warning,
			QString::fromLocal8Bit("Warning"),
			QString::fromLocal8Bit("���� �߿��� �ùķ��̼��� �� �� �����ϴ�."),
			QMessageBox::StandardButton::Ok);
		dialog.exec();
		break;
	}
	}
}
