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

$Revision: 5615 $:
$Author: cohen@irascible.com $:
$Date: 2011-11-14 14:41:31 +0100 (Mon, 14 Nov 2011) $

********************************************************************/

#ifndef CURSORMASTER_H
#define CURSORMASTER_H

#include <QCursor>
#include <QObject>

class CursorKeyListener 
{
	public:
		virtual void cursorKeyEvent(Qt::KeyboardModifiers) = 0;
};

class CursorMaster : public QObject {
Q_OBJECT

protected:
	CursorMaster();

public:
	static CursorMaster * instance();
	static void initCursors();

	void addCursor(QObject * listener, const QCursor & cursor);
	void removeCursor(QObject * listener);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void deleteCursor(QObject *);
	void moveCursor();

public:
	static QCursor * BendpointCursor;
	static QCursor * NewBendpointCursor;
	static QCursor * MakeWireCursor;
	static QCursor * MakeCurveCursor;
	static QCursor * RubberbandCursor;
	static QCursor * MoveCursor;	
	static QCursor * BendlegCursor;	
	static QCursor * RotateCursor;	

protected:
	static CursorMaster TheCursorMaster;
};

#endif
