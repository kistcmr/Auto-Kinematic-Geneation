#ifndef __SUB_MENU_WIDGET__
#define __SUB_MENU_WIDGET__

#include <QWidget>
#include <QListWidget>
#include <romas/romas.hpp>
#include <romas/Block.hpp>

class QLabel;
class QListWidget;
class MainWindow;


class SubMenuWidget : public QWidget
{
	Q_OBJECT

public :
	SubMenuWidget(MainWindow *main_window);

	void addBlockCandidate(romas::BlockInfo block_info);

	const bool isBlockCandidatesVisible(void);
	const bool isBlockSelectedVisible(void);
	void setBlockCandidatesVisible(bool visible);
	void setBlockSelectedVisible(bool visible);

public slots:


private :
	QLabel *block_candidates_label_;
	QListWidget *block_candidates_listwidget_;
	QLabel *block_selected_label_;
	QListWidget *block_selected_listwidget_;
	MainWindow *main_window_;
};



class BlockCandidate : public QListWidgetItem
{

public:
	BlockCandidate(const QIcon &icon, const QString &text, romas::BlockInfo block_info);

	romas::BlockInfo getBlockInfo(void) { return block_info_; }
private :

	romas::BlockInfo block_info_;
};

#endif
