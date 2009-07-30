#include "preferences.h"

Preferences::Preferences(DrawingCanvas* d, int s)
{
    _canvas = d;
    _drawingStyle = s;
    _colorChanges = Atom::labelToColor;

    _listWidget = new QListWidget();
    _listWidget->addItem(tr("Atom Colors"));
    _listWidget->setFixedWidth(100);

    _periodicTable = createPeriodicTable();

    _applyButton = new QPushButton(tr("Apply"));
    _closeButton = new QPushButton(tr("Close"));
    _revertButton = new QPushButton(tr("Restore to Defaults"));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(_revertButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(_applyButton);
    buttonsLayout->addWidget(_closeButton);

    connect(_revertButton, SIGNAL(pressed()),
            this, SLOT(restoreDefaults()));
    connect(_closeButton, SIGNAL(pressed()),
            this, SLOT(close()));
    connect(_closeButton, SIGNAL(pressed()),
            this, SLOT(revert()));
    connect(_applyButton, SIGNAL(pressed()),
            this, SLOT(savePreferences()));
    
    _stackedLayout = new QStackedLayout();
    _stackedLayout->addWidget(_periodicTable);

    connect(_listWidget, SIGNAL(currentRowChanged(int)),
            _stackedLayout, SLOT(setCurrentIndex(int)));

    _listWidget->setCurrentRow(0);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(_listWidget);
    horizontalLayout->addLayout(_stackedLayout);

    QVBoxLayout *periodicTablePageLayout = new QVBoxLayout;
    periodicTablePageLayout->addLayout(horizontalLayout);
    periodicTablePageLayout->addLayout(buttonsLayout);
    setLayout(periodicTablePageLayout);

    setWindowTitle(tr("Preferences"));
}

void Preferences::revert()
{
    if(_listWidget->currentRow() == 0){
        _colorChanges = Atom::labelToColor;
        _canvas->updateAtomColors(_colorChanges);    
    }
}

void Preferences::restoreDefaults()
{
    if(_listWidget->currentRow() == 0){
        // Take temp Map out if 'Restore to Defualts" autmoatically writes settings.
        QMap<QString,QVariant> temp;
        temp = Atom::labelToColor;
        Atom::fillLabelToColorMap();
        _colorChanges = Atom::labelToColor;
        Atom::labelToColor = temp;
        _canvas->updateAtomColors(_colorChanges);        
    }
}

void Preferences::savePreferences()
{
    Atom::labelToColor = _colorChanges;
    foreach(Atom * a, _canvas->getAtoms()) {
        a->setDrawingStyle(DrawingInfo::DrawingStyle(_drawingStyle));
    }
    QSettings settings("Andy", "cheMVP");
    settings.setValue("Default Atom Colors", Atom::labelToColor);
}

QToolButton *Preferences::makeAtomButton(const char *label)
{	
    return new AtomButton(_canvas, label);
}

QWidget *Preferences::createPeriodicTable()
{	
    QGridLayout *layout = new QGridLayout;
    layout->setSpacing(0);
    
    layout->addWidget(makeAtomButton("H" ), 0, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("He"), 0, 18, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Li"), 1, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Be"), 1, 1, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("B" ), 1, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("C" ), 1, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("N" ), 1, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("O" ), 1, 16, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("F" ), 1, 17, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ne"), 1, 18, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Na"), 2, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Mg"), 2, 1, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Al"), 2, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Si"), 2, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("P" ), 2, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("S" ), 2, 16, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cl"), 2, 17, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ar"), 2, 18, Qt::AlignHCenter);
    
    layout->addWidget(makeAtomButton("K" ), 3, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ca"), 3, 1, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Sc"), 3, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ti"), 3, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("V" ), 3, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cr"), 3, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Mn"), 3, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Fe"), 3, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Co"), 3, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ni"), 3, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cu"), 3, 11, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Zn"), 3, 12, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Ga"), 3, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ge"), 3, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("As"), 3, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Se"), 3, 16, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Br"), 3, 17, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Kr"), 3, 18, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Rb"), 4, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Sr"), 4, 1, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Y" ), 4, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Zr"), 4, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Nb"), 4, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Mo"), 4, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Tc"), 4, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ru"), 4, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Rh"), 4, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pd"), 4, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ag"), 4, 11, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cd"), 4, 12, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("In"), 4, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Sn"), 4, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Sb"), 4, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Te"), 4, 16, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("I" ), 4, 17, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Xe"), 4, 18, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Cs"), 5, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ba"), 5, 1, Qt::AlignHCenter);

    layout->addItem(new QSpacerItem(10,10), 5, 2);

    layout->addWidget(makeAtomButton("Lu"), 5, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Hf"), 5, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ta"), 5, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("W" ), 5, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Re"), 5, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Os"), 5, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ir"), 5, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pt"), 5, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Au"), 5, 11, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Hg"), 5, 12, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Tl"), 5, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pb"), 5, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Bi"), 5, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Po"), 5, 16, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("At"), 5, 17, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Rn"), 5, 18, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Fr"), 6, 0, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ra"), 6, 1, Qt::AlignHCenter);

    layout->addItem(new QSpacerItem(10,10), 6, 2);

    layout->addWidget(makeAtomButton("Lr"), 6, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Rf"), 6, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Db"), 6, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Sg"), 6, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Bh"), 6, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Hs"), 6, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Mt"), 6, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ds"), 6, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Rg"), 6, 11, Qt::AlignHCenter);

    layout->addItem(new QSpacerItem(10,10), 7, 2);

    layout->addWidget(makeAtomButton("La"), 8, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ce"), 8, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pr"), 8, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Nd"), 8, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pm"), 8, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Sm"), 8, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Eu"), 8, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Gd"), 8, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Tb"), 8, 11, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Dy"), 8, 12, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Ho"), 8, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Er"), 8, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Tm"), 8, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Yb"), 8, 16, Qt::AlignHCenter);

    layout->addWidget(makeAtomButton("Ac"), 9, 3, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Th"), 9, 4, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pa"), 9, 5, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("U" ), 9, 6, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Np"), 9, 7, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Pu"), 9, 8, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Am"), 9, 9, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cm"), 9, 10, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Bk"), 9, 11, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Cf"), 9, 12, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Es"), 9, 13, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Fm"), 9, 14, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("Md"), 9, 15, Qt::AlignHCenter);
    layout->addWidget(makeAtomButton("No"), 9, 16, Qt::AlignHCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
