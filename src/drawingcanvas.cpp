#include "drawingcanvas.h"
#include <QColorDialog>

#define SIGN(a, b) (b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a))
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

extern "C" {
	extern void DGESVD(char*, char*, int*, int*, double*, int*, double*, double*, int*, double*, int*, double*, int*, int*);
};

DrawingCanvas::DrawingCanvas(DrawingInfo *info, FileParser *in_parser, QObject *parent)
		: QGraphicsScene(parent),
		parser(in_parser),
		drawingInfo(info),
		myBackgroundColor(Qt::white),
		myMoveCursor(QPixmap(":/images/cursor_move.png")),
		myRotateCursor(QPixmap(":/images/cursor_rotate.png")),
		myBackgroundAlpha(DEFAULT_BACKGROUND_OPACITY/100.0*255)
{
	myMode 				= Select;
	bondline 			= 0;
	selectionRectangle 	= 0;
	myArrow				= 0;
	myTempMoveItem		= 0;
	// Hack to make the background border disappear (unless background color is changed)
	//myBackgroundColor.setAlpha(myBackgroundAlpha);
	myBackgroundColor.setAlpha(0);

	setBackgroundBrush(QBrush(myBackgroundColor));
	setSceneRect(QRectF(0, 0, DEFAULT_SCENE_SIZE_X,  DEFAULT_SCENE_SIZE_Y));

	// If the user provided a filename from the command line, there's a molecule in the parser.
	// The filename is empty, however if that molecule came from a project.
	if(parser->numMolecules() && !in_parser->fileName().isEmpty()) {
		loadFromParser();
	}
}

void DrawingCanvas::updateAtomColors(QMap<QString, QVariant> newColors)
{
	foreach(Atom *atom, atomsList){
		atom->setColor(newColors.value(atom->symbol()).value<QColor>());
	}
	update();
}

void DrawingCanvas::focusOutEvent(QFocusEvent *event)
{
	if(event->reason() != Qt::TabFocusReason){
		QGraphicsScene::focusOutEvent(event);
	}else{
		QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QGraphicsScene::keyPressEvent(newEvent);
	}
}

void DrawingCanvas::drawBackground(QPainter *painter, const QRectF &)
{
	if(myBackgroundColor.alpha() == 0) return;
	painter->setBrush(myBackgroundColor);
	painter->drawRect(sceneRect());
}

void DrawingCanvas::clearAll()
{
	foreach(QGraphicsItem* item, items()) {
		removeItem(item);
		delete item;
	}
	atomsList.clear();
	bondsList.clear();
	anglesList.clear();
	arrowsList.clear();
	textLabelsList.clear();
}

void DrawingCanvas::storeLabeledBonds()
{
	for(int i = 0; i < bondsList.size(); i++){
		if(bondsList[i]->hasLabel()){
			persistantBonds.push_back(i);
		}
	}
}

void DrawingCanvas::restoreLabeledBonds()
{
	foreach(int i, persistantBonds){
		addBondLabel(i);
	}
	persistantBonds.clear();
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

void DrawingCanvas::setBondLabelPrecision(int val)
{
	drawingInfo->setBondPrecision(val);
	foreach(Bond *bond, bondsList){
		if(bond->hasLabel())
			bond->label()->updateLabel();
	}
	update();
}

void DrawingCanvas::setAngleLabelPrecision(int val)
{
	drawingInfo->setAnglePrecision(val);
	foreach(Angle *angle, anglesList){
		angle->label()->updateLabel();
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
	drawingInfo->setDrawingStyle(DrawingInfo::DrawingStyle(style));
	foreach(Atom *atom, atomsList){
		atom->setDrawingStyle(DrawingInfo::DrawingStyle(style));
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
	return(sqrt(pow(atom1->x()-atom2->x(),2.0)+
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
	foreach(Arrow *arrow, arrowsList){
		arrow->setAcceptsHovers(arg);
	}
	// TODO angles, labels, arrows...
}

void DrawingCanvas::loadFromParser()
{
	// Do nothing if there are no molecules to display.
	if (parser->numMolecules() == 0) {
		// Tell the user about it.
		QMessageBox::warning(NULL, tr("Warning"), tr("No coordinates were read in. Try again."));
		return;
	}

	Molecule* molecule = parser->molecule();
	std::vector<AtomEntry*> atoms = molecule->atomsList();
	int nAtoms = atoms.size();
	// First add the Atoms
	for(int i = 0; i < nAtoms; ++i){
		Atom* atom = new Atom(atoms[i]->Label, drawingInfo);
		atom->setX(atoms[i]->x);
		atom->setY(atoms[i]->y);
		atom->setZ(atoms[i]->z);
		atom->setID(i+1);
		addItem(atom);
		atomsList.push_back(atom);
	}

	// Once we've found the center of mass, we know the molecule extents
	translateToCenterOfMass();

	// Having determined the size and scale factor, update the radii and label sizes
	drawingInfo->setAtomLabelFont(DEFAULT_ATOM_LABEL_FONT);
	foreach(Atom *atom, atomsList){
		atom->computeRadius();
		atom->setLabelFontSize(DEFAULT_ATOM_LABEL_FONT_SIZE);
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
	refresh();
}

void DrawingCanvas::rotateFromInitialCoordinates()
{
	// This function just updates the coordinates back to the input orientation
	// before rotating by the angles stored in drawinginfo
	Molecule *molecule = parser->molecule();
	std::vector<AtomEntry*> atoms = molecule->atomsList();
	int nAtoms = atoms.size();
	// Restore the input orientation
	for(int i = 0; i < nAtoms; ++i){
		atomsList[i]->setX(atoms[i]->x);
		atomsList[i]->setY(atoms[i]->y);
		atomsList[i]->setZ(atoms[i]->z);
	}
	// Once we've found the center of mass, we know the molecule extents
	translateToCenterOfMass();
	refresh();
}

double ** block_matrix(unsigned long int n, unsigned long int m)
{
	double **A=NULL;
	double *B=NULL;
	unsigned long int i;

	if(!m || !n) return((double **) NULL);

	if ((A = (double **) malloc(n * (unsigned long int)sizeof(double *)))==NULL) {
		fprintf(stderr,"block_matrix: trouble allocating memory for row pointers\n");
		fprintf(stderr,"nrow = %ld\n",n);
		exit(1);
	}

	if ((B = (double *) malloc(m*n * (unsigned long int)sizeof(double)))==NULL) {
		fprintf(stderr,"block_matrix: trouble allocating memory\n");
		fprintf(stderr,"nrow = %ld ncol = %ld\n",n,m);
		exit(1);
	}

	memset(B, '\0', m*n*(unsigned long int)sizeof(double));

	for (i = 0; i < n; i++) {
		A[i] = &(B[i*m]);
	}

	return(A);
}

void DrawingCanvas::determineRotationAngles()
{
	if(atomsList.size() < 1)
		return;

	Molecule *molecule = parser->molecule();
	std::vector<AtomEntry*> oldGeom = molecule->atomsList();
	QList<Atom*> newGeom = atomsList;

	if(oldGeom.size() < 1)
		return;

	double old_x = 0, old_y = 0, old_z = 0, new_x = 0, new_y = 0, new_z = 0;
	for(unsigned int i = 0; i < oldGeom.size(); i++)
	{
		old_x += oldGeom[i]->x;
		old_y += oldGeom[i]->y;
		old_z += oldGeom[i]->z;
	}
	for(int i = 0; i < newGeom.size(); i++)
	{
		new_x += newGeom[i]->x();
		new_y += newGeom[i]->y();
		new_z += newGeom[i]->z();
	}
	old_x /= oldGeom.size();
	old_y /= oldGeom.size();
	old_z /= oldGeom.size();
	new_x /= newGeom.size();
	new_y /= newGeom.size();
	new_z /= newGeom.size();

	std::vector<AtomEntry*> oldTransformed;
	std::vector<AtomEntry*> newTransformed;

	for(unsigned int i = 0; i < oldGeom.size(); i++)
	{
		oldTransformed.push_back(new AtomEntry);
		newTransformed.push_back(new AtomEntry);
		oldTransformed[i]->x = oldGeom[i]->x - old_x;
		oldTransformed[i]->y = oldGeom[i]->y - old_y;
		oldTransformed[i]->z = oldGeom[i]->z - old_z;
		newTransformed[i]->x = newGeom[i]->x() - new_x;
		newTransformed[i]->y = newGeom[i]->y() - new_y;
		newTransformed[i]->z = newGeom[i]->z() - new_z;
	}


	int m = 3;
	int n = 3;
	double* a = new double[m*n];
	int ldvt = n;
	double* vt = new double[ldvt*n];
	// Create correlation matrix with weighted coordinates
	for(int i = 0; i < newGeom.size(); i++)
	{
		a[0] += oldTransformed[i]->x*newTransformed[i]->x;
		a[1] += oldTransformed[i]->y*newTransformed[i]->x;
		a[2] += oldTransformed[i]->z*newTransformed[i]->x;
		a[3] += oldTransformed[i]->x*newTransformed[i]->y;
		a[4] += oldTransformed[i]->y*newTransformed[i]->y;
		a[5] += oldTransformed[i]->z*newTransformed[i]->y;
		a[6] += oldTransformed[i]->x*newTransformed[i]->z;
		a[7] += oldTransformed[i]->y*newTransformed[i]->z;
		a[8] += oldTransformed[i]->z*newTransformed[i]->z;
	}
	for(int i = 0; i < 9; i++)
		a[i] /= oldGeom.size();


	double **rotationMatrix = new double*[3];
	for(int i = 0; i < 3; i++)
		rotationMatrix[i] = new double[3];

	rotationMatrix[0][0]=a[0]*vt[0]+a[1]*vt[3]+a[2]*vt[6];
	rotationMatrix[0][1]=a[0]*vt[1]+a[1]*vt[4]+a[2]*vt[7];
	rotationMatrix[0][2]=a[0]*vt[2]+a[1]*vt[5]+a[2]*vt[8];

	rotationMatrix[1][0]=a[3]*vt[0]+a[4]*vt[3]+a[5]*vt[6];
	rotationMatrix[1][1]=a[3]*vt[1]+a[4]*vt[4]+a[5]*vt[7];
	rotationMatrix[1][2]=a[3]*vt[2]+a[4]*vt[5]+a[5]*vt[8];

	rotationMatrix[2][0]=a[6]*vt[0]+a[7]*vt[3]+a[8]*vt[6];
	rotationMatrix[2][1]=a[6]*vt[1]+a[7]*vt[4]+a[8]*vt[7];
	rotationMatrix[2][2]=a[6]*vt[2]+a[7]*vt[5]+a[8]*vt[8];


	std::printf("\nRotation:\n");
	for(int r = 0; r < 3; r++)
	{
		for(int c = 0; c < 3; c++)
			std::printf("%6.4f ", rotationMatrix[r][c]);
		std::printf("\n");
	}

	printf("Stored Rotations: %6.4f %6.4f %6.4f\n", xRot, yRot, zRot);
	// Determine the corresponding euler angles x->y->z
	double theta, phi, omega;
	if(!(abs(rotationMatrix[2][0])-1E-8 < 1 && abs(rotationMatrix[2][0]+1E-8 > 1)))
	{
		double theta1 = -asin(rotationMatrix[2][0]);
		double theta2 = PI - theta1;
		double omega1 = atan2(rotationMatrix[2][1]/cos(theta1), rotationMatrix[2][2]/(cos(theta1)));
		double omega2 = atan2(rotationMatrix[2][1]/cos(theta2), rotationMatrix[2][2]/(cos(theta2)));
		double phi1 = atan2(rotationMatrix[1][0]/cos(theta1), rotationMatrix[0][0]/cos(theta1));
		double phi2 = atan2(rotationMatrix[1][0]/cos(theta2), rotationMatrix[0][0]/cos(theta2));
		std::printf("Rotate 1: %6.4f %6.4f %6.4f\n", omega1*RAD_TO_DEG, theta1*RAD_TO_DEG, phi1*RAD_TO_DEG);
		std::printf("Rotate 2: %6.4f %6.4f %6.4f\n", omega2*RAD_TO_DEG, theta2*RAD_TO_DEG, phi2*RAD_TO_DEG);
	}
	else
	{
		phi = 0;
		double delta = atan2(rotationMatrix[0][1], rotationMatrix[0][2]);
		if(rotationMatrix[2][0] == -1)
		{
			theta = PI/2;
			omega = phi + delta;
		}
		else
		{
			theta = -PI/2;
			omega = -phi + delta;
		}
		std::printf("Rotate 3: %6.4f %6.4f %6.4f\n", omega*RAD_TO_DEG, theta*RAD_TO_DEG, phi*RAD_TO_DEG);
	}
}

/**
 * Given a matrix A[1..m][1..n], this routine computes its singular value decomposition,
 * A = U dot W dot V_transpose. U replaces A on output. W is output as a vector W[1...n].
 * V (not V_transpose) is output as V[1...n][1...n].
 */
void DrawingCanvas::svdcmp(double **a, int m, int n, double w[], double **v)
{
	bool flag;
	int i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	double* rv1 = new double[n];
	g = scale = anorm = 0.0;

	// Householder reduction to bidiagonal form
	for(i = 0; i < n; i++) {
		l = i+2;
		rv1[i] = scale*g;
		g = s = scale = 0.0;
		if(i < m) {
			for(k = i; k < m; k++)
				scale += fabs(a[k][i]);
			if(scale != 0.0) {
				for(k = i; k < m; k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f = a[i][i];
				g = -SIGN(sqrt(s),f);
				h = f*g-s;
				a[i][i] = f-g;
				for(j = l-1; j < n; j++) {
					for(s = 0.0, k = i; k < m; k++)
						s += a[k][i]*a[k][j];
					f = s/h;
					for(k = i; k < m; k++)
						a[k][j] += f*a[k][i];
				}
				for(k = i; k < m; k++)
					a[k][i] *= scale;
			}
		}
		w[i] = scale*g;
		g = s = scale = 0.0;
		if(i+1 <= m && i+1 != n) {
			for(k = l-1; k < n; k++)
				scale += fabs(a[i][k]);
			if(scale != 0.0) {
				for(k = l-1; k < n; k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f = a[i][l-1];
				g = -SIGN(sqrt(s),f);
				h = f*g - s;
				a[i][l-1] = f-g;
				for(k = l-1; k < n; k++)
					rv1[k] = a[i][k]/h;
				for(j = l-1; j < m; j++) {
					for(s = 0.0, k = l-1; k < n; k++)
						s += a[j][k]*a[i][k];
					for(k = l-1; k < n; k++)
						a[j][k] += s*rv1[k];
				}
				for(k = l-1; k < n; k++)
					a[i][k] *= scale;
			}
		}
		anorm = MAX(anorm, (fabs(w[i])+fabs(rv1[i])));
	}

	// Accumulation of right-hand transformations
	for(i = n-1; i >= 0; i--) {
		if(i < n-1) {
			if(g != 0.0) {
				for(j = l; j < n; j++)
					v[j][i] = (a[i][j]/a[i][l])/g;
				for(j = l; j < n; j++) {
					for(s = 0.0, k =l; k < n; k++)
						s += a[i][k]*v[k][j];
					for(k = l; k < n; k++)
						v[k][j] += s*v[k][i];
				}
			}
			for(j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	// Accumulation of left-hand transformations
	for(i = MIN(m,n)-1; i >= 0; i--) {
		l = i+1;
		g = w[i];
		for(j = l; j < n; j++)
			a[i][j] = 0.0;
		if(g != 0.0) {
			g = 1.0/g;
			for(j = l; j < n; j++) {
				for(s = 0.0, k = l; k < m; k++)
					s += a[k][i]*a[k][j];
				f = (s/a[i][i])*g;
				for(k = i; k < m; k++)
					a[k][j] += f*a[k][i];
			}
			for(j = i; j < m; j++)
				a[j][i] *= g;
		} else {
			for(j = i; j < m; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	// Diagonalization of the bidiagonal form
	for( k = n-1; k >= 0; k--) {
		for(its = 0; its < 30; its++) {
			flag = true;
			for(l = k; l >= 0; l--) {
				nm = l-1;
				if(fabs(rv1[l])+anorm == anorm) {
					flag = false;
					break;
				}
				if(fabs(w[nm])+anorm == anorm)
					break;
			}
			if(flag) {
				c = 0.0;
				s = 1.0;
				for(i = l; i < k+1; i++) {
					f = s*rv1[i];
					rv1[i] = c*rv1[i];
					if(fabs(f) + anorm == anorm)
						break;
					g = w[i];
					h = pythag(f,g);
					w[i] = h;
					h = 1.0/h;
					c = g*h;
					s= -f*h;
					for(j = 0; j < m; j++) {
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y*c+z*s;
						a[j][i] = z*c-y*s;
					}
				}
			}
			z = w[k];
			if(l == k) {
				if(z < 0.0) {
					w[k] = -z;
					for(j = 0; j < n; j++)
						v[j][k] = -v[j][k];
				}
				break;
			}
			if(its == 29){
				std::cout << "No convergence in 30 SVD iterations. Exiting..." << std::endl;
				exit(1);
			}
			x = w[l];
			nm = k-1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g = pythag(f, 1.0);
			f = ((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c = s = 1.0;
			for(j = l; j <= nm; j++) {
				i = j+1;
				g = rv1[i];
				y = w[i];
				h = s*g;
				g = c*g;
				z = pythag(f,h);
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x*c+g*s;
				g = g*c-x*s;
				h = y*s;
				y *= c;
				for(jj = 0; jj < n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x*c+z*s;
					v[jj][i] = z*c-x*s;
				}
				z = pythag(f,h);
				w[j] = z;
				if(z) {
					z = 1.0/z;
					c = f*z;
					s = h*z;
				}
				f = c*g+s*y;
				x = c*y-s*g;
				for(jj = 0; jj < m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y*c+z*s;
					a[jj][i] = z*c-y*s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
}

double DrawingCanvas::pythag(double a, double b)
{
	double absa = fabs(a);
	double absb = fabs(b);
	if(absa > absb)
		return absa*sqrt(1.0 + pow((absb/absa), 2));
	else
		return (absb == 0.0 ? 0.0 : absb*sqrt(1.0 + pow((absa/absb), 2)));
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
	drawingInfo->setAtomLabelFont(font.family());
	update();
}

void DrawingCanvas::toggleAtomNumberSubscripts()
{
	foreach(Atom *atom, atomsList){
		if(!atom->isSelected()) continue;
		if(atom->symbol() == "H"){
			// This is a hydrogen - default behavior is to not use subscripts
			if(atom->label().isEmpty()){
				atom->setLabel(QString::number(atom->ID()));
			}else{
				atom->setLabel(QString());
			}
		}else{
			// This is not a hydrogen - default behavior is to use subscripts
			if(atom->labelHasSubscript()){
				atom->setLabelSubscript(QString());
			}else{
				atom->setLabelSubscript(QString::number(atom->ID()));
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

QList<Angle*>::iterator DrawingCanvas::angleExists(Atom *atom1, Atom *atom2, Atom *atom3){
	QList<Angle*>::iterator pos;
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
					QList<Angle*>::iterator anglePos = angleExists(atom1, atom2, atom3);
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
			bond->toggleDashing();
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

void DrawingCanvas::addBondLabel(int i)
{
	if(bondsList[i]->label()==0) {
		bondsList[i]->toggleLabel();
		addItem(bondsList[i]->label());
	}
}

void DrawingCanvas::performRotation()
{
	// Assumes the cartesians are centered at the center of mass
	double zMin = 0.0;
	double zMax = 0.0;
	//   int nAtoms = atomsList.size(); // unused variable

	double phiX = drawingInfo->xRot() * DEG_TO_RAD;
	double phiY = drawingInfo->yRot() * DEG_TO_RAD;
	double phiZ = drawingInfo->zRot() * DEG_TO_RAD;

	double cx = cos(phiX);
	double sx = sin(phiX);
	double cy = cos(phiY);
	double sy = sin(phiY);
	double cz = cos(phiZ);
	double sz = sin(phiZ);

	foreach(Atom *atom, atomsList){
		double x = atom->x();
		double y = atom->y();
		double z = atom->z();

		// RX RY RZ
		double xVal = ( cy*cz*x - cy*sz*y + sy*z );
		double yVal = ( cx*sz*x + cz*(sx*sy*x + cx*y) - sx*(sy*sz*y + cy*z) );
		double zVal = ( -(cx*cz*sy*x) + sx*sz*x + cz*sx*y + cx*sy*sz*y + cx*cy*z );

		atom->setX(xVal);
		atom->setY(yVal);
		atom->setZ(zVal);
		drawingInfo->setXRot(0);
		drawingInfo->setYRot(0);
		drawingInfo->setZRot(0);

		double perspective = 1.0;
		if(drawingInfo->getUsePerspective()) {
			perspective += zVal * drawingInfo->scaleFactor() * drawingInfo->perspective();
		}
		atom->setPos(perspective*xVal*drawingInfo->scaleFactor() + drawingInfo->dX(),
					 perspective*yVal*drawingInfo->scaleFactor() + drawingInfo->dY());
		atom->setZValue(zVal*drawingInfo->scaleFactor());

		if(atom->z() > zMax){
			zMax = atom->z();
		}
		if(atom->z() < zMin){
			zMin = atom->z();
		}
	}
	drawingInfo->setMaxZ(zMax);
	drawingInfo->setMinZ(zMin);
	// Having kept track of the extents of the Z values of the atoms for things
	// like fogging
	zMin = zMax = 0.0;
	foreach(Bond *bond, bondsList){
		double midZVal = bond->computeMidZ();
		if(midZVal > zMax){
			zMax = midZVal;
		}
		if(midZVal < zMin){
			zMin = midZVal;
		}
	}
	drawingInfo->setMaxBondZ(zMax);
	drawingInfo->setMinBondZ(zMin);

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


void DrawingCanvas::setBackgroundColor()
{
	QColor color = QColorDialog::getColor(myBackgroundColor);
	if(color.isValid())
	{
		color.setAlpha(myBackgroundAlpha);
		myBackgroundColor = color;
		update();
	}
}

void DrawingCanvas::setBackgroundOpacity(int val)
{
	myBackgroundAlpha = (int)(255*val/100);
	myBackgroundColor.setAlpha(myBackgroundAlpha);
	update();
}

void DrawingCanvas::setAtomColors()
{
	bool selected = false;
	foreach(Atom* atom, atomsList) {
		if(atom->isSelected()) {
			selected = true;
			break;
		}
	}

	if(selected) {
		QColor color = QColorDialog::getColor();
		if(color.isValid()) {
			foreach(Atom* atom, atomsList) {
				if(atom->isSelected()) {
					atom->setColor(color);
				}
			}
		}
	}
}

void DrawingCanvas::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Canvas");
	writer->writeAttribute("background", QString("%1 %2 %3 %4").arg(myBackgroundColor.red()).arg(myBackgroundColor.green()).arg(myBackgroundColor.blue()).arg(myBackgroundAlpha));

	// Undo/Redo doesn't delete the items from the lists, so those size() can't be used to generate count
	// This should be updated and fixed before release for efficiency.
	int visibleItems = 0;
	QList<QGraphicsItem *> itemsList = items();
	foreach(Atom* a, atomsList)
		if(itemsList.contains(a))
			visibleItems++;
	foreach(Bond* b, bondsList)
		if(itemsList.contains(b))
			visibleItems++;
	foreach(Label* l, textLabelsList)
		if(itemsList.contains(l))
			visibleItems++;
	foreach(Angle* a, anglesList)
		if(itemsList.contains(a->label())) // Angle itself is never added, only its components
			visibleItems++;
	foreach(Arrow* a, arrowsList)
		if(itemsList.contains(a))
			visibleItems++;
	writer->writeAttribute("items", QString("%1").arg(visibleItems));

	foreach(Atom* a, atomsList)
		a->serialize(writer);
	foreach(Bond* b, bondsList)
		b->serialize(writer);
	foreach(Label* l, textLabelsList)
		l->serialize(writer);
	foreach(Angle* a, anglesList)
		a->serialize(writer);
	foreach(Arrow* a, arrowsList)
		a->serialize(writer);
	writer->writeEndElement();
}

DrawingCanvas* DrawingCanvas::deserialize(QXmlStreamReader* reader, DrawingInfo *drawingInfo, FileParser *parser)
{
	reader->readNextStartElement();
	Q_ASSERT(reader->isStartElement() && reader->name() == "Canvas");

	DrawingCanvas* canvas = new DrawingCanvas(drawingInfo, parser);
	QStringList color = reader->attributes().value("background").toString().split(" ");
	canvas->myBackgroundColor = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt());
	canvas->myBackgroundAlpha = color[3].toInt();
	int items = reader->attributes().value("items").toString().toInt();
	for(int i = 0; i < items; i++)
	{
		reader->readNextStartElement();
		if(reader->name() == "Atom") {
			Atom* a = Atom::deserialize(reader, drawingInfo);
			canvas->addItem(a);
			canvas->atomsList.push_back(a);
		}
		else if(reader->name() == "Bond") {
			Bond* b = Bond::deserialize(reader, drawingInfo, canvas->atomsList);
			canvas->addItem(b);
			if(b->hasLabel())
				canvas->addItem(b->label());
			canvas->bondsList.push_back(b);
		}
		else if(reader->name() == "Label") {
			Label* l = Label::deserialize(reader, drawingInfo, canvas);
			canvas->textLabelsList.push_back(l);
		}
		else if(reader->name() == "Angle") {
			Angle* a = Angle::deserialize(reader, drawingInfo, canvas->atomsList, canvas);
			canvas->addItem(a->marker1());
			canvas->addItem(a->marker2());
			canvas->anglesList.push_back(a);
		}
		else if(reader->name() == "Arrow") {
			Arrow* a = Arrow::deserialize(reader, drawingInfo);
			canvas->addItem(a);
			canvas->addItem(a->startBox());
			canvas->addItem(a->endBox());
			canvas->arrowsList.push_back(a);
		}
		reader->skipCurrentElement();
	}
	reader->skipCurrentElement();
	return canvas;
}
