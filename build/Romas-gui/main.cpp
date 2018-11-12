#include "MainWindow.hpp"
#include <QtWidgets/QApplication>
#include <QWidget>

#include <locale>

int main(int argc, char *argv[])
{

	setlocale(LC_ALL, "Korean");
	QApplication a(argc, argv);

	MainWindow w;
//	QWidget w;
//	QMainWindow w;
	w.show();
	return a.exec();
}
