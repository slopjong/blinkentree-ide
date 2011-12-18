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

#include "hole.h"
#include "../utils/graphicsutils.h"
#include "../fsvgrenderer.h"
#include "../sketch/infographicsview.h"
#include "../svg/svgfilesplitter.h"
#include "../commands.h"
#include "../utils/textutils.h"
#include "../viewlayer.h"
#include "partlabel.h"
#include "../connectors/nonconnectoritem.h"
#include "../connectors/svgidlayer.h"

#include <QDomNodeList>
#include <QDomDocument>
#include <QDomElement>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QGroupBox>
#include <QSettings>

static const int rowHeight = 21;

QHash<QString, QString> Hole::HoleSizes;
const QString Hole::AutorouteViaHoleSize = "autorouteViaHoleSize";
const QString Hole::AutorouteViaRingThickness = "autorouteViaRingThickness";

Hole::Hole( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel)
	: PaletteItem(modelPart, viewIdentifier, viewGeometry, id, itemMenu, doLabel)
{
	initHoleSettings(m_holeSettings);

	if (HoleSizes.count() == 0) {
		setUpHoleSizes();
	}

	QString holeSize = modelPart->prop("hole size").toString();
	QStringList sizes = holeSize.split(",");
	if (sizes.count() != 2) {
		holeSize = HoleSizes.value(holeSize, "");
		if (holeSize.isEmpty()) {
			holeSize = modelPart->properties().value("hole size", ".035in,0.2in");
			modelPart->setProp("hole size", holeSize);
		}
		sizes = holeSize.split(",");
	}

	m_holeSettings.holeDiameter = sizes.at(0);
	m_holeSettings.ringThickness = sizes.at(1);

	m_otherLayerRenderer = m_extraRenderer = NULL;
}

Hole::~Hole() {
}

void Hole::initHoleSettings(HoleSettings & holeSettings) 
{
	holeSettings.diameterEdit = holeSettings.thicknessEdit = NULL;
	holeSettings.diameterValidator = holeSettings.thicknessValidator = NULL;
	holeSettings.unitsComboBox = holeSettings.sizesComboBox = NULL;
	holeSettings.holeDiameterRange = Hole::holeDiameterRange;
	holeSettings.ringThicknessRange = Hole::ringThicknessRange;
}


void Hole::setUpHoleSizes() {
	QSettings settings;
	QString ringThickness = settings.value(AutorouteViaRingThickness, "").toString();
	QString holeSize = settings.value(AutorouteViaHoleSize, "").toString();

	QFile file(":/resources/vias.xml");

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument domDocument;
	if (!domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		DebugDialog::debug(QString("failed loading properties %1 line:%2 col:%3").arg(errorStr).arg(errorLine).arg(errorColumn));
		return;
	}

	QDomElement root = domDocument.documentElement();
	if (root.isNull()) return;
	if (root.tagName() != "vias") return;

	QDomElement ve = root.firstChildElement("via");
	while (!ve.isNull()) {
		QString rt = ve.attribute("ringthickness");
		QString hs = ve.attribute("holesize");
		QString name = ve.attribute("name");
		if (ve.attribute("default").compare("yes") == 0) {
			if (ringThickness.isEmpty()) settings.setValue(AutorouteViaRingThickness, rt);
			if (holeSize.isEmpty()) settings.setValue(AutorouteViaHoleSize, hs);
		}
		HoleSizes.insert(name, QString("%1,%2").arg(hs).arg(rt));
		ve = ve.nextSiblingElement("via");
	}
}


void Hole::setProp(const QString & prop, const QString & value) {
	if (prop.compare("hole size", Qt::CaseInsensitive) == 0) {
		setHoleSize(value, false);
		return;
	}

	PaletteItem::setProp(prop, value);
}

QString Hole::holeSize() {
	return holeSize(m_holeSettings);
}

QString Hole::holeSize(HoleSettings & holeSettings) {
	return QString("%1,%2").arg(holeSettings.holeDiameter).arg(holeSettings.ringThickness);
}

void Hole::setHoleSize(QString holeSize, bool force) {
	if (setHoleSize(holeSize, force, m_holeSettings)) {
		setBoth(m_holeSettings.holeDiameter, m_holeSettings.ringThickness);
		modelPart()->setProp("hole size", holeSize);

		if (m_partLabel) m_partLabel->displayTextsIf();	
	}
}

bool Hole::setHoleSize(QString & holeSize, bool force, HoleSettings & holeSettings)
{
	QString hashedHoleSize = HoleSizes.value(holeSize);
	QStringList sizes;
	if (hashedHoleSize.isEmpty()) {
		sizes = holeSize.split(",");
	}
	else {
		sizes = hashedHoleSize.split(",");
		holeSize = sizes[0] + "," + sizes[1];
	}
	if (sizes.count() < 2) return false;

	if (!force && (holeSettings.holeDiameter.compare(sizes.at(0)) == 0) && (holeSettings.ringThickness.compare(sizes.at(1)) == 0)) 
	{
		return false;
	}

	holeSettings.holeDiameter = sizes.at(0);
	holeSettings.ringThickness = sizes.at(1);
	updateEditTexts(holeSettings);
	updateValidators(holeSettings);
	updateSizes(holeSettings);
	return true;
}

void Hole::setBoth(const QString & holeDiameter, const QString & ringThickness) {
	if (this->m_viewIdentifier != ViewIdentifierClass::PCBView) return;

	QStringList connectorIDs;
	ItemBase * otherLayer = setBothSvg(holeDiameter, ringThickness, connectorIDs);

	// there's only one NonConnectorItem
	foreach (SvgIdLayer * svgIdLayer, m_extraRenderer->setUpNonConnectors()) {
		if (svgIdLayer == NULL) continue;

		setBothNonConnectors(this, svgIdLayer);
		if (otherLayer != NULL) {
			setBothNonConnectors(otherLayer, svgIdLayer);
		}

		delete svgIdLayer;
	}
}

ItemBase * Hole::setBothSvg(const QString & holeDiameter, const QString & ringThickness, const QStringList & connectorIDs) 
{
	QString svg = makeSvg(holeDiameter, ringThickness, m_viewLayerID);
	loadExtraRenderer(svg.toUtf8());

	QString setColor;
	QStringList noIDs;

	QString osvg;
	ItemBase * otherLayer = NULL;
	foreach (ItemBase * layerKin, m_layerKin) {
		if (layerKin->hasNonConnectors()) {
			otherLayer = layerKin;
			break;
		}
	}

	if (otherLayer) {
		osvg = makeSvg(holeDiameter, ringThickness, otherLayer->viewLayerID());
		if (m_otherLayerRenderer == NULL) {
			m_otherLayerRenderer = new FSvgRenderer(this);
		}

		QByteArray result = m_otherLayerRenderer->loadSvg(osvg.toLatin1(), m_filename, connectorIDs, noIDs,  noIDs, "", "", true);
		if (!result.isEmpty()) {
			qobject_cast<PaletteItemBase *>(otherLayer)->setSharedRendererEx(m_otherLayerRenderer);
		}
	}
	
	return otherLayer;
}

void Hole::setBothNonConnectors(ItemBase * itemBase, SvgIdLayer * svgIdLayer) {
	foreach (QGraphicsItem * child, itemBase->childItems()) {
		NonConnectorItem * nonConnectorItem = dynamic_cast<NonConnectorItem *>(child);
		if (nonConnectorItem == NULL) continue;

		nonConnectorItem->setRect(svgIdLayer->m_rect);
		nonConnectorItem->setRadius(svgIdLayer->m_radius, svgIdLayer->m_strokeWidth);
		break;
	}
}


QString Hole::makeSvg(const QString & holeDiameter, const QString & ringThickness, ViewLayer::ViewLayerID viewLayerID) 
{
	double hd = TextUtils::convertToInches(holeDiameter) * GraphicsUtils::StandardFritzingDPI;
	double rt = TextUtils::convertToInches(ringThickness) * GraphicsUtils::StandardFritzingDPI;

	double wInches = (hd + rt + rt) / GraphicsUtils::StandardFritzingDPI;
	QString svg = TextUtils::makeSVGHeader(1, GraphicsUtils::StandardFritzingDPI, wInches, wInches);

	QString setColor;
	if (viewLayerID == ViewLayer::Copper0) {
		setColor = ViewLayer::Copper0Color;
	}
	else if (viewLayerID == ViewLayer::Copper1) {
		setColor = ViewLayer::Copper1Color;
	}

	svg += QString("<g id='%1'>").arg(ViewLayer::viewLayerXmlNameFromID(viewLayerID));
	
	QString id = makeID();
	if (hd == 0) {
		svg += QString("<circle cx='%1' cy='%1' r='%1' fill='%2' id='%3' />")
					.arg(rt)
					.arg(setColor)
					.arg(id);
	}
	else {
		svg += QString("<circle fill='none' cx='%1' cy='%1' r='%2' stroke-width='%3' stroke='%4' id='%5' />")
			.arg((hd / 2) + rt)
			.arg((hd / 2) + (rt / 2))
			.arg(rt)
			.arg(setColor)
			.arg(id);
		svg += QString("<circle drill='0' fill='black' cx='%1' cy='%1' r='%2' stroke-width='0'  />")   // set the drill attribute to 0 for gerber translation
			.arg((hd / 2) + rt)
			.arg(hd / 2);
	}
  		
	svg += "</g></svg>";
	return svg;
}

QString Hole::makeID() {
	return FSvgRenderer::NonConnectorName + "0";
}

QString Hole::getProperty(const QString & key) {
	if (key.compare("hole size", Qt::CaseInsensitive) == 0) {
		return m_holeSettings.holeDiameter;
	}

	return PaletteItem::getProperty(key);
}

void Hole::addedToScene(bool temporary)
{
	if (this->scene()) {
		setHoleSize(holeSize(m_holeSettings), true);
	}

    return PaletteItem::addedToScene(temporary);
}

bool Hole::hasCustomSVG() {
	switch (m_viewIdentifier) {
		case ViewIdentifierClass::PCBView:
			return true;
		default:
			return ItemBase::hasCustomSVG();
	}
}

ItemBase::PluralType Hole::isPlural() {
	return Plural;
}

QString Hole::retrieveSvg(ViewLayer::ViewLayerID viewLayerID, QHash<QString, QString> & svgHash, bool blackOnly, double dpi) 
{
	if (m_viewIdentifier != ViewIdentifierClass::PCBView || 
		(viewLayerID != ViewLayer::Copper0 && viewLayerID != ViewLayer::Copper1)) 
	{
		return PaletteItemBase::retrieveSvg(viewLayerID, svgHash, blackOnly, dpi);
	}

	QStringList holeSize = m_modelPart->prop("hole size").toString().split(",");
	if (holeSize.length() == 2) {
		QString svg = makeSvg(holeSize.at(0), holeSize.at(1), viewLayerID);
		if (!svg.isEmpty()) {
			QString xmlName = ViewLayer::viewLayerXmlNameFromID(viewLayerID);
			SvgFileSplitter splitter;
			bool result = splitter.splitString(svg, xmlName);
			if (!result) {
				return "";
			}
			result = splitter.normalize(dpi, xmlName, blackOnly);
			if (!result) {
				return "";
			}
			return splitter.elementString(xmlName);
		}
	}

	return PaletteItemBase::retrieveSvg(viewLayerID, svgHash, blackOnly, dpi);
}

bool Hole::collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget) 
{
	if (prop.compare("hole size", Qt::CaseInsensitive) == 0) {
		returnProp = tr("hole size");

		returnValue = m_modelPart->prop("hole size").toString();
		QWidget * frame = createHoleSettings(parent, m_holeSettings, swappingEnabled, returnValue);

		connect(m_holeSettings.sizesComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(changeHoleSize(const QString &)));
		connect(m_holeSettings.unitsComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(changeUnits(const QString &)));
		connect(m_holeSettings.diameterEdit, SIGNAL(editingFinished()), this, SLOT(changeDiameter()));
		connect(m_holeSettings.thicknessEdit, SIGNAL(editingFinished()), this, SLOT(changeThickness()));

		returnWidget = frame;
		return true;
	}

	return PaletteItem::collectExtraInfo(parent, family, prop, value, swappingEnabled, returnProp, returnValue, returnWidget);
}

void Hole::changeThickness() 
{
	if (changeThickness(m_holeSettings, sender())) {
		QLineEdit * edit = qobject_cast<QLineEdit *>(sender());
		changeHoleSize(m_holeSettings.holeDiameter + "," + edit->text() + m_holeSettings.unitsComboBox->currentText());
	}	
}

bool Hole::changeThickness(HoleSettings & holeSettings, QObject * sender) 
{
	QLineEdit * edit = qobject_cast<QLineEdit *>(sender);
	if (edit == NULL) return false;

	double newValue = edit->text().toDouble();
	QString temp = holeSettings.ringThickness;
	temp.chop(2);
	double oldValue = temp.toDouble();
	return (newValue != oldValue);
}

void Hole::changeDiameter() 
{
	if (changeDiameter(m_holeSettings, sender())) {
		QLineEdit * edit = qobject_cast<QLineEdit *>(sender());
		changeHoleSize(edit->text() + m_holeSettings.unitsComboBox->currentText() + "," + m_holeSettings.ringThickness);
	}
}

bool Hole::changeDiameter(HoleSettings & holeSettings, QObject * sender) 
{
	QLineEdit * edit = qobject_cast<QLineEdit *>(sender);
	if (edit == NULL) return false;

	double newValue = edit->text().toDouble();
	QString temp = holeSettings.holeDiameter;
	temp.chop(2);
	double oldValue = temp.toDouble();
	return (newValue != oldValue);
}

void Hole::changeUnits(const QString & units) 
{
	QString newVal = changeUnits(units, m_holeSettings);
	modelPart()->setProp("hole size", newVal);
}

QString Hole::changeUnits(const QString & units, HoleSettings & holeSettings) 
{
	double hd = TextUtils::convertToInches(holeSettings.holeDiameter);
	double rt = TextUtils::convertToInches(holeSettings.ringThickness);
	QString newVal;
	if (units == "in") {
		newVal = QString("%1in,%2in").arg(hd).arg(rt);
	}
	else {
		newVal = QString("%1mm,%2mm").arg(hd * 25.4).arg(rt * 25.4);
	}

	QStringList sizes = newVal.split(",");
	holeSettings.ringThickness = sizes.at(1);
	holeSettings.holeDiameter = sizes.at(0);

	updateValidators(holeSettings);
	updateSizes(holeSettings);
	updateEditTexts(holeSettings);
	
	return newVal;
}

void Hole::updateValidators(HoleSettings & holeSettings)
{
	if (holeSettings.diameterValidator == NULL) return;
	if (holeSettings.thicknessValidator == NULL) return;
	if (holeSettings.unitsComboBox == NULL) return;

	QString units = holeSettings.unitsComboBox->currentText();
	QPointF hdRange = holeSettings.holeDiameterRange(holeSettings.ringThickness);
	QPointF rtRange = holeSettings.ringThicknessRange(holeSettings.holeDiameter);

	double multiplier = (units == "mm") ? 25.4 : 1.0;
	holeSettings.diameterValidator->setRange(hdRange.x() * multiplier, hdRange.y() * multiplier, 3);
	holeSettings.thicknessValidator->setRange(rtRange.x() * multiplier, rtRange.y() * multiplier, 3);
}


QPointF Hole::ringThicknessRange(const QString & holeDiameter) {
	double hd = TextUtils::convertToInches(holeDiameter);
	QPointF p(hd > 0 ? 0 : .001, 1.0);
	return p;
}

QPointF Hole::holeDiameterRange(const QString & ringThickness) {
	double rt = TextUtils::convertToInches(ringThickness);
	QPointF p(rt > 0 ? 0 : .001, 1.0);
	return p;
}


void Hole::changeHoleSize(const QString & newSize) {
	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->setProp(this, "hole size", tr("hole size"), this->holeSize(), newSize, true);
	}
}

void Hole::updateEditTexts(HoleSettings & holeSettings) {
	if (holeSettings.diameterEdit == NULL) return;
	if (holeSettings.thicknessEdit == NULL) return;
	if (holeSettings.unitsComboBox == NULL) return;

	double hd = TextUtils::convertToInches(holeSettings.holeDiameter);
	double rt = TextUtils::convertToInches(holeSettings.ringThickness);

	QString newVal;
	if (holeSettings.unitsComboBox->currentText() == "in") {
		newVal = QString("%1,%2").arg(hd).arg(rt);
	}
	else {
		newVal = QString("%1,%2").arg(hd * 25.4).arg(rt * 25.4);
	}

	QStringList sizes = newVal.split(",");
	holeSettings.diameterEdit->setText(sizes.at(0));
	holeSettings.thicknessEdit->setText(sizes.at(1));
}

void Hole::updateSizes(HoleSettings &  holeSettings) {
	if (holeSettings.sizesComboBox == NULL) return;

	int newIndex = -1;

	QPointF current(TextUtils::convertToInches(holeSettings.holeDiameter), TextUtils::convertToInches(holeSettings.ringThickness));
	for (int ix = 0; ix < holeSettings.sizesComboBox->count(); ix++) {
		QString key = holeSettings.sizesComboBox->itemText(ix);
		QString value = HoleSizes.value(key, "");
		QStringList sizes;
		if (value.isEmpty()) {
			sizes = key.split(",");
		}
		else {
			sizes = value.split(",");
		}
		if (sizes.count() < 2) continue;

		QPointF p(TextUtils::convertToInches(sizes.at(0)), TextUtils::convertToInches(sizes.at(1)));
		if (p == current) {
			newIndex = ix;
			break;
		}
	}

	if (newIndex < 0) {
		QString newItem = holeSettings.holeDiameter + "," + holeSettings.ringThickness;
		holeSettings.sizesComboBox->addItem(newItem);
		newIndex = holeSettings.sizesComboBox->findText(newItem);

		HoleSizes.insert(newItem, newItem);
	}

	// don't want to trigger another undo command
	bool wasBlocked = holeSettings.sizesComboBox->blockSignals(true);
	holeSettings.sizesComboBox->setCurrentIndex(newIndex);
	holeSettings.sizesComboBox->blockSignals(wasBlocked);
}

bool Hole::canEditPart() {
	return false;
}

bool Hole::hasPartNumberProperty()
{
	return false;
}

QWidget * Hole::createHoleSettings(QWidget * parent, HoleSettings & holeSettings, bool swappingEnabled, const QString & currentHoleSize) {
	QFrame * frame = new QFrame(parent);
	frame->setObjectName("infoViewPartFrame");

	QVBoxLayout * vBoxLayout = new QVBoxLayout(frame);
	vBoxLayout->setMargin(0);
	vBoxLayout->setContentsMargins(0, 0, 0, 0);
	vBoxLayout->setSpacing(0);

	holeSettings.sizesComboBox = new QComboBox(frame);
	holeSettings.sizesComboBox->setEditable(false);
	holeSettings.sizesComboBox->setObjectName("infoViewComboBox");

	vBoxLayout->addWidget(holeSettings.sizesComboBox);

    QFrame * hFrame = new QFrame(frame);
    QHBoxLayout * hLayout = new QHBoxLayout(hFrame);
	hLayout->setMargin(0);

	QGroupBox * subFrame = new QGroupBox(tr("advanced settings"), frame);
	subFrame->setObjectName("infoViewGroupBox");

	QGridLayout * gridLayout = new QGridLayout(subFrame);
	gridLayout->setMargin(0);

	holeSettings.unitsComboBox = new QComboBox(subFrame);
    holeSettings.unitsComboBox->setMaximumWidth(60);
	holeSettings.unitsComboBox->setMinimumHeight(rowHeight);
	holeSettings.unitsComboBox->setMaximumHeight(rowHeight);
	holeSettings.unitsComboBox->setEditable(false);
	holeSettings.unitsComboBox->addItem("mm");
	holeSettings.unitsComboBox->addItem("in");
	gridLayout->addWidget(holeSettings.unitsComboBox, 0, 2, 2, 1);
	holeSettings.unitsComboBox->setObjectName("infoViewComboBox");

	holeSettings.diameterEdit = new QLineEdit(subFrame);
	holeSettings.diameterEdit->setMinimumHeight(rowHeight);
	holeSettings.diameterValidator = new QDoubleValidator(holeSettings.diameterEdit);
	holeSettings.diameterValidator->setNotation(QDoubleValidator::StandardNotation);
	holeSettings.diameterEdit->setValidator(holeSettings.diameterValidator);
	gridLayout->addWidget(holeSettings.diameterEdit, 0, 1);
	holeSettings.diameterEdit->setObjectName("infoViewLineEdit");

	QLabel * label = new QLabel(tr("Hole Diameter"));
	label->setMinimumHeight(rowHeight);
	label->setObjectName("infoViewLabel");
	gridLayout->addWidget(label, 0, 0);

	holeSettings.thicknessEdit = new QLineEdit(subFrame);
	holeSettings.thicknessEdit->setMinimumHeight(rowHeight);
	holeSettings.thicknessValidator = new QDoubleValidator(holeSettings.thicknessEdit);
	holeSettings.thicknessValidator->setNotation(QDoubleValidator::StandardNotation);
	holeSettings.thicknessEdit->setValidator(holeSettings.thicknessValidator);
	gridLayout->addWidget(holeSettings.thicknessEdit, 1, 1);
	holeSettings.thicknessEdit->setObjectName("infoViewLineEdit");

	label = new QLabel(tr("Ring Thickness"));
	label->setMinimumHeight(rowHeight);
	gridLayout->addWidget(label, 1, 0);
	label->setObjectName("infoViewLabel");

	gridLayout->setContentsMargins(10, 2, 0, 2);
	gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 3);
	gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);

    hLayout->addWidget(subFrame);
    hLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum));
    vBoxLayout->addWidget(hFrame);

	holeSettings.sizesComboBox->addItems(HoleSizes.keys());
	holeSettings.sizesComboBox->setEnabled(swappingEnabled);
	holeSettings.unitsComboBox->setEnabled(swappingEnabled);
	holeSettings.unitsComboBox->setCurrentIndex(currentHoleSize.contains("in") ? 1 : 0);

	holeSettings.diameterEdit->setEnabled(swappingEnabled);
	holeSettings.thicknessEdit->setEnabled(swappingEnabled);

	updateEditTexts(holeSettings);
	updateValidators(holeSettings);
	updateSizes(holeSettings);

	return frame;
}

bool Hole::rotationAllowed() {
	return false;
}

bool Hole::rotation45Allowed() {
	return false;
}

bool Hole::canFindConnectorsUnder() {
	return false;
}
