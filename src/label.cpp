#include "label.h"
#include "drawingcanvas.h"

Label::Label(LabelType type, double value, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
		:QGraphicsTextItem(parent, scene),
		myType(type),
		myDX(0.0),
		myDY(0.0),
		myValue(value),
		_info(info)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setTextInteractionFlags(Qt::NoTextInteraction);
	setZValue(1000.0);
	setFont(QFont(DEFAULT_LABEL_FONT, DEFAULT_LABEL_FONT_SIZE));
	if(myType != TextLabelType)
		updateLabel();
	this->currentFormat = new QTextCharFormat();
	this->currentFormat->setFont(QFont(DEFAULT_LABEL_FONT));
	setToolTip(tr("Double click to edit"));
}

void Label::keyPressEvent(QKeyEvent *event)
{
	QTextCursor cursor = textCursor();
	if(event->key() == Qt::Key_Tab)
	{
		cursor.insertText("\t", *(this->currentFormat));
		setTextCursor(cursor);
		setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	else if(event->key() == Qt::Key_Up)
	{
		cursor.movePosition(QTextCursor::Start, (event->modifiers() & Qt::ShiftModifier) ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor, 1);
		this->setTextCursor(cursor);

	}
	else if(event->key() == Qt::Key_Down)
	{
		cursor.movePosition(QTextCursor::End, (event->modifiers() & Qt::ShiftModifier) ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor, 1);
		this->setTextCursor(cursor);
	}
	else
	{
		int length = toPlainText().length();
		if(event->key() == Qt::Key_Right && cursor.position() == length)
			return;
		else if(event->key() == Qt::Key_Left && cursor.position() == 0)
			return;
		else
		{
			QGraphicsTextItem::keyPressEvent(event);
			if(length < toPlainText().length())
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				cursor.insertText(c, *(this->currentFormat));
			}
		}
	}
	this->currentFormat = new QTextCharFormat(cursor.charFormat());
	emit characterEntered();
}

void Label::focusOutEvent(QFocusEvent *event)
{
	if(event->reason() == Qt::TabFocusReason)
	{
		QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QGraphicsTextItem::keyPressEvent(newEvent);
		event->accept();
	}
	else
	{
		QGraphicsTextItem::focusOutEvent(event);
		if(this->toPlainText().length() == 0)
			this->scene()->removeItem(this);
	}
}

void Label::updateLabel()
{
	if(myType == BondLabelType) {
		myString.setNum(myValue, 'f', _info->getBondPrecision());
	} else if(myType == AngleLabelType) {
		myString.setNum(myValue, 'f', _info->getAnglePrecision());
	}
	setPlainText(myString);
}

void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(textInteractionFlags() == Qt::NoTextInteraction) {
		setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Label::setBold(bool bold)
{
	this->currentFormat->setFontWeight(bold ? QFont::Bold : QFont::Normal);
	QTextCursor cursor = this->textCursor();
	if(textInteractionFlags() & Qt::TextEditorInteraction)
		cursor.setCharFormat(*currentFormat);
	else
	{
		int length = toPlainText().length();
		cursor.setPosition(0);
		while(cursor.position() < length)
		{
			cursor.setPosition(cursor.position() + 1);
			QTextCharFormat format = cursor.charFormat();
			if(format.font().bold() != bold)
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				format.setFontWeight(bold ? QFont::Bold : QFont::Normal);
				cursor.insertText(c, format);
			}
		}
	}
}

void Label::setItalic(bool italic)
{
	this->currentFormat->setFontItalic(italic);
	QTextCursor cursor = this->textCursor();
	if(textInteractionFlags() & Qt::TextEditorInteraction)
		cursor.setCharFormat(*currentFormat);
	else
	{
		int length = toPlainText().length();
		cursor.setPosition(0);
		while(cursor.position() < length)
		{
			cursor.setPosition(cursor.position() + 1);
			QTextCharFormat format = cursor.charFormat();
			if(format.font().italic() != italic)
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				format.setFontItalic(italic);
				cursor.insertText(c, format);
			}
		}
	}
}

void Label::setUnderline(bool underline)
{
	this->currentFormat->setFontUnderline(underline ? QTextCharFormat::SingleUnderline : QTextCharFormat::NoUnderline);
	QTextCursor cursor = this->textCursor();
	if(textInteractionFlags() & Qt::TextEditorInteraction)
		cursor.setCharFormat(*currentFormat);
	else
	{
		int length = toPlainText().length();
		cursor.setPosition(0);
		while(cursor.position() < length)
		{
			cursor.setPosition(cursor.position() + 1);
			QTextCharFormat format = cursor.charFormat();
			if(format.font().underline() != underline)
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				format.setFontUnderline(underline ? QTextCharFormat::SingleUnderline : QTextCharFormat::NoUnderline);
				cursor.insertText(c, format);
			}
		}
	}
}

void Label::setCurrentFont(QFont font)
{
	this->currentFormat->setFontFamily(font.family());
	QTextCursor cursor = this->textCursor();
	if(textInteractionFlags() & Qt::TextEditorInteraction)
		cursor.setCharFormat(*currentFormat);
	else
	{
		int length = toPlainText().length();
		cursor.setPosition(0);
		while(cursor.position() < length)
		{
			cursor.setPosition(cursor.position() + 1);
			QTextCharFormat format = cursor.charFormat();
			if(format.font().family() != font.family())
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				format.setFontFamily(font.family());
				cursor.insertText(c, format);
			}
		}
	}
}

void Label::setCurrentFontSize(int size)
{
	this->currentFormat->setFontPointSize(size);
	QTextCursor cursor = this->textCursor();
	if(textInteractionFlags() & Qt::TextEditorInteraction)
		cursor.setCharFormat(*currentFormat);
	else
	{
		int length = toPlainText().length();
		cursor.setPosition(0);
		while(cursor.position() < length)
		{
			cursor.setPosition(cursor.position() + 1);
			QTextCharFormat format = cursor.charFormat();
			if(format.font().pointSize() != size)
			{
				QString c = toPlainText().left(cursor.position()).right(1);
				cursor.deletePreviousChar();
				format.setFontPointSize(size);
				cursor.insertText(c, format);
			}
		}
	}
}

QFont Label::getCurrentFont()
{
	if(textInteractionFlags() & Qt::TextEditorInteraction)
	{
		QTextCursor cursor = this->textCursor();
		if(cursor.hasSelection())
		{
			QTextCursor tempCursor(cursor);
			int start = cursor.selectionStart();
			int end = cursor.selectionEnd();
			bool swapped = false;
			if(start > end)
			{
				int temp = start;
				start = end;
				end = temp;
				swapped = true;
			}
			tempCursor.setPosition((swapped) ? cursor.position()-1 : cursor.position()+1);
			return tempCursor.charFormat().font();
		}
		return cursor.charFormat().font();
	}
	else
		return currentFormat->font();
}

void Label::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Label");
	writer->writeAttribute("type", QString("%1").arg(myType));
	writer->writeAttribute("string", myString);
	writer->writeAttribute("text", toPlainText());
	writer->writeAttribute("dx", QString("%1").arg(myDX));
	writer->writeAttribute("dy", QString("%1").arg(myDY));
	writer->writeAttribute("value", QString("%1").arg(myValue));

	// Determine and write all the font formats used throughout the label
	// Credit: Jesse Yates
	QTextCursor* cursor = new QTextCursor(this->document());
	QMap<QString, QList<FontFormatTuple*>* > fontMap;
	int start = cursor->position();
	QString prev = "";
	QString format = "";
	QList<FontFormatTuple*>* values;
	int i = 0;
	for(i = start; !cursor->atEnd(); i++)
	{
		cursor->setPosition(cursor->position()+1);
		format = cursor->charFormat().font().toString();
		if(fontMap.contains(format))
		{
			values = fontMap[format];
			if(format == prev)
				((*values)[values->size() - 1])->end = i;
			else
			{
				FontFormatTuple* temp = new FontFormatTuple(format, i, i);
				values->append(temp);
			}
		}
		else
		{
			values = new QList<FontFormatTuple*>();
			FontFormatTuple* temp = new FontFormatTuple(format, i, i);
			values->append(temp);
		}
		fontMap.insert(format, values);
		prev = format;
	}

	if(fontMap.contains(format))
	{
		values = fontMap[format];
		((*values)[values->size() - 1])->end = i-1;
	}

	std::vector<FontFormatTuple*> list;
	foreach(QList<FontFormatTuple*>* ql, fontMap.values())
	{
		foreach(FontFormatTuple* v, *ql)
			list.push_back(v);
	}
	sort(list.begin(), list.end(), FontFormatTuple::compareTo);

	writer->writeAttribute("formats", QString("%1").arg(list.size()));
	foreach(FontFormatTuple* t, list)
		t->serialize(writer);

	writer->writeEndElement();

	delete cursor;
	// Delete lots of other stuff here, too
}

Label* Label::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QGraphicsScene* scene)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "Label");
	QXmlStreamAttributes attr = reader->attributes();
	LabelType type = Label::LabelType(attr.value("type").toString().toInt());
	Label* l = new Label(type, attr.value("value").toString().toDouble(), drawingInfo, NULL, scene);
	QString text = attr.value("text").toString();
	l->myString = attr.value("string").toString();
	l->setPlainText("");
	l->myDX = attr.value("dx").toString().toInt();
	l->myDY = attr.value("dy").toString().toInt();
	l->myValue = attr.value("value").toString().toDouble();

	int formats = attr.value("formats").toString().toInt();
	QTextCursor cursor(l->document());
	QTextCharFormat textFormat;
	QFont textFont;
	for(int i = 0; i < formats; i++) {
		FontFormatTuple* s = FontFormatTuple::deserialize(reader);
		QString t = text.left(s->end+1).right(s->end - s->start + 1);
		textFont.fromString(s->format);
		textFormat.setFont(textFont);
		cursor.insertText(t, textFormat);
	}
	return l;
}
