#include "splashscreen.h"

SplashScreen::SplashScreen(const QPixmap &image):
	QSplashScreen(image, Qt::WindowStaysOnTopHint)
{
    showMessage("Version " + QString(CHEMVP_VERSION)+"\nCreated by Andy Simmonett, Justin Turney, and H. Parker Shelton\nCenter for Computational Chemistry, University of Georgia",
                        Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    show();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);
	close();
}

void SplashScreen::keyPressEvent(QKeyEvent *event)
{
	Q_UNUSED(event);
	close();
}

void SplashScreen::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	close();
}
