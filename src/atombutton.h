#ifndef ATOMBUTTON_H
#define ATOMBUTTON_H

#include <QToolButton>
#include <QPushButton>
#include <QDialog>
#include <QColor>

#include "atom.h"
#include "preferences.h"

class AtomButton : public QToolButton
{
	Q_OBJECT
	
public:
	AtomButton(const char* label);

public slots:
	void setAtomDefaultColor();
	
protected:
	void refreshColor();
	QString _label;
};

#endif /* ATOMBUTTON_H */
