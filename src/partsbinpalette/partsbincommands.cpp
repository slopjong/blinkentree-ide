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



#include "partsbincommands.h"
#include "partsbinpalettewidget.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinBaseCommand::PartsBinBaseCommand(class PartsBinPaletteWidget* bin, QUndoCommand* parent)
	: QUndoCommand(parent)
{
	m_bin = bin;
}

class PartsBinPaletteWidget* PartsBinBaseCommand::bin() {
	return m_bin;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinAddRemoveArrangeCommand::PartsBinAddRemoveArrangeCommand(class PartsBinPaletteWidget* bin, QString moduleID, QUndoCommand *parent)
	: PartsBinBaseCommand(bin, parent)
{
	m_moduleID = moduleID;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinAddRemoveCommand::PartsBinAddRemoveCommand(class PartsBinPaletteWidget* bin, QString moduleID, int index, QUndoCommand *parent)
	: PartsBinAddRemoveArrangeCommand(bin, moduleID, parent)
{
	m_index = index;
}

void PartsBinAddRemoveCommand::add() {
	m_bin->addPart(m_moduleID, m_index);
}

void PartsBinAddRemoveCommand::remove() {
	m_bin->removePart(m_moduleID);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinAddCommand::PartsBinAddCommand(class PartsBinPaletteWidget* bin, QString moduleID, int index, QUndoCommand *parent)
	: PartsBinAddRemoveCommand(bin, moduleID, index, parent) {}

void PartsBinAddCommand::undo() {
	remove();
}

void PartsBinAddCommand::redo() {
	add();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinRemoveCommand::PartsBinRemoveCommand(class PartsBinPaletteWidget* bin, QString moduleID, int index, QUndoCommand *parent)
	: PartsBinAddRemoveCommand(bin, moduleID, index, parent) {}

void PartsBinRemoveCommand::undo() {
	add();
}

void PartsBinRemoveCommand::redo() {
	remove();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PartsBinArrangeCommand::PartsBinArrangeCommand(class PartsBinPaletteWidget* bin, QString moduleID, int oldIndex, int newIndex, QUndoCommand *parent)
	: PartsBinAddRemoveArrangeCommand(bin, moduleID, parent)
{
	m_oldIndex = oldIndex;
	m_newIndex = newIndex;
}

void PartsBinArrangeCommand::undo() {

}

void PartsBinArrangeCommand::redo() {

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
