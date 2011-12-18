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

$Revision: 4406 $:
$Author: cohen@irascible.com $:
$Date: 2010-08-08 18:37:07 +0200 (Sun, 08 Aug 2010) $

********************************************************************/

#ifndef RATSNESTCOLORS_H
#define RATSNESTCOLORS_H

#include <QColor>
#include <QDomElement>
#include <QHash>
#include <QStringList>
#include "../viewidentifierclass.h"

class RatsnestColors {

public:
	RatsnestColors(const QDomElement &);
	~RatsnestColors();

	static void initNames();
	static void cleanup();
	static const QColor & netColor(ViewIdentifierClass::ViewIdentifier m_viewIdentifier);
	static bool findConnectorColor(const QStringList & names, QColor & color);
	static bool isConnectorColor(ViewIdentifierClass::ViewIdentifier m_viewIdentifier, const QColor &);
	static void reset(ViewIdentifierClass::ViewIdentifier m_viewIdentifier);

protected:
	const QColor & getNextColor();

protected:
	ViewIdentifierClass::ViewIdentifier m_viewIdentifier;
	int m_index;
	QList<class RatsnestColor *> m_ratsnestColors;

	static QHash<ViewIdentifierClass::ViewIdentifier, RatsnestColors *> m_viewList;
	static QHash<QString, class RatsnestColor *> m_allNames;
};


#endif
