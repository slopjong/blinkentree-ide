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

$Revision: 5299 $:
$Author: cohen@irascible.com $:
$Date: 2011-07-28 16:51:21 +0200 (Thu, 28 Jul 2011) $

********************************************************************/

#ifndef SCREWTERMINAL_H
#define SCREWTERMINAL_H

#include "paletteitem.h"

class ScrewTerminal : public PaletteItem 
{
	Q_OBJECT

public:
	// after calling this constructor if you want to render the loaded svg (either from model or from file), MUST call <renderImage>
	ScrewTerminal(ModelPart *, ViewIdentifierClass::ViewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel);
	~ScrewTerminal();

	PluralType isPlural();
	QStringList collectValues(const QString & family, const QString & prop, QString & value);

public:
	static QString genFZP(const QString & moduleid);
	static QString genModuleID(QMap<QString, QString> & currPropsMap);
	static QString makeBreadboardSvg(const QString & expectedFileName);
	static QString makeSchematicSvg(const QString & expectedFileName);
	static QString makePcbSvg(const QString & expectedFileName);

protected:
	static void initSpacings();


};

#endif
