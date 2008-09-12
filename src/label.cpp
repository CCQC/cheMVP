#include <QtGui>

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
     setFlag(QGraphicsItem::ItemIsMovable, true);
     setFlag(QGraphicsItem::ItemIsSelectable, true);
     setFlag(QGraphicsItem::ItemIsFocusable, true);
     setTextInteractionFlags(Qt::NoTextInteraction);
     setZValue(1000.0);
     updateLabel();
     setFont(QFont(DEFAULT_LABEL_FONT));
     updateFontSize();
     // If the zoom factor changes or the window size changes, we need to adjust
     // the size of the font for the labels accordingly
     connect(myDrawingInfo, SIGNAL(scaleFactorChanged()), this, SLOT(updateFontSize()));
 }

  
 void Label::updateFontSize()
 {
	 QFont myFont(font());
 	 myFont.setPointSizeF(double(myFontSize)*myDrawingInfo->scaleFactor()/150.0);
 	 setFont(myFont);
 }
 
 
// void Label::keyPressEvent(QKeyEvent *event)
// {
//	 if(event->key() == Qt::Key_Tab){
//		 std::cout<< "received a tab" << std::endl;
//		 textCursor().insertText("\t");
//	 }else{
//		 QGraphicsTextItem::keyPressEvent(event);
//	 }
// }
 
 
 void Label::updateLabel()
 {
	 myString.setNum(myValue, 'f', myPrecision);
	 setPlainText(myString);
 }

 
 void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
 {
	 if (textInteractionFlags() == Qt::NoTextInteraction){
	   setTextInteractionFlags(Qt::TextEditorInteraction);
//	   grabKeyboard();
	 }
//	 QGraphicsTextItem::mouseDoubleClickEvent(event);
 }
