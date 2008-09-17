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
#include "drawingcanvas.h"
#include "drawinginfo.h"
#include "fileparser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow(FileParser *parser);
   enum FileType {TIFF, PNG, PDF, PostScript, SVG, Unknown};

public slots:
   void setXRotation(int phi);
   void setYRotation(int phi);
   void setZRotation(int phi);

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
	void createToolBox();
	void createActions();
    void createMenus();
    void createToolbars();
    FileType determineFileType(const QString &fileName);
    void saveImage(const QString &fileName);
    void loadFile();
    QIcon textToIcon(const QString &string);

//	QToolButton *makeAtomButton(const char *label);
	QSlider *createSlider();
//	QWidget *createPeriodicTable();
//	QWidget *createBuilderWidget();
	QWidget *createAppearanceWidget();
	QWidget *createAnglesWidget();
	QWidget *createBondsWidget();
	QWidget *createAtomsWidget();
	QWidget *createAnnotationWidget();

//    QWidget *builderWidget;
    QWidget *appearanceWidget;
    QWidget *bondsWidget;
    QWidget *anglesWidget;
    QWidget *atomsWidget;
    QWidget *annotationWidget;

//	QButtonGroup *periodicTableButtonGroup;
    QButtonGroup *mouseModeButtonGroup;
    QButtonGroup *atomDrawingStyleButtonGroup;

    
    QLineEdit *atomLabelInput;
    
    QToolBar *mouseModeToolBar;
    QToolBar *fileToolBar;
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
    
    QSpinBox	   *backgroundOpacitySpinBox;
    QSpinBox       *zoomSpinBox;
    QDoubleSpinBox *atomSizeSpinBox;
    QDoubleSpinBox *bondSizeSpinBox;
    
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;

    QString currentSaveFile;
    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *editMenu;
    QMenu *insertMenu;
    QMenu *insertSymbolMenu;

    QToolBox *toolBox;
    
};

#endif
