#include "mainwindow.h"

void MainWindow::createToolBox(QMap<QString, QString>* options, int width, int height)
{
	if(options == NULL)
		options = defaultToolBoxOptions();

	QWidget *appearanceWidget 	   = createAppearanceWidget(options);
	QWidget *bondsAndAnglesWidget  = createBondsAndAnglesWidget(options);
	QWidget *atomsWidget           = createAtomsWidget(options);
	animationWidget                = createAnimationWidget(options);

	toolBox = new QToolBox;
	//  toolBox->addItem(annotationWidget, tr("Annotation"));
	toolBox->addItem(appearanceWidget, tr("Appearance"));
	toolBox->addItem(atomsWidget, tr("Atoms"));
	toolBox->addItem(bondsAndAnglesWidget, tr("Angles and Bonds"));
	toolBox->addItem(animationWidget, tr("Animation"));
	toolBox->setCurrentWidget(atomsWidget);

	toolBox->setGeometry(0, 0, width, height);
	toolBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

QWidget *MainWindow::createAppearanceWidget(QMap<QString, QString>* options)
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout;

	bool useFogging = options->value("FOGGING_ON").toInt();

	QGroupBox *foggingGroupBox = new QGroupBox(tr("Fogging"));
	useFoggingBox = new QCheckBox(tr("Use fogging"));
	useFoggingBox->setToolTip(tr("Add a fog effect to more distant atoms to emphasize those in the foreground\n0%: no fogging applied\n100%: most distant atoms completely invisible"));
	useFoggingBox->setChecked(useFogging);
	foggingScaleBox = new QSpinBox();
	foggingScaleBox->setSuffix("%");
	QLabel *foggingLabel = new QLabel(tr("Opacity:"));
	foggingLabel->setVisible(useFogging);
	QHBoxLayout *foggingBoxLayout = new QHBoxLayout();
	foggingBoxLayout->addWidget(useFoggingBox);
	foggingBoxLayout->addStretch(20);
	foggingBoxLayout->addWidget(foggingLabel);
	foggingBoxLayout->addWidget(foggingScaleBox);
	foggingScaleBox->setMaximum(100);
	foggingScaleBox->setValue(options->value("FOGGING_SCALE").toInt());
	foggingScaleBox->setAccelerated(true);
	foggingScaleBox->setVisible(useFogging);
	connect(useFoggingBox, SIGNAL(toggled(bool)), foggingScaleBox, SLOT(setVisible(bool)));
	connect(useFoggingBox, SIGNAL(toggled(bool)), foggingLabel, SLOT(setVisible(bool)));
	foggingGroupBox->setLayout(foggingBoxLayout);
	layout->addWidget(foggingGroupBox);

	// The Orientation Box
	QGroupBox *rotationGroupBox = new QGroupBox(tr("Rotation"));
	QGridLayout *rotationLayout = new QGridLayout;
	QValidator *intValidator = new QIntValidator(0, 360, this);
	xRotationBox = new QLineEdit(options->value("X_ROTATION"));
	yRotationBox = new QLineEdit(options->value("Y_ROTATION"));
	zRotationBox = new QLineEdit(options->value("Z_ROTATION"));
	xRotationBox->setValidator(intValidator);
	yRotationBox->setValidator(intValidator);
	zRotationBox->setValidator(intValidator);
	xRotationBox->setToolTip(tr("Type in an angle (in degrees) to rotate the molecule from its initial orientation"));
	yRotationBox->setToolTip(tr("Type in an angle (in degrees) to rotate the molecule from its initial orientation"));
	zRotationBox->setToolTip(tr("Type in an angle (in degrees) to rotate the molecule from its initial orientation"));
	connect(xRotationBox, SIGNAL(textChanged(const QString&)), this, SLOT(rotateFromInitialCoordinates()));
	connect(yRotationBox, SIGNAL(textChanged(const QString&)), this, SLOT(rotateFromInitialCoordinates()));
	connect(zRotationBox, SIGNAL(textChanged(const QString&)), this, SLOT(rotateFromInitialCoordinates()));
	xLabel = new QLabel("X:");
	yLabel = new QLabel("Y:");
	zLabel = new QLabel("Z:");
	rotationLayout->addWidget(xLabel,0,0);
	rotationLayout->addWidget(xRotationBox,0,1);
	rotationLayout->addWidget(yLabel,0,2);
	rotationLayout->addWidget(yRotationBox,0,3);
	rotationLayout->addWidget(zLabel,0,4);
	rotationLayout->addWidget(zRotationBox,0,5);
	rotationGroupBox->setLayout(rotationLayout);
	layout->addWidget(rotationGroupBox);

	QGroupBox *backgroundColorGroupBox = new QGroupBox(tr("Background"));
	QGridLayout *backgroundColorLayout = new QGridLayout;
	backgroundColorButton = new QPushButton(tr("Background Color"));
	backgroundColorButton->setToolTip(tr("Change the background color"));
	backgroundColorLayout->addWidget(backgroundColorButton, 0, 0, 1, 2);
	backgroundColorLayout->addWidget(new QLabel(tr("Background Opacity")), 1, 0);
	backgroundOpacitySpinBox = new QSpinBox;
	backgroundOpacitySpinBox->setToolTip(tr("Change the opacity of the background"));
	backgroundOpacitySpinBox->setSuffix("%");
	backgroundOpacitySpinBox->setRange(0, 100);
	backgroundOpacitySpinBox->setValue(options->value("BACKGROUND_OPACITY").toInt());
	backgroundColorLayout->addWidget(backgroundOpacitySpinBox, 1, 1);
	backgroundColorGroupBox->setLayout(backgroundColorLayout);
	layout->addWidget(backgroundColorGroupBox);

	QGroupBox *zoomGroupBox = new QGroupBox(tr("Zoom"));
	QGridLayout *zoomLayout = new QGridLayout;
	QLabel *zoomTitle = new QLabel(tr("Zoom:"));
	zoomSpinBox = new QSpinBox();
	zoomSpinBox->setMinimum(1);
	zoomSpinBox->setMaximum(200);
	zoomSpinBox->setSuffix("%");
	zoomSpinBox->setAccelerated(true);
	zoomSpinBox->setValue(options->value("ZOOM").toInt());

	connect(zoomSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	zoomLayout->addWidget(zoomTitle, 0, 0);
	zoomLayout->addWidget(zoomSpinBox, 0, 1);
	zoomGroupBox->setLayout(zoomLayout);
	layout->addWidget(zoomGroupBox);

	widget->setLayout(layout);
	return widget;
}

QWidget *MainWindow::createAnnotationWidget(QMap<QString, QString>* options)
{
	Q_UNUSED(options);

	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout;

	QGroupBox *insertSymbolBox 	    = new QGroupBox(tr("Insert Symbol"));
	insertSymbolBox->addAction(insertAngstromAction);
	insertSymbolBox->addAction(insertDegreeAction);
	insertSymbolBox->addAction(insertPlusMinusAction);
	layout->addWidget(insertSymbolBox);
	widget->addAction(insertPlusMinusAction);

	widget->setLayout(layout);
	return widget;
}

QWidget *MainWindow::createBondsAndAnglesWidget(QMap<QString, QString>* options)
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout;

	// The label buttons
	QGroupBox *labelsGroupBox = new QGroupBox(tr("Labels"));
	QGridLayout *labelsLayout = new QGridLayout;

	// The bond labels
	toggleBondLabelsButton = new QPushButton(tr("Toggle Bond Labels"));
	toggleBondLabelsButton->setToolTip(tr("Toggle the bond length labels of the selected bonds"));
	labelsLayout->addWidget(toggleBondLabelsButton, 0, 0, 1, 2);
	bondLabelsPrecisionBox = new QSpinBox();
	bondLabelsPrecisionBox->setToolTip(tr("Set the precision of the bond length labels"));
	bondLabelsPrecisionBox->setValue(options->value("BOND_LABEL_PRECISION").toInt());
	bondLabelsPrecisionBox->setFocusPolicy(Qt::NoFocus);
	QLabel *bondLabelsPrecisionLabel = new QLabel(tr("Bond label precision:"));
	labelsLayout->addWidget(bondLabelsPrecisionLabel, 1, 0);
	labelsLayout->addWidget(bondLabelsPrecisionBox, 1, 1);

	// The angle labels
	toggleAngleLabelsButton = new QPushButton(tr("Toggle Angle Labels"));
	toggleAngleLabelsButton->setToolTip(tr("Select three or more atoms to toggle the angle markers and labels.  Only angles between bonds will be drawn"));
	labelsLayout->addWidget(toggleAngleLabelsButton, 2, 0, 1, 2);
	angleLabelsPrecisionBox = new QSpinBox();
	angleLabelsPrecisionBox->setToolTip(tr("Set the precision of the angle labels"));
	angleLabelsPrecisionBox->setValue(options->value("ANGLE_LABEL_PRECISION").toInt());
	angleLabelsPrecisionBox->setFocusPolicy(Qt::NoFocus);
	QLabel *angleLabelsPrecisionLabel = new QLabel(tr("Angle label precision:"));
	labelsLayout->addWidget(angleLabelsPrecisionLabel, 3, 0);
	labelsLayout->addWidget(angleLabelsPrecisionBox, 3, 1);

	labelsGroupBox->setLayout(labelsLayout);
	layout->addWidget(labelsGroupBox);

	// The bond appearance
	QGroupBox *bondSizeWidget   = new QGroupBox(tr("Bond Appearance"));
	QGridLayout *bondSizeLayout = new QGridLayout;

	// The bond dashing
	toggleBondDashingButton = new QPushButton(tr("Toggle Bond Dashing"));
	toggleBondDashingButton->setToolTip(tr("Toggle dashed / solid bonds for the selected bonds"));
	bondSizeLayout->addWidget(toggleBondDashingButton, 0, 0, 1, 2);

	// The bond thickness
	QLabel *bondSizeLabel       = new QLabel("Bond thickness = ");
	bondSizeSpinBox             = new QDoubleSpinBox();
	bondSizeSpinBox->setSuffix(QString(" Angstrom"));
	bondSizeSpinBox->setDecimals(3);
	bondSizeSpinBox->setSingleStep(0.005);
	bondSizeSpinBox->setSpecialValueText(tr("Select Bonds"));
	bondSizeSpinBox->setFocusPolicy(Qt::NoFocus);
	bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
	bondSizeSpinBox->setAccelerated(true);
	bondSizeLayout->addWidget(bondSizeLabel, 1, 0);
	bondSizeLayout->addWidget(bondSizeSpinBox, 1, 1);
	connect(bondSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeBondSize()));
	bondSizeWidget->setLayout(bondSizeLayout);
	layout->addWidget(bondSizeWidget);

	widget->setLayout(layout);
	return widget;
}

QWidget *MainWindow::createAnimationWidget(QMap<QString, QString>* options)
{
	Q_UNUSED(options);

	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout;

	QGridLayout *animationLayout = new QGridLayout;
	QGroupBox *animationGroupBox = new QGroupBox(tr("Steps"));
	QLabel *animationLabel = new QLabel("Step Number = ");
	animationSlider = createSlider(0);
	animationLayout->addWidget(animationLabel, 0, 0);
	animationLayout->addWidget(animationSlider, 0, 1);
	animationGroupBox->setLayout(animationLayout);

	connect(animationSlider, SIGNAL(valueChanged(int)), this, SLOT(setGeometryStep(int)));

	layout->addWidget(animationGroupBox);
	widget->setLayout(layout);
	return widget;
}

QWidget *MainWindow::createAtomsWidget(QMap<QString, QString>* options)
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout;

	QGroupBox *atomSizeGroupBox = new QGroupBox(tr("Size"));
	QGridLayout *atomSizeLayout = new QGridLayout;
	QLabel *atomSizeLabel       = new QLabel("Atom size = ");
	atomSizeSpinBox             = new QDoubleSpinBox();
	atomSizeSpinBox->setSuffix(QString(" vdW radii"));
	atomSizeSpinBox->setDecimals(2);
	atomSizeSpinBox->setSingleStep(0.02);
	atomSizeSpinBox->setSpecialValueText(tr("Select Atoms"));
	atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
	atomSizeSpinBox->setFocusPolicy(Qt::NoFocus);
	atomSizeSpinBox->setAccelerated(true);
	atomSizeLayout->addWidget(atomSizeLabel, 0, 0);
	atomSizeLayout->addWidget(atomSizeSpinBox, 0, 1);
	connect(atomSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeAtomSize()));
	atomSizeGroupBox->setLayout(atomSizeLayout);
	layout->addWidget(atomSizeGroupBox);

	QGroupBox *atomColorGroupBox = new QGroupBox(tr("Atom Color"));
	QGridLayout *atomColorLayout = new QGridLayout;
	atomColorButton = new QPushButton(tr("Color Selected Atoms"));
	atomColorButton->setToolTip(tr("Change the selected atom color"));
	atomColorLayout->addWidget(atomColorButton, 0, 0, 1, 2);
	atomColorGroupBox->setLayout(atomColorLayout);
	layout->addWidget(atomColorGroupBox);


	QGroupBox *drawingStyleBox 	     = new QGroupBox(tr("Drawing Style"));
	atomDrawingStyleButtonGroup      = new QButtonGroup;
	QGridLayout *drawingStyleLayout  = new QGridLayout;
	simpleAtomDrawingButton 	     = new QRadioButton(tr("Simple"));
	houkMolAtomDrawingButton 	     = new QRadioButton(tr("HoukMol"));
	simpleColoredAtomDrawingButton   = new QRadioButton(tr("Simple Colored"));
	gradientColoredAtomDrawingButton = new QRadioButton(tr("Gradient"));
	atomDrawingStyleButtonGroup->addButton(simpleAtomDrawingButton, int(DrawingInfo::Simple));
	atomDrawingStyleButtonGroup->addButton(simpleColoredAtomDrawingButton, int(DrawingInfo::SimpleColored));
	atomDrawingStyleButtonGroup->addButton(houkMolAtomDrawingButton, int(DrawingInfo::HoukMol));
	atomDrawingStyleButtonGroup->addButton(gradientColoredAtomDrawingButton, int(DrawingInfo::Gradient));
	drawingStyleLayout->addWidget(simpleAtomDrawingButton, 0, 0);
	drawingStyleLayout->addWidget(simpleColoredAtomDrawingButton, 0, 1);
	drawingStyleLayout->addWidget(houkMolAtomDrawingButton, 1, 0);
	drawingStyleLayout->addWidget(gradientColoredAtomDrawingButton, 1, 1);
	atomDrawingStyleButtonGroup->button(options->value("ATOM_DRAWING_STYLE").toInt())->setChecked(true);
	drawingStyleBox->setLayout(drawingStyleLayout);
	layout->addWidget(drawingStyleBox);

	// The atom labels
	QGroupBox *labelStyleBox 	    = new QGroupBox(tr("Atom Label"));
	QGridLayout *labelStyleLayout   = new QGridLayout;

	// The atom numbers as label subscripts
	toggleAtomNumberSubscriptsButton = new QPushButton(tr("Toggle Atom Number Subscripts"));
	toggleAtomNumberSubscriptsButton->setToolTip(tr("Add/remove atom numbers as a subscript to the selected atoms"));
	labelStyleLayout->addWidget(toggleAtomNumberSubscriptsButton, 0, 0, 1, 2);

	// The label text
	atomLabelInput = new QLineEdit;
	atomLabelInput->setText(tr("Select Atoms"));
	atomLabelInput->setToolTip(tr("Text entered here will be used as the label for the selected atom(s).  Anything appended after an underscore will be used as a subscript, anything after a carat is a superscript"));
	//    atomLabelInput->setFocusPolicy(Qt::NoFocus);
	labelStyleLayout->addWidget(atomLabelInput, 1, 0, 1, 2);
	connect(atomLabelInput, SIGNAL(returnPressed()), this, SLOT(setAtomLabels()));

	// The label font
	atomLabelFontCombo = new QFontComboBox();
	atomLabelFontCombo->setEditText(tr("Select Atoms"));
	atomLabelFontCombo->setToolTip(tr("The font for the selected atoms"));
	atomLabelFontCombo->setFocusPolicy(Qt::NoFocus);
	labelStyleLayout->addWidget(atomLabelFontCombo, 2, 0, 1, 2);

	// The label font size
	atomLabelFontSizeCombo = new QComboBox;
	atomLabelFontSizeCombo->setEditable(true);
	atomLabelFontSizeCombo->setToolTip(tr("The font size for the selected atoms"));
	for(int i = 4; i < 35; ++i)
		atomLabelFontSizeCombo->addItem(QString().setNum(i));
	atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
	atomLabelFontSizeCombo->setFocusPolicy(Qt::NoFocus);
	labelStyleLayout->addWidget(atomLabelFontSizeCombo, 3, 0, 1, 2);

	atomFontSizeButtonGroup      	 = new QButtonGroup;
	smallLabelAtomDrawingButton 	 = new QRadioButton(tr("Small Label"));
	largeLabelAtomDrawingButton 	 = new QRadioButton(tr("Large Label"));
	atomFontSizeButtonGroup->addButton(smallLabelAtomDrawingButton, int(Atom::SmallLabel));
	atomFontSizeButtonGroup->addButton(largeLabelAtomDrawingButton, int(Atom::LargeLabel));
	atomFontSizeButtonGroup->button(options->value("ATOM_LABEL_SIZE").toInt())->setChecked(true);
	labelStyleLayout->addWidget(smallLabelAtomDrawingButton, 4, 0);
	labelStyleLayout->addWidget(largeLabelAtomDrawingButton, 4, 1);

	labelStyleBox->setLayout(labelStyleLayout);
	layout->addWidget(labelStyleBox);

	widget->setLayout(layout);
	return widget;
}

QSlider* MainWindow::createSlider(int max)
{
	QSlider *slider = new QSlider(Qt::Horizontal);
	slider->setRange(0, max);
	slider->setSingleStep(1);
	slider->setPageStep(15);
	return slider;
}

// Changes should also be reflected in mainwindow_filesave::openProject()
QMap<QString, QString>* MainWindow::defaultToolBoxOptions()
{
	QMap<QString, QString>* options = new QMap<QString, QString>();

	// Appearance
	options->insert("FOGGING_ON", QString("%1").arg(false));
	options->insert("FOGGING_SCALE", QString("%1").arg(DEFAULT_FOGGING_SCALE));
	options->insert("X_ROTATION", "0");
	options->insert("Y_ROTATION", "0");
	options->insert("Z_ROTATION", "0");
	options->insert("BACKGROUND_OPACITY", QString("%1").arg(DEFAULT_BACKGROUND_OPACITY));
	options->insert("ZOOM", "100");

	// Bonds and Angles
	options->insert("BOND_LABEL_PRECISION", QString("%1").arg(DEFAULT_BOND_LABEL_PRECISION));
	options->insert("ANGLE_LABEL_PRECISION", QString("%1").arg(DEFAULT_ANGLE_LABEL_PRECISION));

	// Atoms
	options->insert("ATOM_DRAWING_STYLE", QString("%1").arg(DrawingInfo::Gradient));
	options->insert("ATOM_LABEL_SIZE", QString("%1").arg(Atom::SmallLabel));

	return options;
}
