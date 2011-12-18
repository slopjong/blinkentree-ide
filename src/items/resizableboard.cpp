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

#include "resizableboard.h"
#include "../utils/resizehandle.h"
#include "../utils/graphicsutils.h"
#include "../fsvgrenderer.h"
#include "../sketch/infographicsview.h"
#include "../svg/svgfilesplitter.h"
#include "../commands.h"
#include "moduleidnames.h"
#include "../layerattributes.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QRegExp>
#include <qmath.h>

static QString BoardLayerTemplate = "";
static QString SilkscreenLayerTemplate = "";
static const int LineThickness = 8;
static const QRegExp HeightExpr("height=\\'\\d*px");

QString ResizableBoard::customShapeTranslated;


QString Board::oneLayerTranslated;
QString Board::twoLayersTranslated;

Board::Board( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel)
	: PaletteItem(modelPart, viewIdentifier, viewGeometry, id, itemMenu, doLabel)
{
}

Board::~Board() {
}

void Board::paintHover(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	Q_UNUSED(option);
	painter->save();
	painter->setOpacity(0);
	painter->fillPath(this->hoverShape(), QBrush(hoverColor));
	painter->restore();
}


QStringList Board::collectValues(const QString & family, const QString & prop, QString & value) {
	if (prop.compare("layers", Qt::CaseInsensitive) == 0) {
		QStringList result;
		if (oneLayerTranslated.isEmpty()) {
			oneLayerTranslated = tr("one layer (single-sided)");
		}
		if (twoLayersTranslated.isEmpty()) {
			twoLayersTranslated = tr("two layers (double-sided)");
		}

		result.append(oneLayerTranslated);
		result.append(twoLayersTranslated);

		if (value == "1") {
			value = oneLayerTranslated;
		}
		else if (value == "2") {
			value = twoLayersTranslated;
		}

		return result;
	}

	return PaletteItem::collectValues(family, prop, value);
}


bool Board::rotation45Allowed() {
	return false;
}

bool Board::stickyEnabled() {
	return false;
}

ItemBase::PluralType Board::isPlural() {
	return Plural;
}

bool Board::canFindConnectorsUnder() {
	return false;
}

///////////////////////////////////////////////////////////

ResizableBoard::ResizableBoard( ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const ViewGeometry & viewGeometry, long id, QMenu * itemMenu, bool doLabel)
	: Board(modelPart, viewIdentifier, viewGeometry, id, itemMenu, doLabel)
{
	m_keepAspectRatio = false;
	m_widthEditor = m_heightEditor = NULL;

	m_resizeGripTL = m_resizeGripTR = m_resizeGripBL = m_resizeGripBR = NULL;

	m_silkscreenRenderer = NULL;
	m_inResize = NULL;

}

ResizableBoard::~ResizableBoard() {
}

QVariant ResizableBoard::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
		case ItemSelectedChange:
			if (m_resizeGripBL) {
				m_resizeGripBL->setVisible(value.toBool());
				m_resizeGripBR->setVisible(value.toBool());
				m_resizeGripTL->setVisible(value.toBool());
				m_resizeGripTR->setVisible(value.toBool());
			}
			break;
		default:
			break;
   	}

    return Board::itemChange(change, value);
}


void ResizableBoard::addedToScene(bool temporary) {
	loadTemplates();
	if (this->scene()) {
		if (hasGrips()) {
			m_resizeGripTL = new ResizeHandle(QPixmap(":/resources/images/itemselection/cornerHandlerActiveTopLeft.png"), Qt::SizeFDiagCursor, this);
			connect(m_resizeGripTL, SIGNAL(mousePressSignal(QGraphicsSceneMouseEvent *, ResizeHandle *)), this, SLOT(handleMousePressSlot(QGraphicsSceneMouseEvent *, ResizeHandle *)));
			m_resizeGripTR = new ResizeHandle(QPixmap(":/resources/images/itemselection/cornerHandlerActiveTopRight.png"), Qt::SizeBDiagCursor, this);
			connect(m_resizeGripTR, SIGNAL(mousePressSignal(QGraphicsSceneMouseEvent *, ResizeHandle *)), this, SLOT(handleMousePressSlot(QGraphicsSceneMouseEvent *, ResizeHandle *)));
			m_resizeGripBL = new ResizeHandle(QPixmap(":/resources/images/itemselection/cornerHandlerActiveBottomLeft.png"), Qt::SizeBDiagCursor, this);
			connect(m_resizeGripBL, SIGNAL(mousePressSignal(QGraphicsSceneMouseEvent *, ResizeHandle *)), this, SLOT(handleMousePressSlot(QGraphicsSceneMouseEvent *, ResizeHandle *)));
			m_resizeGripBR = new ResizeHandle(QPixmap(":/resources/images/itemselection/cornerHandlerActiveBottomRight.png"), Qt::SizeFDiagCursor, this);
			connect(m_resizeGripBR, SIGNAL(mousePressSignal(QGraphicsSceneMouseEvent *, ResizeHandle *)), this, SLOT(handleMousePressSlot(QGraphicsSceneMouseEvent *, ResizeHandle *)));
			connect(m_resizeGripTL, SIGNAL(zoomChangedSignal(double)), this, SLOT(handleZoomChangedSlot(double)));
		}
		if (m_resizeGripBL) {
			setInitialSize();
			positionGrips();
		}
	}

	PaletteItem::addedToScene(temporary);
}

void ResizableBoard::loadTemplates() {
	if (BoardLayerTemplate.isEmpty()) {
		QFile file(":/resources/templates/resizableBoard_boardLayerTemplate.txt");
		file.open(QFile::ReadOnly);
		BoardLayerTemplate = file.readAll();
		file.close();
	}
	if (SilkscreenLayerTemplate.isEmpty()) {
		QFile file(":/resources/templates/resizableBoard_silkscreenLayerTemplate.txt");
		file.open(QFile::ReadOnly);
		SilkscreenLayerTemplate = file.readAll();
		file.close();
	}
}

double ResizableBoard::minWidth() {
	return 0.5 * FSvgRenderer::printerScale();
}

double ResizableBoard::minHeight() {
	return 0.5 * FSvgRenderer::printerScale();
}

bool ResizableBoard::hasGrips() {
	return moduleID().contains(ModuleIDNames::RectangleModuleIDName);
}

void ResizableBoard::mouseMoveEvent(QGraphicsSceneMouseEvent * event) {
	if (m_inResize == NULL) {
		Board::mouseMoveEvent(event);
		return;
	}

	QRectF rect = boundingRect();
	rect.moveTopLeft(this->pos());

	double oldX1 = rect.x();
	double oldY1 = rect.y();
	double oldX2 = oldX1+rect.width();
	double oldY2 = oldY1+rect.height();
	double newX = event->scenePos().x() + m_inResize->resizeOffset().x();
	double newY = event->scenePos().y() + m_inResize->resizeOffset().y();
	QRectF newR;
	
	double minW = minWidth();
	double minH = minHeight();

	if (m_inResize == m_resizeGripBR) {
		if (newX - oldX1 < minW) {
			newX = oldX1 + minW;
		}
		if (newY - oldY1 < minH) {
			newY = oldY1 + minH;
		}

		if (m_keepAspectRatio) {
			double w = (newY - oldY1) * m_aspectRatio.width() / m_aspectRatio.height();
			double h = (newX - oldX1) * m_aspectRatio.height() / m_aspectRatio.width();
			if (qAbs(w + oldX1 - newX) <= qAbs(h + oldY1 - newY)) {
				newX = oldX1 + w;
			}
			else {
				newY = oldY1 + h;
			}
		}

		newR.setRect(0, 0, newX - oldX1, newY - oldY1);
	}
	else if (m_inResize == m_resizeGripTL) {
		oldX2 = m_originalRect.left() + m_originalRect.width();
		oldY2 = m_originalRect.top() + m_originalRect.height();

		if (oldX2 - newX < minW) {
			newX = oldX2 - minW;
		}
		if (oldY2 - newY < minH) {
			newY = oldY2 - minH;
		}

		QPointF p(newX, newY);
		if (p != this->pos()) {
			this->setPos(p);
		}

		if (m_keepAspectRatio) {
			double w = (oldY2 - newY) * m_aspectRatio.width() / m_aspectRatio.height();
			double h = (oldX2 - newX) * m_aspectRatio.height() / m_aspectRatio.width();
			if (qAbs(w + newX - oldX2) <= qAbs(h + newY - oldY2)) {
				oldX2 = newX + w;
			}
			else {
				oldY2 = newY + h;
			}
		}

		newR.setRect(0, 0, oldX2 - newX, oldY2 - newY);
	}
	else if (m_inResize == m_resizeGripTR) {
		if (newX - oldX1 < minW) {
			newX = oldX1 + minW;
		}

		oldY2 = m_originalRect.top() + m_originalRect.height();
		if (oldY2 - newY < minH) {
			newY = oldY2 - minH;
		}

		QPointF p(oldX1, newY);
		if (p != this->pos()) {
			this->setPos(p);
		}

		if (m_keepAspectRatio) {
			double w = (oldY2 - newY) * m_aspectRatio.width() / m_aspectRatio.height();
			double h = (newX - oldX1) * m_aspectRatio.height() / m_aspectRatio.width();
			if (qAbs(w + newX - oldX1) <= qAbs(h + newY - oldY2)) {
				newX = oldX1 + w;
			}
			else {
				oldY2 = newY + h;
			}
		}

		newR.setRect(0, 0, newX - oldX1, oldY2 - newY);
		//DebugDialog::debug(QString("new rect %1 %2 %3").arg(newY).arg(newR.height()).arg(newY + newR.height()));
	}
	else if (m_inResize == m_resizeGripBL) {
		oldX2 = m_originalRect.left() + m_originalRect.width();
		if (oldX2 - newX < minW) {
			newX = oldX2 - minW;
		}
		if (newY - oldY1 < minH) {
			newY = oldY1 + minH;
		}

		QPointF p(newX, oldY1);
		if (p != this->pos()) {
			this->setPos(p);
		}

		if (m_keepAspectRatio) {
			double w = (newY - oldY1) * m_aspectRatio.width() / m_aspectRatio.height();
			double h = (oldX2 - newX) * m_aspectRatio.height() / m_aspectRatio.width();
			if (qAbs(w + oldX2 - newX) <= qAbs(h + oldY1 - newY)) {
				oldX2 = newX + w;
			}
			else {
				newY = oldY1 + h;
			}
		}


		newR.setRect(0, 0, oldX2 - newX, newY - oldY1);
	}

	LayerHash lh;
	resizePixels(newR.width(), newR.height(), lh);
	event->accept();
}

void ResizableBoard::mouseReleaseEvent(QGraphicsSceneMouseEvent * event) {
	if (m_inResize == NULL) {
		Board::mouseReleaseEvent(event);
		return;
	}

	this->ungrabMouse();
	event->accept();
	m_inResize = NULL;

	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView) {
		infoGraphicsView->viewItemInfo(this);
	}
}

void ResizableBoard::handleMousePressSlot(QGraphicsSceneMouseEvent * event, ResizeHandle * resizeHandle)
{
	if (m_spaceBarWasPressed) return;

	m_originalRect = boundingRect();
	m_originalRect.moveTopLeft(this->pos());

	if (resizeHandle == m_resizeGripBR) {
		QSizeF sz = this->boundingRect().size();
		resizeHandle->setResizeOffset(this->pos() + QPointF(sz.width(), sz.height()) - event->scenePos());
	}
	else if (resizeHandle == m_resizeGripTL) {
		resizeHandle->setResizeOffset(this->pos() - event->scenePos());
	}
	else if (resizeHandle == m_resizeGripTR) {
		resizeHandle->setResizeOffset(QPointF(this->pos().x() + this->boundingRect().width(), this->pos().y())  - event->scenePos());
	}
	else if (resizeHandle == m_resizeGripBL) {
		resizeHandle->setResizeOffset(QPointF(this->pos().x(), this->pos().y() + this->boundingRect().height())  - event->scenePos());
	}

	m_inResize = resizeHandle;
	this->grabMouse();

	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView) {
		setInitialSize();
		infoGraphicsView->viewItemInfo(this);
	}
}

void ResizableBoard::handleZoomChangedSlot(double scale) {
	Q_UNUSED(scale);
	positionGrips();
}

void ResizableBoard::positionGrips() {
	if (m_resizeGripBL == NULL) return;

	// TODO:  figure out how to position these on a rotated board

	QSizeF sz = this->boundingRect().size();
	double scale = m_resizeGripBL->currentScale();

	// assuming all the handles are the same size, offset to the center
	QSizeF hsz = m_resizeGripBL->boundingRect().size();
	double dx = hsz.width() / (scale * 2);
	double dy = hsz.height() / (scale * 2);

	m_resizeGripBR->setPos(sz.width() - dx, sz.height() - dy);
	m_resizeGripBL->setPos(-dx, sz.height() - dy);
	m_resizeGripTR->setPos(sz.width() - dx, -dy);
	m_resizeGripTL->setPos(-dx, -dy);
}

bool ResizableBoard::setUpImage(ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier, const LayerHash & viewLayers, ViewLayer::ViewLayerID viewLayerID, ViewLayer::ViewLayerSpec viewLayerSpec, bool doConnectors, LayerAttributes & layerAttributes, QString & error)
{
	bool result = Board::setUpImage(modelPart, viewIdentifier, viewLayers, viewLayerID, viewLayerSpec, doConnectors, layerAttributes, error);
	if ((viewIdentifier == theViewIdentifier()) && result) {
		positionGrips();
	}

	return result;
}

ViewIdentifierClass::ViewIdentifier ResizableBoard::theViewIdentifier() {
	return ViewIdentifierClass::PCBView;
}

void ResizableBoard::resizePixels(double w, double h, const LayerHash & viewLayers) {
	resizeMM(GraphicsUtils::pixels2mm(w, FSvgRenderer::printerScale()), GraphicsUtils::pixels2mm(h, FSvgRenderer::printerScale()), viewLayers);
}

void ResizableBoard::resizeMM(double mmW, double mmH, const LayerHash & viewLayers) {
	if (mmW == 0 || mmH == 0) {
		QString error;
		LayerAttributes layerAttributes;
		setUpImage(modelPart(), m_viewIdentifier, viewLayers, m_viewLayerID, m_viewLayerSpec, true, layerAttributes, error);
		modelPart()->setProp("height", QVariant());
		modelPart()->setProp("width", QVariant());
		// do the layerkin
		positionGrips();
		return;
	}

	QRectF r = this->boundingRect();
	if (qAbs(GraphicsUtils::pixels2mm(r.width(), FSvgRenderer::printerScale()) - mmW) < .001 &&
		qAbs(GraphicsUtils::pixels2mm(r.height(), FSvgRenderer::printerScale()) - mmH) < .001) 
	{
		positionGrips();
		return;
	}

	resizeMMAux(mmW, mmH);
}


void ResizableBoard::resizeMMAux(double mmW, double mmH) {


	double milsW = GraphicsUtils::mm2mils(mmW);
	double milsH = GraphicsUtils::mm2mils(mmH);

	QString s = makeFirstLayerSvg(mmW, mmH, milsW, milsH);

	bool result = loadExtraRenderer(s.toUtf8());
	if (result) {
		modelPart()->setProp("width", mmW);
		modelPart()->setProp("height", mmH);

		if (m_widthEditor) {
			m_widthEditor->setText(QString::number(qRound(mmW * 10) / 10.0));
		}
		if (m_heightEditor) {
			m_heightEditor->setText(QString::number(qRound(mmH * 10) / 10.0));
		}
	}
	//	DebugDialog::debug(QString("fast load result %1 %2").arg(result).arg(s));

	positionGrips();

	foreach (ItemBase * itemBase, m_layerKin) {
		QString s = makeNextLayerSvg(itemBase->viewLayerID(), mmW, mmH, milsW, milsH);
		if (!s.isEmpty()) {
			if (m_silkscreenRenderer == NULL) {
				m_silkscreenRenderer = new FSvgRenderer(itemBase);
			}
			bool result = m_silkscreenRenderer->fastLoad(s.toUtf8());
			if (result) {
				qobject_cast<PaletteItemBase *>(itemBase)->setSharedRendererEx(m_silkscreenRenderer);
				itemBase->modelPart()->setProp("width", mmW);
				itemBase->modelPart()->setProp("height", mmH);
			}
			break;
		}
	}
}

void ResizableBoard::loadLayerKin( const LayerHash & viewLayers, ViewLayer::ViewLayerSpec viewLayerSpec) {
	loadTemplates();				
	Board::loadLayerKin(viewLayers, viewLayerSpec);
	double w = m_modelPart->prop("width").toDouble();
	if (w != 0) {
		resizeMM(w, m_modelPart->prop("height").toDouble(), viewLayers);
	}
}

void ResizableBoard::setInitialSize() {
	double w = m_modelPart->prop("width").toDouble();
	if (w == 0) {
		// set the size so the infoGraphicsView will display the size as you drag
		QSizeF sz = this->boundingRect().size();
		modelPart()->setProp("width", GraphicsUtils::pixels2mm(sz.width(), FSvgRenderer::printerScale())); 
		modelPart()->setProp("height", GraphicsUtils::pixels2mm(sz.height(), FSvgRenderer::printerScale())); 
	}
}

QString ResizableBoard::retrieveSvg(ViewLayer::ViewLayerID viewLayerID, QHash<QString, QString> & svgHash, bool blackOnly, double dpi)
{
	double w = m_modelPart->prop("width").toDouble();
	if (w != 0) {
		double h = m_modelPart->prop("height").toDouble();
		QString xml = makeLayerSvg(viewLayerID, w, h, GraphicsUtils::mm2mils(w), GraphicsUtils::mm2mils(h));
		if (!xml.isEmpty()) {
			QString xmlName = ViewLayer::viewLayerXmlNameFromID(viewLayerID);
			SvgFileSplitter splitter;
			bool result = splitter.splitString(xml, xmlName);
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

	return Board::retrieveSvg(viewLayerID, svgHash, blackOnly, dpi);
}

QSizeF ResizableBoard::getSizeMM() {
	double w = m_modelPart->prop("width").toDouble();
	double h = m_modelPart->prop("height").toDouble();
	return QSizeF(w, h);
}

QString ResizableBoard::makeLayerSvg(ViewLayer::ViewLayerID viewLayerID, double mmW, double mmH, double milsW, double milsH) 
{
	switch (viewLayerID) {
		case ViewLayer::Board:
			return makeBoardSvg(mmW, mmH, milsW, milsH);
		case ViewLayer::Silkscreen1:
			return makeSilkscreenSvg(mmW, mmH, milsW, milsH);
			break;
		default:
			return "";
	}
}

QString ResizableBoard::makeNextLayerSvg(ViewLayer::ViewLayerID viewLayerID, double mmW, double mmH, double milsW, double milsH) {

	if (viewLayerID == ViewLayer::Silkscreen1) return makeSilkscreenSvg(mmW, mmH, milsW, milsH);

	return "";
}

QString ResizableBoard::makeFirstLayerSvg(double mmW, double mmH, double milsW, double milsH) {
	return makeBoardSvg(mmW, mmH, milsW, milsH);
}

QString ResizableBoard::makeBoardSvg(double mmW, double mmH, double milsW, double milsH) {
	return BoardLayerTemplate
		.arg(mmW).arg(mmH)			
		.arg(milsW).arg(milsH)
		.arg(milsW - LineThickness).arg(milsH - LineThickness);
}

QString ResizableBoard::makeSilkscreenSvg(double mmW, double mmH, double milsW, double milsH) {
	return SilkscreenLayerTemplate
		.arg(mmW).arg(mmH)
		.arg(milsW).arg(milsH)
		.arg(milsW - LineThickness).arg(milsH - LineThickness);
}

void ResizableBoard::rotateItem(double degrees) {
	// TODO: this hack only works for 90 degree rotations
	// eventually need to make this work for other angles
	// what gets screwed up is the drag handles

	if (moduleID().contains(ModuleIDNames::RectangleModuleIDName)) {
		if (degrees == 90 || degrees == -90 || degrees == 270 || degrees == -270) {
			QRectF r = this->boundingRect();
			r.moveTopLeft(pos());
			QPointF c = r.center();
			ViewGeometry vg;
			vg.setLoc(QPointF(c.x() - (r.height() / 2.0), c.y() - (r.width() / 2.0)));	
			double w = m_modelPart->prop("width").toDouble();
			double h = m_modelPart->prop("height").toDouble();
			LayerHash viewLayers;
			resizeMM(h, w, viewLayers);
			moveItem(vg);
		}
	}
	else {
		Board::rotateItem(degrees);
	}
}

void ResizableBoard::saveParams() {
	double w = modelPart()->prop("width").toDouble();
	double h = modelPart()->prop("height").toDouble();
	m_boardSize = QSizeF(w, h);
	m_boardPos = pos();
}

void ResizableBoard::getParams(QPointF & p, QSizeF & s) {
	p = m_boardPos;
	s = m_boardSize;
}

bool ResizableBoard::hasCustomSVG() {
	return theViewIdentifier() == m_viewIdentifier;
}

bool ResizableBoard::collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget)
{
	bool result = Board::collectExtraInfo(parent, family, prop, value, swappingEnabled, returnProp, returnValue, returnWidget);

	if (prop.compare("shape", Qt::CaseInsensitive) == 0) {

		returnProp = tr("shape");

		if (!m_modelPart->prop("height").isValid()) { 
			// display uneditable width and height
			QFrame * frame = new QFrame();
			frame->setObjectName("infoViewPartFrame");		

			QVBoxLayout * vboxLayout = new QVBoxLayout();
			vboxLayout->setAlignment(Qt::AlignLeft);
			vboxLayout->setSpacing(0);
			vboxLayout->setMargin(0);
			vboxLayout->setContentsMargins(0, 3, 0, 0);

			QRectF r = this->boundingRect();
			double w = qRound(GraphicsUtils::pixels2mm(r.width(), FSvgRenderer::printerScale()) * 100) / 100.0;
			QLabel * l1 = new QLabel(tr("width: %1mm").arg(w));	
			l1->setMargin(0);
			l1->setObjectName("infoViewLabel");		

			double h = qRound(GraphicsUtils::pixels2mm(r.height(), FSvgRenderer::printerScale()) * 100) / 100.0;
			QLabel * l2 = new QLabel(tr("height: %1mm").arg(h));
			l2->setMargin(0);
			l2->setObjectName("infoViewLabel");		

			if (returnWidget) vboxLayout->addWidget(qobject_cast<QWidget *>(returnWidget));
			vboxLayout->addWidget(l1);
			vboxLayout->addWidget(l2);

			frame->setLayout(vboxLayout);

			returnValue = l1->text() + "," + l2->text();
			returnWidget = frame;
			return true;
		}

		double w = qRound(m_modelPart->prop("width").toDouble() * 10) / 10.0;	// truncate to 1 decimal point
		double h = qRound(m_modelPart->prop("height").toDouble() * 10) / 10.0;  // truncate to 1 decimal point

		QFrame * frame = new QFrame();
		QVBoxLayout * vboxLayout = new QVBoxLayout();
		vboxLayout->setAlignment(Qt::AlignLeft);
		vboxLayout->setSpacing(1);
		vboxLayout->setContentsMargins(0, 3, 0, 0);
		vboxLayout->setMargin(0);

		QFrame * subframe1 = new QFrame();
		QHBoxLayout * hboxLayout1 = new QHBoxLayout();
		hboxLayout1->setAlignment(Qt::AlignLeft);
		hboxLayout1->setContentsMargins(0, 0, 0, 0);
		hboxLayout1->setSpacing(2);

		QLabel * l1 = new QLabel(tr("width(mm)"));	
		l1->setMargin(0);
		l1->setObjectName("infoViewLabel");	
		QLineEdit * e1 = new QLineEdit();
		e1->setEnabled(swappingEnabled);
		QDoubleValidator * validator = new QDoubleValidator(e1);
		validator->setRange(0.1, 999.9, 1);
		validator->setNotation(QDoubleValidator::StandardNotation);
		e1->setObjectName("infoViewLineEdit");	
		e1->setValidator(validator);
		e1->setMaxLength(5);
		e1->setText(QString::number(w));
		m_widthEditor = e1;

		QFrame * subframe2 = new QFrame();
		QHBoxLayout * hboxLayout2 = new QHBoxLayout();
		hboxLayout2->setAlignment(Qt::AlignLeft);
		hboxLayout2->setContentsMargins(0, 0, 0, 0);
		hboxLayout2->setSpacing(2);

		QLabel * l2 = new QLabel(tr("height(mm)"));
		l2->setMargin(0);
		l2->setObjectName("infoViewLabel");	
		QLineEdit * e2 = new QLineEdit();
		e2->setEnabled(swappingEnabled);
		validator = new QDoubleValidator(e2);
		validator->setRange(0.1, 999.9, 1);
		validator->setNotation(QDoubleValidator::StandardNotation);
		e2->setObjectName("infoViewLineEdit");	
		e2->setValidator(validator);
		e2->setMaxLength(5);
		e2->setText(QString::number(h));
		m_heightEditor = e2;

		hboxLayout1->addWidget(l1);
		hboxLayout1->addWidget(e1);

		hboxLayout2->addWidget(l2);
		hboxLayout2->addWidget(e2);

		subframe1->setLayout(hboxLayout1);
		subframe2->setLayout(hboxLayout2);

		if (returnWidget != NULL) vboxLayout->addWidget(qobject_cast<QWidget *>(returnWidget));
		vboxLayout->addWidget(subframe1);
		vboxLayout->addWidget(subframe2);

		frame->setLayout(vboxLayout);

		connect(e1, SIGNAL(editingFinished()), this, SLOT(widthEntry()));
		connect(e2, SIGNAL(editingFinished()), this, SLOT(heightEntry()));

		returnWidget = frame;
		return true;
	}

	return result;
}

QStringList ResizableBoard::collectValues(const QString & family, const QString & prop, QString & value) {
	QStringList result = Board::collectValues(family, prop, value);

	if (prop.compare("shape", Qt::CaseInsensitive) == 0) {
		if (customShapeTranslated.isEmpty()) {
			customShapeTranslated = tr("Import Shape...");
		}
		result.append(customShapeTranslated);
	}

	return result;
}

void ResizableBoard::widthEntry() {
	QLineEdit * edit = qobject_cast<QLineEdit *>(sender());
	if (edit == NULL) return;

	double newVal = edit->text().toDouble();
	double oldVal =  m_modelPart->prop("height").toDouble();

	if (oldVal == newVal) return;

	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->resizeBoard(oldVal, newVal, true);
	}
}

void ResizableBoard::heightEntry() {
	QLineEdit * edit = qobject_cast<QLineEdit *>(sender());
	if (edit == NULL) return;

	double newVal = edit->text().toDouble();
	double oldVal =  m_modelPart->prop("width").toDouble();

	if (oldVal == newVal) return;

	InfoGraphicsView * infoGraphicsView = InfoGraphicsView::getInfoGraphicsView(this);
	if (infoGraphicsView != NULL) {
		infoGraphicsView->resizeBoard(oldVal, newVal, true);
	}
}

void ResizableBoard::calcRotation(QTransform & rotation, QPointF center, ViewGeometry & vg2) 
{
	if (moduleID().contains(ModuleIDNames::RectangleModuleIDName)) {
		// because these boards don't actually rotate yet
		QRectF r = boundingRect();
		//QPointF test(center.x() - (r.height() / 2.0), center.y() - (r.width() / 2.0));
		QPointF p0 = pos();
		double angle = atan2(rotation.m12(), rotation.m11()) * 180 / M_PI;
		if (angle < 0) angle += 360;
		if (qAbs(angle - 90) < 1) {			// degrees == 90
			p0 += r.bottomLeft();
		}
		else if (qAbs(angle - 270) < 1) {		// degrees == -90
			p0 += r.topRight();
		}
		else if (qAbs(angle - 180) < 1) {		// degrees == 180
			p0 += r.bottomRight();
		}
		else if (qAbs(angle - 0) < 1) {
		}
		else {
			// we're screwed: only multiples of 90 for now.
			DebugDialog::debug(QString("non-90 degree rotation for board %1").arg(angle));
		}
		QPointF d0 = p0 - center;
		QPointF d0t = rotation.map(d0);
		vg2.setLoc(d0t + center);
	}
	else {
		Board::calcRotation(rotation, center, vg2);
	}
}

bool ResizableBoard::hasPartNumberProperty()
{
	return false;
}
