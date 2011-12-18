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

#ifndef ERCDATA_H
#define ERCDATA_H

#include <QString>
#include <QDomElement>
#include <QHash>
#include <QList>

class ValidReal {
public:
	ValidReal();

	bool isValid();
	double value();
	void setValue(double);
	bool setValue(const QString &);

protected:
	bool m_ok;
	double m_value;

};

class ErcData {

public:
	enum EType {
		Ground,
		VCC,
		UnknownEType
	};

	enum CurrentFlow {
		Source,
		Sink,
		UnknownFlow
	};

	enum Ignore {
		Never,
		Always,
		IfUnconnected
	};
	
public:
	ErcData(const QDomElement & ercElement);

	bool writeToElement(QDomElement & ercElement, QDomDocument & doc);
	EType eType();
	Ignore ignore();

protected:
	void readVoltage(QDomElement &);
	void readCurrent(QDomElement &);
	void writeVoltage(QDomElement &, QDomDocument &);
	void writeCurrent(QDomElement &, QDomDocument &);
	
protected:
	EType m_eType;
	Ignore m_ignore;
	ValidReal m_voltage;
	ValidReal m_voltageMin;
	ValidReal m_voltageMax;
	ValidReal m_current;
	ValidReal m_currentMin;
	ValidReal m_currentMax;
	CurrentFlow m_currentFlow;
};

#endif
