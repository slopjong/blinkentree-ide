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

$Revision: 5666 $:
$Author: cohen@irascible.com $:
$Date: 2011-12-05 18:07:55 +0100 (Mon, 05 Dec 2011) $

********************************************************************/

#include "connector.h"
#include "connectorshared.h"
#include "connectoritem.h"
#include "../debugdialog.h"
#include "../model/modelpart.h"
#include "bus.h"
#include "../fsvgrenderer.h"
#include "ercdata.h"

QHash <Connector::ConnectorType, QString > Connector::Names;

static inline int QuickHash(ViewIdentifierClass::ViewIdentifier viewIdentifier, ViewLayer::ViewLayerID viewLayerID) {
	return (1000 * viewIdentifier) + viewLayerID;
}

Connector::Connector( ConnectorShared * connectorShared, ModelPart * modelPart)
{
	m_modelPart = modelPart;
	m_connectorShared = connectorShared;
	m_bus = NULL;
}

Connector::~Connector() {
	//DebugDialog::debug(QString("deleting connector %1 %2").arg((long) this, 0, 16).arg(connectorSharedID()));
	foreach (ConnectorItem * connectorItem, m_connectorItems.values()) {
		connectorItem->clearConnector();
	}
}

void Connector::initNames() {
	if (Names.count() == 0) {
		Names.insert(Connector::Male, "male");
		Names.insert(Connector::Female, "female");
		Names.insert(Connector::Wire, "wire");
		Names.insert(Connector::Pad, "pad");
	}

}

Connector::ConnectorType Connector::connectorTypeFromName(const QString & name) {
	QHashIterator<ConnectorType, QString> i(Names);
    while (i.hasNext()) {
        i.next();
		if (name.compare(i.value(), Qt::CaseInsensitive ) == 0) {
			return i.key();
		}
    }

	return Connector::Unknown;
}

const QString & Connector::connectorNameFromType(ConnectorType type) {
	return Names[type];
}

Connector::ConnectorType Connector::connectorType() {
	if (m_connectorShared != NULL) {
		return m_connectorShared->connectorType();
	}

	return Connector::Unknown;
}

ConnectorShared * Connector::connectorShared() {
	return m_connectorShared;
}

void Connector::addViewItem(ConnectorItem * item) {
	//item->debugInfo(QString("add view item c:%1 ci:%2 b:%3").arg((long) this, 0, 16).arg((long) item, 0, 16).arg((long) m_bus.data(), 0, 16));
	m_connectorItems.insert(QuickHash(item->attachedToViewIdentifier(), item->attachedToViewLayerID()), item);	
}

void Connector::removeViewItem(ConnectorItem * item) {

	//DebugDialog::debug(QString("remove view item c:%1 ci:%2 b:%3").arg((long) this, 0, 16).arg((long) item, 0, 16).arg((long) m_bus.data(), 0, 16));
	m_connectorItems.remove(QuickHash(item->attachedToViewIdentifier(), item->attachedToViewLayerID()));
}

void Connector::connectTo(Connector * connector) {

	if (this->modelPart() == NULL) {
		DebugDialog::debug("connecting bus connector 1");
	}
	else if (connector->modelPart() == NULL) {
		DebugDialog::debug("connecting bus connector 2");
	}

	if (!m_toConnectors.contains(connector)) {
		m_toConnectors.append(connector);
	}
	if (!connector->m_toConnectors.contains(this)) {
		connector->m_toConnectors.append(this);
	}
}

void Connector::disconnectFrom(Connector * connector) {
	m_toConnectors.removeOne(connector);
	connector->m_toConnectors.removeOne(this);
}

void Connector::saveAsPart(QXmlStreamWriter & writer) {
	writer.writeStartElement("connector");
	writer.writeAttribute("id", connectorShared()->id());
	writer.writeAttribute("type", connectorShared()->connectorTypeString());
	writer.writeAttribute("name", connectorShared()->sharedName());
	writer.writeTextElement("description", connectorShared()->description());
	writer.writeStartElement("views");
	QMultiHash<ViewIdentifierClass::ViewIdentifier,SvgIdLayer *> pins = m_connectorShared->pins();
	foreach (ViewIdentifierClass::ViewIdentifier currView, pins.uniqueKeys()) {
		writer.writeStartElement(ViewIdentifierClass::viewIdentifierXmlName(currView));
		foreach (SvgIdLayer * svgIdLayer, pins.values(currView)) {
			writer.writeStartElement("p");
			writeLayerAttr(writer, svgIdLayer->m_svgViewLayerID);
			writeSvgIdAttr(writer, currView, svgIdLayer->m_svgId);
			writeTerminalIdAttr(writer, currView, svgIdLayer->m_terminalId);
			writer.writeEndElement();
		}
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeEndElement();
}

void Connector::writeLayerAttr(QXmlStreamWriter &writer, ViewLayer::ViewLayerID viewLayerID) {
	writer.writeAttribute("layer",ViewLayer::viewLayerXmlNameFromID(viewLayerID));
}

void Connector::writeSvgIdAttr(QXmlStreamWriter &writer, ViewIdentifierClass::ViewIdentifier view, QString connId) {
	Q_UNUSED(view);
	writer.writeAttribute("svgId",connId);
}

void Connector::writeTerminalIdAttr(QXmlStreamWriter &writer, ViewIdentifierClass::ViewIdentifier view, QString terminalId) {
        if((view == ViewIdentifierClass::BreadboardView || view == ViewIdentifierClass::SchematicView)
            &&
           (!terminalId.isNull() && !terminalId.isEmpty()) ) {
		writer.writeAttribute("terminalId",terminalId);
	} else {
		return;
	}
}

const QList<Connector *> & Connector::toConnectors() {
	return m_toConnectors;
}

ConnectorItem * Connector::connectorItemByViewLayerID(ViewIdentifierClass::ViewIdentifier viewIdentifier, ViewLayer::ViewLayerID viewLayerID) {
	return m_connectorItems.value(QuickHash(viewIdentifier, viewLayerID), NULL);
}

bool Connector::connectionIsAllowed(Connector* that)
{
	Connector::ConnectorType thisConnectorType = connectorType();
	Connector::ConnectorType thatConnectorType = that->connectorType();
	if (thisConnectorType == Connector::Unknown) return false;
	if (thatConnectorType == Connector::Unknown) return false;		// unknowns are celebate

	if (thisConnectorType == Connector::Wire) return true;
	if (thatConnectorType == Connector::Wire) return true;		// wires are bisexual

	return (thisConnectorType != thatConnectorType);		// otherwise heterosexual
}

const QString & Connector::connectorSharedID() {
	if (m_connectorShared == NULL) return ___emptyString___;

	return m_connectorShared->id();
}

const QString & Connector::connectorSharedName() {
	if (m_connectorShared == NULL) return ___emptyString___;

	if (!m_connectorLocalName.isEmpty()) {
		return m_connectorLocalName;
	}

	return m_connectorShared->sharedName();
}

const QString & Connector::connectorSharedDescription() {
	if (m_connectorShared == NULL) return ___emptyString___;

	return m_connectorShared->description();
}

ErcData * Connector::connectorSharedErcData() {
	if (m_connectorShared == NULL) return NULL;

	return m_connectorShared->ercData();
}

const QString & Connector::busID() {
	if (m_bus == NULL) return ___emptyString___;

	return m_bus->id();
}

Bus * Connector::bus() {
	return m_bus;
}

void Connector::setBus(Bus * bus) {
	m_bus = bus;
}

void Connector::unprocess(ViewIdentifierClass::ViewIdentifier viewIdentifier, ViewLayer::ViewLayerID viewLayerID) {
	SvgIdLayer * svgIdLayer = m_connectorShared->fullPinInfo(viewIdentifier, viewLayerID);
	if (svgIdLayer != NULL) {
		svgIdLayer->m_processed = false;
	}
}

SvgIdLayer * Connector::fullPinInfo(ViewIdentifierClass::ViewIdentifier viewIdentifier, ViewLayer::ViewLayerID viewLayerID) {
	if (m_connectorShared == NULL) return NULL;

	return m_connectorShared->fullPinInfo(viewIdentifier, viewLayerID);
}

long Connector::modelIndex() {
	if (m_modelPart != NULL) return m_modelPart->modelIndex();

	DebugDialog::debug(QString("saving bus connector item: how is this supposed to work?"));
	return 0;
}


ModelPart * Connector::modelPart() {
	return m_modelPart;
}

int Connector::connectorItemCount() {
	return m_connectorItems.count();
}

QList< QPointer<ConnectorItem> > Connector::viewItems() {
	return m_connectorItems.values();
}

const QString & Connector::legID(ViewIdentifierClass::ViewIdentifier viewID, ViewLayer::ViewLayerID viewLayerID) {
	if (m_connectorShared) return m_connectorShared->legID(viewID, viewLayerID);

	return ___emptyString___;
}

void Connector::setConnectorLocalName(const QString & name) {
	if (m_connectorShared != NULL && name.compare(m_connectorShared->sharedName()) == 0) {
		m_connectorLocalName.clear();
		return;
	}

	m_connectorLocalName = name;
}

const QString & Connector::connectorLocalName() {
	return m_connectorLocalName;
}
