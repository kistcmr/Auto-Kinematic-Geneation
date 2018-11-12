#ifndef __CONFIG_PAGES_HPP__
#define __CONFIG_PAGES_HPP__

#include <QWidget>

class QLineEdit;
class QPushButton;

class BlockPage : public QWidget
{
	Q_OBJECT

public:
	BlockPage(QWidget *parent = 0);
	bool isChanged(void) { return changed_; }
	void setBlockLibraryPath(QString path);
	QString blockLibraryPath(void);

public slots :
	void blockLibraryPathPushButtonClicked(void);

private :
	bool changed_;

	QLineEdit *block_library_path_line_edit_;
	QPushButton *block_library_path_push_button_;
};

class CompilePage : public QWidget
{
	Q_OBJECT

public:
	CompilePage(QWidget *parent = 0);
	bool isChanged(void) { return changed_; }

	void setVcvarsallPath(QString path);
	QString vcvarsallPath(void);

	void setMsbuildPath(QString path);
	QString msbuildPath(void);

public slots :
	void vcvarsallPathPushButtonClicked(void);
	void msbuildPathPushButtonClicked(void);

private:
	bool changed_;

	QLineEdit *vcvarsall_path_line_edit_;
	QLineEdit *msbuild_path_line_edit_;
	QPushButton *vcvarsall_path_push_button_;
	QPushButton *msbuild_path_push_button_;

};

class SimulationPage : public QWidget
{
	Q_OBJECT

public:
	SimulationPage(QWidget *parent = 0);
	bool isChanged(void) { return changed_; }
	void setBlockLibraryPath(QString path);
	QString blockLibraryPath(void);

	public slots :
	void blockLibraryPathPushButtonClicked(void);

private:
	bool changed_;

	QLineEdit *block_library_path_line_edit_;
	QPushButton *block_library_path_push_button_;
};

#endif
