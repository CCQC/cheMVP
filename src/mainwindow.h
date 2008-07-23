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
	void updateMenus();
	void setAtomLabels();
	void changeAtomSize();
	void changeBondSize();
//    void periodicTableButtonGroupClicked(QAbstractButton *button);
    void deleteItem();
    void mouseModeButtonGroupClicked(int);
    void openFile();
    void save();
    void saveAs();

private:
	void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    FileType determineFileType(const QString &fileName);
    void saveImage(const QString &fileName);
    void loadFile();

//	QToolButton *makeAtomButton(const char *label);
	QSlider *createSlider();
//	QWidget *createPeriodicTable();
//	QWidget *createBuilderWidget();
	QWidget *createAppearanceWidget();
	QWidget *createAnglesWidget();
	QWidget *createBondsWidget();
	QWidget *createAtomsWidget();

//    QWidget *builderWidget;
    QWidget *appearanceWidget;
    QWidget *bondsWidget;
    QWidget *anglesWidget;
    QWidget *atomsWidget;

//	QButtonGroup *periodicTableButtonGroup;
    QButtonGroup *mouseModeButtonGroup;
    QButtonGroup *atomDrawingStyleButtonGroup;

    QLineEdit *atomLabelInput;
    
    QToolBar *mouseModeToolBar;
    QToolBar *fileToolBar;

    DrawingCanvas *canvas;
    QGraphicsView *view;
    DrawingInfo *drawingInfo;
    FileParser *parser;

    QComboBox *atomLabelFontSizeCombo;
    
    QFontComboBox *atomLabelFontCombo;
    QAction *deleteAction;
    QAction *openAction;
    QAction *exitAction;
    QAction *saveAction;
    QAction *saveAsAction;

    QPushButton *toggleBondLabelsButton;
    QPushButton *toggleBondDashingButton;
    QPushButton *toggleAngleLabelsButton;
    
    QRadioButton *simpleAtomDrawingButton;
    QRadioButton *houkMolAtomDrawingButton;
    QRadioButton *simpleColoredAtomDrawingButton;
    
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

    QToolBox *toolBox;
    
};

#endif
