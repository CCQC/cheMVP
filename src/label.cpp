#include <QtGui>
#include <QtDebug>

 #include "label.h"
 #include "drawingcanvas.h"

 Label::Label(LabelType type, double value, int precision, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
     : QGraphicsTextItem(parent, scene),
     myType(type),
     myValue(value),
     myPrecision(precision),
     myDrawingInfo(info),
     myColor(Qt::black),
     myFontSize(DEFAULT_LABEL_FONT_SIZE),
     myDY(0.0),
     myDX(0.0)
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
	 connect(myDrawingInfo, SIGNAL(scaleFactorChanged()), this, SLOT(updateFontSize()));
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
	 }else{
		 QGraphicsTextItem::keyPressEvent(event);
	 }
 }
 
 
 void Label::focusOutEvent(QFocusEvent *event)
 {
		std::cout<<"Label focus out"<<std::endl;
	 if(event->reason()==Qt::TabFocusReason){
  		 QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
  		 QGraphicsTextItem::keyPressEvent(newEvent);
  		 event->accept();
	 }else{
		 QGraphicsTextItem::focusOutEvent(event);
	 }
 }

 
 void Label::updateFontSize()
 {
	 QFont myFont(font());
	 // The denominator is completely arbitrary and is chosen to make the font size
	 // appear on a reasonable scale
 	 myFont.setPointSizeF(double(myFontSize)*myDrawingInfo->scaleFactor()/80.0);
 	 setFont(myFont);
 }
 
 
 void Label::updateLabel()
 {
	 myString.setNum(myValue, 'f', myPrecision);
	 setPlainText(myString);
 }

 
 void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
 {
	 if (textInteractionFlags() == Qt::NoTextInteraction){
	   setTextInteractionFlags(Qt::TextEditorInteraction);
//	   setFlag(QGraphicsItem::ItemIsFocusable,false);
	 }
	 QGraphicsTextItem::mouseDoubleClickEvent(event);
 }

 
 void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }


 void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }