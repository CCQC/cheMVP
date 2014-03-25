#include "atombutton.h"
#include <QColorDialog>

QMap<QString, QVariant> Preferences::_colorChanges;

AtomButton::AtomButton(DrawingCanvas* d, const char* label)
{
    _label = label;
    _canvas = d;
    
    int dimension1 = 18;
    int dimension2 = 18;

    QSize icon_box(dimension1, dimension2);
    // Start by drawing the button icon
    QPixmap icon(icon_box);
    icon.fill(Atom::labelToColor.value(_label).value<QColor>());
    QPainter painter(&icon);
    QString text(_label);
    QFont font;
    font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(QRectF(0, 0, dimension1, dimension2), Qt::AlignCenter, text);

    // Now make the button and set it up
    setIcon(icon);
    setText(text);
    setIconSize(icon_box);
    setAutoExclusive(true);

    connect(this, SIGNAL(clicked()), this, SLOT(setAtomColor()));
    connect(this, SIGNAL(clicked()), this, SLOT(toggle()));
}

void AtomButton::setAtomColor()
{
    QColor color = QColorDialog::getColor();
    if(color.isValid())
    {
        Preferences::_colorChanges[_label] = color;
        _canvas->updateAtomColors(Preferences::_colorChanges);
        refreshColor();
    }
}

void AtomButton::refreshColor()
{
    int dimension1 = 18;
    int dimension2 = 18;

    QSize icon_box(dimension1, dimension2);
    QPixmap icon(icon_box);
    icon.fill(Preferences::_colorChanges.value(_label).value<QColor>());
    QPainter painter(&icon);
    QString text(_label);
    QFont font;
    font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(QRectF(0, 0, dimension1, dimension2), Qt::AlignCenter, text);

    setIcon(icon);
    setText(text);
    setIconSize(icon_box);

}
