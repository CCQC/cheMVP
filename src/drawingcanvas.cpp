#include <QtGui>

#include "drawingcanvas.h"
#include "fileparser.h"
#include "molecule.h"
#include "bond.h"
#include "atom.h"
#include "defines.h"

DrawingCanvas::DrawingCanvas(QMenu *itemMenu, DrawingInfo *info, FileParser *in_parser, QObject *parent):
    QGraphicsScene(parent),
//    elementToAdd("C"),
    drawingInfo(info),
    myAtomNumberSubscripts(false),
    myBackgroundColor(Qt::white),
    myBackgroundAlpha(0),
    myMoveCursor(QPixmap(":/images/cursor_move.png")),
    myRotateCursor(QPixmap(":/images/cursor_rotate.png")),
    mySelectCursor(QPixmap(":/images/cursor_select.png")),
    parser(in_parser)
{
    myItemMenu 			= itemMenu;
    myMode 				= Select;
    bondline 			= 0;    
    selectionRectangle 	= 0;
    myArrow             = 0;
    myTempMoveItem      = 0;
    myBackgroundColor.setAlpha(myBackgroundAlpha);
    setBackgroundBrush(QBrush(myBackgroundColor));
    setSceneRect(QRectF(0, 0, DEFAULT_SCENE_SIZE_X,  DEFAULT_SCENE_SIZE_Y));

    //If the user provided a filename from the command line, there's a molecule in the parser
    if(parser->numMolecules()){
    	loadFromParser();
    }
}


void DrawingCanvas::drawBackground(QPainter *painter, const QRectF &rect)
{
	if(myBackgroundColor.alpha() == 0) return;
	painter->setBrush(myBackgroundColor);
	painter->drawRect(sceneRect());
}


void DrawingCanvas::clearAll()
{
	foreach(QGraphicsItem *item, items()){
		removeItem(item);
		delete item;
	}
	atomsList.clear();
	bondsList.clear();
	anglesList.clear();
	arrowsList.clear();
	textLabelsList.clear();
}


void DrawingCanvas::unselectAll()
{
    foreach(QGraphicsItem *item, items()) {
        item->setSelected(false);
        if(ITEM_IS_LABEL){
        	Label *label = dynamic_cast<Label*>(item);
            QTextCursor cursor = label->textCursor();
            cursor.clearSelection();
            label->setTextCursor(cursor);
        	label->setTextInteractionFlags(Qt::NoTextInteraction);
        	label->clearFocus();
        }
    }    
    update();
}


void DrawingCanvas::selectAll()
{
    foreach(QGraphicsItem *item, items()) {
        item->setSelected(true);
    }    
    update();
}


void DrawingCanvas::setAtomLabels(QString text)
{
	foreach(Atom *atom, atomsList)
	{
		if(atom->isSelected()){
			atom->setLabel(text);
		}
	}
	update();
}


void DrawingCanvas::setAtomDrawingStyle(int style)
{
	foreach(Atom *atom, atomsList){
		atom->setDrawingStyle(Atom::DrawingStyle(style));
	}
	update();
}


void DrawingCanvas::setAtomFontSizeStyle(int style)
{
	foreach(Atom *atom, atomsList){
		atom->setFontSizeStyle(Atom::FontSizeStyle(style));
	}
	update();
}


double DrawingCanvas::bondLength(Atom* atom1, Atom* atom2)
{
	  return(sqrt(	pow(atom1->x()-atom2->x(),2.0)+
			  		pow(atom1->y()-atom2->y(),2.0)+
			  		pow(atom1->z()-atom2->z(),2.0)));
}


void DrawingCanvas::setAcceptsHovers(bool arg)
{
	foreach(Atom *atom, atomsList){
		atom->setAcceptsHovers(arg);
	}
	foreach(Bond *bond, bondsList){
		bond->setAcceptsHovers(arg);
	}
	// TODO angles, labels, arrows...
}


void DrawingCanvas::loadFromParser()
{
	Molecule *molecule = parser->molecule();
	std::vector<AtomEntry*> atoms = molecule->atomsList();
	int nAtoms = atoms.size();
	// First add the Atoms
	for(int i = 0; i < nAtoms; ++i){
		Atom* atom = new Atom(atoms[i]->Label, drawingInfo);
		atom->setX(atoms[i]->x);
		atom->setY(atoms[i]->y);
		atom->setZ(atoms[i]->z);
		addItem(atom);
		atomsList.push_back(atom);
	}

	// Once we've found the center of mass, we know the molecule extents
	translateToCenterOfMass();
	
	// Having determined the size and scale factor, update the radii and label sizes 
	foreach(Atom *atom, atomsList){
		atom->computeRadius();
		atom->setLabelFontSize(DEFAULT_ATOM_LABEL_FONT_SIZE);
		atom->setLabelFont(DEFAULT_ATOM_LABEL_FONT);
	}

	// Now add the Bonds
	double cutoffScale = 1.2;
	for(int atom1 = 0; atom1 < nAtoms; ++atom1){
	  for(int atom2 = 0; atom2 < atom1; ++atom2){
		if(bondLength(atomsList[atom1],atomsList[atom2])<
	      (atomsList[atom1]->radius()+atomsList[atom2]->radius())*cutoffScale){
	       Bond* bond = new Bond(atomsList[atom1], atomsList[atom2], drawingInfo);
	       addItem(bond);
	       bondsList.push_back(bond);
		}
	  }
	}	
	performRotation();
	refresh();
}


void DrawingCanvas::updateBonds()
{
	foreach(Bond *bond, bondsList){
		bond->updatePosition();
	}
}


void DrawingCanvas::updateTextLabels()
{
	foreach(Label *label, textLabelsList){
		label->setPos(drawingInfo->dX()+label->dX(), drawingInfo->dY()+label->dY());
	}
}


void DrawingCanvas::updateAngles()
{
	foreach(Angle *angle, anglesList){
		angle->updatePosition();
	}
}


void DrawingCanvas::updateArrows()
{
	foreach(Arrow *arrow, arrowsList){
		arrow->updatePosition();
	}
}


void DrawingCanvas::setMode(Mode mode)
{
    myMode = mode;
}


void DrawingCanvas::atomLabelFontChanged(const QFont &font)
{
	foreach(Atom *atom, atomsList){
		if(atom->isSelected()){
			atom->setLabelFont(font.family());
		}
	}
	update();
}


void DrawingCanvas::toggleAtomNumberSubscripts()
{
	// TODO Improve this algorithm - check case-by-case for existing subscripts
	if(myAtomNumberSubscripts){
		myAtomNumberSubscripts = false;
		foreach(Atom *atom, atomsList){
			if(!atom->isSelected()) continue;
			if(atom->label().indexOf('_') == -1){
				atom->setLabelSubscript(QString());
				atom->setLabel(QString());
			}else{
				atom->setLabelSubscript(QString());
			}
		}
	}else{
		myAtomNumberSubscripts = true;	
		for(int atom = 0; atom < atomsList.size(); ++atom){
			if(!atomsList[atom]->isSelected()) continue;
			if(!atomsList[atom]->label().size()){
				// If there's no label, assign the number to the label
				atomsList[atom]->setLabel(QString().setNum(atom+1));
				atomsList[atom]->setLabelSubscript(QString());
			}else{
				atomsList[atom]->setLabelSubscript(QString().setNum(atom+1));
			}
		}
	}
	update();
}


void DrawingCanvas::atomLabelFontSizeChanged(const QString &size)
{
	foreach(Atom *atom, atomsList){
		if(atom->isSelected()){
			atom->setLabelFontSize(size.toInt());
		}
	}
	update();
}


void DrawingCanvas::translateToCenterOfMass()
{
	double xCOM = 0.0;
	double yCOM = 0.0;
	double zCOM = 0.0;
	double totalMass = 0.0;
	
	// Start by computing the center of mass
	foreach(Atom *atom, atomsList){
		xCOM += atom->x() * atom->mass();
		yCOM += atom->y() * atom->mass();
		zCOM += atom->z() * atom->mass();
		totalMass += atom->mass();
	}
	xCOM /= totalMass;
	yCOM /= totalMass;
	zCOM /= totalMass;
	double rMax = 0.0;
	foreach(Atom *atom, atomsList){
		double tempX = atom->x();
		atom->setX(tempX - xCOM);
		double tempY = atom->y();
		atom->setY(tempY - yCOM);
		double tempZ = atom->z();
		atom->setZ(tempZ - zCOM);
		double r = sqrt(atom->x()*atom->x() + atom->y()*atom->y() + atom->z()*atom->z());
		rMax = (r>rMax ? r : rMax);
	}
	drawingInfo->setMoleculeMaxDimension(rMax + EXTRA_DRAWING_SPACE);
	drawingInfo->determineScaleFactor();
}


bool DrawingCanvas::isBonded(Atom *atom1, Atom *atom2){
	foreach(Bond *bond, bondsList){
		if((bond->startAtom() == atom1 && bond->endAtom() == atom2) ||
		   (bond->startAtom() == atom2 && bond->endAtom() == atom1)){
			return true;
		}
	}
	return false;
}


std::vector<Angle*>::iterator DrawingCanvas::angleExists(Atom *atom1, Atom *atom2, Atom *atom3){
	std::vector<Angle*>::iterator pos;
	for(pos = anglesList.begin(); pos != anglesList.end(); ++pos){
		Angle *angle = *pos;
		if((angle->startAtom() == atom1 && angle->centerAtom() == atom2 && angle->endAtom() == atom3) ||
		   (angle->startAtom() == atom3 && angle->centerAtom() == atom2 && angle->endAtom() == atom1)){
			return pos;
		}
	}
	pos = anglesList.end();
	// Checking the bounds on the iterator later will reveal whether the angle exists in the list,
	// as long as we return an iterator beyond the end of the list
	return ++pos;
}


void DrawingCanvas::toggleAngleLabels()
{
	// This is quite cumbersome, which stems from my reluctance to use numbers to label the atoms
	// so I can begin removing and inserting atoms more easily should I chose to in the future...
	for(int a1 = 0; a1 < atomsList.size(); ++a1){
		Atom *atom1 = atomsList[a1];
		if(!atom1->isSelected()) continue;
		for(int a2 = 0; a2 < atomsList.size(); ++a2){
			Atom *atom2 = atomsList[a2];
			if(!atom2->isSelected()) continue;
			for(int a3 = 0; a3 != a1; ++a3){
				Atom *atom3 = atomsList[a3];
				if(!atom3->isSelected()) continue;
				if(a1 == a2 || a2 == a3) continue;
				if(isBonded(atom1, atom2) && isBonded(atom2, atom3)){
					std::vector<Angle*>::iterator anglePos = angleExists(atom1, atom2, atom3); 
					if(anglePos <= anglesList.end()){
						// Remove angle
						Angle *angle = *anglePos;
						removeItem(angle->label());
						removeItem(angle->marker1());
						removeItem(angle->marker2());
						anglesList.erase(anglePos);
						delete angle;
					}else{
						//add angle
						Angle *angle = new Angle(atom1, atom2, atom3, drawingInfo);
						addItem(angle->label());
						addItem(angle->marker1());
						addItem(angle->marker2());
						anglesList.push_back(angle);
					}
				}
			}
		}
	}
}


void DrawingCanvas::toggleBondDashing()
{
	Bond *bond;
	foreach(bond, bondsList)
	{
		if(bond->isSelected()){
			if(bond->label()==0){
				bond->toggleDashing();
			}
		}
	}	
	refresh();
}


void DrawingCanvas::toggleBondLabels()
{
	Bond *bond;
	foreach(bond, bondsList)
	{
		if(bond->isSelected()){
			if(bond->label()==0){
				bond->toggleLabel();
				addItem(bond->label());
			}else{
				removeItem(bond->label());
				bond->toggleLabel();
			}
		}
	}
}


void DrawingCanvas::performRotation()
{
	// Assumes the cartesians are centered at the center of mass
	double zMax = -10000.0;
	int nAtoms = atomsList.size();
	
    double phiX = drawingInfo->xRot() * DEG_TO_RAD;
    double phiY = drawingInfo->yRot() * DEG_TO_RAD;
    double phiZ = drawingInfo->zRot() * DEG_TO_RAD;
	for(int atom = 0; atom < nAtoms; ++atom){
	  Atom *pAtom = atomsList[atom];
	  double x = pAtom->x();
	  double y = -pAtom->y();
	  double z = pAtom->z();
	  // There's some great info about the various coordinate systems at
	  // http://www.stellarsoftware.com/euler3.htm
	  // For now I want to use simple lab-fixed Cartesian axes, but this might change

      double cx = cos(phiX);
	  double sx = sin(-phiX);
	  double cy = cos(phiY);
	  double sy = sin(phiY);
	  double cz = cos(phiZ);
	  double sz = sin(phiZ);
	  
//	  double cr = cz;
//	  double sr = sz;
//	  double ct = cx;
//	  double st = sx;
//	  double cp = cy;
//	  double sp = sy;
//	  double xVal = (     cp*cr*x       +     -sr*cp*y       +    sp*z ) * drawingInfo->scaleFactor();
//	  double yVal = ( st*sp*cr+ct*sr*x  +   ct*cr-sr*st*sp*y + -cp*st*z) * drawingInfo->scaleFactor();
//    double zVal = (-sp*ct*cr+st*sr *x +  sp*sr*ct+st*cr*y  +  ct*cp*z) * drawingInfo->scaleFactor();
   	  // RZ RY RX 
//  	  double xVal = ( cy*cz*x - cx*sz*y + sx*sz*z + cz*sy*(sx*y + cx*z)		) * drawingInfo->scaleFactor();
//  	  double yVal = ( cy*sz*x + cx*cz*y + sx*sy*sz*y - cz*sx*z + cx*sy*sz*z	) * drawingInfo->scaleFactor();
//  	  double zVal = ( -(sy*x) + cy*sx*y + cx*cy*z							) * drawingInfo->scaleFactor();
	  // RX RY RZ 
  	  double xVal = ( cy*cz*x - cy*sz*y + sy*z									) * drawingInfo->scaleFactor();
  	  double yVal = ( cx*sz*x + cz*(sx*sy*x + cx*y) - sx*(sy*sz*y + cy*z)		) * drawingInfo->scaleFactor();
  	  double zVal = ( -(cx*cz*sy*x) + sx*sz*x + cz*sx*y + cx*sy*sz*y + cx*cy*z	) * drawingInfo->scaleFactor();
	  pAtom->setPos(xVal + drawingInfo->dX(), yVal + drawingInfo->dY());
	  pAtom->setZValue(zVal);
	  // TODO this will need to be normalized to make drawings that come from files and those that
	  // are drawn by the user.  i.e. scene coordinates have a different scale to cartesians
	  if(pAtom->z() > zMax){
		  zMax = pAtom->zValue();
	  }
	}

// Normalize the Z values so that the perspective stuff looks 
//	for(int atom = 0; atom < nAtoms; ++atom){
//	  Atom *pAtom = atomsList[atom];
//	  double oldZ = pAtom->zValue();
//	  pAtom->setZValue(oldZ - zMax);
//	}

}

void DrawingCanvas::setXRotation(int phi)
{
	getAngleInBounds(phi);
	drawingInfo->setXRot(phi);
	// Emitting this signal causes the main window's setXRotation to be triggered
	// which will call the performRotation() and update() routines automatically
	emit xRotChanged(phi);
}


void DrawingCanvas::setYRotation(int phi)
{
	getAngleInBounds(phi);
	drawingInfo->setYRot(phi);
	// Emitting this signal causes the main window's setYRotation to be triggered
	// which will call the performRotation() and update() routines automatically
	emit yRotChanged(phi);
}


void DrawingCanvas::setZRotation(int phi)
{
	getAngleInBounds(phi);
	drawingInfo->setZRot(phi);
	// Emitting this signal causes the main window's setZRotation to be triggered
	// which will call the performRotation() and update() routines automatically
	emit zRotChanged(phi);
}


void DrawingCanvas::refresh()
{
	performRotation();
    updateBonds();
    updateAngles();
    updateArrows();
    updateTextLabels();
	update();
}


void DrawingCanvas::getAngleInBounds(int &angle)
{
    while (angle < -180)
         angle += 360;
    while (angle > 180)
         angle -= 360;
}


void DrawingCanvas::setBackgroundColor()
{
	QColor color = QColorDialog::getColor(myBackgroundColor);
	color.setAlpha(myBackgroundAlpha);
	myBackgroundColor = color;
	update();
	
}


void DrawingCanvas::setBackgroundOpacity(int val)
{
	myBackgroundAlpha = (int)(255*val/100);
	myBackgroundColor.setAlpha(myBackgroundAlpha);
	update();
}
