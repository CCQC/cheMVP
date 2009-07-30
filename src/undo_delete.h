#ifndef UNDO_DELETE_H_
#define UNDO_DELETE_H_

#include <QUndoCommand>
#include "drawingcanvas.h"

class RemoveItemCommand : public QUndoCommand
 {
 public:
     RemoveItemCommand(DrawingCanvas *canvas, QUndoCommand *parent = 0);
     void undo();
     void redo();

 private:
     DrawingCanvas *myCanvas;
     QList<QGraphicsItem*> myList;
 };
#endif /*UNDO_DELETE_H_*/
