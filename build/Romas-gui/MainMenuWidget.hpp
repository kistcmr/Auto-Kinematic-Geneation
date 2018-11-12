#ifndef __MAIN_MENU_WIDGET__
#define __MAIN_MENU_WIDGET__

#include <QWidget>

class QPushButton;
class MainWindow;

class MainMenuWidget : public QWidget
{
	Q_OBJECT

public :
	MainMenuWidget(MainWindow *main_window);


	const bool isBlockHandlerVisible(void);
	void setBlockHandlerVisible(bool visible);

public slots:
	void simulatorClicked(void);
	void blockGeneratorClicked(void);


private :
	QPushButton* load_blocks_pushbutton_;
	QPushButton* save_assembly_pushbutton_;
	QPushButton* load_assembly_pushbutton_;
	QPushButton* reset_assembly_pushbutton_;
	QPushButton* header_file_pushbutton_;
	QPushButton* simulator_pushbutton_;
	QPushButton* block_generator_pushbutton_;

	QPushButton* delete_pushbutton_;
	QPushButton* edit_pushbutton_;
	QPushButton* target_pushbutton_;
	QPushButton* reference_pushbutton_;

	MainWindow* main_window_;
};


#endif
