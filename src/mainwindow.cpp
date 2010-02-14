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
	
    // Selecting items causes an update of the menus to reflect the current selected items' settings
    connect(canvas, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    createActions();
    createToolBox();
    createMenus();
    createToolbars();

    Atom::fillLabelToVdwRadiusMap();
    Atom::fillLabelToMassMap();
	
    QSettings colorSettings;
    QMap<QString, QVariant> colorMap = colorSettings.value("Default Atom Colors", QVariant(QMap<QString, QVariant>())).toMap();
    if(colorMap.isEmpty())
        Atom::fillLabelToColorMap();
	else
        Atom::labelToColor = colorMap;

    QHBoxLayout *layout = new QHBoxLayout;
    view = new DrawingDisplay(canvas, drawingInfo);
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    view->setGeometry(0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    toolBox->setGeometry(0, 0, static_cast<int>(DEFAULT_TOOLBOX_WIDTH), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
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

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("Recently Opened Files", QVariant(recentlyOpenedFiles));
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
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

QIcon MainWindow::textToIcon(const QString &string)
{
    QSize iconBox(32, 32);
    // Start by drawing the button icon
    QPixmap pixmap(iconBox);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QFont font;
    font.setPointSize(22);
    painter.setFont(font);
    painter.drawText(QRectF(0, 0, 32, 32), Qt::AlignCenter, string);
    return QIcon(pixmap);
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
    if(mouseModeButtonGroup->checkedId() == DrawingCanvas::AddBond){
        view->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::setGeometryStep(int geom)
{
    assert(geom >= 0 && geom < parser->numMolecules());
    parser->setCurrent(geom);
    canvas->storeLabeledBonds();
    canvas->clearAll();
    canvas->loadFromParser();
    canvas->restoreLabeledBonds();
    setWindowTitle(tr("%1 - cheMVP").arg(parser->fileName()));
}

void MainWindow::rotateFromInitialCoordinates()
{
    drawingInfo->setXRot(xRotationBox->text().toInt());
    drawingInfo->setYRot(yRotationBox->text().toInt());
    drawingInfo->setZRot(zRotationBox->text().toInt());
    canvas->rotateFromInitialCoordinates();
}

void MainWindow::setAddArrowMode()
{
    mouseModeButtonGroupClicked((int) DrawingCanvas::AddArrow);
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


void MainWindow::foggingToggled(int useFogging)
{
    drawingInfo->setUseFogging(useFogging);
    useFoggingBox->setEnabled((bool)useFogging);
    useFoggingBox->setVisible(false);
    std::cout<<"I was called!"<<std::endl;
}


void MainWindow::changeBondSize()
{
    if(bondSizeSpinBox->value() == bondSizeSpinBox->minimum()) return;

    if(bondSizeSpinBox->specialValueText().size()) {
        bondSizeSpinBox->setSpecialValueText(tr(""));
        bondSizeSpinBox->setValue(DEFAULT_BOND_THICKNESS);
    }else{
        QGraphicsItem *item;
        foreach(item, canvas->selectedItems()) {
            if(item->type() == Bond::Type) {
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
