/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2010 Fachhochschule Potsdam - http://fh-potsdam.de

Fritzing is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fritzing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fritzing.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************

$Revision: 4183 $:
$Author: cohen@irascible.com $:
$Date: 2010-05-06 22:30:19 +0200 (Thu, 06 May 2010) $

********************************************************************/


#include "editabletextwidget.h"
#include "../utils/misc.h"

EditableTextWidget::EditableTextWidget(QString text, WaitPushUndoStack *undoStack, QWidget *parent, QString title, bool edited, bool noSpacing)
	: AbstractEditableLabelWidget(text, undoStack, parent, title, edited, noSpacing) {
	m_textEdit = new QTextEdit(this);
	m_textEdit->setFixedHeight(70);
	toStandardMode();
}

QString EditableTextWidget::editionText() {
	return m_textEdit->toPlainText();
}
void EditableTextWidget::setEditionText(QString text) {
	m_textEdit->setText(text);
}
QWidget* EditableTextWidget::myEditionWidget() {
	return m_textEdit;
}

void EditableTextWidget::setEmptyTextToEdit() {
	m_textEdit->setText("");
}
