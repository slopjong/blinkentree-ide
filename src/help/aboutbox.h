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

$Revision: 4193 $:
$Author: cohen@irascible.com $:
$Date: 2010-05-20 16:51:52 +0200 (Thu, 20 May 2010) $

********************************************************************/

#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QScrollArea>

class AboutBox : public QWidget {
	Q_OBJECT

private:
	AboutBox(QWidget *parent = 0);
	void resetScrollAnimation();

private:
	//QScrollArea *m_scrollArea;
	class ExpandingLabel * m_expandingLabel;
	bool m_restartAtTop;
	QTime m_startTime;
	QTimer *m_autoScrollTimer;

public:
	static void hideAbout();
	static void showAbout();
	static void closeAbout(); // Maybe we don't need close as a public method (we only want to hide)

public slots:
	void scrollCredits();

protected:
	static AboutBox* singleton;

protected:
	void closeEvent ( QCloseEvent * event );
	void keyPressEvent ( QKeyEvent * event );
	void show();
};

#endif
