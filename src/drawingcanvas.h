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
    
    /* A quick explanation of the modes:-
     * AddBond     - Adds a bond
     * AddArrow    - Adds an arrow
     * AddText     - Adds a text box
     * Rotate      - Rotates the molecule about its center of mass
     * Select      - does many things, depending what is clicked:-
     * 		- empty space deselects everything, unless the mouse is dragged which draws a selection box
     * 		- an atom,  the atom is (de)selected, unless the mouse moves which triggers the TempMoveAll mode
     * 		- any other moveable item clicking (de)selects, unless the mouse is moved which triggers TempMove
     * TempMove    - The select mode has clicked an object and requested a move, so only this object is moved
     * TempMoveAll - If the object clicked was an atom, everything moves
     */	
    
    DrawingCanvas(QMenu *itemMenu, DrawingInfo *drawingInfo, FileParser *parser, QObject *parent = 0);

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
    void processProjectFile(QSettings &settings, bool saveFile);
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
	void focusOutEvent (QFocusEvent *event); 

signals:
    void xRotChanged(int phi);
    void yRotChanged(int phi);
    void zRotChanged(int phi);
    void mouseModeChanged(int mode);

private:
	void getAngleInBounds180(int &phi);
	void getAngleInBounds360(int &phi);
    void setXRotation(int phi);
    void setYRotation(int phi);
    void setZRotation(int phi);
    double bondLength(Atom* atom1, Atom* atom2);
    bool isBonded(Atom* atom1, Atom* atom2);
    QList<Angle*>::iterator angleExists(Atom* atom1, Atom* atom2, Atom* atom3);

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
    QPointF mouseOrigin;
    QColor myBackgroundColor;
    QCursor myMoveCursor;
    QCursor myRotateCursor;
    int myBackgroundAlpha;
    int numMouseMoves;
    QList<Atom*> atomsList;
    QList<Bond*> bondsList;
    QList<Angle*> anglesList;
    QList<Arrow*> arrowsList;
    QList<Label*> textLabelsList;
};

#endif
