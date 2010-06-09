#include "mainwindow.h"

void MainWindow::createActions()
{
    undoAction = new QAction(QIcon(":/images/undo.png"),tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setEnabled(false);
    connect(undoAction, SIGNAL(triggered()), undoStack, SLOT(undo()));

    redoAction = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    QList<QKeySequence> redoShortcuts;
    redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
    redoAction->setShortcuts(redoShortcuts);
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), undoStack, SLOT(redo()));

    deleteAction = new QAction(QIcon(":/images/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete selected item(s) from drawing"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
    
    openAction = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAction->setStatusTip(tr("Open a File"));
    openAction->setShortcut(tr("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    exitAction = new QAction(QIcon(":/images/exit.png"),tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Quit ChemVP"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("About CheMVP"), this);
    aboutAction->setStatusTip(tr("About CheMVP"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutCheMVP()));

    showPreferencesAction = new QAction(tr("Preferences"), this);
    connect(showPreferencesAction, SIGNAL(triggered()), this, SLOT(showPreferences()));

    saveAction = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(QIcon(":/images/saveas.png"), tr("&Save As"), this);
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setStatusTip(tr("Save under a new name"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
	
	selectAllAction = new QAction(this);
    selectAllAction->setShortcut(tr("Ctrl+A"));
    selectAllAction->setStatusTip(tr("Select All"));
    selectAllAction->setText(tr("Select All"));

    unselectAllAction = new QAction(this);
    unselectAllAction->setShortcut(tr("Ctrl+Shift+A"));
    unselectAllAction->setStatusTip(tr("Unselect All"));
    unselectAllAction->setText(tr("Unselect All"));

    addArrowAction = new QAction(QIcon(":/images/addarrow.png"), tr("Add Arrow"), this);
    addArrowAction->setStatusTip(tr("Add Arrow"));
    connect(addArrowAction, SIGNAL(triggered(bool)), this, SLOT(setAddArrowMode()));
    
    insertTextActionGroup = new QActionGroup(this);

    insertAngstromAction = new QAction(insertTextActionGroup);
    insertAngstromAction->setIconText(QChar((ushort)0x00C5));
    //  insertAngstromAction->setIcon(textToIcon(QChar((ushort)0x00C5)));
    insertAngstromAction->setToolTip(tr("Insert ")+QChar((ushort)0x00C5)+tr(" at current cursor position"));
    
    insertDegreeAction = new QAction(insertTextActionGroup);
    insertDegreeAction->setIconText(QChar((ushort)0x00B0));
    //  insertDegreeAction->setIcon(textToIcon(QChar((ushort)0x00B0)));
    insertDegreeAction->setToolTip(tr("Insert ")+QChar((ushort)0x00B0)+tr(" at current cursor position"));
    
    insertPlusMinusAction = new QAction(insertTextActionGroup);
    insertPlusMinusAction->setIconText(QChar((ushort)0x00B1));
    //  insertPlusMinusAction->setIcon(textToIcon(QChar((ushort)0x00B1)));
    insertPlusMinusAction->setToolTip(tr("Insert ")+QChar((ushort)0x00B1)+tr(" at current cursor position"));
    
    connect(insertTextActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertTextAtCursor(QAction *)));
}

void MainWindow::deleteItem()
{
    if (canvas->selectedItems().isEmpty())
        return;
    
    // Hide bond labels when they're deleted. - HPS
    // Prevents bond deletion from being undoable,
    // but fixes geometry time step from reinstating deleted bond
    foreach(QGraphicsItem* i, canvas->selectedItems()){
        if(i->type() == Label::BondLabelType){
            foreach(Bond* b, canvas->getBonds()){
                if(b->label() == i)
                    b->toggleLabel();
            }
        }
    }
    
    QUndoCommand *removeItemCommand = new RemoveItemCommand(canvas);
    undoStack->push(removeItemCommand);
}

void MainWindow::aboutCheMVP()
{
    QPixmap pixmap("../images/splash.png");
    SplashScreen* splash = new SplashScreen(pixmap);
    splash->startTimer(2147483647);
}

void MainWindow::showPreferences()
{
    Preferences* prefs = new Preferences(canvas, drawingInfo->getDrawingStyle());
    prefs->exec();
}
