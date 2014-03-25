#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "mainwindow.h"

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int &argc, char **argv);
	bool loadFile(const QString &fileName);

	MainWindow* mainWindow;

protected:
	bool event(QEvent *event);
};

#endif // APPLICATION_H
