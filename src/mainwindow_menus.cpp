#include "mainwindow.h"

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
	updateTextLabelMenus();
	
	QList<double> atomScaleFactors;
	QList<double> bondScaleFactors;
	QList<QString> atomLabels;
	QList<QString> atomLabelFonts;
	QList<int> atomLabelFontSizes;
	
	foreach(QGraphicsItem *item, canvas->selectedItems()){
		if(item->type() == Bond::Type){
			Bond *bond = dynamic_cast<Bond*>(item);
			bondScaleFactors.append(bond->thickness());
		}
		if(item->type() == Atom::Type){
			Atom *atom = dynamic_cast<Atom*>(item);
			atomScaleFactors.append(atom->scaleFactor());
			atomLabels.append(atom->label());
			atomLabelFonts.append(atom->labelFont().family());
			atomLabelFontSizes.append(atom->fontSize());
		}
	}
	
	// Make the atom size menu appear correctly
	if(atomScaleFactors.size()){
		bool sameValues = true;
		foreach(double i, atomScaleFactors){
			if(fabs(i - atomScaleFactors[0])>TINY){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomSizeSpinBox->setSpecialValueText(tr(""));
			atomSizeSpinBox->setValue(atomScaleFactors[0]);
		}else{
			atomSizeSpinBox->setSpecialValueText(tr("  "));
			atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
		}
	}else{
		atomSizeSpinBox->setSpecialValueText(tr("Select Atoms"));
		atomSizeSpinBox->setValue(atomSizeSpinBox->minimum());
	}
	
	// Make the atom labels menu appears correctly
	if(atomLabels.size()){
		bool sameValues = true;
		foreach(QString i, atomLabels){
			if(i != atomLabels[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelInput->setText(atomLabels[0]);
		}else{
			atomLabelInput->setText("");
		}
	}else{
		atomLabelInput->setText(tr("Select Atoms"));
	}
	
	// Make the bond thickness menu appears correctly
	if(bondScaleFactors.size()){
		bool sameValues = true;
		foreach(double i, bondScaleFactors){
			if(fabs(i - bondScaleFactors[0])>TINY){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			bondSizeSpinBox->setSpecialValueText(tr(""));
			bondSizeSpinBox->setValue(bondScaleFactors[0]);
		}else{
			bondSizeSpinBox->setSpecialValueText(tr("  "));
			bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
		}
	}else{
		bondSizeSpinBox->setSpecialValueText(tr("Select Bonds"));
		bondSizeSpinBox->setValue(bondSizeSpinBox->minimum());
	}
	
	// Make the atom labels font menu appears correctly
	if(atomLabelFonts.size()){
		bool sameValues = true;
		foreach(QString i, atomLabelFonts){
			if(i != atomLabelFonts[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelFontCombo->setEditText(atomLabelFonts[0]);
		}else{
			atomLabelFontCombo->setEditText(tr(""));
		}
	}else{
		atomLabelFontCombo->setEditText(tr("Select Atoms"));
	}
	
	// Make the atom labels font size menu appears correctly
	if(atomLabelFontSizes.size()){
		bool sameValues = true;
		foreach(int i, atomLabelFontSizes){
			if(i != atomLabelFontSizes[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			atomLabelFontSizeCombo->setEditText(QString().setNum(atomLabelFontSizes[0]));
		}else{
			atomLabelFontSizeCombo->setEditText(tr(""));
		}
	}else{
		atomLabelFontSizeCombo->setEditText(tr("Select Atoms"));
	}
}

// TODO deactivate bond and angle buttons unless selection is appropriate
void MainWindow::updateTextLabelMenus()
{
	// This is just like the update menus function, but is called seperately, as it should be 
	// called every time the selected text changes, but the atoms, bonds, etc. might not have changed
	// in this case, so we don't want to process them.
	// Start by looking to see if one of the labels is being edited
	bool hasLabel = false;
	Label *labelBeingEdited = 0;
	foreach(QGraphicsItem* item,canvas->items()){
		if(item->isSelected()){
			if(ITEM_IS_LABEL){
				hasLabel = true;
				Label *label = dynamic_cast<Label*>(item);
				if(label->textInteractionFlags() & Qt::TextEditorInteraction){
					labelBeingEdited = label;
					break;
				}
			}
		}
	}
	
	// Nothing to edit - bail now after setting some text in the menus
	if(!hasLabel){
		textFontCombo->setEditText(tr("Select Labels"));
		textFontSizeCombo->setEditText(tr(""));
		boldTextButton->setDown(false);			
		italicTextButton->setDown(false);			
		underlineTextButton->setDown(false);			
		return;
	}

	if(labelBeingEdited==0){
		// Labels are just selected, but not being edited, process the settings
		QList<int> textLabelFontSizes;
		QList<QString> textLabelFonts;
		QList<bool> textLabelsUnderlined;
		QList<bool> textLabelsItalic;
		QList<bool> textLabelsBold;
		foreach(QGraphicsItem* item,canvas->items()){
			if(item->isSelected()){
				if(ITEM_IS_LABEL){
					Label *label = dynamic_cast<Label*>(item);
					textLabelFontSizes.append(label->fontSize());
					textLabelFonts.append(label->font().family());
					textLabelsUnderlined.append(label->font().underline());
					textLabelsBold.append(label->font().bold());
					textLabelsItalic.append(label->font().italic());
				}
			}
		}
		bool sameValues;

		// Process the fonts
		sameValues = true;
		foreach(QString i, textLabelFonts){
			if(i != textLabelFonts[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			textFontCombo->setEditText(textLabelFonts[0]);			
		}else{
			textFontCombo->setEditText(tr(""));
		}

		// Process the font sizes
		sameValues = true;
		foreach(int i, textLabelFontSizes){
			if(i != textLabelFontSizes[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			textFontSizeCombo->setEditText(QString().setNum(textLabelFontSizes[0]));			
		}else{
			textFontSizeCombo->setEditText(tr(""));
		}
		
		// Process the font boldness
		sameValues = true;
		foreach(bool i, textLabelsBold){
			if(i != textLabelsBold[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			boldTextButton->setDown(false);			
		}else{
			boldTextButton->setDown(textLabelsBold[0]);			
		}

		// Process the font italics
		sameValues = true;
		foreach(bool i, textLabelsItalic){
			if(i != textLabelsItalic[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			italicTextButton->setDown(false);			
		}else{
			italicTextButton->setDown(textLabelsItalic[0]);			
		}

		// Process the font underline
		sameValues = true;
		foreach(bool i, textLabelsUnderlined){
			if(i != textLabelsUnderlined[0]){
				sameValues = false;
				break;
			}
		}
		if(sameValues){
			underlineTextButton->setDown(false);			
		}else{
			underlineTextButton->setDown(textLabelsUnderlined[0]);			
		}
	}else{
		// There is a label being edited, make the menu reflect the settings
	}
}