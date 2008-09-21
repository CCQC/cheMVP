#include "mainwindow.h"

void MainWindow::createToolBox()
{
//    QWidget *builderWidget 		= createBuilderWidget();
    QWidget *appearanceWidget 	= createAppearanceWidget();
    QWidget *bondsWidget        = createBondsWidget();
    QWidget *anglesWidget       = createAnglesWidget();
    QWidget *atomsWidget        = createAtomsWidget();
    QWidget *annotationWidget   = createAnnotationWidget();

    toolBox = new QToolBox;
//    toolBox->addItem(builderWidget, tr("Builder"));
    toolBox->addItem(anglesWidget, tr("Angles"));
    toolBox->addItem(annotationWidget, tr("Annotation"));
    toolBox->addItem(appearanceWidget, tr("Appearance"));
    toolBox->addItem(atomsWidget, tr("Atoms"));
    toolBox->addItem(bondsWidget, tr("Bonds"));
}


//QWidget *MainWindow::createBuilderWidget()
//{
//    QGridLayout *layout = new QGridLayout;
//
//    periodicTableButtonGroup = new QButtonGroup;
//    connect(periodicTableButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
//                this, SLOT(periodicTableButtonGroupClicked(QAbstractButton *)));
//    layout->addWidget(createPeriodicTable(), 0, 0);
//    layout->setRowStretch(3, 10);
//    layout->setColumnStretch(2, 10);
//
//    
//    QWidget *widget = new QWidget;
//    widget->setLayout(layout);
//
//    return widget;	
//}


QWidget *MainWindow::createAppearanceWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    
    // The Orientation Sliders
    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    QLabel *xTitle = new QLabel("x");
    QLabel *yTitle = new QLabel("y");
    QLabel *zTitle = new QLabel("z");
    xLabel = new QLabel();
    yLabel = new QLabel();
    zLabel = new QLabel();
    setXRotation(0);
    setYRotation(0);
    setZRotation(0);

    connect(xSlider, SIGNAL(valueChanged(int)), this, SLOT(setXRotation(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), this, SLOT(setYRotation(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), this, SLOT(setZRotation(int)));
    connect(canvas,   SIGNAL(xRotChanged(int)), this, SLOT(setXRotation(int)));
    connect(canvas,   SIGNAL(yRotChanged(int)), this, SLOT(setYRotation(int)));
    connect(canvas,   SIGNAL(zRotChanged(int)), this, SLOT(setZRotation(int)));
   
    QGroupBox *rotationGroupBox = new QGroupBox(tr("Rotation"));
    QGridLayout *sliderLayout = new QGridLayout;
    sliderLayout->addWidget(xTitle,  0, 0);
    sliderLayout->addWidget(yTitle,  1, 0);
    sliderLayout->addWidget(zTitle,  2, 0);
    sliderLayout->addWidget(xSlider, 0, 1);
    sliderLayout->addWidget(ySlider, 1, 1);
    sliderLayout->addWidget(zSlider, 2, 1);
    sliderLayout->addWidget(xLabel,  0, 2);
    sliderLayout->addWidget(yLabel,  1, 2);
    sliderLayout->addWidget(zLabel,  2, 2);
    sliderLayout->setColumnMinimumWidth(2,32);
    rotationGroupBox->setLayout(sliderLayout);
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

    QGroupBox *zoomGroupBox = new QGroupBox(tr("Drawing Size"));
    QGridLayout *zoomLayout = new QGridLayout;
    QLabel *zoomTitle = new QLabel(tr("Zoom:"));
    zoomSpinBox = new QSpinBox();
    zoomSpinBox->setRange(0, 500);
    zoomSpinBox->setSuffix("%");
    zoomSpinBox->setAccelerated(true);
    zoomSpinBox->setValue(100);
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
//    QGridLayout *insertSymbolLayout = new QGridLayout;
//    insertSymbolLayout->addWidget(insertAngstromAction, 0, 1);
//    insertSymbolLayout->addWidget(insertDegreeAction, 0, 1);
//    insertSymbolLayout->addWidget(insertPlusMinusAction, 0, 1);
//    insertSymbolBox->setLayout(insertSymbolLayout);
    insertSymbolBox->addAction(insertAngstromAction);
    insertSymbolBox->addAction(insertDegreeAction);
    insertSymbolBox->addAction(insertPlusMinusAction);
    layout->addWidget(insertSymbolBox);
    widget->addAction(insertPlusMinusAction);

    widget->setLayout(layout);
    return widget;	
}


QWidget *MainWindow::createBondsWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    // The bond labels    
    QGroupBox *bondLabelsGroupBox = new QGroupBox(tr("Labels"));
    QGridLayout *bondLabelsLayout = new QGridLayout;
    toggleBondLabelsButton = new QPushButton(tr("Toggle Bond Labels"));
    toggleBondLabelsButton->setToolTip(tr("Toggle the bond length labels of the selected bonds"));
    bondLabelsLayout->addWidget(toggleBondLabelsButton);
    bondLabelsGroupBox->setLayout(bondLabelsLayout);
    layout->addWidget(bondLabelsGroupBox);
    connect(toggleBondLabelsButton, SIGNAL(pressed()),
                canvas, SLOT(toggleBondLabels()));


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
    atomSizeSpinBox->setAccelerated(true);
    atomSizeLayout->addWidget(atomSizeLabel, 0, 0);
    atomSizeLayout->addWidget(atomSizeSpinBox, 0, 1);
    connect(atomSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeAtomSize()));
    atomSizeGroupBox->setLayout(atomSizeLayout);
    layout->addWidget(atomSizeGroupBox);


    QGroupBox *drawingStyleBox 	     = new QGroupBox(tr("Drawing Style"));    
    atomDrawingStyleButtonGroup      = new QButtonGroup;
    QGridLayout *drawingStyleLayout  = new QGridLayout;
    simpleAtomDrawingButton 	     = new QRadioButton(tr("Simple"));
    houkMolAtomDrawingButton 	     = new QRadioButton(tr("HoukMol"));
    simpleColoredAtomDrawingButton 	 = new QRadioButton(tr("Simple Colored"));
    gradientColoredAtomDrawingButton = new QRadioButton(tr("Gradient"));
    simpleAtomDrawingButton->setChecked(true);
    atomDrawingStyleButtonGroup->addButton(simpleAtomDrawingButton, int(Atom::Simple));
    atomDrawingStyleButtonGroup->addButton(simpleColoredAtomDrawingButton, int(Atom::SimpleColored));
    atomDrawingStyleButtonGroup->addButton(houkMolAtomDrawingButton, int(Atom::HoukMol));
    atomDrawingStyleButtonGroup->addButton(gradientColoredAtomDrawingButton, int(Atom::Gradient));
    drawingStyleLayout->addWidget(simpleAtomDrawingButton, 0, 0);
    drawingStyleLayout->addWidget(simpleColoredAtomDrawingButton, 0, 1);
    drawingStyleLayout->addWidget(houkMolAtomDrawingButton, 1, 0);
    drawingStyleLayout->addWidget(gradientColoredAtomDrawingButton, 1, 1);
    connect(atomDrawingStyleButtonGroup, SIGNAL(buttonClicked(int)),
                 canvas, SLOT(setAtomDrawingStyle(int)));
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
    labelStyleLayout->addWidget(atomLabelInput, 1, 0, 1, 2);
    connect(atomLabelInput, SIGNAL(returnPressed()), this, SLOT(setAtomLabels()));
    // The label font
    atomLabelFontCombo = new QFontComboBox();
    atomLabelFontCombo->setEditText(tr("Select Atoms"));
    atomLabelFontCombo->setToolTip(tr("The font for the selected atoms"));
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


QWidget *MainWindow::createAnglesWidget()
{
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;

    //The labels
    QGroupBox *labelsGroupBox = new QGroupBox(tr("Labels"));
    QGridLayout *labelsGroupBoxLayout = new QGridLayout;
    toggleAngleLabelsButton = new QPushButton(tr("Toggle Angle Labels"));
    toggleAngleLabelsButton->setToolTip(tr("Select three or more atoms to toggle the angle markers and labels.  Only angles between bonds will be drawn"));
    connect(toggleAngleLabelsButton, SIGNAL(pressed()),
                    canvas, SLOT(toggleAngleLabels()));
    labelsGroupBoxLayout->addWidget(toggleAngleLabelsButton, 0, 0);
    labelsGroupBox->setLayout(labelsGroupBoxLayout);
    
    layout->addWidget(labelsGroupBox);
    
    widget->setLayout(layout);
    return widget;	
}


QSlider *MainWindow::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-180, 180);
    slider->setSingleStep(1);
    slider->setPageStep(15);
    return slider;		
}
