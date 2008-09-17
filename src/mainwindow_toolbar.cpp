#include "mainwindow.h"

void MainWindow::createToolbars()
{
	// The "File" menus
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->addAction(saveAsAction);
    
    // The "Mouse Mode" menus
    mouseModeToolBar = addToolBar(tr("Mouse Mode"));
    mouseModeButtonGroup = new QButtonGroup;
    connect(mouseModeButtonGroup, SIGNAL(buttonClicked(int)),
                 this, SLOT(mouseModeButtonGroupClicked(int)));
    
    QToolButton *selectButton = new QToolButton;
    selectButton->setToolTip(tr("Select / Move"));
    selectButton->setCheckable(true);
    selectButton->setChecked(true);
    selectButton->setIcon(QIcon(":/images/select.png"));
    mouseModeButtonGroup->addButton(selectButton, int(DrawingCanvas::Select));
    mouseModeToolBar->addWidget(selectButton);

    QToolButton *rotateButton = new QToolButton;
    rotateButton->setToolTip(tr("Rotate Molecule"));
    rotateButton->setIcon(QIcon(":/images/rotate.png"));
    rotateButton->setCheckable(true);
    mouseModeButtonGroup->addButton(rotateButton,int(DrawingCanvas::Rotate));
    mouseModeToolBar->addWidget(rotateButton);

    QToolButton *addBondButton = new QToolButton;
    addBondButton->setToolTip(tr("Add Bond"));
    addBondButton->setCheckable(true);
    addBondButton->setIcon(QIcon(":/images/addbond.png"));
    mouseModeButtonGroup->addButton(addBondButton, int(DrawingCanvas::AddBond));
    mouseModeToolBar->addWidget(addBondButton);

    QToolButton *addArrowButton = new QToolButton;
    addArrowButton->setToolTip(tr("Add Arrow"));
    addArrowButton->setCheckable(true);
    addArrowButton->setIcon(QIcon(":/images/addarrow.png"));
    mouseModeButtonGroup->addButton(addArrowButton, int(DrawingCanvas::AddArrow));
    mouseModeToolBar->addWidget(addArrowButton);
    
    QToolButton *addTextButton = new QToolButton;
    addTextButton->setIcon(QIcon(":/images/addtext.png"));
    addTextButton->setToolTip(tr("Add Text"));
    addTextButton->setCheckable(true);
    mouseModeButtonGroup->addButton(addTextButton, int(DrawingCanvas::AddText));
    mouseModeToolBar->addWidget(addTextButton);
    
    connect(canvas, SIGNAL(mouseModeChanged(int)), this, SLOT(mouseModeButtonGroupClicked(int)));
    
    // The text settings
    editSelectedTextToolBar = addToolBar("Edit Selected Text");
    // Bold
    boldTextButton = new QToolButton;
    boldTextButton->setIcon(QIcon(":/images/boldtext.png"));
    boldTextButton->setToolTip(tr("Bold text"));
    boldTextButton->setCheckable(true);
    editSelectedTextToolBar->addWidget(boldTextButton);
    connect(boldTextButton, SIGNAL(clicked(bool)), this, SLOT(setTextBoxFonts()));
    // Italic
    italicTextButton = new QToolButton;
    italicTextButton->setIcon(QIcon(":/images/italictext.png"));
    italicTextButton->setToolTip(tr("Italic text"));
    italicTextButton->setCheckable(true);
    editSelectedTextToolBar->addWidget(italicTextButton);
    connect(italicTextButton, SIGNAL(clicked(bool)), this, SLOT(setTextBoxFonts()));
    // Underline
    underlineTextButton = new QToolButton;
    underlineTextButton->setIcon(QIcon(":/images/underlinetext.png"));
    underlineTextButton->setToolTip(tr("Underline text"));
    underlineTextButton->setCheckable(true);
    editSelectedTextToolBar->addWidget(underlineTextButton);
    connect(underlineTextButton, SIGNAL(clicked(bool)), this, SLOT(setTextBoxFonts()));
    // Font
    textFontCombo = new QFontComboBox();
//    textFontCombo->setFontFilters(QFontComboBox::MonospacedFonts);
    textFontCombo->setEditText(tr("Select Labels"));
    textFontCombo->setToolTip(tr("The font for the selected atoms"));
    textFontCombo->setFocusPolicy(Qt::ClickFocus);
    editSelectedTextToolBar->addWidget(textFontCombo);
    connect(textFontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(setTextBoxFonts()));
    // The label font size
    textFontSizeCombo = new QComboBox;
    textFontSizeCombo->setFocusPolicy(Qt::ClickFocus);
    textFontSizeCombo->setEditable(true);
    textFontSizeCombo->setToolTip(tr("The font size for the selected text"));
    for (int i = 4; i < 20; ++i){
      textFontSizeCombo->addItem(QString().setNum(i));
    }
    textFontSizeCombo->setEditText(tr(""));
    editSelectedTextToolBar->addWidget(textFontSizeCombo);
    connect(textFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(setTextBoxFonts()));
}
