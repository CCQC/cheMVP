#ifndef DRAWINGDISPLAY_H_
#define DRAWINGDISPLAY_H_

#include <QGraphicsView>
#include "drawingcanvas.h"
#include "drawinginfo.h"

class DrawingDisplay : public QGraphicsView
{
public:
    DrawingDisplay(DrawingCanvas *scene, DrawingInfo *info);
    void resizeEvent (QResizeEvent *event);
    void focusOutEvent (QFocusEvent *event);
private:
    DrawingInfo *drawingInfo;
    DrawingCanvas *canvas;
};

#endif /*DRAWINGDISPLAY_H_*/
