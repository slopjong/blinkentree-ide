/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2009 Fachhochschule Potsdam - http://fh-potsdam.de

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

$Revision: 2085 $:
$Author: cohen@irascible.com $:
$Date: 2009-01-06 12:15:02 +0100 (Tue, 06 Jan 2009) $

********************************************************************/

#ifndef PCBXML_H
#define PCBXML_H
//
#include <QDomElement>
#include <QSvgGenerator>
#include "svgdomdocument.h"
//
class PcbXML  
{

public:
	PcbXML(const QDomElement & pcbDocument);
	
	QString getSvgFile();
	
private:
	SVGDomDocument * m_svg;
	QString m_svgFile;
	QDomElement m_svgroot;
	QDomElement m_silkscreen;
	QDomElement m_copper;
	QDomElement m_keepout;
	QDomElement m_mask;
	QDomElement m_outline;
	int m_markx;
	int m_marky;
	int m_minx;
	int m_miny;
	int m_maxx;
	int m_maxy;
	int m_pinCount;
	int m_padCount;
	QString m_units; // length units for the root element coordinates
	
	void drawNode(QDomNode node);
	void drawPin(QDomNode node);
	void drawPad(QDomNode node);
	void drawElementLine(QDomNode node);
	void drawElementArc(QDomNode node);
	void drawMark(QDomNode node);
	void minMax(int, int, int);
	void shiftCoordinates();
};
#endif
