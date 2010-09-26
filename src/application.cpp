#include "mainwindow.h"
#include "application.h"

using namespace std;

Application::Application(int &argc, char **argv): QApplication(argc, argv)
{
	this->mainWindow = NULL;
}

// Handle Mac OS X open file events
bool Application::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::FileOpen:
			return loadFile(static_cast<QFileOpenEvent *>(event)->file());
		default:
			return QApplication::event(event);
	}
}

// Launch new main window from open file event
bool Application::loadFile(const QString &fileName)
{
	if (fileName.isEmpty()) {
		return false;
	}

	// New program launch from file open
	bool newWindow = false;
	if(this->mainWindow == NULL)
	{
		FileParser *parser = new FileParser("");
		this->mainWindow = new MainWindow(parser);
		newWindow = true;
	}

	this->mainWindow->openProject(fileName, newWindow);
	this->mainWindow->raise();
	this->mainWindow->showNormal();

	return true;
}
