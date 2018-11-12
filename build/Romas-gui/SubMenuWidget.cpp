
#include "SubMenuWidget.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <romas/Block.hpp>
#include <QIcon>
#include <QPixMap>
#include "MainWindow.hpp"

SubMenuWidget::SubMenuWidget(MainWindow *main_window)
	: main_window_(main_window)
{
	block_candidates_label_ = new QLabel("Block Candidates");
	block_candidates_listwidget_ = new QListWidget();

	block_candidates_listwidget_->setViewMode(QListWidget::IconMode);
	block_candidates_listwidget_->setIconSize(QSize(64, 64));

	block_candidates_listwidget_->setResizeMode(QListView::ResizeMode::Adjust);

	block_selected_label_ = new QLabel("Block Selected");
	block_selected_listwidget_ = new QListWidget();

	setBlockSelectedVisible(false);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(block_candidates_label_);
	layout->addWidget(block_candidates_listwidget_);
	layout->addWidget(block_selected_label_);
	layout->addWidget(block_selected_listwidget_);
	layout->addStretch();
	setLayout(layout);

	connect(block_candidates_listwidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), main_window_, SLOT(blockCandidateDoubleClickedEvent(QListWidgetItem*)));
}

const bool SubMenuWidget::isBlockCandidatesVisible(void)
{
	return block_candidates_listwidget_->isVisible();
}

void SubMenuWidget::setBlockCandidatesVisible(bool visible)
{
	block_candidates_label_->setVisible(visible);
	block_candidates_listwidget_->setVisible(visible);
}

const bool SubMenuWidget::isBlockSelectedVisible(void)
{
	return block_selected_listwidget_->isVisible();
}

void SubMenuWidget::setBlockSelectedVisible(bool visible)
{
	block_selected_label_->setVisible(visible);
	block_selected_listwidget_->setVisible(visible);
}

void SubMenuWidget::addBlockCandidate(romas::BlockInfo block_info)
{
	if (!isBlockCandidatesVisible())
		setBlockCandidatesVisible(true);

	QString name = QString::fromStdString(block_info.name());

	if (block_info.isBase())
		name += QString(" (B)");

	if (block_info.isTool())
		name += QString(" (T)");

	std::string thumbnail_filename = block_info.thumbnail();
	std::string path = block_info.xmlPath();
	QString thumbnail = QString::fromStdString(path + thumbnail_filename);

	QPixmap pixmap(thumbnail, "PNG");

	QIcon icon(pixmap);
	
	BlockCandidate *block_candidate = new BlockCandidate(icon, name, block_info);

	//	block_candidates_listwidget_->addItem(name);

	block_candidates_listwidget_->addItem(block_candidate);

}

BlockCandidate::BlockCandidate(const QIcon &icon, const QString &text, romas::BlockInfo block_info)
	: QListWidgetItem(icon, text)
	, block_info_(block_info)
{


}

