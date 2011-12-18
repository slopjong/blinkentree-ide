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



#ifndef PARTSBINCOMMANDS_H
#define PARTSBINCOMMANDS_H

#include <QUndoCommand>
#include <QHash>


class PartsBinBaseCommand : public QUndoCommand
{
public:
	PartsBinBaseCommand(class PartsBinPaletteWidget*, QUndoCommand* parent = 0);
	class PartsBinPaletteWidget* bin();

protected:
    class PartsBinPaletteWidget* m_bin;
};

class PartsBinAddRemoveArrangeCommand : public PartsBinBaseCommand
{
public:
	PartsBinAddRemoveArrangeCommand(class PartsBinPaletteWidget*, QString moduleID, QUndoCommand *parent = 0);

protected:
    QString m_moduleID;
};

class PartsBinAddRemoveCommand : public PartsBinAddRemoveArrangeCommand
{
public:
	PartsBinAddRemoveCommand(class PartsBinPaletteWidget*, QString moduleID, int index, QUndoCommand *parent = 0);

protected:
	void add();
	void remove();
    int m_index;
};

class PartsBinAddCommand : public PartsBinAddRemoveCommand
{
public:
	PartsBinAddCommand(class PartsBinPaletteWidget*, QString moduleID, int index = -1, QUndoCommand *parent = 0);
    void undo();
    void redo();
};

class PartsBinRemoveCommand : public PartsBinAddRemoveCommand
{
public:
	PartsBinRemoveCommand(class PartsBinPaletteWidget*, QString moduleID, int index = -1, QUndoCommand *parent = 0);
    void undo();
    void redo();
};

class PartsBinArrangeCommand : public PartsBinAddRemoveArrangeCommand
{
public:
	PartsBinArrangeCommand(class PartsBinPaletteWidget*, QString moduleID, int oldIndex, int newIndex, QUndoCommand *parent = 0);
    void undo();
    void redo();

protected:
	int m_oldIndex;
	int m_newIndex;
};

#endif // PARTSBINCOMMANDS_H
