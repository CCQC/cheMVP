#include <QtGui>

#include <iostream>
#include <iomanip>
#include "mainwindow.h"
#include "drawingdisplay.h"
#include "drawingcanvas.h"
#include "defines.h"


MainWindow::MainWindow(FileParser *parser_in):
	parser(parser_in)
{
    undoStack = new QUndoStack();
    drawingInfo = new DrawingInfo();
    canvas = new DrawingCanvas(itemMenu, drawingInfo, parser);
	// Selecting items causes an update of the menus to reflect the current
	// selected items' settings
    connect(canvas, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

//    setCursor(canvas->selectCursor());
    
    createActions();
    createToolBox();
    createMenus();
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    view = new DrawingDisplay(canvas, drawingInfo);
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    view->setGeometry(0, 0, DEFAULT_SCENE_SIZE_X, DEFAULT_SCENE_SIZE_Y);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    toolBox->setGeometry(0, 0, DEFAULT_TOOLBOX_WIDTH, DEFAULT_SCENE_SIZE_Y);
    toolBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    toolBox->setCurrentIndex(toolBox->indexOf(appearanceWidget));

	drawingInfo->setHeight(view->sceneRect().height());
	drawingInfo->setWidth(view->sceneRect().width());
	drawingInfo->determineScaleFactor();
	canvas->setSceneRect(view->sceneRect());
	canvas->refresh();
	
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(view);
    splitter->addWidget(toolBox);
    layout->addWidget(splitter);
    
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    loadFile();

    // The undo/redo framework needs to update the buttons appropriately
    connect(undoStack, SIGNAL(canRedoChanged(bool)),
            redoAction, SLOT(setEnabled(bool)));
    connect(undoStack, SIGNAL(canUndoChanged(bool)),
            undoAction, SLOT(setEnabled(bool)));
    
}


//void MainWindow::periodicTableButtonGroupClicked(QAbstractButton *button)
//{
//    canvas->setElementToAdd(button->text());
//    canvas->setMode(DrawingCanvas::InsertAtom);
//    canvas->update();
//    view->update();
//}

void MainWindow::insertTextAtCursor(QAction *action)
{
	foreach(QGraphicsItem* item,canvas->items()){
		if(ITEM_IS_LABEL){
			Label *label = dynamic_cast<Label*>(item);
			if(label->textInteractionFlags() & Qt::TextEditorInteraction){
				QTextCursor cursor = label->textCursor();
				cursor.insertText(action->iconText());
			}
		}
	}	
	update();
}


void MainWindow::setTextBoxFonts()
{
	// Start by looking to see if one of the labels is being edited
	bool hasLabel = false;
	Label *labelBeingEdited = 0;
	foreach(QGraphicsItem* item, canvas->items()){
		if(ITEM_IS_LABEL){
			hasLabel = true;
			Label *label = dynamic_cast<Label*>(item);
			if(label->textInteractionFlags() & Qt::TextEditorInteraction){
				labelBeingEdited = label;
				break;
			}
		}
	}
	
	// Nothing to edit - bail now
	if(!hasLabel) return;
	
	if(labelBeingEdited!=0){
		// One of the labels is being edited - operate only on this selection
		QTextCursor cursor = labelBeingEdited->textCursor();
		QTextCharFormat format(cursor.blockCharFormat());
		format.setFontWeight((boldTextButton->isChecked() ? QFont::Bold : QFont::Normal));
		format.setFontItalic(italicTextButton->isChecked());
		format.setFontUnderline((underlineTextButton->isChecked() ?
				                 QTextCharFormat::SingleUnderline : QTextCharFormat::NoUnderline));
		format.setFontFamily(textFontCombo->currentFont().family());
	    cursor.mergeCharFormat(format);
	    // TODO This will change the entire label's size.  Think about allowing different sizes within labels.
		if(textFontSizeCombo->currentText().size()){
			labelBeingEdited->setFontSize(textFontSizeCombo->currentText().toInt());
		}
	}else{
		// No editor - operate on selected text boxes
		foreach(QGraphicsItem* item,canvas->items()){
			if(ITEM_IS_LABEL){
				Label *label = dynamic_cast<Label*>(item);	
				if(label->isSelected()){
					QFont myFont(label->font());
					myFont.setUnderline(underlineTextButton->isChecked());
					myFont.setItalic(italicTextButton->isChecked());
					myFont.setWeight((boldTextButton->isChecked() ? QFont::Bold : QFont::Normal));
					myFont.setFamily(textFontCombo->currentFont().family());
					if(textFontSizeCombo->currentText().size()){
						label->setFontSize(textFontSizeCombo->currentText().toInt());
					}
					label->setFont(myFont);
				}
			}
		}		
	}
	drawingInfo->determineScaleFactor();
	canvas->refresh();
}


void MainWindow::openFile()
{
     QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
   	 parser->setFileName(fileName);
     loadFile();
}


void MainWindow::loadFile()
{
	
	if (!parser->fileName().isEmpty()) {
	    parser->readFile();
	    canvas->clearAll();
	    canvas->loadFromParser();
	    setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));
	}
}


void MainWindow::deleteItem()
{
	 if (canvas->selectedItems().isEmpty())
	     return;
	
	 QUndoCommand *removeItemCommand = new RemoveItemCommand(canvas);
	 undoStack->push(removeItemCommand);
}


void MainWindow::mouseModeButtonGroupClicked(int buttonID)
{
	// This is just in case the mode was changed automatically by the canvas
	foreach(QAbstractButton *button, mouseModeButtonGroup->buttons()){
		if(mouseModeButtonGroup->id(button) == buttonID){
			button->setChecked(true);
		}
	}
	
    canvas->setMode(DrawingCanvas::Mode(mouseModeButtonGroup->checkedId()));
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Rotate){
    	canvas->setAcceptsHovers(false);
    }else{
    	canvas->setAcceptsHovers(true);
    }
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Rotate){
    	QApplication::setOverrideCursor(canvas->rotateCursor());
    }
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Select){
    	QApplication::restoreOverrideCursor();
    }
}


void MainWindow::setXRotation(int phi)
{
	xLabel->setText(QString::number(phi));
	xSlider->setValue(phi);
	drawingInfo->setXRot(phi);
	canvas->refresh();
}


void MainWindow::setYRotation(int phi)
{
	yLabel->setText(QString::number(phi));	
	ySlider->setValue(phi);
	drawingInfo->setYRot(phi);
	canvas->refresh();
}


void MainWindow::setZRotation(int phi)
{
	zLabel->setText(QString::number(phi));	
	zSlider->setValue(phi);
	drawingInfo->setZRot(phi);
	canvas->refresh();
}


void MainWindow::setAddArrowMode()
{
  mouseModeButtonGroupClicked((int) DrawingCanvas::AddArrow);
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(selectAllAction);
    editMenu->addAction(unselectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    
    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    
    
    insertMenu = menuBar()->addMenu(tr("&Insert"));
    insertMenu->addAction(addArrowAction);
    //insertMenu->addAction(addBondAction);
    insertMenu->addSeparator();
    insertSymbolMenu = insertMenu->addMenu(tr("Special Symbol"));
    insertSymbolMenu->addAction(insertAngstromAction);
    insertSymbolMenu->addAction(insertDegreeAction);
    insertSymbolMenu->addAction(insertPlusMinusAction);
    
}




void MainWindow::setAtomLabels()
{
	canvas->setAtomLabels(atomLabelInput->text());
}


//QToolButton *MainWindow::makeAtomButton(const char *label)
//{
//    int dimension1 = 14;
//    int dimension2 = 12;
//	
//    QSize icon_box(dimension1, dimension2);
//	// Start by drawing the button icon
//	QPixmap icon(icon_box);
//    icon.fill(Qt::transparent);
//    QPainter painter(&icon);
//    QString text(label);
//	QFont font;
//	font.setPointSize(12);
//	painter.setFont(font);
//	painter.drawText(QRectF(0, 0, dimension1, dimension2), Qt::AlignCenter, text); 
//	
//	// Now make the button and set it up
//	QToolButton *button = new QToolButton;
//	button->setIcon(icon);
//	button->setText(text);
//    button->setIconSize(icon_box);
//    button->setCheckable(true);
//    periodicTableButtonGroup->addButton(button);
//    
//    return button;
//}
//

//QWidget *MainWindow::createPeriodicTable()
//{
//    QGridLayout *layout = new QGridLayout;
//    layout->setSpacing(0);
//    
//    layout->addWidget(makeAtomButton("H" ), 0, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("He"), 0, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Li"), 1, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Be"), 1, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("B" ), 1, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("C" ), 1, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("N" ), 1, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("O" ), 1, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("F" ), 1, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ne"), 1, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Na"), 2, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mg"), 2, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Al"), 2, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Si"), 2, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("P" ), 2, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("S" ), 2, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cl"), 2, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ar"), 2, 9, Qt::AlignHCenter);
//    
//    layout->addWidget(makeAtomButton("K" ), 3, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ca"), 3, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ga"), 3, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ge"), 3, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("As"), 3, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Se"), 3, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Br"), 3, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Kr"), 3, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Rb"), 4, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sr"), 4, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("In"), 4, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sn"), 4, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sb"), 4, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Te"), 4, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("I" ), 4, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Xe"), 4, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Cs"), 5, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ba"), 5, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tl"), 5, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pb"), 5, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Bi"), 5, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Po"), 5, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("At"), 5, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Rn"), 5, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Fr"), 6, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ra"), 6, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ac"), 6, 1, Qt::AlignHCenter);
//
//    layout->addItem(new QSpacerItem(10,10), 7, 1);
//
//    layout->addWidget(makeAtomButton("Sc"), 8, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ti"), 8, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("V" ), 8, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cr"), 8, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mn"), 8, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Fe"), 8, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Co"), 8, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ni"), 8, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cu"), 8, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Zn"), 8, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Y" ), 9, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Zr"), 9, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Nb"), 9, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Mo"), 9, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tc"), 9, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ru"), 9, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Rh"), 9, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pd"), 9, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ag"), 9, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cd"), 9, 9, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("La"), 10, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Hf"), 10, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ta"), 10, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("W" ), 10, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Re"), 10, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Os"), 10, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ir"), 10, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pt"), 10, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Au"), 10, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Hg"), 10, 9, Qt::AlignHCenter);
    
//    layout->addItem(new QSpacerItem(10,10), 11, 1);
//
//    layout->addWidget(makeAtomButton("Ce"), 12, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pr"), 12, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Nd"), 12, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pm"), 12, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Sm"), 12, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Eu"), 12, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Gd"), 12, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tb"), 12, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Dy"), 12, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Ho"), 12, 9, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Er"), 12, 10, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Tm"), 12, 11, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Yb"), 12, 12, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Lu"), 12, 13, Qt::AlignHCenter);
//
//    layout->addWidget(makeAtomButton("Th"), 13, 0, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pa"), 13, 1, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("U" ), 13, 2, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Np"), 13, 3, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Pu"), 13, 4, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Am"), 13, 5, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cm"), 13, 6, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Bk"), 13, 7, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Cf"), 13, 8, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Es"), 13, 9, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Fm"), 13, 10, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Md"), 13, 11, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("No"), 13, 12, Qt::AlignHCenter);
//    layout->addWidget(makeAtomButton("Lr"), 13, 13, Qt::AlignHCenter);

//    QWidget *widget = new QWidget;
//    widget->setLayout(layout);
//
//    return widget;
//	
//}




void MainWindow::changeAtomSize()
{
	if(atomSizeSpinBox->value() == atomSizeSpinBox->minimum()) return;

	if(atomSizeSpinBox->specialValueText().size()){
		atomSizeSpinBox->setSpecialValueText(tr(""));
		atomSizeSpinBox->setValue(DEFAULT_ATOM_SCALE_FACTOR);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas->selectedItems()){
			if(item->type() == Atom::Type){
				Atom *atom = dynamic_cast<Atom*>(item);
				atom->setScaleFactor(atomSizeSpinBox->value()); 
			}
		}
		canvas->refresh();
	}	
}


void MainWindow::changeBondSize()
{
	if(bondSizeSpinBox->value() == bondSizeSpinBox->minimum()) return;

	if(bondSizeSpinBox->specialValueText().size()){
		bondSizeSpinBox->setSpecialValueText(tr(""));
		bondSizeSpinBox->setValue(DEFAULT_BOND_THICKNESS);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas->selectedItems()){
			if(item->type() == Bond::Type){
				Bond *bond = dynamic_cast<Bond*>(item);
				bond->setThickness(bondSizeSpinBox->value()); 
			}
		}
		canvas->refresh();
	}	
}

void MainWindow::changeZoom(int val)
{
    drawingInfo->setZoom(val);
    canvas->refresh();
}
