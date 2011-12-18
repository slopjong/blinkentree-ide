/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2011 Fachhochschule Potsdam - http://fh-potsdam.de

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

$Revision: 5594 $:
$Author: cohen@irascible.com $:
$Date: 2011-11-05 21:03:53 +0100 (Sat, 05 Nov 2011) $

********************************************************************/


#ifndef STACKTABBAR_H_
#define STACKTABBAR_H_

#include <QTabBar>
#include <QTimer>

class StackTabBar : public QTabBar {
	Q_OBJECT
	public:
		StackTabBar(class StackTabWidget *parent);

	protected:
		void dragEnterEvent(QDragEnterEvent* event);
		void dragLeaveEvent(QDragLeaveEvent* event);
        void dragMoveEvent(QDragMoveEvent* event);
		void dropEvent(QDropEvent* event);
		void paintEvent(QPaintEvent *event);

		bool mimeIsAction(const class QMimeData* m, const QString& action);

	public slots:
		void setIndex();

	protected slots:
		void showContextMenu(const QPoint &point);

	protected:
		class StackTabWidget* m_parent;
		QTimer m_dragMoveTimer;
};

#endif /* STACKTABBAR_H_ */
