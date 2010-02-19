#include <QtGui>
#include <QtDebug>

#include "label.h"
#include "drawingcanvas.h"

Label::Label(LabelType type, double value, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
        :QGraphicsTextItem(parent, scene),
        myType(type),
        myFontSize(DEFAULT_LABEL_FONT_SIZE),
        myDX(0.0),
        myDY(0.0),
        myValue(value),
        _info(info)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setZValue(1000.0);
    if(myType != TextLabelType){
        updateLabel();
    }
    setFont(QFont(DEFAULT_LABEL_FONT));
    updateFontSize();
    // If the zoom factor changes or the window size changes, we need to adjust
    // the size of the font for the labels accordingly
    connect(_info, SIGNAL(scaleFactorChanged()), this, SLOT(updateFontSize()));
    setToolTip(tr("Double click to edit"));
}

void Label::keyPressEvent(QKeyEvent *event)
{
    std::cout<<"key press event "<<event->key()<<"  "<<Qt::Key_Tab<<std::endl;
    if(event->key()==Qt::Key_Tab){
        QTextCursor t = textCursor();
        t.insertText("\t");
        setTextCursor(t);
        std::cout<<"inserting tab"<<std::endl;
        setTextInteractionFlags(Qt::TextEditorInteraction);
    } else {
        QGraphicsTextItem::keyPressEvent(event);
    }
}

void Label::focusOutEvent(QFocusEvent *event)
{
    if(event->reason()==Qt::TabFocusReason) {
        QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
        QGraphicsTextItem::keyPressEvent(newEvent);
        event->accept();
    } else {
		QGraphicsTextItem::focusOutEvent(event);
		if(this->toPlainText().length() == 0)
			this->scene()->removeItem(this);
    }
}

void Label::updateFontSize()
{
    QFont myFont(font());
    // The denominator is completely arbitrary and is chosen to make the font size
    // appear on a reasonable scale
    myFont.setPointSizeF(double(myFontSize) * _info->scaleFactor() / 80.0);
    setFont(myFont);
}

void Label::updateLabel()
{
    if(myType == BondLabelType) {
        myString.setNum(myValue, 'f', _info->getBondPrecision());
    } else if(myType == AngleLabelType) {
        myString.setNum(myValue, 'f', _info->getAnglePrecision());
    }
    setPlainText(myString);
}

void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // A null event to prevent unwanted deselection
    Q_UNUSED(event);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // A null event to prevent unwanted deselection
    Q_UNUSED(event);
}

void Label::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Label");
	writer->writeAttribute("type", QString("%1").arg(myType));
	writer->writeAttribute("string", myString);
	writer->writeAttribute("fontSize", QString("%1").arg(myFontSize));
	writer->writeAttribute("dx", QString("%1").arg(myDX));
	writer->writeAttribute("dy", QString("%1").arg(myDY));
	writer->writeAttribute("value", QString("%1").arg(myValue));
	writer->writeEndElement();
}

Label* Label::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QGraphicsScene* scene)
{
	Q_ASSERT(reader->name() == "Label");
	QXmlStreamAttributes attr = reader->attributes();
	LabelType type;
	switch(attr.value("type").toString().toInt())
	{
		case (UserType + BONDLABELTYPE) : type = BondLabelType; break;
		case (UserType + ANGLELABELTYPE) : type = AngleLabelType; break;
		case (UserType + TEXTLABELTYPE) : type = TextLabelType; break;
			
	}
	Label* l = new Label(type, attr.value("value").toString().toDouble(), drawingInfo, NULL, scene);
	l->myString = attr.value("string").toString();
	l->myFontSize = attr.value("fontSize").toString().toInt();
	l->myDX = attr.value("dx").toString().toInt();
	l->myDY = attr.value("dy").toString().toInt();
	l->myValue = attr.value("value").toString().toDouble();
	return l;
}