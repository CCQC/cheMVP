#include <QtGui>

#include "atom.h"

std::map<QString, double> Atom::labelToVdwRadius;
std::map<QString, double> Atom::labelToMass;
std::map<QString, QColor> Atom::labelToColor;

Atom::Atom(QString element, DrawingInfo *info, QGraphicsItem *parent)
    :QGraphicsEllipseItem(parent),
	myDrawingStyle(Gradient),
	myFontSizeStyle(SmallLabel),
	myX(0.0),
    myY(0.0),
    myZ(0.0),
	myScaleFactor(DEFAULT_ATOM_SCALE_FACTOR),
	mySymbol(element),
	myID(0),
	hoverOver(false),
	line_color(Qt::black),
	text_color(Qt::black),
	fill_color(Qt::white),
	drawingInfo(info)
{
	fillLabelToVdwRadiusMap();
    fillLabelToMassMap();
    fillLabelToColorMap();

    setDrawingStyle(myDrawingStyle);

    myLabel = (mySymbol == "H" ? "" : mySymbol);

    //This is a bit of a hack.  All atoms after Xe have Xe's vdW radius
    if(labelToVdwRadius[mySymbol]==0.0 && mySymbol!="X"){
        myRadius = 1.44;
    }else{
        myRadius = labelToVdwRadius[mySymbol];
    }
    myMass = labelToMass[mySymbol];
    if(myMass == 0.0 && mySymbol != "X"){
        QString errorMessage = "I don't know the mass of the atom " + mySymbol;
        error(errorMessage,__FILE__,__LINE__);
        return;
    }
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptsHoverEvents(true);
    setAcceptDrops(true);
    myEffectiveRadius = drawingInfo->scaleFactor() * (1.0 + zValue() * drawingInfo->perspective())
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

void Atom::setDrawingStyle(DrawingStyle style)
{
    if(style == Simple){
        fill_color = Qt::white;
    }else{
        fill_color = labelToColor[mySymbol];
    }
    myDrawingStyle = style;
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
    myEffectiveRadius = drawingInfo->scaleFactor() * (1.0 + zValue() * drawingInfo->perspective())
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
    linestyle.setWidthF((hoverOver ? drawingInfo->scaleFactor()*0.03 : drawingInfo->scaleFactor()*0.01));
    linestyle.setColor(Qt::black);
    painter->setPen(linestyle);

    // The circle defnining the atom
    if(myDrawingStyle == Gradient){
        // Define a gradient pattern to fill the atom
        QRadialGradient gradient(QPointF(0.0, 0.0), myEffectiveRadius,
                                 QPointF(myEffectiveRadius/2.1, -myEffectiveRadius/2.1));
        gradient.setColorAt(0.0, Qt::white);
        gradient.setColorAt(1.0, fill_color);
        // PDF looks bad when rendering gradient - here's a workaround
        painter->setPen(Qt::transparent);
        painter->setBrush(gradient);
        painter->drawEllipse(rect());
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
        if(myDrawingStyle == Simple || myDrawingStyle == SimpleColored  || myDrawingStyle == Gradient){
            painter->drawArc(v_line_box, 1440, 2880);
        }else if(myDrawingStyle == HoukMol){
            painter->drawArc(v_line_box, 1440, -2880);
        }
    }

    // Draw the blob for HoukMol rip-off
    if(myDrawingStyle == HoukMol){
        QPointF startPoint(-myEffectiveRadius/1.8, -myEffectiveRadius/20.0);
        QPointF endPoint(-myEffectiveRadius/20.0, -myEffectiveRadius/1.8);
        QPointF midPoint1(-myEffectiveRadius/1.2, -myEffectiveRadius/1.2);
        QPointF midPoint2(-myEffectiveRadius/2.1, -myEffectiveRadius/2.1);
        QPainterPath path(startPoint);
        path.quadTo(midPoint1, endPoint);
        path.quadTo(midPoint2, startPoint);
        painter->setPen(QPen(fill_color));
        painter->setBrush(Qt::white);
        painter->drawPath(path);
    }else if(myDrawingStyle == SimpleColored){
        QPointF startPoint(myEffectiveRadius/1.8, -myEffectiveRadius/20.0);
        QPointF endPoint(myEffectiveRadius/20.0, -myEffectiveRadius/1.8);
        QPointF midPoint1(myEffectiveRadius/1.2, -myEffectiveRadius/1.2);
        QPointF midPoint2(myEffectiveRadius/2.1, -myEffectiveRadius/2.1);
        QPainterPath path(startPoint);
        path.quadTo(midPoint1, endPoint);
        path.quadTo(midPoint2, startPoint);
        painter->setPen(QPen(fill_color));
        painter->setBrush(Qt::white);
        painter->drawPath(path);
    }

    // Now draw the atomic symbol on there
    setLabelFontSize(myFontSize);
    QFontMetricsF labelFM(myLabelFont);
    // TODO check these offsets.  I think there's a bug in the height reported by fontmetrics
    QPointF labelPos;
    if(myFontSizeStyle == LargeLabel){
        labelPos = QPointF(-labelFM.width(myLabel)/2.0, labelFM.height()/3.0);
    }else{
        labelPos = QPointF(-labelFM.width(myLabel)/2.0, labelFM.height()/3.5);
    }
    painter->setPen(text_color);
    painter->setBrush(text_color);
    painter->setFont(myLabelFont);
    painter->drawText(labelPos, myLabel);
    // If there's a subscript to be drawn, do it
    if(myLabelSubscript.size()){
        QFont subscriptFont(myLabelFont.family(), myLabelFont.pointSizeF()/2.0);
        painter->setFont(subscriptFont);
        qreal hOffset = labelFM.width(myLabel);
        QFontMetricsF subscriptFM(subscriptFont);
        qreal vOffset = subscriptFM.height()/3.0;

        painter->drawText(labelPos + QPointF(hOffset, vOffset), myLabelSubscript);
    }

    // If there's a superscript to be drawn, do it
    if(myLabelSuperscript.size()){
        QFont superscriptFont(myLabelFont.family(), myLabelFont.pointSizeF()/2.0);
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
}

void Atom::fillLabelToVdwRadiusMap()
{
    static bool filled = false;
    // After Xe, everything looks the same anyway, so 1.44 is used for mega heavy atoms
    if(!filled){
        labelToVdwRadius.insert(std::make_pair(QString("X" ),0.00));
        labelToVdwRadius.insert(std::make_pair(QString("H" ),0.30));
        labelToVdwRadius.insert(std::make_pair(QString("He"),0.93));
        labelToVdwRadius.insert(std::make_pair(QString("Li"),1.23));
        labelToVdwRadius.insert(std::make_pair(QString("Be"),0.90));
        labelToVdwRadius.insert(std::make_pair(QString("B" ),0.82));
        labelToVdwRadius.insert(std::make_pair(QString("C" ),0.77));
        labelToVdwRadius.insert(std::make_pair(QString("N" ),0.75));
        labelToVdwRadius.insert(std::make_pair(QString("O" ),0.73));
        labelToVdwRadius.insert(std::make_pair(QString("F" ),0.72));
        labelToVdwRadius.insert(std::make_pair(QString("Ne"),0.71));
        labelToVdwRadius.insert(std::make_pair(QString("Na"),1.54));
        labelToVdwRadius.insert(std::make_pair(QString("Mg"),1.36));
        labelToVdwRadius.insert(std::make_pair(QString("Al"),1.18));
        labelToVdwRadius.insert(std::make_pair(QString("Si"),1.11));
        labelToVdwRadius.insert(std::make_pair(QString("P" ),1.06));
        labelToVdwRadius.insert(std::make_pair(QString("S" ),1.02));
        labelToVdwRadius.insert(std::make_pair(QString("Cl"),0.99));
        labelToVdwRadius.insert(std::make_pair(QString("Ar"),0.98));
        labelToVdwRadius.insert(std::make_pair(QString("K" ),2.03));
        labelToVdwRadius.insert(std::make_pair(QString("Ca"),1.74));
        labelToVdwRadius.insert(std::make_pair(QString("Sc"),1.44));
        labelToVdwRadius.insert(std::make_pair(QString("Ti"),1.32));
        labelToVdwRadius.insert(std::make_pair(QString("V" ),1.22));
        labelToVdwRadius.insert(std::make_pair(QString("Cr"),1.18));
        labelToVdwRadius.insert(std::make_pair(QString("Mn"),1.17));
        labelToVdwRadius.insert(std::make_pair(QString("Fe"),1.17));
        labelToVdwRadius.insert(std::make_pair(QString("Co"),1.16));
        labelToVdwRadius.insert(std::make_pair(QString("Ni"),1.15));
        labelToVdwRadius.insert(std::make_pair(QString("Cu"),1.17));
        labelToVdwRadius.insert(std::make_pair(QString("Zn"),1.25));
        labelToVdwRadius.insert(std::make_pair(QString("Ga"),1.26));
        labelToVdwRadius.insert(std::make_pair(QString("Ge"),1.22));
        labelToVdwRadius.insert(std::make_pair(QString("As"),1.20));
        labelToVdwRadius.insert(std::make_pair(QString("Se"),1.16));
        labelToVdwRadius.insert(std::make_pair(QString("Br"),1.14));
        labelToVdwRadius.insert(std::make_pair(QString("Kr"),1.12));
        labelToVdwRadius.insert(std::make_pair(QString("Rb"),2.16));
        labelToVdwRadius.insert(std::make_pair(QString("Sr"),1.91));
        labelToVdwRadius.insert(std::make_pair(QString("Y" ),1.62));
        labelToVdwRadius.insert(std::make_pair(QString("Zr"),1.45));
        labelToVdwRadius.insert(std::make_pair(QString("Nb"),1.34));
        labelToVdwRadius.insert(std::make_pair(QString("Mo"),1.30));
        labelToVdwRadius.insert(std::make_pair(QString("Tc"),1.27));
        labelToVdwRadius.insert(std::make_pair(QString("Ru"),1.25));
        labelToVdwRadius.insert(std::make_pair(QString("Rh"),1.25));
        labelToVdwRadius.insert(std::make_pair(QString("Pd"),1.28));
        labelToVdwRadius.insert(std::make_pair(QString("Ag"),1.34));
        labelToVdwRadius.insert(std::make_pair(QString("Cd"),1.48));
        labelToVdwRadius.insert(std::make_pair(QString("In"),1.44));
        labelToVdwRadius.insert(std::make_pair(QString("Sn"),1.41));
        labelToVdwRadius.insert(std::make_pair(QString("Sb"),1.40));
        labelToVdwRadius.insert(std::make_pair(QString("Te"),1.36));
        labelToVdwRadius.insert(std::make_pair(QString("I" ),1.33));
        labelToVdwRadius.insert(std::make_pair(QString("Xe"),1.31));
    }
    filled = true;
}

void Atom::fillLabelToMassMap()
{
    static bool filled = false;
    // These were all "borrowed" from PSI3's masses.h file
    if(!filled){
        labelToMass.insert(std::make_pair(QString("X" ),  0.000000000));
        labelToMass.insert(std::make_pair(QString("H" ),  1.007825032));
        labelToMass.insert(std::make_pair(QString("He"),  4.002603254));
        labelToMass.insert(std::make_pair(QString("Li"),  7.016004548));
        labelToMass.insert(std::make_pair(QString("Be"),  9.012182201));
        labelToMass.insert(std::make_pair(QString("B" ), 11.009305406));
        labelToMass.insert(std::make_pair(QString("C" ), 12.000000000));
        labelToMass.insert(std::make_pair(QString("N" ), 14.003074005));
        labelToMass.insert(std::make_pair(QString("O" ), 15.994914620));
        labelToMass.insert(std::make_pair(QString("F" ), 18.998403224));
        labelToMass.insert(std::make_pair(QString("Ne"), 19.992440175));
        labelToMass.insert(std::make_pair(QString("Na"), 22.989769281));
        labelToMass.insert(std::make_pair(QString("Mg"), 23.985041699));
        labelToMass.insert(std::make_pair(QString("Al"), 26.981538627));
        labelToMass.insert(std::make_pair(QString("Si"), 27.976926532));
        labelToMass.insert(std::make_pair(QString("P" ), 30.973761629));
        labelToMass.insert(std::make_pair(QString("S" ), 31.972070999));
        labelToMass.insert(std::make_pair(QString("Cl"), 34.968852682));
        labelToMass.insert(std::make_pair(QString("Ar"), 39.962383123));
        labelToMass.insert(std::make_pair(QString("K" ), 38.963706679));
        labelToMass.insert(std::make_pair(QString("Ca"), 39.962590983));
        labelToMass.insert(std::make_pair(QString("Sc"), 44.955911909));
        labelToMass.insert(std::make_pair(QString("Ti"), 47.947946281));
        labelToMass.insert(std::make_pair(QString("V" ), 50.943959507));
        labelToMass.insert(std::make_pair(QString("Cr"), 51.940507472));
        labelToMass.insert(std::make_pair(QString("Mn"), 54.938045141));
        labelToMass.insert(std::make_pair(QString("Fe"), 55.934937475));
        labelToMass.insert(std::make_pair(QString("Co"), 58.933195048));
        labelToMass.insert(std::make_pair(QString("Ni"), 57.935342907));
        labelToMass.insert(std::make_pair(QString("Cu"), 62.929597474));
        labelToMass.insert(std::make_pair(QString("Zn"), 63.929142222));
        labelToMass.insert(std::make_pair(QString("Ga"), 68.925573587));
        labelToMass.insert(std::make_pair(QString("Ge"), 73.921177767));
        labelToMass.insert(std::make_pair(QString("As"), 74.921596478));
        labelToMass.insert(std::make_pair(QString("Se"), 79.916521271));
        labelToMass.insert(std::make_pair(QString("Br"), 78.918337087));
        labelToMass.insert(std::make_pair(QString("Kr"), 85.910610729));
        labelToMass.insert(std::make_pair(QString("Rb"), 84.911789737));
        labelToMass.insert(std::make_pair(QString("Sr"), 87.905612124));
        labelToMass.insert(std::make_pair(QString("Y" ), 88.905848295));
        labelToMass.insert(std::make_pair(QString("Zr"), 89.904704416));
        labelToMass.insert(std::make_pair(QString("Nb"), 92.906378058));
        labelToMass.insert(std::make_pair(QString("Mo"), 97.905408169));
        labelToMass.insert(std::make_pair(QString("Tc"), 98.906254747));
        labelToMass.insert(std::make_pair(QString("Ru"),101.904349312));
        labelToMass.insert(std::make_pair(QString("Rh"),102.905504292));
        labelToMass.insert(std::make_pair(QString("Pd"),105.903485715));
        labelToMass.insert(std::make_pair(QString("Ag"),106.90509682 ));
        labelToMass.insert(std::make_pair(QString("Cd"),113.90335854 ));
        labelToMass.insert(std::make_pair(QString("In"),114.903878484));
        labelToMass.insert(std::make_pair(QString("Sn"),119.902194676));
        labelToMass.insert(std::make_pair(QString("Sb"),120.903815686));
        labelToMass.insert(std::make_pair(QString("Te"),129.906224399));
        labelToMass.insert(std::make_pair(QString("I" ),126.904472681));
        labelToMass.insert(std::make_pair(QString("Xe"),131.904153457));
        labelToMass.insert(std::make_pair(QString("Cs"),132.905451932));
        labelToMass.insert(std::make_pair(QString("Ba"),137.905247237));
        labelToMass.insert(std::make_pair(QString("La"),138.906353267));
        labelToMass.insert(std::make_pair(QString("Ce"),139.905438706));
        labelToMass.insert(std::make_pair(QString("Pr"),140.907652769));
        labelToMass.insert(std::make_pair(QString("Nd"),144.912749023));
        labelToMass.insert(std::make_pair(QString("Pm"),151.919732425));
        labelToMass.insert(std::make_pair(QString("Sm"),152.921230339));
        labelToMass.insert(std::make_pair(QString("Eu"),157.924103912));
        labelToMass.insert(std::make_pair(QString("Gd"),158.925346757));
        labelToMass.insert(std::make_pair(QString("Tb"),163.929174751));
        labelToMass.insert(std::make_pair(QString("Dy"),164.93032207 ));
        labelToMass.insert(std::make_pair(QString("Ho"),165.930293061));
        labelToMass.insert(std::make_pair(QString("Er"),168.93421325 ));
        labelToMass.insert(std::make_pair(QString("Tm"),173.938862089));
        labelToMass.insert(std::make_pair(QString("Yb"),174.940771819));
        labelToMass.insert(std::make_pair(QString("Lu"),179.946549953));
        labelToMass.insert(std::make_pair(QString("Hf"),180.947995763));
        labelToMass.insert(std::make_pair(QString("Ta"),183.950931188));
        labelToMass.insert(std::make_pair(QString("W" ),186.955753109));
        labelToMass.insert(std::make_pair(QString("Re"),191.96148069 ));
        labelToMass.insert(std::make_pair(QString("Os"),192.96292643 ));
        labelToMass.insert(std::make_pair(QString("Ir"),194.964791134));
        labelToMass.insert(std::make_pair(QString("Pt"),196.966568662));
        labelToMass.insert(std::make_pair(QString("Au"),201.970643011));
        labelToMass.insert(std::make_pair(QString("Hg"),204.974427541));
        labelToMass.insert(std::make_pair(QString("Tl"),207.976652071));
        labelToMass.insert(std::make_pair(QString("Pb"),208.980398734));
        labelToMass.insert(std::make_pair(QString("Bi"),208.982430435));
        labelToMass.insert(std::make_pair(QString("Po"),210.987496271));
        labelToMass.insert(std::make_pair(QString("At"),222.017577738));
        labelToMass.insert(std::make_pair(QString("Rn"),222.01755173 ));
        labelToMass.insert(std::make_pair(QString("Fr"),228.031070292));
        labelToMass.insert(std::make_pair(QString("Ra"),227.027752127));
        labelToMass.insert(std::make_pair(QString("Ac"),232.038055325));
        labelToMass.insert(std::make_pair(QString("Th"),231.03588399 ));
        labelToMass.insert(std::make_pair(QString("Pa"),238.050788247));
        labelToMass.insert(std::make_pair(QString("U" ),237.048173444));
        labelToMass.insert(std::make_pair(QString("Np"),242.058742611));
        labelToMass.insert(std::make_pair(QString("Pu"),243.06138108 ));
        labelToMass.insert(std::make_pair(QString("Am"),247.07035354 ));
        labelToMass.insert(std::make_pair(QString("Cm"),247.07030708 ));
        labelToMass.insert(std::make_pair(QString("Bk"),251.079586788));
        labelToMass.insert(std::make_pair(QString("Cf"),252.082978512));
        labelToMass.insert(std::make_pair(QString("Es"),257.095104724));
        labelToMass.insert(std::make_pair(QString("Fm"),258.098431319));
        labelToMass.insert(std::make_pair(QString("Md"),255.093241131));
        labelToMass.insert(std::make_pair(QString("No"),260.105504   ));
        labelToMass.insert(std::make_pair(QString("Lr"),263.112547   ));
        labelToMass.insert(std::make_pair(QString("Rf"),255.107398   ));
        labelToMass.insert(std::make_pair(QString("Db"),259.114500   ));
        labelToMass.insert(std::make_pair(QString("Sg"),262.122892   ));
        labelToMass.insert(std::make_pair(QString("Bh"),263.128558   ));
        labelToMass.insert(std::make_pair(QString("Hs"),265.136151   ));
        labelToMass.insert(std::make_pair(QString("Mt"),281.162061   ));
        labelToMass.insert(std::make_pair(QString("Ds"),272.153615   ));
        labelToMass.insert(std::make_pair(QString("Rg"),283.171792   ));
    }
}

void Atom::fillLabelToColorMap()
{
    static bool filled = false;
    if(!filled){
        labelToColor.insert(std::make_pair(QString("X" ), QColor(255,  20, 147)));
        labelToColor.insert(std::make_pair(QString("H" ), QColor(250, 235, 215)));
        labelToColor.insert(std::make_pair(QString("He"), QColor(255, 192, 203)));
        labelToColor.insert(std::make_pair(QString("Li"), QColor(178,  34,  34)));
        labelToColor.insert(std::make_pair(QString("Be"), QColor( 34, 139,  34)));
        labelToColor.insert(std::make_pair(QString("B" ), QColor(  0, 255,   0)));
        labelToColor.insert(std::make_pair(QString("C" ), QColor(142, 158, 174)));
        labelToColor.insert(std::make_pair(QString("N" ), QColor(  0, 191, 255)));
        labelToColor.insert(std::make_pair(QString("O" ), QColor(255,   0,   0)));
        labelToColor.insert(std::make_pair(QString("F" ), QColor(218, 165,  32)));
        labelToColor.insert(std::make_pair(QString("Ne"), QColor(255, 105, 180)));
        labelToColor.insert(std::make_pair(QString("Na"), QColor(  0,   0, 255)));
        labelToColor.insert(std::make_pair(QString("Mg"), QColor( 34, 139,  34)));
        labelToColor.insert(std::make_pair(QString("Al"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Si"), QColor(218, 165,  32)));
        labelToColor.insert(std::make_pair(QString("P" ), QColor(255, 165,   0)));
        labelToColor.insert(std::make_pair(QString("S" ), QColor(255, 255,   0)));
        labelToColor.insert(std::make_pair(QString("Cl"), QColor(  0, 255,   0)));
        labelToColor.insert(std::make_pair(QString("Ar"), QColor(255, 192, 203)));
        labelToColor.insert(std::make_pair(QString("K" ), QColor(255,  20, 147)));
        labelToColor.insert(std::make_pair(QString("Ca"), QColor(128, 128, 128)));
        labelToColor.insert(std::make_pair(QString("Sc"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Ti"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("V" ), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Cr"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Mn"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Fe"), QColor(255, 165,   0)));
        labelToColor.insert(std::make_pair(QString("Co"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Ni"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Cu"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Zn"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Ga"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Ge"), QColor( 85, 107,  47)));
        labelToColor.insert(std::make_pair(QString("As"), QColor(253, 245, 230)));
        labelToColor.insert(std::make_pair(QString("Se"), QColor(152, 251, 152)));
        labelToColor.insert(std::make_pair(QString("Br"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Kr"), QColor( 50, 205,  50)));
        labelToColor.insert(std::make_pair(QString("Rb"), QColor(165,  42,  42)));
        labelToColor.insert(std::make_pair(QString("Sr"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Y" ), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Zr"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Nb"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Mo"), QColor(255, 127,  80)));
        labelToColor.insert(std::make_pair(QString("Tc"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Ru"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Rh"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Pd"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Ag"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Cd"), QColor(255, 140,   0)));
        labelToColor.insert(std::make_pair(QString("In"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Sn"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Sb"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("Te"), QColor(190, 190, 190)));
        labelToColor.insert(std::make_pair(QString("I" ), QColor(160,  32, 240)));
        labelToColor.insert(std::make_pair(QString("Xe"), QColor(255, 105, 180)));
        labelToColor.insert(std::make_pair(QString("Cs"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ba"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("La"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ce"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pr"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Nd"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pm"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Sm"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Eu"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Gd"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Tb"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Dy"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ho"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Er"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Tm"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Yb"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Lu"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Hf"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ta"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("W" ), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Re"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Os"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ir"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pt"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Au"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Hg"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Tl"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pb"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Bi"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Po"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("At"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Rn"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Fr"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ra"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ac"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Th"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pa"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("U" ), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Np"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Pu"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Am"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Cm"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Bk"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Cf"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Es"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Fm"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Md"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("No"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Lr"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Rf"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Db"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Sg"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Bh"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Hs"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Mt"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Ds"), QColor(183, 189, 199)));
        labelToColor.insert(std::make_pair(QString("Rg"), QColor(183, 189, 199)));
    }
}

//void Atom::processProjectFile(QSettings &settings, bool saveFile)
//{
//	if(saveFile){
//		project.setValue("element",mySymbol);
//	}else{
//		
//	}
//}
