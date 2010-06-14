#include "drawingcanvas.h"

// TODO add double click events to make text selection easier
void DrawingCanvas::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(items().size() == 0){
		unselectAll();
		return;
	}
	mouseOrigin = mouseEvent->scenePos();
	numMouseMoves = 0;
	switch (myMode) {
	case AddArrow:
		Arrow *arrow;
		arrow = new Arrow(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), drawingInfo);
		myArrow = arrow;
		addItem(arrow);
		addItem(arrow->startBox());
		addItem(arrow->endBox());
		arrowsList.push_back(arrow);
		break;
	case AddBond:
		bondline = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
												mouseEvent->scenePos()));
		bondline->setPen(QPen(Qt::black, 5, Qt::DashLine));
		addItem(bondline);
		break;
	case Select:
		// Is there an item under the cursor?
		if(items(mouseEvent->scenePos()).size()){
			QApplication::setOverrideCursor(myMoveCursor);
			if(items(mouseEvent->scenePos())[0]->type() == Atom::Type){
				setMode(TempMoveAll);
			}else{
				setMode(TempMove);
				myTempMoveItem = items(mouseEvent->scenePos())[0];
				// This is essential for getting the double click events
				QGraphicsScene::mousePressEvent(mouseEvent);
			}
		}else{
			unselectAll();
			QApplication::setOverrideCursor(Qt::CrossCursor);
			//		selectionRectangle = new QGraphicsRectItem(QRectF(mouseEvent->scenePos(),		// HPS
			//														  mouseEvent->scenePos()));
			selectionRectangle = new QGraphicsRectItem(QRectF(mouseEvent->scenePos().x(),
															  mouseEvent->scenePos().y(),
															  0, 0));
			selectionRectangle->setPen(QPen(Qt::black, 1,Qt::DashLine));
			addItem(selectionRectangle);
		}
		emit updateTextToolbars();
		break;
	default:
		;
	}
}

void DrawingCanvas::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	switch (myMode) {
	case AddBond:
		if(bondline != 0){
			QLineF newLine(bondline->line().p1(), mouseEvent->scenePos());
			bondline->setLine(newLine);
		}
		// This is so that hover events are emitted correctly
		QGraphicsScene::mouseMoveEvent(mouseEvent);
		break;
	case AddArrow:
		if(myArrow != 0){
			myArrow->updatePosition(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
		}
		break;
	case TempMove:
		if(myTempMoveItem != 0){
			QGraphicsItem *item;
			item = myTempMoveItem;
			if(ITEM_IS_LABEL){
				Label *label = dynamic_cast<Label*>(myTempMoveItem);
				label->setDX(label->dX() + mouseEvent->scenePos().x() - mouseOrigin.x());
				label->setDY(label->dY() + mouseEvent->scenePos().y() - mouseOrigin.y());
				if(numMouseMoves==1){
					numMouseMoves = 0;
					mouseOrigin = mouseEvent->scenePos();
				}
				++numMouseMoves;
				refresh();
			}else if(item->type() == DragBox::Type){
				DragBox *dragBox = dynamic_cast<DragBox*>(item);
				dragBox->setDX(dragBox->dX() + mouseEvent->scenePos().x() - mouseOrigin.x());
				dragBox->setDY(dragBox->dY() + mouseEvent->scenePos().y() - mouseOrigin.y());
				if(numMouseMoves==1){
					numMouseMoves = 0;
					mouseOrigin = mouseEvent->scenePos();
				}
				++numMouseMoves;
				updateArrows();
				update();
			}else if(item->type() == Arrow::Type){
				Arrow *arrow = dynamic_cast<Arrow*>(item);
				arrow->incDX(mouseEvent->scenePos().x() - mouseOrigin.x());
				arrow->incDY(mouseEvent->scenePos().y() - mouseOrigin.y());
				if(numMouseMoves==1){
					numMouseMoves = 0;
					mouseOrigin = mouseEvent->scenePos();
				}
				++numMouseMoves;
				updateArrows();
				update();
			}else if((myTempMoveItem->flags() & QGraphicsItem::ItemIsMovable)){
				myTempMoveItem->setPos(mouseEvent->scenePos());
				updateArrows();
				update();
			}
		}
		break;
	case TempMoveAll:
		QGraphicsScene::mouseMoveEvent(mouseEvent);
		drawingInfo->incDX(mouseEvent->scenePos().x() - mouseOrigin.x());
		drawingInfo->incDY(mouseEvent->scenePos().y() - mouseOrigin.y());
		if(numMouseMoves==1){
			numMouseMoves = 0;
			mouseOrigin = mouseEvent->scenePos();
		}
		++numMouseMoves;
		refresh();
		break;
	case Select:
		if(selectionRectangle != 0){
			//QRectF newRect(QRectF(mouseOrigin, mouseEvent->scenePos())); // HPS
			QRectF newRect(QRectF(mouseOrigin.x(), mouseOrigin.y(),
								  mouseEvent->scenePos().x()-mouseOrigin.x(),
								  mouseEvent->scenePos().y()-mouseOrigin.y()));
			selectionRectangle->setRect(newRect);
		}else{
			// This is so that hover events are emitted correctly
			QGraphicsScene::mouseMoveEvent(mouseEvent);
		}
		break;
	case Rotate:
		int dx, dy, dz;
		dx = (int)(mouseEvent->scenePos().x() - mouseOrigin.x());
		dy = (int)(mouseEvent->scenePos().y() - mouseOrigin.y());
		dz = dy;
		if(mouseEvent->buttons() & Qt::LeftButton) {
			if(mouseEvent->modifiers() & Qt::ShiftModifier) {
				if(mouseEvent->scenePos().x() < drawingInfo->midX())
					drawingInfo->setZRot(-dz);
				else
					drawingInfo->setZRot(dz);
			} else {
				drawingInfo->setXRot(-dy);
				drawingInfo->setYRot(dx);
			}
			++numMouseMoves;
			if(numMouseMoves==1) {
				numMouseMoves = 0;
				mouseOrigin = mouseEvent->scenePos();
			}
		}
		refresh();
		break;
	default:
		;
	}
}

void DrawingCanvas::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(items().size() == 0){
		unselectAll();
		return;
	}
	switch (myMode) {
	case AddBond:
		if (bondline != 0 && myMode == AddBond) {
			QList<QGraphicsItem *>::iterator iter;
			QList<QGraphicsItem *> startItems = items(bondline->line().p1());
			QList<QGraphicsItem *> endItems = items(bondline->line().p2());
			Atom *atom1 	= 0;
			Atom *atom2 	= 0;
			int nstartItems = startItems.size();
			int nendItems 	= endItems.size();
			for(int item = 0; item <  nstartItems; ++item){
				if(startItems[item]->type()==Atom::Type){
					atom1 = qgraphicsitem_cast<Atom*>(startItems[item]);
					break;
				}
			}
			for(int item = 0; item <  nendItems; ++item){
				if(endItems[item]->type()==Atom::Type){
					atom2 = qgraphicsitem_cast<Atom*>(endItems[item]);
					break;
				}
			}

			if(atom1!=0 && atom2!=0 && atom1!=atom2){
				Bond *bond = new Bond(atom1, atom2, drawingInfo);
				bondsList.push_back(bond);
				addItem(bond);
			}
			removeItem(bondline);
			delete bondline;

		}
		bondline = 0;
		emit mouseModeChanged(int(Select));
		break;
	case Select:
		if(selectionRectangle != 0){
			foreach(QGraphicsItem* item, selectionRectangle->collidingItems()){
				if(item->flags() & QGraphicsItem::ItemIsSelectable){
					item->setSelected(true);
				}
			}
			removeItem(selectionRectangle);
			delete selectionRectangle;
		}
		selectionRectangle = 0;
		QApplication::restoreOverrideCursor();
		break;
	case TempMove:
		QApplication::restoreOverrideCursor();
		// If the mouse didn't move, we just want to to toggle selections
		if(!numMouseMoves){
			myTempMoveItem->setSelected((myTempMoveItem->isSelected() ? false : true));
		}else{
		}
		myTempMoveItem = 0;
		emit mouseModeChanged(int(Select));
		break;
	case TempMoveAll:
		QApplication::restoreOverrideCursor();
		// If the mouse didn't move, we just want to to toggle selections
		if(!numMouseMoves){
			QGraphicsItem *item;
			if(!items(mouseEvent->scenePos()).size()) return;
			item = items(mouseEvent->scenePos())[0];
			item->setSelected((item->isSelected() ? false : true));
		}
		setMode(Select);
		break;
	case AddText:
		Label *label;
		label = new Label(Label::TextLabelType, 0.0, drawingInfo);
		addItem(label);
		textLabelsList.push_back(label);
		label->setDX(mouseEvent->scenePos().x()-drawingInfo->dX());
		label->setDY(mouseEvent->scenePos().y()-drawingInfo->dY());
		label->setPos(mouseEvent->scenePos());
		label->setTextInteractionFlags(Qt::TextEditorInteraction);
		label->setFocus();
		emit updateTextToolbars();
		emit mouseModeChanged(int(Select));
		break;
	case AddArrow:
		myArrow = 0;
		emit mouseModeChanged(int(Select));
		break;
	default:
		;
	}
}
