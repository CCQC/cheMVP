#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QStackedLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QGridLayout>
#include <QListWidget>
#include <QPainter>
#include <QWidget>
#include <QDialog>
#include <QList>

#include "drawinginfo.h"
#include "drawingcanvas.h"
#include "defines.h"
#include "atombutton.h"

class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences(DrawingCanvas* d, int s);

    static QMap<QString, QVariant> _colorChanges;

public slots:
    void savePreferences();
    void restoreDefaults();
    void revert();

protected:
    int _drawingStyle;
    DrawingCanvas* _canvas;

    QWidget* _periodicTable;
    QList<QToolButton*> _atomButtons;

    QListWidget* _listWidget;
    QStackedWidget* _stackedWidget;
    QPushButton* _applyButton;
    QPushButton* _closeButton;
    QPushButton* _revertButton;

    QWidget* createPeriodicTable();
    QToolButton* makeAtomButton(const char* label);
};

#endif /* PREFERENCES_H */
