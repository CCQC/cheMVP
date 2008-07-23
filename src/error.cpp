#include "error.h"

void error(QString message, char * file, int line)
{
	message += "\n\nThe error occured in ";
	message += file;
	message += ", line ";
	message += QString::number(line);
	
	QMessageBox msgBox(QMessageBox::Warning, QDialog::tr("QMessageBox::warning()"),
	                        QDialog::tr(message.toLatin1()), 0, 0);
	msgBox.exec();
}
