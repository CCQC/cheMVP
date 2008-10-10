#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QtSvg>
#include <QSvgGenerator>
#include <QToolBox>
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QUndoCommand>
#include <QDebug>
#include <QSettings>
#include "drawingcanvas.h"
#include "drawinginfo.h"
#include "fileparser.h"
#include "undo_delete.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow(FileParser *parser);
   enum FileType {TIFF, PNG, PDF, PostScript, SVG, CVP, Unknown};

public slots:
   void setXRotation(int phi);
   void setYRotation(int phi);
   void setZRotation(int phi);
   void setGeometryStep(int);
   void setCurrentSaveFile(QString file) {currentSaveFile = file;}
   void saveAndExit();

private slots:
	void insertTextAtCursor(QAction *action);
	void updateTextLabelMenus();
	void updateMenus();
	void setAtomLabels();
	void changeAtomSize();
	void changeBondSize();
	void setTextBoxFonts();
//    void periodicTableButtonGroupClicked(QAbstractButton *button);
    void deleteItem();
    void mouseModeButtonGroupClicked(int);
    void openFile();
    void save();
    void saveAs();
    void changeZoom(int);
    void setAddArrowMode();

private:
	void focusOutEvent(QFocusEvent *event);
	void createToolBox();
	void createActions();
    void createMenus();
    void createToolbars();
    void processProjectFile(const QString &fileName, bool saveFile);
    FileType determineFileType(const QString &fileName);
    void saveImage(const QString &fileName);
    void loadFile();
    QIcon textToIcon(const QString &string);

	QSlider *createSlider(int max);
	QWidget *createAppearanceWidget();
	QWidget *createBondsAndAnglesWidget();
	QWidget *createAtomsWidget();
	QWidget *createAnnotationWidget();
    QWidget *createAnimationWidget();
    
    // Not needed. -Jet
    // QWidget *appearanceWidget;
    // QWidget *bondsAndAnglesWidget;
    // QWidget *atomsWidget;
    // QWidget *annotationWidget;
    // QWidget *animationWidget;

    QButtonGroup *mouseModeButtonGroup;
    QButtonGroup *atomDrawingStyleButtonGroup;
    QButtonGroup *atomFontSizeButtonGroup;

    QLineEdit *atomLabelInput;
    
    QToolBar *mouseModeToolBar;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *editSelectedTextToolBar;

    DrawingCanvas *canvas;
    QGraphicsView *view;
    DrawingInfo *drawingInfo;
    FileParser *parser;

    QComboBox *atomLabelFontSizeCombo;
    QComboBox *textFontSizeCombo;
    
    QFontComboBox *atomLabelFontCombo;
    QFontComboBox *textFontCombo;
    
    QActionGroup *insertTextActionGroup;
    QAction *deleteAction;
    QAction *openAction;
    QAction *exitAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *insertAngstromAction;
    QAction *insertDegreeAction;
    QAction *insertPlusMinusAction;
    QAction *addArrowAction;
    QAction *unselectAllAction;
    QAction *selectAllAction;
    QAction *undoAction;
    QAction *redoAction;
    

    QPushButton *toggleBondLabelsButton;
    QPushButton *toggleBondDashingButton;
    QPushButton *toggleAngleLabelsButton;
    QPushButton *toggleAtomNumberSubscriptsButton;
    QPushButton *backgroundColorButton;

    QToolButton *boldTextButton;
    QToolButton *italicTextButton;
    QToolButton *underlineTextButton;
    
    QRadioButton *simpleAtomDrawingButton;
    QRadioButton *houkMolAtomDrawingButton;
    QRadioButton *simpleColoredAtomDrawingButton;
    QRadioButton *gradientColoredAtomDrawingButton;
    QRadioButton *largeLabelAtomDrawingButton;
    QRadioButton *smallLabelAtomDrawingButton;
    
    QSpinBox	   *backgroundOpacitySpinBox;
    QSpinBox       *zoomSpinBox;
    QSpinBox       *bondLabelsPrecisionBox;
    QSpinBox       *angleLabelsPrecisionBox;
    QDoubleSpinBox *atomSizeSpinBox;
    QDoubleSpinBox *bondSizeSpinBox;
    
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;

    QSlider *animationSlider;
    
    QString currentSaveFile;
    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *editMenu;
    QMenu *insertMenu;
    QMenu *insertSymbolMenu;

    QToolBox *toolBox;
    
    QUndoStack *undoStack;
};

#endif
