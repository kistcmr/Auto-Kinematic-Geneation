#include <QtWidgets>

#include "ConfigDialog.hpp"
#include "ConfigPages.hpp"
#include <QDialogButtonBox>

ConfigDialog::ConfigDialog(romas::Config *config)
{
	contents_widget_ = new QListWidget;
	contents_widget_->setViewMode(QListView::IconMode);
	contents_widget_->setIconSize(QSize(56, 56));
	contents_widget_->setMovement(QListView::Static);
	contents_widget_->setMaximumWidth(90);
	contents_widget_->setSpacing(8);
	contents_widget_->setMinimumHeight(400);
//	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	block_page_ = new BlockPage();
	compile_page_ = new CompilePage();
	simulation_page_ = new SimulationPage();

	pages_widget_ = new QStackedWidget;
	pages_widget_->addWidget(block_page_);
	pages_widget_->addWidget(compile_page_);
	pages_widget_->addWidget(simulation_page_);

//	QPushButton *closeButton = new QPushButton(tr("Close"));

	createIcons();
	contents_widget_->setCurrentRow(0);

//	connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);

	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(contents_widget_);
	horizontalLayout->addWidget(pages_widget_, 1);

//	QHBoxLayout *buttonsLayout = new QHBoxLayout;
//	buttonsLayout->addStretch(1);
//	buttonsLayout->addWidget(closeButton);

	dialog_button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
//	dialog_button_box_->setLayoutDirection()

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
//	mainLayout->addLayout(buttonsLayout);
	mainLayout->addWidget(dialog_button_box_);

	setLayout(mainLayout);

	setWindowTitle(tr("Configure Romas"));

	config_ = config;
	
	block_page_->setBlockLibraryPath(QString::fromStdString(config_->blockLibraryPath()));
	compile_page_->setVcvarsallPath(QString::fromStdString(config_->vcvarsallPath()));
	compile_page_->setMsbuildPath(QString::fromStdString(config_->msbuildPath()));

	setMinimumWidth(500);
//	setMinimumHeight(400);

//	connect(dialog_button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(dialog_button_box_->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(dialogButtonBoxAccepted()));
	connect(dialog_button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ConfigDialog::createIcons()
{
	QListWidgetItem *block_list_widget_item = new QListWidgetItem(contents_widget_);
	block_list_widget_item->setIcon(QIcon(":/MainWindow/Resources/bricks.png"));
	block_list_widget_item->setText(tr("Block"));
	block_list_widget_item->setTextAlignment(Qt::AlignCenter);
	block_list_widget_item->setSizeHint(QSize(60, 60));
	block_list_widget_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem *update_list_widget_item = new QListWidgetItem(contents_widget_);
	update_list_widget_item->setIcon(QIcon(":/MainWindow/Resources/compile.png"));
	update_list_widget_item->setText(tr("Compile"));
	update_list_widget_item->setTextAlignment(Qt::AlignCenter);
	update_list_widget_item->setSizeHint(QSize(60, 60));
	update_list_widget_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem *simulation_list_widget_item = new QListWidgetItem(contents_widget_);
	simulation_list_widget_item->setIcon(QIcon(":/MainWindow/Resources/simulation.png"));
	simulation_list_widget_item->setText(tr("Simulation"));
	simulation_list_widget_item->setTextAlignment(Qt::AlignCenter);
	simulation_list_widget_item->setSizeHint(QSize(60, 60));
	simulation_list_widget_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	connect(contents_widget_, &QListWidget::currentItemChanged, this, &ConfigDialog::changePage);
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;

	pages_widget_->setCurrentIndex(contents_widget_->row(current));
}

void ConfigDialog::dialogButtonBoxAccepted(void)
{
	if (block_page_->isChanged())
	{
		config_->setBlockLibraryPath(block_page_->blockLibraryPath().toStdString());
		config_->save();
	}

	if (compile_page_->isChanged())
	{
		config_->setVcvarsallPath(compile_page_->vcvarsallPath().toStdString());
		config_->setMsbuildPath(compile_page_->msbuildPath().toStdString());
		config_->save();
	}

	emit accept();
}
