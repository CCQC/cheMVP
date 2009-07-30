#ifndef ATOMBUTTON_H
#define ATOMBUTTON_H

#include <QToolButton>
#include <QPushButton>
#include <QDialog>
#include <QColor>

#include "atom.h"
#include "drawingcanvas.h"
#include "preferences.h"

class AtomButton : public QToolButton
{
    Q_OBJECT

public:
    AtomButton(DrawingCanvas* d, const char* label);

public slots:
    void setAtomColor();

protected:
    void refreshColor();
    DrawingCanvas* _canvas;
    QString _label;
};

#endif /* ATOMBUTTON_H */
