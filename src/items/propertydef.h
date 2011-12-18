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

#ifndef PROPERTYDEF_H
#define PROPERTYDEF_H

#include <QHash>
#include <QList>
#include <QStringList>

struct PropertyDef {
	QString name;
	QString symbol;
	double minValue;
	double maxValue;
	QString defaultValue;
	bool numeric;
	bool editable;
	QList<double> menuItems;
	QStringList sMenuItems;
	QHash<QString, QString> adjuncts;
	QStringList suffixes;
};

class PropertyDefMaster
{
public:
	static void initPropertyDefs(class ModelPart *, QHash<PropertyDef *, QString> & propertyDefs);

protected:
	static void loadPropertyDefs();

protected:
	static QList <PropertyDef *> PropertyDefs;
};

#endif // PROPERTYDEF_H
