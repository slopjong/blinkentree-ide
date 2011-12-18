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

#ifndef SVGIDLAYER_H
#define SVGIDLAYER_H

#include <QString>

#include "../viewlayer.h"

struct SvgIdLayer 
{
	QString m_svgId;
	QString m_terminalId;
	ViewLayer::ViewLayerID m_svgViewLayerID;
	bool m_svgVisible;
	bool m_hybrid;
	bool m_processed;
	QRectF m_rect;		
	QPointF m_point;	
	double m_radius;
	double m_strokeWidth;
	QString m_legId;
	QString m_legColor;
	double m_legStrokeWidth;
	QLineF m_legLine;

	SvgIdLayer();
	SvgIdLayer * copyLayer();
};


#endif
