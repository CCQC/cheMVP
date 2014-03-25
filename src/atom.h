#ifndef AtomDrawing_H
#define AtomDrawing_H

#include <QGraphicsItem>
#include <QString>
#include <QtGui>
#include <QMap>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <math.h>

#include "error.h"
#include "defines.h"
#include "drawinginfo.h"

class Atom : public QGraphicsEllipseItem
{
public:
    enum {Type = UserType + ATOMTYPE};
    enum FontSizeStyle {SmallLabel, LargeLabel};

    static QMap<QString, QVariant> labelToVdwRadius;
    static QMap<QString, QVariant> labelToMass;
    static QMap<QString, QVariant> labelToColor;

    static void fillLabelToVdwRadiusMap();
    static void fillLabelToMassMap();
    static void fillLabelToColorMap();

    Atom(QString element, DrawingInfo *info, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const {return Type;}
    int fontSize() const {return myFontSize;}
    double radius() const {return myRadius;}
    double mass() const {return myMass;}
    double x() const {return myX;}
    double y() const {return myY;}
    double z() const {return myZ;}
    bool labelHasSubscript() {return !myLabelSubscript.isEmpty();}
    bool labelHasSuperscript() {return !myLabelSubscript.isEmpty();}
    int ID() const {return myID;}
    double scaleFactor() const {return myScaleFactor;}
    double effectiveRadius() const {return myEffectiveRadius;}
    QString symbol() const {return mySymbol;}
    QString label() {return (myLabel +
                             (myLabelSubscript.size() ? "_" + myLabelSubscript : "") +
                             (myLabelSuperscript.size() ? "^" + myLabelSuperscript : ""));}
    void computeRadius();
    void setLabelSubscript(const QString &string) {myLabelSubscript = string;}
    void setLabelSuperscript(const QString &string) {myLabelSuperscript = string;}
    void setLabelFontSize(int val) {myFontSize = val;
        _info->getAtomLabelFont().setPointSizeF(double(val)*myEffectiveRadius/20.0);}
    void setScaleFactor(double val) {myScaleFactor = val;}
    void setX(double val) {myX = val;}
    void setY(double val) {myY = val;}
    void setZ(double val) {myZ = val;}
    void setLabel(const QString &text);
    void setAcceptsHovers(bool arg) {if(!arg) hoverOver = false; setAcceptHoverEvents(arg);}
    void setDrawingStyle(DrawingInfo::DrawingStyle style);
    void setColor(QColor color) {fill_color = color;}
    void setFontSizeStyle(FontSizeStyle style);
    void setID(int val) {myID = val;}
	void serialize(QXmlStreamWriter* writer);
	static Atom* deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo);

    static double bondLength(Atom*, Atom*);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    double myEffectiveRadius;
    FontSizeStyle myFontSizeStyle;
    double myMass;
    double myRadius;
    double myX;
    double myY;
    double myZ;
    double myScaleFactor;
    QString myLabel;
    QString mySymbol;
    QString myLabelSubscript;
    QString myLabelSuperscript;
    int myFontSize;
    int myID;
    bool hoverOver;
    QColor fill_color;
    DrawingInfo *_info;
};

#endif
