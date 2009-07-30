#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QStackedLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QGridLayout>
#include <QListWidget>
#include <QPainter>
#include <QWidget>
#include <QDialog>
#include <QList>

#include "atombutton.h"
#include "drawinginfo.h"

class Preferences : public QDialog
{
	Q_OBJECT
	
public:
	Preferences(QList<Atom*> a, int s);
	
	static QMap<QString, QVariant> _colorChanges;
	
public slots:
	void applyPreferences();
	
protected:
	QList<Atom*> _atomsList;
	int _drawingStyle;
	
	QWidget* _periodicTable;
	
	QListWidget* _listWidget;
	QStackedLayout* _stackedLayout;
	QPushButton* _applyButton;
	QPushButton* _closeButton;
	
	QWidget* createPeriodicTable();
	QToolButton* makeAtomButton(const char* label);
};

#endif /* PREFERENCES_H */
