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

$Revision: 5308 $:
$Author: cohen@irascible.com $:
$Date: 2011-07-30 21:09:56 +0200 (Sat, 30 Jul 2011) $

********************************************************************/

#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QXmlStreamWriter>
#include <QDomElement>
#include <QKeyEvent>
#include <QDialog>
#include <QLineEdit>
#include <QTextCursor>

#include "../items/itembase.h"

class Note : public ItemBase
{
Q_OBJECT

public:
	Note(class ModelPart*, ViewIdentifierClass::ViewIdentifier, const ViewGeometry &, long id, QMenu * itemMenu);
	
	void saveGeometry();
	bool itemMoved();
	void saveInstanceLocation(QXmlStreamWriter &);
	void moveItem(ViewGeometry &);
	void findConnectorsUnder();
	void setText(const QString & text, bool checkSize);
	QString text();
	void setSize(const QSizeF & size);
	void setHidden(bool hide);
	void setInactive(bool inactivate);
	bool hasPartLabel();
	bool stickyEnabled();
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QPainterPath shape() const;
	bool hasPartNumberProperty();
	bool rotationAllowed();
	bool rotation45Allowed();

protected:
	QRectF boundingRect() const;
	void positionGrip();
	void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
	bool eventFilter(QObject * object, QEvent * event);
	void checkSize(QSizeF & newSize);
	void connectSlots();
	void disconnectSlots();
	void forceFormat(int position, int charsAdded);

protected slots:
	void contentsChangeSlot(int position, int charsAdded, int charsRemoved);
	void contentsChangedSlot();
	void linkDialog();
	void handleZoomChangedSlot(double scale);
	void handleMousePressSlot(QGraphicsSceneMouseEvent * event, class ResizeHandle * resizeHandle);

public:
	static const int emptyMinWidth;
	static const int emptyMinHeight;
	static const int initialMinWidth;
	static const int initialMinHeight;
	static QString initialTextString;

protected:
	QRectF m_rect;
    QPen m_pen;
    QBrush m_brush;
	class ResizeHandle * m_resizeGrip;
	class ResizeHandle * m_inResize;
	QGraphicsTextItem * m_graphicsTextItem;
	int m_charsAdded;
	int m_charsPosition;

};

class LinkDialog : public QDialog
{
Q_OBJECT

public:
	LinkDialog(QWidget *parent = 0);
	~LinkDialog();

	void setUrl(const QString &);
	void setText(const QString &);
	QString text();
	QString url();

protected:
	QLineEdit * m_urlEdit;
	QLineEdit * m_textEdit;

};


#endif
