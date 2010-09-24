#include "mainwindow.h"

void MainWindow::createToolbars()
{
	// The "File" menus
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);
	fileToolBar->addAction(saveAsAction);

	// The "Edit" menus
	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(undoAction);
	editToolBar->addAction(redoAction);

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
	rotateButton->setToolTip(tr("Rotate Molecule\nClick and drag to rotate about x and y\nHold shift, click and drag to rotate about z"));
	rotateButton->setIcon(QIcon(":/images/rotate.png"));
	rotateButton->setCheckable(true);
	mouseModeButtonGroup->addButton(rotateButton,int(DrawingCanvas::Rotate));
	mouseModeToolBar->addWidget(rotateButton);

	QToolButton *addBondButton = new QToolButton;
	addBondButton->setToolTip(tr("Add Bond\nClick an atom, (keep the button held down) and release on another atom to form a new bond"));
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

	// The text settings
	editSelectedTextToolBar = addToolBar("Edit Selected Text");
	// Bold
	boldTextButton = new QToolButton;
	boldTextButton->setIcon(QIcon(":/images/boldtext.png"));
	boldTextButton->setToolTip(tr("Bold text"));
	boldTextButton->setCheckable(true);
	editSelectedTextToolBar->addWidget(boldTextButton);
	// Italic
	italicTextButton = new QToolButton;
	italicTextButton->setIcon(QIcon(":/images/italictext.png"));
	italicTextButton->setToolTip(tr("Italic text"));
	italicTextButton->setCheckable(true);
	editSelectedTextToolBar->addWidget(italicTextButton);
	// Underline
	underlineTextButton = new QToolButton;
	underlineTextButton->setIcon(QIcon(":/images/underlinetext.png"));
	underlineTextButton->setToolTip(tr("Underline text"));
	underlineTextButton->setCheckable(true);
	editSelectedTextToolBar->addWidget(underlineTextButton);
	// Font
	textFontCombo = new QFontComboBox();
//  textFontCombo->setFontFilters(QFontComboBox::MonospacedFonts);
	textFontCombo->setCurrentFont(QFont(DEFAULT_LABEL_FONT));
	textFontCombo->setToolTip(tr("The font for the selected atoms"));
	textFontCombo->setFocusPolicy(Qt::ClickFocus);
	editSelectedTextToolBar->addWidget(textFontCombo);
	// The label font size
	textFontSizeCombo = new QComboBox;
	textFontSizeCombo->setFocusPolicy(Qt::ClickFocus);
	textFontSizeCombo->setEditable(true);
	textFontSizeCombo->setToolTip(tr("The font size for the selected text"));
	for (int i = 4; i < 20; ++i)
		textFontSizeCombo->addItem(QString().setNum(i));
	textFontSizeCombo->setCurrentIndex(textFontSizeCombo->findText(QString().setNum(DEFAULT_LABEL_FONT_SIZE)));
	editSelectedTextToolBar->addWidget(textFontSizeCombo);

	textFontCombo->setEnabled(false);
	textFontSizeCombo->setEnabled(false);
	boldTextButton->setEnabled(false);
	italicTextButton->setEnabled(false);
	underlineTextButton->setEnabled(false);
	mouseModeToolBar->setEnabled(false);
	enableLabelSignals();
}

void MainWindow::disableLabelSignals()
{
	disconnect(boldTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelBoldness(bool)));
	disconnect(italicTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelItalics(bool)));
	disconnect(underlineTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelUnderline(bool)));
	disconnect(textFontCombo, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(setLabelFont(QFont)));
	disconnect(textFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),this, SLOT(setLabelFontSize(QString)));
}

void MainWindow::enableLabelSignals()
{
	connect(boldTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelBoldness(bool)));
	connect(italicTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelItalics(bool)));
	connect(underlineTextButton, SIGNAL(toggled(bool)), this, SLOT(setLabelUnderline(bool)));
	connect(textFontCombo, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(setLabelFont(QFont)));
	connect(textFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),this, SLOT(setLabelFontSize(QString)));
}

// TODO deactivate bond and angle buttons unless selection is appropriate
void MainWindow::updateTextLabelToolbar()
{
	// This is just like the update menus function, but is called seperately, as it should be
	// called every time the selected text changes, but the atoms, bonds, etc. might not have changed
	// in this case, so we don't want to process them.
	// Start by looking to see if one of the labels is being edited

	disableLabelSignals();

	int selectedLabels = 0;
	foreach(QGraphicsItem* item, canvas->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				selectedLabels++;
		}
	}

	// No labels are selected
	if(selectedLabels == 0)
	{
		textFontCombo->setCurrentFont(QFont(DEFAULT_LABEL_FONT));
		textFontSizeCombo->setCurrentIndex(textFontSizeCombo->findText(QString().setNum(DEFAULT_LABEL_FONT_SIZE)));
		boldTextButton->setChecked(false);
		italicTextButton->setChecked(false);
		underlineTextButton->setChecked(false);
		textFontCombo->setEnabled(false);
		textFontSizeCombo->setEnabled(false);
		boldTextButton->setEnabled(false);
		italicTextButton->setEnabled(false);
		underlineTextButton->setEnabled(false);
		return;
	}
	else if(selectedLabels == 1)
	{
		textFontCombo->setEnabled(true);
		textFontSizeCombo->setEnabled(true);
		boldTextButton->setEnabled(true);
		italicTextButton->setEnabled(true);
		underlineTextButton->setEnabled(true);
		bool found = false;

		foreach(QGraphicsItem* item, canvas->items())
		{
			if(ITEM_IS_LABEL)
			{
				Label* label = dynamic_cast<Label*>(item);
				if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				{
					found = true;
					QFont currentFont = label->getCurrentFont();
					textFontCombo->setCurrentFont(currentFont.family());
					textFontSizeCombo->setCurrentIndex(textFontSizeCombo->findText(QString().setNum(currentFont.pointSize())));
					boldTextButton->setChecked(currentFont.bold());
					italicTextButton->setChecked(currentFont.italic());
					underlineTextButton->setChecked(currentFont.underline());
				}
			}
			if(found)
				break;
		}
	}
	else
	{
		textFontCombo->setEnabled(true);
		textFontSizeCombo->setEnabled(true);
		boldTextButton->setEnabled(true);
		italicTextButton->setEnabled(true);
		underlineTextButton->setEnabled(true);

		// Labels are just selected, but not being edited, process the settings
		QList<int> textLabelFontSizes;
		QList<QString> textLabelFonts;
		QList<bool> textLabelsUnderlined;
		QList<bool> textLabelsItalic;
		QList<bool> textLabelsBold;
		foreach(QGraphicsItem* item,canvas->items()){
			if(item->isSelected()){
				if(ITEM_IS_LABEL){
					Label *label = dynamic_cast<Label*>(item);
					QFont font = label->getCurrentFont();
					textLabelFontSizes.append(font.pointSize());
					textLabelFonts.append(font.family());
					textLabelsUnderlined.append(font.underline());
					textLabelsBold.append(font.bold());
					textLabelsItalic.append(font.italic());
				}
			}
		}
		bool sameValues;

		// Process the fonts
		sameValues = true;
		foreach(QString i, textLabelFonts){
			if(i != textLabelFonts[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			textFontCombo->setEditText(textLabelFonts[0]);
		}else{
			textFontCombo->setEditText(tr(""));
		}

		// Process the font sizes
		sameValues = true;
		foreach(int i, textLabelFontSizes){
			if(i != textLabelFontSizes[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			textFontSizeCombo->setEditText(QString().setNum(textLabelFontSizes[0]));
		}else{
			textFontSizeCombo->setEditText(tr(""));
		}

		// Process the font boldness
		sameValues = true;
		foreach(bool i, textLabelsBold){
			if(i != textLabelsBold[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			boldTextButton->setChecked(false);
		}else{
			boldTextButton->setChecked(textLabelsBold[0]);
		}

		// Process the font italics
		sameValues = true;
		foreach(bool i, textLabelsItalic){
			if(i != textLabelsItalic[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			italicTextButton->setChecked(false);
		}else{
			italicTextButton->setChecked(textLabelsItalic[0]);
		}

		// Process the font underline
		sameValues = true;
		foreach(bool i, textLabelsUnderlined){
			if(i != textLabelsUnderlined[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			underlineTextButton->setChecked(false);
		}else{
			underlineTextButton->setChecked(textLabelsUnderlined[0]);
		}
	}

	enableLabelSignals();
}

void MainWindow::activateToolBar()
{
	editMenu->setEnabled(true);
	itemMenu->setEnabled(true);
	insertMenu->setEnabled(true);
	mouseModeToolBar->setEnabled(true);

	deleteAction->setEnabled(true);
	saveAction->setEnabled(true);
	saveAsAction->setEnabled(true);
	selectAllAction->setEnabled(true);
	unselectAllAction->setEnabled(true);
	addArrowAction->setEnabled(true);
	insertTextActionGroup->setEnabled(true);

	if(undoStack->count() > 0)
		undoStack->clear();
}
