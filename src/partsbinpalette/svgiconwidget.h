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

$Revision: 5143 $:
$Author: cohen@irascible.com $:
$Date: 2011-07-01 02:37:01 +0200 (Fri, 01 Jul 2011) $

********************************************************************/



#ifndef SVGICONWIDGET_H_
#define SVGICONWIDGET_H_

#include <QGraphicsWidget>
#include <QGraphicsPixmapItem>
#include <QToolTip>
#include <QPointer>
#include <QPixmap>

#include "../model/modelpart.h"
#include "../items/itembase.h"


class SvgIconPixmapItem : public QGraphicsPixmapItem {

public:
	SvgIconPixmapItem(const QPixmap & pixmap, QGraphicsItem * parent = 0);
	void setPlural(bool);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);

protected:
	bool m_plural;
};

class SvgIconWidget : public QGraphicsWidget
{
Q_OBJECT
public:
	SvgIconWidget(ModelPart *, ViewIdentifierClass::ViewIdentifier, ItemBase *, bool plural);
	~SvgIconWidget();
	ItemBase * itemBase() const;
	ModelPart * modelPart() const;
	const QString &moduleID() const;

	static void initNames();
	static void cleanup();

protected:
	void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


protected:
	QPointer<ItemBase> m_itemBase;
	SvgIconPixmapItem * m_pixmapItem;
	QString m_moduleId;
};


#endif /* SVGICONWIDGET_H_ */
