#include <QtGui>

#include <iostream>
#include <iomanip>
#include <assert.h>
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
    connect(undoStack, SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)));
    connect(undoStack, SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)));
    
}


void MainWindow::saveAndExit()
{
	if(currentSaveFile.size()){
		std::cout<<"Saving file..."<<currentSaveFile.toStdString()<<std::endl;
		save();
		exit(0);
	}
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
	std::cout<<"Main window focus out"<<std::endl;
	if(event->reason() != Qt::TabFocusReason){
		QMainWindow::focusOutEvent(event);
	}else{
	    QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
	    QMainWindow::keyPressEvent(newEvent);
	}
}


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
        animationSlider->setRange(0, parser->numMolecules() - 1);
        animationSlider->setValue(parser->current());
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
    	view->setCursor(canvas->rotateCursor());
    }
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::AddText){
    	view->setCursor(Qt::IBeamCursor);
    }
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Select){
    	view->setCursor(Qt::ArrowCursor);
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

void MainWindow::setGeometryStep(int geom)
{
    assert(geom >= 0 && geom < parser->numMolecules());
    parser->setCurrent(geom);
    canvas->clearAll();
    canvas->loadFromParser();
    setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));
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
