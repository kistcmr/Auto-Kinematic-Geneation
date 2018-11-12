#ifndef __CONFIG_DIALOG_HPP_
#define __CONFIG_DIALOG_HPP_

#include <QDialog>
#include <romas/Config.hpp>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class BlockPage;
class CompilePage;
class QDialogButtonBox;
class SimulationPage;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(romas::Config *config);

public slots:
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	void dialogButtonBoxAccepted(void);

private:
	void createIcons();

	QListWidget *contents_widget_;
	QStackedWidget *pages_widget_;
	romas::Config *config_;
	BlockPage *block_page_;
	CompilePage *compile_page_;
	SimulationPage *simulation_page_;
	QDialogButtonBox *dialog_button_box_;
};

#endif
