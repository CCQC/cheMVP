#ifndef DrawingCanvas_H
#define DrawingCanvas_H

#include <QGraphicsScene>
#include <QMenu>
#include <QList>
#include <math.h>
#include "atom.h"
#include "bond.h"
#include "angle.h"
#include "arrow.h"
#include "drawinginfo.h"
#include "fileparser.h"


class DrawingCanvas : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { AddBond, AddArrow, AddText, Select, Rotate, TempMove, TempMoveAll };
    
    DrawingCanvas(QMenu *itemMenu, DrawingInfo *drawingInfo, FileParser *parser, QObject *parent = 0);

//    void setElementToAdd(const QString &element) {elementToAdd = element;}
    void clearAll();
    void performRotation();
    void updateBonds();
    void updateAngles();
    void updateArrows();
    void updateTextLabels();
    void setAcceptsHovers(bool arg);
    void loadFromParser();
    void refresh();
    void setAtomLabels(QString text);
    void drawBackground(QPainter *painter, const QRectF &rect);
    const QCursor& rotateCursor() {return myRotateCursor;}
public slots:
	void unselectAll();
	void selectAll();
	void setBackgroundOpacity(int val);
	void setBackgroundColor();
	void toggleBondDashing();
	void toggleAtomNumberSubscripts();
	void atomLabelFontChanged(const QFont &);
	void atomLabelFontSizeChanged(const QString &);
    void setMode(Mode mode);
    void toggleBondLabels();
    void toggleAngleLabels();
    void setAtomDrawingStyle(int style);
    void setAtomFontSizeStyle(int style);
    void setBondLabelPrecision(int val);
    void setAngleLabelPrecision(int val);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void translateToCenterOfMass();

signals:
//    void itemSelected(QGraphicsItem *item);
    void xRotChanged(int phi);
    void yRotChanged(int phi);
    void zRotChanged(int phi);
    void mouseModeChanged(int mode);

private:
	void getAngleInBounds(int &phi);
    void setXRotation(int phi);
    void setYRotation(int phi);
    void setZRotation(int phi);
    double bondLength(Atom* atom1, Atom* atom2);
    bool isBonded(Atom* atom1, Atom* atom2);
    QList<Angle*>::iterator angleExists(Atom* atom1, Atom* atom2, Atom* atom3);

    // TODO add new falfkasf;laksjf
    QMenu *myItemMenu;
    FileParser *parser;
    DrawingInfo *drawingInfo;
    QGraphicsItem *myTempMoveItem;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QPointF lastMousePosition;
    QGraphicsLineItem *bondline;
    Arrow *myArrow;
    QGraphicsRectItem *selectionRectangle;
//    QString elementToAdd;
    QPointF mouseOrigin;
    QColor myBackgroundColor;
    QCursor myMoveCursor;
    QCursor myRotateCursor;
    int myBackgroundAlpha;
    int numMouseMoves;
    bool myAtomNumberSubscripts;
    QList<Atom*> atomsList;
    QList<Bond*> bondsList;
    QList<Angle*> anglesList;
    QList<Arrow*> arrowsList;
    QList<Label*> textLabelsList;
};

#endif
