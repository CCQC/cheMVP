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
#include <QMap>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "splashscreen.h"
#include "drawingcanvas.h"
#include "drawingdisplay.h"
#include "drawinginfo.h"
#include "fileparser.h"
#include "undo_delete.h"
#include "preferences.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(FileParser *parser);
	~MainWindow();
	enum FileType {TIFF, PNG, PDF, PostScript, SVG, CHMVP, Unknown};

public slots:
	void setGeometryStep(int);
	void setCurrentSaveFile(QString file) {currentSaveFile = file;}
	void saveAndExit();

private slots:
	void insertTextAtCursor(QAction *action);
	void updateTextLabelToolbar();
	void updateMenus();
	void setAtomLabels();
	void changeAtomSize();
	void changeBondSize();
	void setLabelBoldness(bool b);
	void setLabelItalics(bool b);
	void setLabelUnderline(bool b);
	void setLabelFont(QFont f);
	void setLabelFontSize(QString s);
	void deleteItem();
	void mouseModeButtonGroupClicked(int);
	void rotateFromInitialCoordinates();
	void openFile();
	void save();
	void saveAs();
	void saveProject(QString filename);
	void openProject(QString filename);
	void changeZoom(int);
	void setAddArrowMode();
	void aboutCheMVP();
	void showPreferences();
	void openRecentFile();

private:
	void focusOutEvent(QFocusEvent *event);
	void createToolBox(int width = DEFAULT_TOOLBOX_WIDTH, int height = DEFAULT_SCENE_SIZE_Y);
	QMap<QString, QString>* defaultToolBoxOptions();
	void createActions();
	void createMenus();
	void createToolbars();
	void updateRecentFiles();
	FileType determineFileType(const QString &fileName);
	void saveImage(const QString &fileName);
	void foggingToggled(int useFogging);
	void loadFile();
	void resetSignalsOnFileLoad();
	void resetButtonsOnFileLoad(bool project);
	QIcon textToIcon(const QString &string);
	void mouseReleaseEvent(QMouseEvent *);
	void disableLabelSignals();
	void enableLabelSignals();
	void resetToolBox(QMap<QString, QString>* options);
	void activateToolBar();

	QWidget *createAppearanceWidget(QMap<QString, QString>* options);
	QWidget *createBondsAndAnglesWidget(QMap<QString, QString>* options);
	QWidget *createAtomsWidget(QMap<QString, QString>* options);
	QWidget *createAnnotationWidget(QMap<QString, QString>* options);
	QWidget *createAnimationWidget(QMap<QString, QString>* options);
	QToolButton *makeAtomButton(const char *);
	QSlider *createSlider(int max);
	QCheckBox *useFoggingBox;
	QLabel* foggingLabel;

	QWidget *animationWidget;

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
	QSplitter* splitter;
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
	QAction *aboutAction;
	QAction *showPreferencesAction;

	QPushButton *toggleBondLabelsButton;
	QPushButton *toggleBondDashingButton;
	QPushButton *toggleAngleLabelsButton;
	QPushButton *toggleAtomNumberSubscriptsButton;
	QPushButton *backgroundColorButton;
	QPushButton *atomColorButton;

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
	QSpinBox       *foggingScaleBox;
	QDoubleSpinBox *atomSizeSpinBox;
	QDoubleSpinBox *bondSizeSpinBox;

	QLineEdit *xRotationBox;
	QLineEdit *yRotationBox;
	QLineEdit *zRotationBox;

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

	QList<QString> recentlyOpenedFiles;
	QList<QAction*> recentFileActions;
	QAction* separatorAction;
};

#endif
