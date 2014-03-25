#include "atom.h"

QMap<QString, QVariant> Atom::labelToVdwRadius;
QMap<QString, QVariant> Atom::labelToMass;
QMap<QString, QVariant> Atom::labelToColor;

Atom::Atom(QString element, DrawingInfo *i, QGraphicsItem *parent)
		:QGraphicsEllipseItem(parent),
		myFontSizeStyle(SmallLabel),
		myX(0.0),
		myY(0.0),
		myZ(0.0),
		myScaleFactor(DEFAULT_ATOM_SCALE_FACTOR),
		mySymbol(element),
		myID(0),
		hoverOver(false),
		fill_color(Qt::white),
		_info(i)
{
	setDrawingStyle(_info->getDrawingStyle());

	myLabel = (mySymbol == "H" ? "" : mySymbol);

	//This is a bit of a hack.  All atoms after Xe have Xe's vdW radius
	if(labelToVdwRadius.value(mySymbol).toDouble()==0.0 && mySymbol!="X"){
		myRadius = 1.44;
	}else{
		myRadius = labelToVdwRadius.value(mySymbol).toDouble();
	}
	myMass = labelToMass.value(mySymbol).toDouble();
	 if(myMass == 0.0 && mySymbol != "X"){
		QString errorMessage = "I don't know the mass of the atom " + mySymbol;
		error(errorMessage,__FILE__,__LINE__);
		return;
	}
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setAcceptHoverEvents(true);
	setAcceptDrops(true);
	myEffectiveRadius = _info->scaleFactor() * (1.0 + zValue() * _info->perspective())
						* myRadius * myScaleFactor;
	setRect(QRectF(-myEffectiveRadius, -myEffectiveRadius, 2.0*myEffectiveRadius, 2.0*myEffectiveRadius));
}

void Atom::setLabel(const QString &text)
{
	// Regular expression to match C_x^y
	QRegExp rx("([A-Za-z0-9]*)([_^][A-Za-z0-9]+)?([_^]\\w+)?", Qt::CaseInsensitive, QRegExp::RegExp2);
	myLabel.clear();
	myLabelSubscript.clear();
	myLabelSuperscript.clear();
	if(rx.exactMatch(text) == true) {
		myLabel = rx.cap(1);
		if (rx.cap(2).startsWith('_')) {
			myLabelSubscript += rx.cap(2).remove(0, 1);
		} else if (rx.cap(2).startsWith('^')) {
			myLabelSuperscript += rx.cap(2).remove(0, 1);
		}
		if (rx.cap(3).startsWith('_')) {
			myLabelSubscript += rx.cap(3).remove(0, 1);
		} else if (rx.cap(3).startsWith('^')) {
			myLabelSuperscript += rx.cap(3).remove(0, 1);
		}
	}
}

void Atom::setDrawingStyle(DrawingInfo::DrawingStyle style)
{
	if(style == DrawingInfo::Simple){
		fill_color = Qt::white;
	}else{
		fill_color = labelToColor.value(mySymbol).value<QColor>();
	}
}

void Atom::setFontSizeStyle(FontSizeStyle style)
{
	if(style == LargeLabel){
		setLabelFontSize(DEFAULT_LARGE_ATOM_LABEL_FONT_SIZE);
	}else{
		setLabelFontSize(DEFAULT_ATOM_LABEL_FONT_SIZE);
	}
	myFontSizeStyle = style;
}

void Atom::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = true;
	update();
}

void Atom::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	hoverOver = false;
	update();
}

QRectF Atom::boundingRect() const
{
	return rect();
}

void Atom::computeRadius()
{
	myEffectiveRadius = _info->scaleFactor() * (1.0 + zValue() * _info->perspective())
						* myRadius * myScaleFactor;
}

void Atom::paint(QPainter *painter,
				 const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	computeRadius();

	//The myEffectiveRadius changes on zooming/rotation so we must always update it
	setRect(QRectF(-myEffectiveRadius, -myEffectiveRadius, 2.0*myEffectiveRadius, 2.0*myEffectiveRadius));
	// If the item is selected, use a lighter color for the filling
	QPen linestyle;
	// If we're hovering over the item, use thicker lines
	linestyle.setWidthF((hoverOver ? _info->scaleFactor()*0.04 : _info->scaleFactor()*0.01));
	linestyle.setColor(Qt::black);
	painter->setPen(linestyle);
	// The outline of the atom is a little bit too diffuse, here's another more diffuse circle
	// that accounts for the width of the line so that the gradient and fogging options look pretty
	float lineWidth = linestyle.widthF() / 2.0;
	QRectF fillRect(-myEffectiveRadius-lineWidth, -myEffectiveRadius-lineWidth,
					2.0*myEffectiveRadius+2.0*lineWidth, 2.0*myEffectiveRadius+2.0*lineWidth);

	// The circle defnining the atom
	if(_info->getDrawingStyle() == DrawingInfo::Gradient){
		// Define a gradient pattern to fill the atom
		QRadialGradient gradient(QPointF(0.0, 0.0), myEffectiveRadius,
								 QPointF(myEffectiveRadius/2.1, -myEffectiveRadius/2.1));
		gradient.setColorAt(0.0, Qt::white);
		gradient.setColorAt(1.0, fill_color);
		gradient.setSpread(QGradient::RepeatSpread);
		// PDF looks bad when rendering gradient - here's a workaround
		painter->setPen(Qt::transparent);
		painter->setBrush(gradient);
		painter->drawEllipse(fillRect);
		painter->setBrush(Qt::NoBrush);
		painter->setPen(linestyle);
		painter->drawEllipse(rect());
	}else{
		painter->setBrush(fill_color);
		painter->drawEllipse(rect());
	}

	// Draw the arcs for the "3D" look
	if(myFontSizeStyle != LargeLabel){
		// These boxes define the path of the two arcs
		QRectF h_line_box(-myEffectiveRadius,     -myEffectiveRadius/2.0, 2.0*myEffectiveRadius, myEffectiveRadius);
		QRectF v_line_box(-myEffectiveRadius/2.0, -myEffectiveRadius,     myEffectiveRadius,     2.0*myEffectiveRadius);
		// 2880 is 180 degrees: QT wants angles in 1/16ths of a degree
		painter->drawArc(h_line_box, 0, -2880);
		// The direction of the vertical arc depends on the style
		if(_info->getDrawingStyle() == DrawingInfo::Simple ||
		   _info->getDrawingStyle() == DrawingInfo::SimpleColored ||
		   _info->getDrawingStyle() == DrawingInfo::Gradient) {
			painter->drawArc(v_line_box, 1440, 2880);
		}else if(_info->getDrawingStyle() == DrawingInfo::HoukMol){
			painter->drawArc(v_line_box, 1440, -2880);
		}
	}

	// Draw the blob for HoukMol rip-off
	if(_info->getDrawingStyle() == DrawingInfo::HoukMol){
		QPointF startPoint(-myEffectiveRadius/1.8, -myEffectiveRadius/20.0);
		QPointF endPoint(-myEffectiveRadius/20.0, -myEffectiveRadius/1.8);
		QPointF midPoint1(-myEffectiveRadius/1.2, -myEffectiveRadius/1.2);
		QPointF midPoint2(-myEffectiveRadius/2.1, -myEffectiveRadius/2.1);
		QPainterPath path(startPoint);
		path.quadTo(midPoint1, endPoint);
		path.quadTo(midPoint2, startPoint);
		painter->setPen(QPen());
		painter->setBrush(Qt::white);
		painter->drawPath(path);
	}else if(_info->getDrawingStyle() == DrawingInfo::SimpleColored){
		QPointF startPoint(myEffectiveRadius/1.8, -myEffectiveRadius/20.0);
		QPointF endPoint(myEffectiveRadius/20.0, -myEffectiveRadius/1.8);
		QPointF midPoint1(myEffectiveRadius/1.2, -myEffectiveRadius/1.2);
		QPointF midPoint2(myEffectiveRadius/2.1, -myEffectiveRadius/2.1);
		QPainterPath path(startPoint);
		path.quadTo(midPoint1, endPoint);
		path.quadTo(midPoint2, startPoint);
		painter->setPen(QPen(Qt::transparent));
		painter->setBrush(Qt::white);
		painter->drawPath(path);
	}

	// Now draw the atomic symbol on there
	setLabelFontSize(myFontSize);
	QFontMetricsF labelFM(_info->getAtomLabelFont());
	// TODO check these offsets.  I think there's a bug in the height reported by fontmetrics
	QPointF labelPos;
	if(myFontSizeStyle == LargeLabel){
		labelPos = QPointF(-labelFM.width(myLabel)/2.0, labelFM.height()/3.0);
	}else{
		labelPos = QPointF(-labelFM.width(myLabel)/2.0, labelFM.height()/3.5);
	}
	painter->setPen(_info->getAtomTextColor());
	painter->setBrush(_info->getAtomTextColor());
	painter->setFont(_info->getAtomLabelFont());
	painter->drawText(labelPos, myLabel);
	// If there's a subscript to be drawn, do it
	if(myLabelSubscript.size()){
		QFont subscriptFont(_info->getAtomLabelFont().family(), (int)(_info->getAtomLabelFont().pointSizeF()/2.0));
		painter->setFont(subscriptFont);
		qreal hOffset = labelFM.width(myLabel);
		QFontMetricsF subscriptFM(subscriptFont);
		qreal vOffset = subscriptFM.height()/3.0;

		painter->drawText(labelPos + QPointF(hOffset, vOffset), myLabelSubscript);
	}

	// If there's a superscript to be drawn, do it
	if(myLabelSuperscript.size()){
		QFont superscriptFont(_info->getAtomLabelFont().family(),(int)(_info->getAtomLabelFont().pointSizeF()/2.0));
		painter->setFont(superscriptFont);
		qreal hOffset  = labelFM.width(myLabel);
		qreal vOffset2 = labelFM.height();
		QFontMetricsF superscriptFM(superscriptFont);
		qreal vOffset = superscriptFM.height()/3.0;

		painter->drawText(labelPos + QPointF(hOffset, - vOffset2 + 2.0*vOffset), myLabelSuperscript);
	}

	// Draw a semi-transparent green circle over any selected atoms
	if(isSelected()){
		painter->setBrush(SELECTED_COLOR);
		painter->drawEllipse(rect());
	}
	// Draw a semi-transparent white circle for fogging
	if(_info->getUseFogging()){
		double dZ = _info->maxZ() - _info->minZ();
		double thisZ = fabs(myZ - _info->maxZ());
		double opacity = (dZ > TINY ? 2.56*(_info->getFoggingScale())*(thisZ/dZ) : 0.0);
		opacity = (opacity < 0 ? 0 : opacity);
		opacity = (opacity > 255 ? 255 : opacity);
		painter->setPen(Qt::transparent);
		painter->setBrush(QColor(255,255,255,opacity));
		painter->drawEllipse(fillRect);
	}
}

double Atom::bondLength(Atom *s, Atom *e)
{
	return(sqrt(pow(s->x() - e->x(), 2.0) +
				pow(s->y() - e->y(), 2.0) +
				pow(s->z() - e->z(), 2.0)));
}

void Atom::fillLabelToVdwRadiusMap()
{
	static bool filled = false;
	// After Xe, everything looks the same anyway, so 1.44 is used for mega heavy atoms
	if(!filled){
		labelToVdwRadius.insert(QString("X" ),0.00);
		labelToVdwRadius.insert(QString("H" ),0.30);
		labelToVdwRadius.insert(QString("He"),0.93);
		labelToVdwRadius.insert(QString("Li"),1.23);
		labelToVdwRadius.insert(QString("Be"),0.90);
		labelToVdwRadius.insert(QString("B" ),0.82);
		labelToVdwRadius.insert(QString("C" ),0.77);
		labelToVdwRadius.insert(QString("N" ),0.75);
		labelToVdwRadius.insert(QString("O" ),0.73);
		labelToVdwRadius.insert(QString("F" ),0.72);
		labelToVdwRadius.insert(QString("Ne"),0.71);
		labelToVdwRadius.insert(QString("Na"),1.54);
		labelToVdwRadius.insert(QString("Mg"),1.36);
		labelToVdwRadius.insert(QString("Al"),1.18);
		labelToVdwRadius.insert(QString("Si"),1.11);
		labelToVdwRadius.insert(QString("P" ),1.06);
		labelToVdwRadius.insert(QString("S" ),1.02);
		labelToVdwRadius.insert(QString("Cl"),0.99);
		labelToVdwRadius.insert(QString("Ar"),0.98);
		labelToVdwRadius.insert(QString("K" ),2.03);
		labelToVdwRadius.insert(QString("Ca"),1.74);
		labelToVdwRadius.insert(QString("Sc"),1.44);
		labelToVdwRadius.insert(QString("Ti"),1.32);
		labelToVdwRadius.insert(QString("V" ),1.22);
		labelToVdwRadius.insert(QString("Cr"),1.18);
		labelToVdwRadius.insert(QString("Mn"),1.17);
		labelToVdwRadius.insert(QString("Fe"),1.17);
		labelToVdwRadius.insert(QString("Co"),1.16);
		labelToVdwRadius.insert(QString("Ni"),1.15);
		labelToVdwRadius.insert(QString("Cu"),1.17);
		labelToVdwRadius.insert(QString("Zn"),1.25);
		labelToVdwRadius.insert(QString("Ga"),1.26);
		labelToVdwRadius.insert(QString("Ge"),1.22);
		labelToVdwRadius.insert(QString("As"),1.20);
		labelToVdwRadius.insert(QString("Se"),1.16);
		labelToVdwRadius.insert(QString("Br"),1.14);
		labelToVdwRadius.insert(QString("Kr"),1.12);
		labelToVdwRadius.insert(QString("Rb"),2.16);
		labelToVdwRadius.insert(QString("Sr"),1.91);
		labelToVdwRadius.insert(QString("Y" ),1.62);
		labelToVdwRadius.insert(QString("Zr"),1.45);
		labelToVdwRadius.insert(QString("Nb"),1.34);
		labelToVdwRadius.insert(QString("Mo"),1.30);
		labelToVdwRadius.insert(QString("Tc"),1.27);
		labelToVdwRadius.insert(QString("Ru"),1.25);
		labelToVdwRadius.insert(QString("Rh"),1.25);
		labelToVdwRadius.insert(QString("Pd"),1.28);
		labelToVdwRadius.insert(QString("Ag"),1.34);
		labelToVdwRadius.insert(QString("Cd"),1.48);
		labelToVdwRadius.insert(QString("In"),1.44);
		labelToVdwRadius.insert(QString("Sn"),1.41);
		labelToVdwRadius.insert(QString("Sb"),1.40);
		labelToVdwRadius.insert(QString("Te"),1.36);
		labelToVdwRadius.insert(QString("I" ),1.33);
		labelToVdwRadius.insert(QString("Xe"),1.31);
	}
	filled = true;
}

void Atom::fillLabelToMassMap()
{
	static bool filled = false;
	// These were all "borrowed" from PSI3's masses.h file
	if(!filled){
		labelToMass.insert(QString("X" ),  0.000000000);
		labelToMass.insert(QString("H" ),  1.007825032);
		labelToMass.insert(QString("He"),  4.002603254);
		labelToMass.insert(QString("Li"),  7.016004548);
		labelToMass.insert(QString("Be"),  9.012182201);
		labelToMass.insert(QString("B" ), 11.009305406);
		labelToMass.insert(QString("C" ), 12.000000000);
		labelToMass.insert(QString("N" ), 14.003074005);
		labelToMass.insert(QString("O" ), 15.994914620);
		labelToMass.insert(QString("F" ), 18.998403224);
		labelToMass.insert(QString("Ne"), 19.992440175);
		labelToMass.insert(QString("Na"), 22.989769281);
		labelToMass.insert(QString("Mg"), 23.985041699);
		labelToMass.insert(QString("Al"), 26.981538627);
		labelToMass.insert(QString("Si"), 27.976926532);
		labelToMass.insert(QString("P" ), 30.973761629);
		labelToMass.insert(QString("S" ), 31.972070999);
		labelToMass.insert(QString("Cl"), 34.968852682);
		labelToMass.insert(QString("Ar"), 39.962383123);
		labelToMass.insert(QString("K" ), 38.963706679);
		labelToMass.insert(QString("Ca"), 39.962590983);
		labelToMass.insert(QString("Sc"), 44.955911909);
		labelToMass.insert(QString("Ti"), 47.947946281);
		labelToMass.insert(QString("V" ), 50.943959507);
		labelToMass.insert(QString("Cr"), 51.940507472);
		labelToMass.insert(QString("Mn"), 54.938045141);
		labelToMass.insert(QString("Fe"), 55.934937475);
		labelToMass.insert(QString("Co"), 58.933195048);
		labelToMass.insert(QString("Ni"), 57.935342907);
		labelToMass.insert(QString("Cu"), 62.929597474);
		labelToMass.insert(QString("Zn"), 63.929142222);
		labelToMass.insert(QString("Ga"), 68.925573587);
		labelToMass.insert(QString("Ge"), 73.921177767);
		labelToMass.insert(QString("As"), 74.921596478);
		labelToMass.insert(QString("Se"), 79.916521271);
		labelToMass.insert(QString("Br"), 78.918337087);
		labelToMass.insert(QString("Kr"), 85.910610729);
		labelToMass.insert(QString("Rb"), 84.911789737);
		labelToMass.insert(QString("Sr"), 87.905612124);
		labelToMass.insert(QString("Y" ), 88.905848295);
		labelToMass.insert(QString("Zr"), 89.904704416);
		labelToMass.insert(QString("Nb"), 92.906378058);
		labelToMass.insert(QString("Mo"), 97.905408169);
		labelToMass.insert(QString("Tc"), 98.906254747);
		labelToMass.insert(QString("Ru"),101.904349312);
		labelToMass.insert(QString("Rh"),102.905504292);
		labelToMass.insert(QString("Pd"),105.903485715);
		labelToMass.insert(QString("Ag"),106.90509682 );
		labelToMass.insert(QString("Cd"),113.90335854 );
		labelToMass.insert(QString("In"),114.903878484);
		labelToMass.insert(QString("Sn"),119.902194676);
		labelToMass.insert(QString("Sb"),120.903815686);
		labelToMass.insert(QString("Te"),129.906224399);
		labelToMass.insert(QString("I" ),126.904472681);
		labelToMass.insert(QString("Xe"),131.904153457);
		labelToMass.insert(QString("Cs"),132.905451932);
		labelToMass.insert(QString("Ba"),137.905247237);
		labelToMass.insert(QString("La"),138.906353267);
		labelToMass.insert(QString("Ce"),139.905438706);
		labelToMass.insert(QString("Pr"),140.907652769);
		labelToMass.insert(QString("Nd"),144.912749023);
		labelToMass.insert(QString("Pm"),151.919732425);
		labelToMass.insert(QString("Sm"),152.921230339);
		labelToMass.insert(QString("Eu"),157.924103912);
		labelToMass.insert(QString("Gd"),158.925346757);
		labelToMass.insert(QString("Tb"),163.929174751);
		labelToMass.insert(QString("Dy"),164.93032207 );
		labelToMass.insert(QString("Ho"),165.930293061);
		labelToMass.insert(QString("Er"),168.93421325 );
		labelToMass.insert(QString("Tm"),173.938862089);
		labelToMass.insert(QString("Yb"),174.940771819);
		labelToMass.insert(QString("Lu"),179.946549953);
		labelToMass.insert(QString("Hf"),180.947995763);
		labelToMass.insert(QString("Ta"),183.950931188);
		labelToMass.insert(QString("W" ),186.955753109);
		labelToMass.insert(QString("Re"),191.96148069 );
		labelToMass.insert(QString("Os"),192.96292643 );
		labelToMass.insert(QString("Ir"),194.964791134);
		labelToMass.insert(QString("Pt"),196.966568662);
		labelToMass.insert(QString("Au"),201.970643011);
		labelToMass.insert(QString("Hg"),204.974427541);
		labelToMass.insert(QString("Tl"),207.976652071);
		labelToMass.insert(QString("Pb"),208.980398734);
		labelToMass.insert(QString("Bi"),208.982430435);
		labelToMass.insert(QString("Po"),210.987496271);
		labelToMass.insert(QString("At"),222.017577738);
		labelToMass.insert(QString("Rn"),222.01755173 );
		labelToMass.insert(QString("Fr"),228.031070292);
		labelToMass.insert(QString("Ra"),227.027752127);
		labelToMass.insert(QString("Ac"),232.038055325);
		labelToMass.insert(QString("Th"),231.03588399 );
		labelToMass.insert(QString("Pa"),238.050788247);
		labelToMass.insert(QString("U" ),237.048173444);
		labelToMass.insert(QString("Np"),242.058742611);
		labelToMass.insert(QString("Pu"),243.06138108 );
		labelToMass.insert(QString("Am"),247.07035354 );
		labelToMass.insert(QString("Cm"),247.07030708 );
		labelToMass.insert(QString("Bk"),251.079586788);
		labelToMass.insert(QString("Cf"),252.082978512);
		labelToMass.insert(QString("Es"),257.095104724);
		labelToMass.insert(QString("Fm"),258.098431319);
		labelToMass.insert(QString("Md"),255.093241131);
		labelToMass.insert(QString("No"),260.105504   );
		labelToMass.insert(QString("Lr"),263.112547   );
		labelToMass.insert(QString("Rf"),255.107398   );
		labelToMass.insert(QString("Db"),259.114500   );
		labelToMass.insert(QString("Sg"),262.122892   );
		labelToMass.insert(QString("Bh"),263.128558   );
		labelToMass.insert(QString("Hs"),265.136151   );
		labelToMass.insert(QString("Mt"),281.162061   );
		labelToMass.insert(QString("Ds"),272.153615   );
		labelToMass.insert(QString("Rg"),283.171792   );
	}
}

void Atom::fillLabelToColorMap()
{
	labelToColor.insert(QString("X" ), QColor(255,  20, 147));
	labelToColor.insert(QString("H" ), QColor(250, 235, 215));
	labelToColor.insert(QString("He"), QColor(255, 192, 203));
	labelToColor.insert(QString("Li"), QColor(178,  34,  34));
	labelToColor.insert(QString("Be"), QColor( 34, 139,  34));
	labelToColor.insert(QString("B" ), QColor(  0, 255,   0));
	labelToColor.insert(QString("C" ), QColor(142, 158, 174));
	labelToColor.insert(QString("N" ), QColor(  0, 191, 255));
	labelToColor.insert(QString("O" ), QColor(255,   0,   0));
	labelToColor.insert(QString("F" ), QColor(218, 165,  32));
	labelToColor.insert(QString("Ne"), QColor(255, 105, 180));
	labelToColor.insert(QString("Na"), QColor(  0,   0, 255));
	labelToColor.insert(QString("Mg"), QColor( 34, 139,  34));
	labelToColor.insert(QString("Al"), QColor(190, 190, 190));
	labelToColor.insert(QString("Si"), QColor(218, 165,  32));
	labelToColor.insert(QString("P" ), QColor(255, 165,   0));
	labelToColor.insert(QString("S" ), QColor(255, 255,   0));
	labelToColor.insert(QString("Cl"), QColor(  0, 255,   0));
	labelToColor.insert(QString("Ar"), QColor(255, 192, 203));
	labelToColor.insert(QString("K" ), QColor(255,  20, 147));
	labelToColor.insert(QString("Ca"), QColor(128, 128, 128));
	labelToColor.insert(QString("Sc"), QColor(190, 190, 190));
	labelToColor.insert(QString("Ti"), QColor(190, 190, 190));
	labelToColor.insert(QString("V" ), QColor(190, 190, 190));
	labelToColor.insert(QString("Cr"), QColor(190, 190, 190));
	labelToColor.insert(QString("Mn"), QColor(190, 190, 190));
	labelToColor.insert(QString("Fe"), QColor(255, 165,   0));
	labelToColor.insert(QString("Co"), QColor(165,  42,  42));
	labelToColor.insert(QString("Ni"), QColor(165,  42,  42));
	labelToColor.insert(QString("Cu"), QColor(165,  42,  42));
	labelToColor.insert(QString("Zn"), QColor(165,  42,  42));
	labelToColor.insert(QString("Ga"), QColor(165,  42,  42));
	labelToColor.insert(QString("Ge"), QColor( 85, 107,  47));
	labelToColor.insert(QString("As"), QColor(253, 245, 230));
	labelToColor.insert(QString("Se"), QColor(152, 251, 152));
	labelToColor.insert(QString("Br"), QColor(165,  42,  42));
	labelToColor.insert(QString("Kr"), QColor( 50, 205,  50));
	labelToColor.insert(QString("Rb"), QColor(165,  42,  42));
	labelToColor.insert(QString("Sr"), QColor(190, 190, 190));
	labelToColor.insert(QString("Y" ), QColor(190, 190, 190));
	labelToColor.insert(QString("Zr"), QColor(190, 190, 190));
	labelToColor.insert(QString("Nb"), QColor(190, 190, 190));
	labelToColor.insert(QString("Mo"), QColor(255, 127,  80));
	labelToColor.insert(QString("Tc"), QColor(190, 190, 190));
	labelToColor.insert(QString("Ru"), QColor(190, 190, 190));
	labelToColor.insert(QString("Rh"), QColor(190, 190, 190));
	labelToColor.insert(QString("Pd"), QColor(190, 190, 190));
	labelToColor.insert(QString("Ag"), QColor(190, 190, 190));
	labelToColor.insert(QString("Cd"), QColor(255, 140,   0));
	labelToColor.insert(QString("In"), QColor(190, 190, 190));
	labelToColor.insert(QString("Sn"), QColor(190, 190, 190));
	labelToColor.insert(QString("Sb"), QColor(190, 190, 190));
	labelToColor.insert(QString("Te"), QColor(190, 190, 190));
	labelToColor.insert(QString("I" ), QColor(160,  32, 240));
	labelToColor.insert(QString("Xe"), QColor(255, 105, 180));
	labelToColor.insert(QString("Cs"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ba"), QColor(183, 189, 199));
	labelToColor.insert(QString("La"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ce"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pr"), QColor(183, 189, 199));
	labelToColor.insert(QString("Nd"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pm"), QColor(183, 189, 199));
	labelToColor.insert(QString("Sm"), QColor(183, 189, 199));
	labelToColor.insert(QString("Eu"), QColor(183, 189, 199));
	labelToColor.insert(QString("Gd"), QColor(183, 189, 199));
	labelToColor.insert(QString("Tb"), QColor(183, 189, 199));
	labelToColor.insert(QString("Dy"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ho"), QColor(183, 189, 199));
	labelToColor.insert(QString("Er"), QColor(183, 189, 199));
	labelToColor.insert(QString("Tm"), QColor(183, 189, 199));
	labelToColor.insert(QString("Yb"), QColor(183, 189, 199));
	labelToColor.insert(QString("Lu"), QColor(183, 189, 199));
	labelToColor.insert(QString("Hf"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ta"), QColor(183, 189, 199));
	labelToColor.insert(QString("W" ), QColor(183, 189, 199));
	labelToColor.insert(QString("Re"), QColor(183, 189, 199));
	labelToColor.insert(QString("Os"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ir"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pt"), QColor(183, 189, 199));
	labelToColor.insert(QString("Au"), QColor(183, 189, 199));
	labelToColor.insert(QString("Hg"), QColor(183, 189, 199));
	labelToColor.insert(QString("Tl"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pb"), QColor(183, 189, 199));
	labelToColor.insert(QString("Bi"), QColor(183, 189, 199));
	labelToColor.insert(QString("Po"), QColor(183, 189, 199));
	labelToColor.insert(QString("At"), QColor(183, 189, 199));
	labelToColor.insert(QString("Rn"), QColor(183, 189, 199));
	labelToColor.insert(QString("Fr"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ra"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ac"), QColor(183, 189, 199));
	labelToColor.insert(QString("Th"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pa"), QColor(183, 189, 199));
	labelToColor.insert(QString("U" ), QColor(183, 189, 199));
	labelToColor.insert(QString("Np"), QColor(183, 189, 199));
	labelToColor.insert(QString("Pu"), QColor(183, 189, 199));
	labelToColor.insert(QString("Am"), QColor(183, 189, 199));
	labelToColor.insert(QString("Cm"), QColor(183, 189, 199));
	labelToColor.insert(QString("Bk"), QColor(183, 189, 199));
	labelToColor.insert(QString("Cf"), QColor(183, 189, 199));
	labelToColor.insert(QString("Es"), QColor(183, 189, 199));
	labelToColor.insert(QString("Fm"), QColor(183, 189, 199));
	labelToColor.insert(QString("Md"), QColor(183, 189, 199));
	labelToColor.insert(QString("No"), QColor(183, 189, 199));
	labelToColor.insert(QString("Lr"), QColor(183, 189, 199));
	labelToColor.insert(QString("Rf"), QColor(183, 189, 199));
	labelToColor.insert(QString("Db"), QColor(183, 189, 199));
	labelToColor.insert(QString("Sg"), QColor(183, 189, 199));
	labelToColor.insert(QString("Bh"), QColor(183, 189, 199));
	labelToColor.insert(QString("Hs"), QColor(183, 189, 199));
	labelToColor.insert(QString("Mt"), QColor(183, 189, 199));
	labelToColor.insert(QString("Ds"), QColor(183, 189, 199));
	labelToColor.insert(QString("Rg"), QColor(183, 189, 199));
}

void Atom::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Atom");
	writer->writeAttribute("id", QString("%1").arg(myID));
	writer->writeAttribute("symbol", mySymbol);
	writer->writeAttribute("x", QString("%1").arg(myX));
	writer->writeAttribute("y", QString("%1").arg(myY));
	writer->writeAttribute("z", QString("%1").arg(myZ));
	writer->writeAttribute("label", myLabel);
	writer->writeAttribute("labelSub", myLabelSubscript);
	writer->writeAttribute("labelSup", myLabelSuperscript);
	writer->writeAttribute("effRadius", QString("%1").arg(myEffectiveRadius));
	writer->writeAttribute("fontSize", QString("%1").arg(myFontSize));
	writer->writeAttribute("fontSizeStyle", QString("%1").arg(myFontSizeStyle));
	writer->writeAttribute("scale", QString("%1").arg(myScaleFactor));
	writer->writeAttribute("color", QString("%1 %2 %3 %4").arg(fill_color.red()).arg(fill_color.green()).arg(fill_color.blue()).arg(fill_color.alpha()));
	writer->writeEndElement();
}

Atom* Atom::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "Atom");
	QXmlStreamAttributes attr = reader->attributes();
	Atom* a = new Atom(attr.value("symbol").toString(), drawingInfo, NULL);
	a->setID(attr.value("id").toString().toInt());
	a->setX(attr.value("x").toString().toDouble());
	a->setY(attr.value("y").toString().toDouble());
	a->setZ(attr.value("z").toString().toDouble());
	a->setLabel(attr.value("label").toString());
	a->setLabelSubscript(attr.value("labelSub").toString());
	a->setLabelSuperscript(attr.value("labelSup").toString());
	a->myEffectiveRadius = attr.value("effRadius").toString().toDouble();
	a->setFontSizeStyle(attr.value("fontSizeStyle").toString().toInt() == 0 ? SmallLabel : LargeLabel);
	a->setLabelFontSize(attr.value("fontSize").toString().toInt());
	a->setScaleFactor(attr.value("scale").toString().toDouble());
	QStringList color = attr.value("color").toString().split(" ");
	a->setColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));
	return a;
}
