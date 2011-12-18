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

$Revision: 5675 $:
$Author: cohen@irascible.com $:
$Date: 2011-12-14 04:57:40 +0100 (Wed, 14 Dec 2011) $

********************************************************************/

#ifndef DIP_H
#define DIP_H


#include "mysterypart.h"

class Dip : public MysteryPart 
{
	Q_OBJECT

public:
	Dip(ModelPart *, ViewIdentifierClass::ViewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel);
	~Dip();

	bool collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget);
	QStringList collectValues(const QString & family, const QString & prop, QString & value);
	bool changePinLabels(bool singleRow, bool sip);

public:
	static QString genSipFZP(const QString & moduleid);
	static QString genDipFZP(const QString & moduleid);
	static QString genModuleID(QMap<QString, QString> & currPropsMap);
	static QString makePcbSvg(const QString & expectedFileName);
	static QString makeSchematicSvg(const QString & expectedFileName);
	static QString makeSchematicSvg(const QStringList & labels);
	static QString makeBreadboardSvg(const QString & expectedFileName);
	static QString makeBreadboardSipSvg(const QString & expectedFileName);
	static QString makeBreadboardDipSvg(const QString & expectedFileName);

protected:
	bool isDIP();
	bool otherPropsChange(const QMap<QString, QString> & propsMap);
	const QStringList & spacings();
	QString retrieveSchematicSvg(QString & svg);
};

#endif
