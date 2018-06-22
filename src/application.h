#ifndef APPLICATION_H
#define APPLICATION_H

#include "mainwindow.h"
#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT

  public:
    Application(int &argc, char **argv);
    bool loadFile(const QString &fileName);

    MainWindow *mainWindow;

  protected:
    bool event(QEvent *event);
};

#endif // APPLICATION_H
