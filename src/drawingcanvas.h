#ifndef DrawingCanvas_H
#define DrawingCanvas_H

#include <QGraphicsScene>
#include <QMenu>
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
    void setElementToAdd(const QString &element) {elementToAdd = element;}
    void clearAll();
    void unselectAll();
    void performRotation();
    void updateBonds();
    void updateAngles();
    void updateArrows();
    void setAcceptsHovers(bool arg);
    void loadFromParser();
    void refresh();
    void setAtomLabels(QString text);
    void drawBackground(QPainter *painter, const QRectF &rect);
public slots:
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
    std::vector<Angle*>::iterator angleExists(Atom* atom1, Atom* atom2, Atom* atom3);

    
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
    QString elementToAdd;
    QPointF mouseOrigin;
    QColor myBackgroundColor;
    int myBackgroundAlpha;
    int numMouseMoves;
    bool myAtomNumberSubscripts;
    std::vector<Atom*> atomsList;
    std::vector<Bond*> bondsList;
    std::vector<Angle*> anglesList;
    std::vector<Arrow *> arrowsList;
};

#endif
