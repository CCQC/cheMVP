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
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    exitAction = new QAction(QIcon(":/images/exit.png"),tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Quit ChemVP"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

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
    connect(selectAllAction, SIGNAL(triggered()), canvas, SLOT(selectAll()));

    unselectAllAction = new QAction(this);
    unselectAllAction->setShortcut(tr("Ctrl+Shift+A"));
    unselectAllAction->setStatusTip(tr("Unselect All"));
    unselectAllAction->setText(tr("Unselect All"));
    connect(unselectAllAction, SIGNAL(triggered()), canvas, SLOT(unselectAll()));

    addArrowAction = new QAction(QIcon(":/images/addarrow.png"), tr("Add Arrow"), this);
    addArrowAction->setStatusTip(tr("Add Arrow"));
    connect(addArrowAction, SIGNAL(triggered(bool)), this, SLOT(setAddArrowMode()));
    
    insertTextActionGroup = new QActionGroup(this);

    insertAngstromAction = new QAction(insertTextActionGroup);
    insertAngstromAction->setIconText(QChar((ushort)0x00C5));
    insertAngstromAction->setIcon(textToIcon(QChar((ushort)0x00C5)));
    insertAngstromAction->setToolTip(tr("Insert ")+QChar((ushort)0x00C5)+tr(" at current cursor position"));
    
    insertDegreeAction = new QAction(insertTextActionGroup);
    insertDegreeAction->setIconText(QChar((ushort)0x00B0));
    insertDegreeAction->setIcon(textToIcon(QChar((ushort)0x00B0)));
    insertDegreeAction->setToolTip(tr("Insert ")+QChar((ushort)0x00B0)+tr(" at current cursor position"));
    
    insertPlusMinusAction = new QAction(insertTextActionGroup);
    insertPlusMinusAction->setIconText(QChar((ushort)0x00B1));
    insertPlusMinusAction->setIcon(textToIcon(QChar((ushort)0x00B1)));
    insertPlusMinusAction->setToolTip(tr("Insert ")+QChar((ushort)0x00B1)+tr(" at current cursor position"));
    
    connect(insertTextActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertTextAtCursor(QAction *)));

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