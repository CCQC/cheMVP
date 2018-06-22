#ifndef SPLASHSCREEN_H_
#define SPLASHSCREEN_H_

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimerEvent>

#include "defines.h"

class SplashScreen : public QSplashScreen
{
  public:
    SplashScreen(const QPixmap &image);
    ~SplashScreen();

  protected:
    void timerEvent(QTimerEvent *);
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
};

#endif /*SPLASHSCREEN_H_*/
