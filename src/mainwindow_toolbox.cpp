#include "mainwindow.h"

void MainWindow::createToolBox()
{
    QWidget *appearanceWidget 	   = createAppearanceWidget();
    QWidget *bondsAndAnglesWidget  = createBondsAndAnglesWidget();
    QWidget *atomsWidget           = createAtomsWidget();
    animationWidget                = createAnimationWidget();
    
    toolBox = new QToolBox;
    //  toolBox->addItem(annotationWidget, tr("Annotation"));
    toolBox->addItem(appearanceWidget, tr("Appearance"));
    toolBox->addItem(atomsWidget, tr("Atoms"));
    toolBox->addItem(bondsAndAnglesWidget, tr("Angles and Bonds"));
    toolBox->addItem(animationWidget, tr("Animation"));
    toolBox->setCurrentWidget(atomsWidget);
}

QWidget *MainWindow::createAppearanceWidget()
{	
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    QGroupBox *foggingGroupBox = new QGroupBox(tr("Fogging"));
    useFoggingBox                    = new QCheckBox(tr("Use fogging"));
    useFoggingBox->setToolTip(tr("Add a fog effect to more distant atoms to emphasize those in the foreground"));
    foggingScaleBox                  = new QSpinBox();
    foggingScaleBox->setSuffix("%");
    QLabel *foggingLabel             = new QLabel(tr("Opacity:"));
    foggingLabel->setVisible(false);
    QHBoxLayout *foggingBoxLayout    = new QHBoxLayout();
    foggingBoxLayout->addWidget(useFoggingBox);
    foggingBoxLayout->addStretch(20);
    foggingBoxLayout->addWidget(foggingLabel);
    foggingBoxLayout->addWidget(foggingScaleBox);
    foggingScaleBox->setValue(DEFAULT_FOGGING_SCALE);
    foggingScaleBox->setMaximum(100);
    foggingScaleBox->setAccelerated(true);
    foggingScaleBox->setVisible(false);
    connect(useFoggingBox,SIGNAL(toggled(bool)),drawingInfo,SLOT(setUseFogging(bool)));
    connect(useFoggingBox,SIGNAL(toggled(bool)),foggingScaleBox,SLOT(setVisible(bool)));
    connect(useFoggingBox,SIGNAL(toggled(bool)),foggingLabel,SLOT(setVisible(bool)));
    connect(useFoggingBox,SIGNAL(toggled(bool)),canvas,SLOT(refresh()));
    connect(foggingScaleBox,SIGNAL(valueChanged(int)),drawingInfo,SLOT(setFoggingScale(int)));
    connect(foggingScaleBox,SIGNAL(valueChanged(int)),canvas,SLOT(refresh()));
    foggingGroupBox->setLayout(foggingBoxLayout);
    layout->addWidget(foggingGroupBox);

    // The Orientation Box
    QGroupBox *rotationGroupBox = new QGroupBox(tr("Rotation"));
    QGridLayout *rotationLayout = new QGridLayout;
    QValidator *intValidator = new QIntValidator(0, 360, this);
    xRotationBox = new QLineEdit("0");
    yRotationBox = new QLineEdit("0");
    zRotationBox = new QLineEdit("0");
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
    connect(backgroundColorButton, SIGNAL(clicked()), canvas, SLOT(setBackgroundColor()));
    backgroundColorLayout->addWidget(new QLabel(tr("Background Opacity")), 1, 0);
    backgroundOpacitySpinBox = new QSpinBox;
    backgroundOpacitySpinBox->setToolTip(tr("Changes how opaque the background is"));
    backgroundOpacitySpinBox->setSuffix("%");
    backgroundOpacitySpinBox->setValue(0);
    backgroundOpacitySpinBox->setRange(0,100);
    connect(backgroundOpacitySpinBox, SIGNAL(valueChanged(int)), canvas, SLOT(setBackgroundOpacity(int)));
    backgroundColorLayout->addWidget(backgroundOpacitySpinBox, 1, 1);
    backgroundColorGroupBox->setLayout(backgroundColorLayout);
    layout->addWidget(backgroundColorGroupBox);

    QGroupBox *zoomGroupBox = new QGroupBox(tr("Zoom"));
    QGridLayout *zoomLayout = new QGridLayout;
    QLabel *zoomTitle = new QLabel(tr("Zoom:"));
    zoomSpinBox = new QSpinBox();
    zoomSpinBox->setRange(0, 50000);
    zoomSpinBox->setSuffix("%");
    zoomSpinBox->setAccelerated(true);
    zoomSpinBox->setValue(100);
    zoomSpinBox->setMinimum(1);
    zoomSpinBox->setMaximum(200);
    connect(zoomSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
    zoomLayout->addWidget(zoomTitle, 0, 0);
    zoomLayout->addWidget(zoomSpinBox, 0, 1);
    zoomGroupBox->setLayout(zoomLayout);
    layout->addWidget(zoomGroupBox);
    
    widget->setLayout(layout);
    return widget;	
}

QWidget *MainWindow::createAnnotationWidget()
{
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

QWidget *MainWindow::createBondsAndAnglesWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    // The label buttons
    QGroupBox *labelsGroupBox = new QGroupBox(tr("Labels"));
    QGridLayout *labelsLayout = new QGridLayout;
    // The bond labels    
    toggleBondLabelsButton = new QPushButton(tr("Toggle Bond Labels"));
    toggleBondLabelsButton->setToolTip(tr("Toggle the bond length labels of the selected bonds"));
    connect(toggleBondLabelsButton, SIGNAL(pressed()), canvas, SLOT(toggleBondLabels()));
    labelsLayout->addWidget(toggleBondLabelsButton, 0, 0, 1, 2);
    bondLabelsPrecisionBox = new QSpinBox(); 
    bondLabelsPrecisionBox->setToolTip(tr("Set the precision of the bond length labels"));
    bondLabelsPrecisionBox->setValue(DEFAULT_BOND_LABEL_PRECISION);
    bondLabelsPrecisionBox->setFocusPolicy(Qt::NoFocus);
    connect(bondLabelsPrecisionBox, SIGNAL(valueChanged(int)), canvas, SLOT(setBondLabelPrecision(int)));
    QLabel *bondLabelsPrecisionLabel = new QLabel(tr("Bond label precision:"));
    labelsLayout->addWidget(bondLabelsPrecisionLabel, 1, 0);
    labelsLayout->addWidget(bondLabelsPrecisionBox, 1, 1);
    // The angle labels
    toggleAngleLabelsButton = new QPushButton(tr("Toggle Angle Labels"));
    toggleAngleLabelsButton->setToolTip(tr("Select three or more atoms to toggle the angle markers and labels.  Only angles between bonds will be drawn"));
    connect(toggleAngleLabelsButton, SIGNAL(pressed()), canvas, SLOT(toggleAngleLabels()));
    labelsLayout->addWidget(toggleAngleLabelsButton, 2, 0, 1, 2);
    angleLabelsPrecisionBox = new QSpinBox(); 
    angleLabelsPrecisionBox->setToolTip(tr("Set the precision of the angle labels"));
    angleLabelsPrecisionBox->setValue(DEFAULT_ANGLE_LABEL_PRECISION);
    angleLabelsPrecisionBox->setFocusPolicy(Qt::NoFocus);
    connect(angleLabelsPrecisionBox, SIGNAL(valueChanged(int)), canvas, SLOT(setAngleLabelPrecision(int)));
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
    connect(toggleBondDashingButton, SIGNAL(pressed()),
            canvas, SLOT(toggleBondDashing()));
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

QWidget *MainWindow::createAnimationWidget()
{
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

QWidget *MainWindow::createAtomsWidget()
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
    connect(atomColorButton, SIGNAL(clicked()), canvas, SLOT(setAtomColors()));
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
    connect(atomDrawingStyleButtonGroup, SIGNAL(buttonClicked(int)), canvas, SLOT(setAtomDrawingStyle(int)));
    gradientColoredAtomDrawingButton->setChecked(true);
    drawingStyleBox->setLayout(drawingStyleLayout);
    layout->addWidget(drawingStyleBox);
    
    // The atom labels
    QGroupBox *labelStyleBox 	    = new QGroupBox(tr("Atom Label"));    
    QGridLayout *labelStyleLayout   = new QGridLayout;
    
    // The atom numbers as label subscripts    
    toggleAtomNumberSubscriptsButton = new QPushButton(tr("Toggle Atom Number Subscripts"));
    toggleAtomNumberSubscriptsButton->setToolTip(tr("Add/remove atom numbers as a subscript to the selected atoms"));
    labelStyleLayout->addWidget(toggleAtomNumberSubscriptsButton, 0, 0, 1, 2);
    connect(toggleAtomNumberSubscriptsButton, SIGNAL(pressed()),
            canvas, SLOT(toggleAtomNumberSubscripts()));
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
    connect(atomLabelFontCombo, SIGNAL(currentFontChanged(const QFont &)),
            canvas, SLOT(atomLabelFontChanged(const QFont &)));
    // The label font size
    atomLabelFontSizeCombo = new QComboBox;
    atomLabelFontSizeCombo->setEditable(true);
    atomLabelFontSizeCombo->setToolTip(tr("The font size for the selected atoms"));
    for (int i = 4; i < 35; ++i){
        atomLabelFontSizeCombo->addItem(QString().setNum(i));
    }
    atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
    atomLabelFontSizeCombo->setFocusPolicy(Qt::NoFocus);
    labelStyleLayout->addWidget(atomLabelFontSizeCombo, 3, 0, 1, 2);
    connect(atomLabelFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            canvas, SLOT(atomLabelFontSizeChanged(const QString &)));

    atomFontSizeButtonGroup      	 = new QButtonGroup;
    smallLabelAtomDrawingButton 	 = new QRadioButton(tr("Small Label"));
    largeLabelAtomDrawingButton 	 = new QRadioButton(tr("Large Label"));
    smallLabelAtomDrawingButton->setChecked(true);
    atomFontSizeButtonGroup->addButton(smallLabelAtomDrawingButton, int(Atom::SmallLabel));
    atomFontSizeButtonGroup->addButton(largeLabelAtomDrawingButton, int(Atom::LargeLabel));
    labelStyleLayout->addWidget(smallLabelAtomDrawingButton, 4, 0);
    labelStyleLayout->addWidget(largeLabelAtomDrawingButton, 4, 1);
    connect(atomFontSizeButtonGroup, SIGNAL(buttonClicked(int)),
            canvas, SLOT(setAtomFontSizeStyle(int)));

    
    labelStyleBox->setLayout(labelStyleLayout);
    layout->addWidget(labelStyleBox);    
    
    widget->setLayout(layout);
    return widget;	
}

QSlider *MainWindow::createSlider(int max)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, max);
    slider->setSingleStep(1);
    slider->setPageStep(15);
    return slider;
}
