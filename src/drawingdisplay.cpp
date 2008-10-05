#include "drawingdisplay.h"

DrawingDisplay::DrawingDisplay(DrawingCanvas *scene, DrawingInfo *info):
	QGraphicsView(scene),
	drawingInfo(info),
	canvas(scene)
{
	
}

void DrawingDisplay::resizeEvent(QResizeEvent *event)
{
	drawingInfo->setHeight(event->size().height());
	drawingInfo->setWidth(event->size().width());
	drawingInfo->determineScaleFactor();
	canvas->setSceneRect(0, 0, event->size().width(), event->size().height());
	canvas->refresh();
}


void DrawingDisplay::focusOutEvent(QFocusEvent *event)
{
	std::cout<<"Drawing display focus out"<<std::endl;
	if(event->reason() != Qt::TabFocusReason){
		QGraphicsView::focusOutEvent(event);
	}else{
	    QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
	    QGraphicsView::keyPressEvent(newEvent);
	}
}
