#include <QtGui>
#include <QString>
#include <QPixmap>
#include <iostream>

#include "mainwindow.h"
#include "splashscreen.h"
#include "fileparser.h"
#include "defines.h"


int main(int argv, char *args[])
{

    Q_INIT_RESOURCE(chemvp);

    QApplication app(argv, args);

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

    //Use the file name (if any) to create a new parser object
    FileParser *parser = new FileParser(cmd_line_arg);
    QPixmap pixmap("../images/splash.png");
    SplashScreen splash(pixmap);
    splash.startTimer(3500); // ~5 sec
    MainWindow mainWindow(parser);
    //Set the icon in the bar at the top of the window
    //but not on X11 - it seems to be having problems
#ifndef Q_WS_X11
    app.setWindowIcon(QIcon("../images/icon.png"));
#endif
    mainWindow.setWindowIconText("cheMVP");
	mainWindow.setWindowTitle("cheMVP");

    mainWindow.setGeometry(30, 50, 1200, 700);
    mainWindow.raise();
    mainWindow.showNormal();

    if(argv == 3){
        mainWindow.setCurrentSaveFile(args[2]);
        mainWindow.saveAndExit();
    }

    return app.exec();
}
