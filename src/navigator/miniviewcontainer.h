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

#ifndef MINIVIEWCONTAINER_H
#define MINIVIEWCONTAINER_H

#include <QWidget>
#include <QPaintEvent>

#include "miniview.h"

class MiniViewContainer : public QWidget
{
	Q_OBJECT
	
public:
	MiniViewContainer(QWidget * parent = 0);
	void setView(QGraphicsView *);	
	void resizeEvent ( QResizeEvent * event ); 
	void filterMousePress();
	void hideHandle(bool hide);
	void forceResize();
	void setTitle(const QString & title);
	QWidget * miniView();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
protected slots:
	void updateFrame();

public slots:
	void miniViewMousePressedSlot();
	void miniViewMouseEnterSlot();
	void miniViewMouseLeaveSlot();
	
signals:
	void navigatorMousePressedSignal(MiniViewContainer *);
	void navigatorMouseEnterSignal(MiniViewContainer *);
	void navigatorMouseLeaveSignal(MiniViewContainer *);
	
protected:
	MiniView * m_miniView;
	class MiniViewFrame * m_frame;
	class MiniViewFrame * m_outerFrame;
	
};


class MiniViewFrame : public QFrame
{
	Q_OBJECT
	
public:
	MiniViewFrame(QBrush &, bool draggable, QWidget * parent = 0);
	
	void setMaxDrag(int x, int y);
	void setMinDrag(int x, int y);
	
protected:
	void paintEvent(QPaintEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	
protected:
	QBrush m_brush;
	QPen m_pen;
	QPoint m_dragOffset;
	QPoint m_originalPos;
	bool m_inDrag;
	bool m_draggable;
	QPoint m_maxDrag;
	QPoint m_minDrag;
	
signals:
	void scrollChangeSignal(double x, double y);
};

#endif
