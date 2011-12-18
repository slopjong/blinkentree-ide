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

$Revision: 5554 $:
$Author: cohen@irascible.com $:
$Date: 2011-10-20 10:09:29 +0200 (Thu, 20 Oct 2011) $

********************************************************************/


#ifndef STACKTABWIDGET_H_
#define STACKTABWIDGET_H_

#include <QTabWidget>
#include <QTabBar>
#include <QPushButton>
#include <QPointer>

// originally extracted from http://wiki.qtcentre.org/index.php?title=Movable_Tabs
class StackTabWidget : public QTabWidget {
	Q_OBJECT
	public:
		StackTabWidget(QWidget *parent);
		class StackTabBar *stackTabBar();

	public slots:
		void informCurrentChanged(int index);
		void informTabCloseRequested(int index);

	signals:
		void currentChanged(StackTabWidget *, int index);
		void tabCloseRequested(StackTabWidget *, int index);

};

#endif /* STACKTABWIDGET_H_ */
