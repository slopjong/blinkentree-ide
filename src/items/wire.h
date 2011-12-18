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

$Revision: 5443 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-29 13:46:02 +0200 (Mon, 29 Aug 2011) $

********************************************************************/

#ifndef WIRE_H
#define WIRE_H


#include <QGraphicsLineItem>
#include <QLineF>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QHash>
#include <QMenu>

#include "itembase.h"
#include "../utils/cursormaster.h"

class WireMenu : public QMenu {
	Q_OBJECT

public:
	WireMenu(const QString & title, QWidget * parent = 0);
	void setWire(Wire *);
	Wire * wire();

protected:
	Wire * m_wire;
};

class WireAction : public QAction {
	Q_OBJECT

public:
	WireAction(QAction *);
	WireAction(const QString & text, QObject * parent);

	void setWire(Wire *);
	Wire * wire();

protected:
	Wire * m_wire;
};

class Wire : public ItemBase, public CursorKeyListener
{
Q_OBJECT

public:
	Wire(class ModelPart *, ViewIdentifierClass::ViewIdentifier, const ViewGeometry &, long id, QMenu * itemMenu, bool initLabel);
	virtual ~Wire();


	// for debugging
	//void setPos(const QPointF & pos);
	// for debugging

    QLineF line() const;
    virtual void setLine(const QLineF &line);
    inline void setLine(double x1, double y1, double x2, double y2);
    QPen pen() const;
    void setPen(const QPen &pen);

	void saveGeometry();
	bool itemMoved();
	void saveInstanceLocation(QXmlStreamWriter &);
	void writeGeometry(QXmlStreamWriter &);
	void moveItem(ViewGeometry & viewGeometry);
	void hoverEnterConnectorItem(QGraphicsSceneHoverEvent * event, class ConnectorItem * item);
	void hoverLeaveConnectorItem(QGraphicsSceneHoverEvent * event, class ConnectorItem * item);

	void initDragEnd(ConnectorItem * dragEnd, QPointF scenePos);
	void initDragCurve(QPointF scenePos);
	bool initNewBendpoint(QPointF scenePos, class Bezier & left, class Bezier & right);
	void connectedMoved(ConnectorItem * from, ConnectorItem * to);
	void setLineAnd(QLineF line, QPointF pos, bool useLine);
	ConnectorItem * otherConnector(ConnectorItem *);
	ConnectorItem * connector0();
	ConnectorItem * connector1();
	virtual class FSvgRenderer * setUp(ViewLayer::ViewLayerID viewLayerID, const LayerHash & viewLayers, class InfoGraphicsView *);
	void findConnectorsUnder();
	void collectChained(QList<Wire *> &, QList<ConnectorItem *> & ends);
	void collectWires(QList<Wire *> & wires);
	bool stickyEnabled();
	void setPcbPenBrush(QBrush & brush);
	bool getRouted();
	void setRouted(bool);
	bool getRatsnest();
	void setRatsnest(bool);
	void setAutoroutable(bool);
	bool getAutoroutable();
	void setNormal(bool);
	bool getNormal();
	bool getTrace();

	bool hasFlag(ViewGeometry::WireFlag);
	bool hasAnyFlag(ViewGeometry::WireFlags);
	void setWireFlags(ViewGeometry::WireFlags);

	QString colorString();
	QString hexString();
	QString shadowHexString();
	void setColorString(QString, double opacity);
	virtual void setColor(const QColor &, double opacity);
	double opacity();
	void setOpacity(double opacity);
	const QColor & color();
	void setWireWidth(double width, InfoGraphicsView *, double hoverStrokeWidth);
	void setPenWidth(double width, InfoGraphicsView *, double hoverStrokeWidth);
	double width();
	double shadowWidth();
	double mils();
	void setExtras(QDomElement &, InfoGraphicsView *);
	Wire * findTraced(ViewGeometry::WireFlags flags, QList<ConnectorItem *>  & ends);
	bool draggingEnd();
	void simpleConnectedMoved(ConnectorItem * to);
	void simpleConnectedMoved(ConnectorItem * from, ConnectorItem * to);
	void setCanChainMultiple(bool);
	bool canChangeColor();
	void collectDirectWires(QList<Wire *> & wires);
	bool isGrounded();
	bool collectExtraInfo(QWidget * parent, const QString & family, const QString & prop, const QString & value, bool swappingEnabled, QString & returnProp, QString & returnValue, QWidget * & returnWidget);
	bool hasPartLabel();
	PluralType isPlural();
	virtual bool canSwitchLayers();
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void paintBody(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	bool hasPartNumberProperty();
	bool rotationAllowed();
	bool rotation45Allowed();
	void addedToScene(bool temporary);
	void setConnectorDimensions(double width, double height);
	void originalConnectorDimensions(double & width, double & height);
	double hoverStrokeWidth();
    QPainterPath hoverShape() const;
    QPainterPath shape() const;
	QRectF boundingRect() const;
	virtual const QLineF & getPaintLine();
	bool canHaveCurve();
	void changeCurve(const class Bezier *);
	bool isCurved();
	const class Bezier * curve();
	const class Bezier * undoCurve();
	QPolygonF sceneCurve(QPointF offset);
	bool hasShadow();
	bool canChainMultiple();
	void cursorKeyEvent(Qt::KeyboardModifiers modifiers);

protected slots:
	void colorEntry(const QString & text);

public:
	static double STANDARD_TRACE_WIDTH;
	static double HALF_STANDARD_TRACE_WIDTH;
	static double THIN_TRACE_WIDTH;

public:
	static void initNames();
	static QRgb getRgb(const QString & name);
	static void cleanup();

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEventAux(QPointF eventPos, Qt::KeyboardModifiers);
	void dragCurve(QPointF eventPos, Qt::KeyboardModifiers);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void paintHover(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); 
	void initEnds(const ViewGeometry &, QRectF defaultRect, class InfoGraphicsView *);
	void connectionChange(ConnectorItem * onMe, ConnectorItem * onIt, bool connect);
	void mousePressConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
	void mouseDoubleClickConnectorEvent(ConnectorItem *);
	void mouseMoveConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
	void mouseReleaseConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
	bool acceptsMouseDoubleClickConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
	bool acceptsMouseMoveConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
	bool acceptsMouseReleaseConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
 	virtual class FSvgRenderer * setUpConnectors(class ModelPart *, ViewIdentifierClass::ViewIdentifier);
	void collectChained(ConnectorItem * connectorItem, QList<Wire *> & chained, QList<ConnectorItem *> & ends);
	void setConnector0Rect();
	void setConnector1Rect();
	void collectWiresAux(QList<Wire *> & wires, ConnectorItem * start);
	void setShadowColor(QColor &);
	void calcNewLine(ConnectorItem * from, ConnectorItem * to, QPointF & p1, QPointF & p2);
	void collectDirectWires(ConnectorItem * connectorItem, QList<Wire *> & wires);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void getConnectedColor(ConnectorItem *, QBrush * &, QPen * &, double & opacity, double & negativePenWidth, bool & negativeOffsetRect);
	bool connectionIsAllowed(ConnectorItem *);
	bool releaseDrag();
	void setIgnoreSelectionChange(bool);
	virtual void setColorFromElement(QDomElement & element);
	void checkVisibility(ConnectorItem * onMe, ConnectorItem * onIt, bool connect);
	void setConnectorDimensionsAux(ConnectorItem *, double width, double height);
	bool isBendpoint(ConnectorItem * connectorItem);
	QPainterPath shapeAux(double width) const;
	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );
	void hoverEnterEvent( QGraphicsSceneHoverEvent * event );
	void updateCursor(Qt::KeyboardModifiers);

protected:
	QLineF	m_line;
	QPen	m_pen;	
	QPointF m_wireDragOrigin;
	bool m_dragEnd;
	bool m_dragCurve;
	bool m_drag0;
	QPointer<class ConnectorItem> m_connectorHover;
	QPointer<class ConnectorItem> m_connector0;
	QPointer<class ConnectorItem> m_connector1;
	QString m_colorName;
	QPen m_shadowPen;
	QBrush m_shadowBrush;
	QPen m_bendpointPen;
	QPen m_bendpoint2Pen;
	double m_bendpointWidth;
	double m_bendpoint2Width;
	bool m_negativeOffsetRect;
	double m_opacity;
	bool m_canChainMultiple;
	bool m_ignoreSelectionChange;
	bool m_markedDeleted;
	QRectF m_originalConnectorRect;
	double m_hoverStrokeWidth;
	bool m_canHaveCurve;
	class Bezier * m_bezier;

public:
	static QStringList colorNames;
	static QHash<QString, QString> colorTrans;
	static QHash<int, QString> widthTrans;
	static QList<int> widths;

protected:
	static QHash<QString, QString> shadowColors;
	static QHash<QString, QString> colors;

signals:
	void wireChangedSignal(Wire* me, const QLineF & oldLine, const QLineF & newLine, QPointF oldPos, QPointF newPos, ConnectorItem * from, ConnectorItem * to);
	void wireChangedCurveSignal(Wire* me, const Bezier * oldB, const Bezier * newB, bool triggerFirstTime);
	void wireSplitSignal(Wire* me, QPointF newPos, QPointF oldPos, const QLineF & oldLine);
	void wireJoinSignal(Wire* me, ConnectorItem * clickedConnectorItem);
};

#endif
