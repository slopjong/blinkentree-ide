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

$Revision: 5309 $:
$Author: cohen@irascible.com $:
$Date: 2011-07-30 21:17:22 +0200 (Sat, 30 Jul 2011) $

********************************************************************/

#ifndef BUS_H
#define BUS_H

#include <QString>
#include <QDomElement>
#include <QHash>
#include <QList>
#include <QXmlStreamWriter>
#include <QGraphicsScene>
#include <QPointer>

class Bus : public QObject 
{
	Q_OBJECT
	
public:
	Bus(class BusShared *, class ModelPart *);
	
	const QString & id();
	const QList<class Connector *> & connectors();
	void addConnector(class Connector *);
	class ModelPart * modelPart();
	
public:
	static QHash<QString, QPointer<Bus> > ___emptyBusList___;

	
protected:

	QList<class Connector *> m_connectors;
	BusShared * m_busShared;
	QPointer<class ModelPart> m_modelPart;
};


#endif
