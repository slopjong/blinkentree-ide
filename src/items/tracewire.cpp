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

$Revision: 5375 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-08 21:38:25 +0200 (Mon, 08 Aug 2011) $

********************************************************************/

#include "tracewire.h"
#include "../sketch/infographicsview.h"
#include "../connectors/connectoritem.h"
#include "../utils/focusoutcombobox.h"


#include <QComboBox>

const int TraceWire::MinTraceWidthMils = 8;
const int TraceWire::MaxTraceWidthMils = 128;

/////////////////////////////////////////////////////////

TraceWire::TraceWire( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier,  const ViewGeometry & viewGeometry, long id, QMenu * itemMenu  ) 
	: ClipableWire(modelPart, viewIdentifier,  viewGeometry,  id, itemMenu, true)
{
	m_canChainMultiple = true;
	m_wireDirection = TraceWire::NoDirection;
}


TraceWire::~TraceWire() 	
{
}

QComboBox * TraceWire::createWidthComboBox(double m, QWidget * parent) 
{
	QComboBox * comboBox = new FocusOutComboBox(parent);  // new QComboBox(parent);
	comboBox->setEditable(true);
	QIntValidator * intValidator = new QIntValidator(comboBox);
	intValidator->setRange(MinTraceWidthMils, MaxTraceWidthMils);
	comboBox->setValidator(intValidator);

	int ix = 0;
	if (!Wire::widths.contains(m)) {
		Wire::widths.append(m);
		qSort(Wire::widths.begin(), Wire::widths.end());
	}
	foreach(long widthValue, Wire::widths) {
		QString widthName = Wire::widthTrans.value(widthValue, "");
		QVariant val((int) widthValue);
		comboBox->addItem(widthName.isEmpty() ? QString::number(widthValue) : widthName, val);
		if (qAbs(m - widthValue) < .01) {
			comboBox->setCurrentIndex(ix);
		}
		ix++;
	}

	return comboBox;

}


bool TraceWire::collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget)
{
	if (prop.compare("width", Qt::CaseInsensitive) == 0) {
		returnProp = tr("width");
		QComboBox * comboBox = createWidthComboBox(mils(), parent);
		comboBox->setEnabled(swappingEnabled);
		comboBox->setObjectName("infoViewComboBox");

		connect(comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(widthEntry(const QString &)));
		returnWidget = comboBox;
		returnValue = comboBox->currentText();

		return true;
	}

	return ClipableWire::collectExtraInfo(parent, family, prop, value, swappingEnabled, returnProp, returnValue, returnWidget);
}


void TraceWire::widthEntry(const QString & text) {

	int w = widthEntry(text, sender());
	if (w == 0) return;

	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->changeWireWidthMils(QString::number(w));
	}
}

int TraceWire::widthEntry(const QString & text, QObject * sender) {

	QComboBox * comboBox = qobject_cast<QComboBox *>(sender);
	if (comboBox == NULL) return 0;

	int w = comboBox->itemData(comboBox->currentIndex()).toInt();
	if (w == 0) {
		// user typed in a number
		w = text.toInt();
	}
	if (!Wire::widths.contains(w)) {
		Wire::widths.append(w);
		qSort(Wire::widths.begin(), Wire::widths.end());
	}

	return w;
}

void TraceWire::setColorFromElement(QDomElement & element) {
	switch (m_viewLayerID) {
		case ViewLayer::Copper0Trace:
			element.setAttribute("color", ViewLayer::Copper0Color);
			break;
		case ViewLayer::Copper1Trace:
			element.setAttribute("color", ViewLayer::Copper1Color);
			break;
		case ViewLayer::SchematicTrace:
			//element.setAttribute("color", "#000000");
		default:
			break;
	}

	Wire::setColorFromElement(element);	
}

bool TraceWire::canSwitchLayers() {
	QList<Wire *> wires;
	QList<ConnectorItem *> ends;
	collectChained(wires, ends);
	if (ends.count() < 2) return false;   // should never happen, since traces have to be connected at both ends

	foreach (ConnectorItem * end, ends) {
		if (end->getCrossLayerConnectorItem() == NULL) return false;
	}

	return true;
}

void TraceWire::setWireDirection(TraceWire::WireDirection wireDirection) {
	m_wireDirection = wireDirection;
}

TraceWire::WireDirection TraceWire::wireDirection() {
	return m_wireDirection;
}

TraceWire * TraceWire::getTrace(ConnectorItem * connectorItem)
{
	return qobject_cast<TraceWire *>(connectorItem->attachedTo());
}

void TraceWire::setSchematic(bool schematic) {
	m_viewGeometry.setSchematicTrace(schematic);
}
