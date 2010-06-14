#ifndef DrawingCanvas_H
#define DrawingCanvas_H

#include <QGraphicsScene>
#include <QMessageBox>
#include <QtGui>
#include <QMenu>
#include <QList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <math.h>
#include "atom.h"
#include "bond.h"
#include "angle.h"
#include "arrow.h"
#include "defines.h"
#include "molecule.h"
#include "drawinginfo.h"
#include "fileparser.h"

class Angle;

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

	DrawingCanvas(DrawingInfo *drawingInfo, FileParser *parser, QObject *parent = 0);

	void clearAll();
	void storeLabeledBonds();
	void restoreLabeledBonds();
	void performRotation();
	void updateBonds();
	void updateAngles();
	void updateArrows();
	void updateTextLabels();
	void setAcceptsHovers(bool arg);
	void loadFromParser();
	void setAtomLabels(QString text);
	void rotateFromInitialCoordinates();
	void drawBackground(QPainter *painter, const QRectF &rect);
	const QCursor& rotateCursor() {return myRotateCursor;}
	void serialize(QXmlStreamWriter* writer);
	static DrawingCanvas* deserialize(QXmlStreamReader* reader, DrawingInfo *drawingInfo, FileParser *parser);
	QList<Bond*> getBonds() {return bondsList;}
	QList<Atom*> getAtoms() {return atomsList;}
	void addBondLabel(int i);
	void updateAtomColors(QMap<QString, QVariant> n);
	int getBackgroundOpacity() {return myBackgroundAlpha;}

public slots:
	void refresh();
	void unselectAll();
	void selectAll();
	void setBackgroundOpacity(int val);
	void setBackgroundColor();
	void setAtomColors();
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
	void determineRotationAngles(); //HPS
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void translateToCenterOfMass();
	void focusOutEvent (QFocusEvent *event);
//	void keyPressEvent(QKeyEvent *event);

signals:
	void xRotChanged(int phi);
	void yRotChanged(int phi);
	void zRotChanged(int phi);
	void mouseModeChanged(int mode);
	void updateTextToolbars();

private:
	double bondLength(Atom* atom1, Atom* atom2);
	bool isBonded(Atom* atom1, Atom* atom2);
	void svdcmp(double **a, int m, int n, double w[], double **v);
	double pythag(double a, double b);
	QList<Angle*>::iterator angleExists(Atom* atom1, Atom* atom2, Atom* atom3);

	double rotationMatrix[3][3];
	double xRot;
	double yRot;
	double zRot;
	bool leftButtonDown;
	FileParser *parser;
	DrawingInfo *drawingInfo;
	QGraphicsItem *myTempMoveItem;
	Mode myMode;
	Arrow *myArrow;
	QPointF startPoint;
	QPointF lastMousePosition;
	QPointF mouseOrigin;
	QGraphicsLineItem *bondline;
	QGraphicsRectItem *selectionRectangle;
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
	QList<int> persistantBonds;
};

#endif
