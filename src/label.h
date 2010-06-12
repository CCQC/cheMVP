#ifndef LABEL_H_
#define LABEL_H_

#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>
#include <iostream>
#include "defines.h"
#include "drawinginfo.h"
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;

class Label : public QGraphicsTextItem
{
	Q_OBJECT

public:
	enum LabelType { BondLabelType = UserType + BONDLABELTYPE,
					 AngleLabelType = UserType + ANGLELABELTYPE,
					 TextLabelType = UserType + TEXTLABELTYPE};

	Label(LabelType type, double value, DrawingInfo *info, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	void focusOutEvent (QFocusEvent *event);
	int type() const { return myType; }
	double dX() {return myDX;}
	double dY() {return myDY;}
	void setDX(double val) {myDX = val;}
	void setDY(double val) {myDY = val;}
	void setFontSize(int val) {myFontSize = val; updateFontSize();}
	int fontSize() const {return myFontSize;}
	void updateLabel();

	void serialize(QXmlStreamWriter* writer);
	static Label* deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QGraphicsScene* scene);

signals:
	void selectedChange(QGraphicsItem *item);

public slots:
	void updateFontSize();

protected:
	void keyPressEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
	LabelType myType;
	QString myString;
	int    myFontSize;
	double myDX;
	double myDY;
	double myValue;
	DrawingInfo* _info;

	class FontFormatTuple { // Holds the character range for a specific font. Credit: Jesse Yates
	public:
		int start, end;
		QString format;

		FontFormatTuple(QString f, int s, int e) { format = f; start = s; end = e; };
		static bool compareTo(FontFormatTuple* first, FontFormatTuple* second) { return first->start < second->start; };
		void serialize(QXmlStreamWriter *writer) {
			writer->writeStartElement("FontFormatTuple");
			writer->writeAttribute("format", format);
			writer->writeAttribute("start", QString("%1").arg(start));
			writer->writeAttribute("end", QString("%1").arg(end));
			writer->writeEndElement();
		};
		static FontFormatTuple* deserialize(QXmlStreamReader* reader) {
			reader->readNextStartElement();
			QXmlStreamAttributes a = reader->attributes();
			reader->skipCurrentElement();
			return new FontFormatTuple(a.value("format").toString(), a.value("start").toString().toInt(), a.value("end").toString().toInt());
		};
	};
};

#endif /*LABEL_H_*/
