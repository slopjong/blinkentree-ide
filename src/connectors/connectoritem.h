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

#ifndef CONNECTORITEM_H
#define CONNECTORITEM_H

#include "nonconnectoritem.h"
#include "connector.h"
#include "../utils/cursormaster.h"

#include <QThread>
#include <QGraphicsLineItem>

class LegItem;

class ConnectorItem : public NonConnectorItem, public CursorKeyListener
{
Q_OBJECT

public:
	ConnectorItem(Connector *, ItemBase* attachedTo);
	~ConnectorItem();

	Connector * connector();
	void connectorHover(class ItemBase *, bool hovering);
	bool connectorHovering();
	void clearConnectorHover();
	void connectTo(ConnectorItem *);
	int connectionsCount();
	void attachedMoved();
	ConnectorItem * removeConnection(ItemBase *);
	void removeConnection(ConnectorItem *, bool emitChange);
	ConnectorItem * firstConnectedToIsh();
	void setTerminalPoint(QPointF);
	QPointF terminalPoint();
	QPointF adjustedTerminalPoint();
	QPointF sceneAdjustedTerminalPoint(ConnectorItem * anchor);
	bool connectedTo(ConnectorItem *);
	const QList< QPointer<ConnectorItem> > & connectedToItems();
	void setHidden(bool hidden);
	void setInactive(bool inactivate);
	ConnectorItem * overConnectorItem();
	void setOverConnectorItem(ConnectorItem *);
	int attachedToItemType();
	ViewLayer::ViewLayerID attachedToViewLayerID();
	ViewLayer::ViewLayerSpec attachedToViewLayerSpec();
	ViewIdentifierClass::ViewIdentifier attachedToViewIdentifier();
	const QString & connectorSharedID();
	const QString & connectorSharedName();
	const QString & connectorSharedDescription();
	class ErcData * connectorSharedErcData();
	const QString & busID();
	ModelPartShared * modelPartShared();
	ModelPart * modelPart();
	class Bus * bus();
	void tempConnectTo(ConnectorItem * item, bool applyColor);
	void tempRemove(ConnectorItem * item, bool applyColor);
	Connector::ConnectorType connectorType();
	bool chained();
	void saveInstance(QXmlStreamWriter & );
	void writeConnector(QXmlStreamWriter & writer, const QString & elementName);
	bool wiredTo(ConnectorItem *, ViewGeometry::WireFlags skipFlags);
	void clearConnector();
	bool connectionIsAllowed(ConnectorItem * other);
	void restoreColor(bool doBuses, int busConnectedCount, bool doCross);
	void showEqualPotential(bool show);
	void setHoverColor();
	bool isGrounded();
	ConnectorItem * chooseFromSpec(ViewLayer::ViewLayerSpec);
	bool connectedToWires();
	bool isCrossLayerConnectorItem(ConnectorItem * candidate);
	bool isCrossLayerFrom(ConnectorItem * candidate);
	bool isInLayers(ViewLayer::ViewLayerSpec);
	ConnectorItem * getCrossLayerConnectorItem();
	void displayRatsnest(QList<ConnectorItem *> & partsConnectorItems);
	void clearRatsnestDisplay(QList<ConnectorItem *> & connectorItems);
	bool marked();
	void setMarked(bool);
	double calcClipRadius();
	bool isEffectivelyCircular();
	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    void debugInfo(const QString & msg);
	double minDimension();
	void setHybrid(bool);
	bool isHybrid();
	void setBigDot(bool);
	bool isBigDot();
	ConnectorItem * findConnectorUnder(bool useTerminalPoint, bool allowAlready, const QList<ConnectorItem *> & exclude, bool displayDragTooltip, ConnectorItem * other);
	ConnectorItem * releaseDrag();	

	// rubberBand leg functions	
	bool isDraggingLeg();
	void setRubberBandLeg(QColor color, double strokeWidth, QLineF parentLine);
	bool hasRubberBandLeg() const;
	void rotateLeg(const QPolygonF &, bool active);
	void setLeg(const QPolygonF &, bool relative, const QString & why);
	void resetLeg(const QPolygonF &, bool relative, bool active, const QString & why);
	const QPolygonF & leg();
	QString makeLegSvg(QPointF offset, double dpi, double printerScale, bool blackOnly);
	QPolygonF sceneAdjustedLeg();
	void prepareToStretch(bool activeStretch);
	void stretchBy(QPointF howMuch);
	void stretchDone(QPolygonF & oldLeg, QPolygonF & newLeg, bool & active);
	void moveDone(int & index0, QPointF & oldPos0, QPointF & newPos0, int & index1, QPointF & oldPos1, QPointF & newPos1);
	void killRubberBandLeg();  // hack; see caller
	QRectF boundingRect() const;
	const QString & legID(ViewIdentifierClass::ViewIdentifier, ViewLayer::ViewLayerID);
	QPainterPath shape() const;
	QPainterPath hoverShape() const;
	void changeLegCurve(int index, const class Bezier *);
	void addLegBendpoint(int index, QPointF, const class Bezier *, const class Bezier *);
	void removeLegBendpoint(int index, const class Bezier *);
	void moveLegBendpoint(int index, QPointF);
	const QVector<class Bezier *> & beziers();
	bool isBendpoint();
	void cursorKeyEvent(Qt::KeyboardModifiers modifiers);
	void setConnectorLocalName(const QString & name);
	void updateTooltip();

protected:
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );
	void hoverMoveEvent( QGraphicsSceneHoverEvent * event );
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	void setNormalColor();
	void setConnectedColor();
	void setUnconnectedColor();
	void setColorAux(QBrush brush, QPen pen, bool paint);
	void setColorAux(const QColor &color, bool paint=true);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void writeTopLevelAttributes(QXmlStreamWriter & writer);
	void writeOtherElements(QXmlStreamWriter & writer);
    static class Wire * directlyWiredToAux(ConnectorItem * source, ConnectorItem * target, ViewGeometry::WireFlags flags, QList<ConnectorItem *> & visited);
	bool isEverVisible();
	void setHiddenOrInactive();
	bool isConnectedToPart();
	void displayTooltip(ConnectorItem * over, ConnectorItem * other);
	void reposition(QPointF sceneDestPos, int draggingIndex);
	void repositionTarget();
	void calcConnectorEnd();
	QPen legPen() const;
	bool legMousePressEvent(QGraphicsSceneMouseEvent *event);
	void repoly(const QPolygonF & poly, bool relative);
	QPainterPath shapeAux(double width) const;

	enum CursorLocation {
		InNotFound = 0,
		InConnector,
		InBendpoint,
		InOrigin,
		InSegment
	};

	CursorLocation findLocation(QPointF, int & bendpointIndex);
	void insertBendpoint(QPointF pos, int bendpointIndex);
	Bezier * insertBendpointAux(QPointF p, int bendpointIndex);

	void removeBendpoint(int bendpointIndex);
	void clearCurves();
	void paintLeg(QPainter * painter);
	void paintLeg(QPainter * painter, bool hasCurves);
	void replaceBezier(int index, const Bezier * newBezier);
	void updateLegCursor(QPointF p, Qt::KeyboardModifiers modifiers);
	void updateWireCursor(Qt::KeyboardModifiers modifiers);
	bool curvyWiresIndicated(Qt::KeyboardModifiers);
	double findT(Bezier * bezier, double blen, double length);

protected:
	QPointer<Connector> m_connector;
	QList< QPointer<ConnectorItem> > m_connectedTo;
	QPointF m_terminalPoint;
	QPointer<ConnectorItem> m_overConnectorItem;
	bool m_connectorHovering;
	bool m_spaceBarWasPressed;
	bool m_hoverEnterSpaceBarWasPressed;
	bool m_checkedEffectively;
	bool m_marked;
	bool m_hybrid;
	bool m_bigDot;
	bool m_rubberBandLeg;
	QPolygonF m_oldPolygon;
	bool m_draggingLeg;
	bool m_draggingCurve;
	int m_draggingLegIndex;
	bool m_activeStretch;
	QPointF m_holdPos;
	QPolygonF m_legPolygon;
	QVector<class Bezier *> m_legCurves;
	double m_legStrokeWidth;
	QColor m_legColor;
	bool m_insertBendpointPossible;
	QPointF m_connectorDetectEnd;
	QPointF m_connectorDrawEnd;
	double m_connectorDrawT;
	double m_connectorDetectT;
	
protected:	
	static QList<ConnectorItem *>  m_equalPotentialDisplayItems;

protected:
	static void collectPart(ConnectorItem * connectorItem, QList<ConnectorItem *> & partsConnectors, ViewLayer::ViewLayerSpec);

public:
	static void collectEqualPotential(QList<ConnectorItem *> & connectorItems, bool crossLayers, ViewGeometry::WireFlags skipFlags);
	static void collectParts(QList<ConnectorItem *> & connectorItems, QList<ConnectorItem *> & partsConnectors, bool includeSymbols, ViewLayer::ViewLayerSpec);
	static void clearEqualPotentialDisplay();
	static bool isGrounded(ConnectorItem * c1, ConnectorItem * c2);
	static void collectConnectorNames(QList<ConnectorItem *> & connectorItems, QStringList & connectorNames);
	static class Wire * directlyWiredTo(ConnectorItem * source, ConnectorItem * target, ViewGeometry::WireFlags flags);

public:
	static const QList<ConnectorItem *> emptyConnectorItemList;
};

Q_DECLARE_METATYPE(ConnectorItem*);

#endif
