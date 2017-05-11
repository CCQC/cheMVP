#include <QtGui>
#include <QString>
#include <QPixmap>
#include <iostream>

#include "mainwindow.h"
#include "application.h"
#include "splashscreen.h"
#include "fileparser.h"
#include "defines.h"

int main(int argv, char *args[])
{
	Q_INIT_RESOURCE(chemvp);

	Application app(argv, args);
	QCoreApplication::setOrganizationName(COMPANY_NAME);
	QCoreApplication::setOrganizationDomain(COMPANY_DOMAIN);
	QCoreApplication::setApplicationName(PROGRAM_NAME);

	//Set the icon in the bar at the top of the window but not on X11 - it seems to be having problems
#ifndef Q_WS_X11
	app.setWindowIcon(QIcon("../images/icon.png"));
#endif

	//Check for, and load an xyz file if requested
	QString cmd_line_arg;
	if(argv<=1){
		cmd_line_arg = "";
	}else{
		cmd_line_arg=args[1];
		if(cmd_line_arg == "--help" || cmd_line_arg == "-h" || argv>3){
			std::cout<<"Usage: chemvp [coordfile]"<<std::endl<<"Where coordfile is an xyz file"<<std::endl;
			exit(EXIT_FAILURE);
		}else{
		}
	}

	if(app.mainWindow == NULL)
	{
		// Use the file name (if any) to create a new parser object
		FileParser *parser = new FileParser(cmd_line_arg);
		app.mainWindow = new MainWindow(parser);

		app.mainWindow->raise();
		app.mainWindow->showNormal();

		if(argv == 3){
			app.mainWindow->setCurrentSaveFile(args[2]);
			app.mainWindow->saveAndExit();
		}
	}

	return app.exec();
}
