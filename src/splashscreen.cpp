#include "splashscreen.h"

SplashScreen::SplashScreen(const QPixmap &image):
	QSplashScreen(image)
{
    showMessage("Version " + QString(CHEMVP_VERSION)+"\nCreated by Andy Simmonett and Justin Turney,\nCenter for Computational Chemistry, University of Georgia",
                        Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    show();
    startTimer(5000);
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::timerEvent(QTimerEvent *event)
{
	close();
}

void SplashScreen::keyPressEvent(QKeyEvent *)
{
	close();
}

void SplashScreen::mousePressEvent(QMouseEvent *)
{
	close();
}
