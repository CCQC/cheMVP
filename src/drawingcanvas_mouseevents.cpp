#include "drawingcanvas.h"



void DrawingCanvas::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
 	if(items().size() == 0){
 		unselectAll();
 		return;
 	}
	mouseOrigin = mouseEvent->scenePos();
	numMouseMoves = 0;
    switch (myMode) {
//        case InsertAtom:
//            Atom *atom;
//            atom = new Atom(elementToAdd, drawingInfo);
//            atom->setPos(mouseEvent->scenePos());
//            addItem(atom);
//            myMode = MoveSelection;
//            break;
        case AddText:
        	Label *label;
        	label = new Label(Label::TextLabelType, 0, 0, drawingInfo);
        	addItem(label);
        	label->setPos(mouseEvent->scenePos());
        	label->setTextInteractionFlags(Qt::TextEditorInteraction);
        	break;
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
//	                QGraphicsScene::mousePressEvent(mouseEvent);
    			}
    		}else{
    			unselectAll();
	        	selectionRectangle = new QGraphicsRectItem(QRectF(mouseEvent->scenePos(),
	        	                                         mouseEvent->scenePos()));
	        	selectionRectangle->setPen(QPen(Qt::black, 1,Qt::DashLine));
	        	addItem(selectionRectangle);
	        }
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
				QRectF newRect(QRectF(mouseOrigin, mouseEvent->scenePos()));
				selectionRectangle->setRect(newRect);
			}else{
				// This is so that hover events are emitted correctly
				QGraphicsScene::mouseMoveEvent(mouseEvent);
			}
			break;
        case Rotate:
        	int dx;
        	int dy;
        	dx = mouseEvent->scenePos().x() - mouseOrigin.x();
        	dy = mouseEvent->scenePos().y() - mouseOrigin.y();
            if (mouseEvent->buttons() & Qt::LeftButton) {
                setXRotation(int(drawingInfo->xRot() +  dy));
                setYRotation(int(drawingInfo->yRot() +  dx));
            } else if (mouseEvent->buttons() & Qt::RightButton) {
                setXRotation(int(drawingInfo->xRot() +  dy));
                setZRotation(int(drawingInfo->zRot() +  dx));
            }
            ++numMouseMoves;
            if(numMouseMoves==1){
            	numMouseMoves = 0;
                mouseOrigin = mouseEvent->scenePos();            	
            }
            refresh();
        	break;
		default:
			;
    }
}


void DrawingCanvas::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
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
			break;
		case TempMove:
			QApplication::restoreOverrideCursor();
			// If the mouse didn't move, we just want to to toggle selections
			if(!numMouseMoves){
				myTempMoveItem->setSelected((myTempMoveItem->isSelected() ? false : true));
//		        foreach(QGraphicsItem *item, items(mouseEvent->scenePos())){
//	        		item->setSelected((item->isSelected() ? false : true));
//		        }
			}else{
//	        	int dX;
//	        	int dY;
//	        	dX = mouseEvent->scenePos().x() - mouseOrigin.x();
//	        	dY = mouseEvent->scenePos().y() - mouseOrigin.y();
//	        	
//	        	if(!items(mouseEvent->scenePos()).size()) return;
//        		if(items(mouseEvent->scenePos())[0]->type()==Label::AngleLabelType ||
//        		   items(mouseEvent->scenePos())[0]->type()==Label::BondLabelType){
//        			Label *label = dynamic_cast<Label*>(items(mouseEvent->scenePos())[0]);
//        			label->setDX(label->dX() + dX);
//        			label->setDY(label->dY() + dY);
//        		}
			}
//    		QGraphicsScene::mouseReleaseEvent(mouseEvent);
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
    	case AddArrow:
    		myArrow = 0;
			emit mouseModeChanged(int(Select));
    		break;
		default:
			;
	}
//    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

