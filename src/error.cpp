#include "error.h"

void error(QString message)
{
	QMessageBox msgBox(QMessageBox::Warning, QDialog::tr("QMessageBox::warning()"),
                       QDialog::tr(message.toLatin1()), 0, 0);
    msgBox.exec();
}

void error(QString message, char* filename, int line)
{
    message += "\n\nThe error occured in ";
    message += filename;
    message += ", line ";
    message += QString::number(line);
	error(message);
}



