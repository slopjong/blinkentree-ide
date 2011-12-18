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

#include "via.h"
#include "../utils/graphicsutils.h"
#include "../fsvgrenderer.h"
#include "../sketch/infographicsview.h"
#include "../utils/textutils.h"
#include "../viewlayer.h"
#include "../connectors/connectoritem.h"
#include "../connectors/svgidlayer.h"


Via::Via( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel)
	: Hole(modelPart, viewIdentifier, viewGeometry, id, itemMenu, doLabel)
{
	m_holeSettings.holeDiameterRange = Via::holeDiameterRange;
	m_holeSettings.ringThicknessRange = Via::ringThicknessRange;
	//DebugDialog::debug(QString("creating via %1 %2 %3").arg((long) this, 0, 16).arg(id).arg(m_viewIdentifier));
}

Via::~Via() {
	//DebugDialog::debug(QString("deleting via %1 %2 %3").arg((long) this, 0, 16).arg(m_id).arg(m_viewIdentifier));
}

void Via::setBoth(const QString & holeDiameter, const QString & ringThickness) {
	if (this->m_viewIdentifier != ViewIdentifierClass::PCBView) return;

	QStringList connectorIDs;
	connectorIDs << "connector0pin";
	ItemBase * otherLayer = setBothSvg(holeDiameter, ringThickness, connectorIDs);

	// there's only one connector
	foreach (Connector * connector, m_modelPart->connectors().values()) {
		if (connector == NULL) continue;

		connector->unprocess(m_viewIdentifier, m_viewLayerID);
		SvgIdLayer * svgIdLayer = connector->fullPinInfo(m_viewIdentifier, m_viewLayerID);
		if (svgIdLayer == NULL) continue;

		bool result = m_extraRenderer->setUpConnector(svgIdLayer, false);
		if (!result) continue;

		setBothConnectors(this, svgIdLayer);
	}

	if (otherLayer) {
		foreach (Connector * connector, m_modelPart->connectors().values()) {
			if (connector == NULL) continue;

			connector->unprocess(m_viewIdentifier, otherLayer->viewLayerID());
			SvgIdLayer * svgIdLayer = connector->fullPinInfo(m_viewIdentifier, otherLayer->viewLayerID());
			if (svgIdLayer == NULL) continue;

			bool result = m_otherLayerRenderer->setUpConnector(svgIdLayer, false);
			if (!result) continue;

			setBothConnectors(otherLayer, svgIdLayer);
		}
	}


}

void Via::setBothConnectors(ItemBase * itemBase, SvgIdLayer * svgIdLayer) 
{
	foreach (ConnectorItem * connectorItem, itemBase->cachedConnectorItems()) {
		connectorItem->setRect(svgIdLayer->m_rect);
		connectorItem->setTerminalPoint(svgIdLayer->m_point);
		connectorItem->setRadius(svgIdLayer->m_radius, svgIdLayer->m_strokeWidth);
		connectorItem->attachedMoved();
		break;
	}
}

QString Via::makeID() {
	return "connector0pin";
}

QPointF Via::ringThicknessRange(const QString & holeDiameter) {
	Q_UNUSED(holeDiameter);
	QPointF p(.001, 1.0);
	return p;
}

QPointF Via::holeDiameterRange(const QString & ringThickness) {
	Q_UNUSED(ringThickness);
	QPointF p(.001, 1.0);
	return p;
}

void Via::setAutoroutable(bool ar) {
	m_viewGeometry.setAutoroutable(ar);
}

bool Via::getAutoroutable() {
	return m_viewGeometry.getAutoroutable();
}

ConnectorItem * Via::connectorItem() {
	foreach (ConnectorItem * connectorItem, cachedConnectorItems()) {
		return connectorItem;
	}

	return NULL;
}

void Via::saveInstanceLocation(QXmlStreamWriter & streamWriter)
{
	streamWriter.writeAttribute("x", QString::number(m_viewGeometry.loc().x()));
	streamWriter.writeAttribute("y", QString::number(m_viewGeometry.loc().y()));
	streamWriter.writeAttribute("wireFlags", QString::number(m_viewGeometry.flagsAsInt()));
	GraphicsUtils::saveTransform(streamWriter, m_viewGeometry.transform());
}
