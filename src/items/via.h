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

#ifndef VIA_H
#define VIA_H

#include "hole.h"

class Via : public Hole 
{
	Q_OBJECT

public:
	Via(ModelPart *, ViewIdentifierClass::ViewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel);
	~Via();

	void setAutoroutable(bool);
	bool getAutoroutable();
	ConnectorItem * connectorItem();
	void saveInstanceLocation(QXmlStreamWriter & streamWriter);

	static QPointF ringThicknessRange(const QString & holeDiameter);
	static QPointF holeDiameterRange(const QString & ringThickness);

protected:
	QString makeID();
	void setBoth(const QString & holeDiameter, const QString &  thickness);

protected:
	static void setBothConnectors(ItemBase * itemBase, SvgIdLayer * svgIdLayer);

};

#endif
