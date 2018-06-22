#include "mainwindow.h"

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    separatorAction = new QAction("Separator", NULL);
    separatorAction->setSeparator(true);
    fileMenu->addAction(separatorAction);
    for (int i = 0; i < MAX_RECENT_FILES; i++) {
        recentFileActions.append(new QAction("Empty", NULL));
        QObject::connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
        fileMenu->addAction(recentFileActions[i]);
    }

    QSettings settings;
    QList<QVariant> list =
        settings.value("Recently Opened Files", QVariant(QList<QString>())).toList();
    for (int i = 0; i < list.size(); i++)
        recentlyOpenedFiles.append(list[i].toString());
    updateRecentFiles();

#ifndef Q_WS_MAC
    fileMenu->addSeparator(); // Unused separator on Macs
#endif
    fileMenu->addAction(aboutAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(selectAllAction);
    editMenu->addAction(unselectAllAction);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(showPreferencesAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);

    insertMenu = menuBar()->addMenu(tr("&Insert"));
    insertMenu->addAction(addArrowAction);
    // insertMenu->addAction(addBondAction);
    insertMenu->addSeparator();
    insertSymbolMenu = insertMenu->addMenu(tr("Special Symbol"));
    insertSymbolMenu->addAction(insertAngstromAction);
    insertSymbolMenu->addAction(insertDegreeAction);
    insertSymbolMenu->addAction(insertPlusMinusAction);

    editMenu->setEnabled(false);
    itemMenu->setEnabled(false);
    insertMenu->setEnabled(false);
}

void MainWindow::updateMenus()
// TODO Add text controls
{
    // The aim here is to what the properties of the selected items are.  If they are all the
    // same, the menus should be updated to reflect that setting.  If the selected items have
    // different properties, the menus should appear blank to indicate this.  This is easy to do
    // in most cases, but comboboxes don't seem to have an easy mechanism for setting text, so
    // a hack is used, involving the special text property, which is displayed when the minimum
    // value is reached.

    // First, make sure that the buttons for the text labels are showing the correct values
    updateTextLabelToolbar();

    QList<double> atomScaleFactors;
    QList<double> bondScaleFactors;
    QList<QString> atomLabels;
    QList<QString> atomLabelFonts;
    QList<int> atomLabelFontSizes;

    foreach (QGraphicsItem *item, canvas->selectedItems()) {
        if (item->type() == Bond::Type) {
            Bond *bond = dynamic_cast<Bond *>(item);
            bondScaleFactors.append(bond->thickness());
        }
        if (item->type() == Atom::Type) {
            Atom *atom = dynamic_cast<Atom *>(item);
            atomScaleFactors.append(atom->scaleFactor());
            atomLabels.append(atom->label());
            atomLabelFonts.append(drawingInfo->getAtomLabelFont().family());
            atomLabelFontSizes.append(atom->fontSize());
        }
    }

    // Make the atom size menu appear correctly
    if (atomScaleFactors.size()) {
        bool sameValues = true;
        foreach (double i, atomScaleFactors) {
            if (fabs(i - atomScaleFactors[0]) > TINY) {
                sameValues = false;
                break;
            }
        }
        if (sameValues) {
            atomSizeSpinBox->setSpecialValueText(tr(""));
            atomSizeSpinBox->setValue(atomScaleFactors[0]);
        } else {
            atomSizeSpinBox->setSpecialValueText(tr("  "));
            atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
        }
    } else {
        atomSizeSpinBox->setSpecialValueText(tr("Select Atoms"));
        atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
    }

    // Make the atom labels menu appears correctly
    if (atomLabels.size()) {
        bool sameValues = true;
        foreach (QString i, atomLabels) {
            if (i != atomLabels[0]) {
                sameValues = false;
                break;
            }
        }
        if (sameValues) {
            atomLabelInput->setText(atomLabels[0]);
        } else {
            atomLabelInput->setText("");
        }
    } else {
        atomLabelInput->setText(tr("Select Atoms"));
    }

    // Make the bond thickness menu appears correctly
    if (bondScaleFactors.size()) {
        bool sameValues = true;
        foreach (double i, bondScaleFactors) {
            if (fabs(i - bondScaleFactors[0]) > TINY) {
                sameValues = false;
                break;
            }
        }
        if (sameValues) {
            bondSizeSpinBox->setSpecialValueText(tr(""));
            bondSizeSpinBox->setValue(bondScaleFactors[0]);
        } else {
            bondSizeSpinBox->setSpecialValueText(tr("  "));
            bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
        }
    } else {
        bondSizeSpinBox->setSpecialValueText(tr("Select Bonds"));
        bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
    }

    // Make the atom labels font menu appears correctly
    if (atomLabelFonts.size()) {
        bool sameValues = true;
        foreach (QString i, atomLabelFonts) {
            if (i != atomLabelFonts[0]) {
                sameValues = false;
                break;
            }
        }
        if (sameValues) {
            atomLabelFontCombo->setEditText(atomLabelFonts[0]);
        } else {
            atomLabelFontCombo->setEditText(tr(""));
        }
    } else {
        atomLabelFontCombo->setEditText(tr("Select Atoms"));
    }

    // Make the atom labels font size menu appears correctly
    if (atomLabelFontSizes.size()) {
        bool sameValues = true;
        foreach (int i, atomLabelFontSizes) {
            if (i != atomLabelFontSizes[0]) {
                sameValues = false;
                break;
            }
        }
        if (sameValues) {
            atomLabelFontSizeCombo->setEditText(QString().setNum(atomLabelFontSizes[0]));
        } else {
            atomLabelFontSizeCombo->setEditText(tr(""));
        }
    } else {
        atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
    }
}
