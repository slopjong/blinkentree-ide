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

$Revision: 5676 $:
$Author: cohen@irascible.com $:
$Date: 2011-12-14 05:51:56 +0100 (Wed, 14 Dec 2011) $

********************************************************************/


#ifndef SKETCHWIDGET_H
#define SKETCHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QUndoStack>
#include <QRubberBand>
#include <QGraphicsEllipseItem>
#include <QSet>
#include <QHash>
#include <QTimer>

#include "../items/paletteitem.h"
#include "../model/palettemodel.h"
#include "../model/sketchmodel.h"
#include "../viewgeometry.h"
#include "infographicsview.h"
#include "../viewlayer.h"
#include "../utils/misc.h"
#include "../commands.h"

struct ItemCount {
	int selCount;
	int hasLabelCount;
	int visLabelCount;
	int itemsCount;
	int selRotatable;
	int sel45Rotatable;
	int selHFlipable;
	int selVFlipable;
	int obsoleteCount;
	int moveLockCount;
	int wireCount;
};

class SizeItem : public QObject, public QGraphicsLineItem
{
	Q_OBJECT
	
public:
	SizeItem();
	~SizeItem();
};

class SketchWidget : public InfoGraphicsView
{
	Q_OBJECT

public:
    SketchWidget(ViewIdentifierClass::ViewIdentifier, QWidget *parent=0, int size=400, int minSize=300);
	~SketchWidget();

	void pushCommand(QUndoCommand *);
    class WaitPushUndoStack * undoStack();
    ItemBase * addItem(ModelPart *, ViewLayer::ViewLayerSpec, BaseCommand::CrossViewType, const ViewGeometry &, long id, long modelIndex, AddDeleteItemCommand * originatingCommand, PaletteItem* item);
	ItemBase * addItem(const QString & moduleID, ViewLayer::ViewLayerSpec, BaseCommand::CrossViewType, const ViewGeometry &, long id, long modelIndex, AddDeleteItemCommand * originatingCommand);
    void deleteItem(long id, bool deleteModelPart, bool doEmit, bool later);
    virtual void deleteItem(ItemBase *, bool deleteModelPart, bool doEmit, bool later);
    void simpleMoveItem(long id, QPointF);
    void moveItem(long id, ViewGeometry &, bool updateRatsnest);
	void moveItem(long id, const QPointF & p, bool updateRatsnest);
	void updateWire(long id, const QString & connectorID, bool updateRatsnest);

    void rotateItem(long id, double degrees);
    void transformItem(long id, const QMatrix &);
    void flipItem(long id, Qt::Orientations orientation);
    void selectItem(long id, bool state, bool updateInfoView, bool doEmit);
    void selectDeselectAllCommand(bool state);
    void changeWire(long fromID, QLineF line, QPointF pos, bool updateConnections, bool updateRatsnest);   
    void changeLeg(long fromID, const QString & connectorID, const QPolygonF &, bool relative, const QString & why);   
    void recalcLeg(long fromID, const QString & connectorID, const QPolygonF &, bool relative, bool active, const QString & why);   
    void rotateLeg(long fromID, const QString & connectorID, const QPolygonF &, bool active);   
    void cut();
    void copy();
    void setPaletteModel(PaletteModel *);
    void setRefModel(class ReferenceModel *refModel);
    void setSketchModel(SketchModel *);
    void setUndoStack(class WaitPushUndoStack *);
    void clearSelection();
	virtual void loadFromModelParts(QList<ModelPart *> & modelParts, BaseCommand::CrossViewType, QUndoCommand * parentCommand, 
									bool offsetPaste, const QRectF * boundingRect, bool seekOutsideConnections, QList<long> & newIDs);
    void changeZ(QHash<long, RealPair * >, double (*pairAccessor)(RealPair *) );
	void sendToBack();
	void sendBackward();
	void bringForward();
	void bringToFront();
	double fitInWindow();
	void rotateX(double degrees, bool rubberBandLegEnabled);
	void flipX(Qt::Orientations orientation, bool rubberBandLegEnabled);
	void addBendpoint(ItemBase * lastHoverEnterItem, ConnectorItem * lastHoverEnterConnectorItem, QPointF lastLocation);
	void flattenCurve(ItemBase * lastHoverEnterItem, ConnectorItem * lastHoverEnterConnectorItem, QPointF lastLocation);

	virtual void deleteSelected(Wire *);
	PaletteItem *getSelectedPart();

    void addViewLayer(ViewLayer *);
    void setAllLayersVisible(bool visible);
    void setLayerVisible(ViewLayer * viewLayer, bool visible, bool doChildLayers);
	void setLayerVisible(ViewLayer::ViewLayerID viewLayerID, bool visible, bool doChildLayers);
    void setLayerActive(ViewLayer * viewLayer, bool active);
	void setLayerActive(ViewLayer::ViewLayerID viewLayerID, bool active);
    bool layerIsVisible(ViewLayer::ViewLayerID);
    bool layerIsActive(ViewLayer::ViewLayerID);
	void sortSelectedByZ(QList<ItemBase *> & bases);
	void sortAnyByZ(const QList<QGraphicsItem *> & items, QList<ItemBase *> & bases);
 	void mousePressConnectorEvent(ConnectorItem *, QGraphicsSceneMouseEvent *);
 	void setBackground(QColor);
 	const QColor& background();
	const QColor& standardBackground();
 	void setItemMenu(QMenu*);
 	void setWireMenu(QMenu*);
	virtual void changeConnection(long fromID,
						  const QString & fromConnectorID,
						  long toID, const QString & toConnectorID,
						  ViewLayer::ViewLayerSpec,
						  bool connect, bool doEmit, 
						  bool updateConnections);

 	ItemCount calcItemCount();

	ViewIdentifierClass::ViewIdentifier viewIdentifier();
	void setViewLayerIDs(ViewLayer::ViewLayerID part, ViewLayer::ViewLayerID wire, ViewLayer::ViewLayerID connector, ViewLayer::ViewLayerID ruler, ViewLayer::ViewLayerID note);
	void stickem(long stickTargetID, long stickSourceID, bool stick);
	void stickyScoop(ItemBase * stickyOne, bool checkCurrent, CheckStickyCommand *);
	void setChainDrag(bool);
	void hoverEnterItem(QGraphicsSceneHoverEvent * event, ItemBase * item);
	void hoverLeaveItem(QGraphicsSceneHoverEvent * event, ItemBase * item);
	void hoverEnterConnectorItem(QGraphicsSceneHoverEvent * event, ConnectorItem * item);
	void hoverLeaveConnectorItem(QGraphicsSceneHoverEvent * event, ConnectorItem * item);
	void cleanUpWires(bool doEmit, class CleanUpWiresCommand *);

	void partLabelChanged(ItemBase *, const QString & oldText, const QString &newtext);
	void noteChanged(ItemBase *, const QString & oldText, const QString &newtext, QSizeF oldSize, QSizeF newSize);

	void setInfoViewOnHover(bool infoViewOnHover);
	PaletteModel * paletteModel();
	virtual ItemBase * addItemAux(ModelPart *, ViewLayer::ViewLayerSpec, const ViewGeometry &, long id, PaletteItem * paletteItem, bool doConnectors, ViewIdentifierClass::ViewIdentifier, bool temporary);
	ItemBase * addItemAuxTemp(ModelPart *, ViewLayer::ViewLayerSpec, const ViewGeometry &, long id, PaletteItem * paletteItem, bool doConnectors, ViewIdentifierClass::ViewIdentifier, bool temporary);

    bool swappingEnabled(ItemBase *);

	virtual void addViewLayers();
	void addPcbViewLayers();
	void addSchematicViewLayers();
	void addBreadboardViewLayers();

	void changeWireColor(long wireId, const QString& color, double opacity);
	void changeWireWidth(long wireId, double width);
	void changeWireFlags(long wireId, ViewGeometry::WireFlags wireFlags);
	void setIgnoreSelectionChangeEvents(bool);
	void hideConnectors(bool hide);
	void saveLayerVisibility();
	void restoreLayerVisibility();
	virtual void updateRoutingStatus(CleanUpWiresCommand*, RoutingStatus &, bool manual);
	virtual bool hasAnyNets();
	void ensureLayerVisible(ViewLayer::ViewLayerID);

	const QString &selectedModuleID();
	virtual bool canDeleteItem(QGraphicsItem * item, int count);
	virtual bool canCopyItem(QGraphicsItem * item, int count);
	const QString & viewName();
	void makeDeleteItemCommand(ItemBase * itemBase, BaseCommand::CrossViewType, QUndoCommand * parentCommand);
	virtual void forwardRoutingStatus(const RoutingStatus &);

	void collectParts(QList<ItemBase *> & partList);

	void movePartLabel(long itemID, QPointF newPos, QPointF newOffset);

	void updateInfoView();
	virtual void setCurrent(bool current);
	void partLabelMoved(ItemBase *, QPointF oldPos, QPointF oldOffset, QPointF newPos, QPointF newOffset);
	void rotateFlipPartLabel(ItemBase *, double degrees, Qt::Orientations flipDirection);
	void rotateFlipPartLabel(long itemID, double degrees, Qt::Orientations flipDirection);
	void showPartLabels(bool show);
	void hidePartLabel(ItemBase * item);
	void noteSizeChanged(ItemBase * itemBase, const QSizeF & oldSize, const QSizeF & newSize);
	void resizeNote(long itemID, const QSizeF & );
	class SelectItemCommand* stackSelectionState(bool pushIt, QUndoCommand * parentCommand);
	QString renderToSVG(double printerScale, const LayerList & partLayers, const LayerList & wireLayers, 
						bool blackOnly, QSizeF & imageSize, ItemBase * board, double dpi, 
						bool selectedItems, bool flatten, bool fillHoles, bool & empty);
	QString renderToSVG(double printerScale, const LayerList & partLayers, const LayerList & wireLayers, 
						bool blackOnly, QSizeF & imageSize, QRectF & offsetRect, double dpi, 
						bool selectedItems, bool flatten, bool fillHoles, bool & empty);
	QString renderToSVG(double printerScale, const LayerList & partLayers, const LayerList & wireLayers, 
								  bool blackOnly, QSizeF & imageSize, QRectF & offsetRect, double dpi, 
								  bool flatten, bool fillHoles, 
								  QList<ItemBase *> & itemBases, QRectF itemsBoundingRect, bool & empty);

	bool spaceBarIsPressed();
	virtual long setUpSwap(ItemBase *, long newModelIndex, const QString & newModuleID, ViewLayer::ViewLayerSpec, bool doEmit, bool noFinalChangeWiresCommand, QUndoCommand * parentCommand);
	ConnectorItem * lastHoverEnterConnectorItem();
	ItemBase * lastHoverEnterItem();
	LayerHash & viewLayers();
	virtual void createTrace(Wire*);
	virtual void updateNet(Wire*);
	void selectAllWires(ViewGeometry::WireFlag);
	virtual void tidyWires();
	const QString & getShortName();
	virtual void setClipEnds(class ClipableWire *, bool);
	void getBendpointWidths(class Wire *, double w, double & w1, double & w2, bool & negativeOffsetRect);
	virtual bool includeSymbols();
	void disconnectAll();
	virtual bool canDisconnectAll();
	virtual bool ignoreFemale();
	virtual ViewLayer::ViewLayerID getWireViewLayerID(const ViewGeometry & viewGeometry, ViewLayer::ViewLayerSpec);
	ItemBase * findItem(long id);
	long createWire(ConnectorItem * from, ConnectorItem * to, ViewGeometry::WireFlags, bool dontUpdate, BaseCommand::CrossViewType, QUndoCommand * parentCommand);
	int selectAllObsolete();
	int selectAllMoveLock();
	void setMoveLock(long id, bool lock);
	int selectAllItemType(ModelPart::ItemType);
	bool partLabelsVisible();
	void restorePartLabel(long itemID, QDomElement & element);
	void loadLogoImage(long itemID, const QString & oldSvg, const QSizeF oldAspectRatio, const QString & oldFilename, const QString & newFilename, bool addName);
	void loadLogoImage(long itemID, const QString & oldSvg, const QSizeF oldAspectRatio, const QString & oldFilename);
	void loadLogoImage(long itemID, const QString & newFilename, bool addName);
	void setNoteFocus(QGraphicsItem *, bool inFocus);

	void alignToGrid(bool);
	bool alignedToGrid();
	void saveZoom(double);
	double retrieveZoom();
	void initGrid();
	virtual double defaultGridSizeInches();
	void clearPasteOffset();
	ViewLayer::ViewLayerSpec defaultViewLayerSpec();
	void addFixedToCenterItem2(class SketchMainHelp *item);
	void collectAllNets(QHash<class ConnectorItem *, int> & indexer, QList< QList<class ConnectorItem *>* > & allPartConnectorItems, bool includeSingletons, bool bothSides);
	virtual bool routeBothSides();
	virtual void changeLayer(long id, double z, ViewLayer::ViewLayerID viewLayerID);
	void ratsnestConnect(ConnectorItem * connectorItem, bool connect);
	void ratsnestConnect(ItemBase *, bool connect);
	virtual void addDefaultParts();
	float getTopZ();
	QGraphicsItem * addWatermark(const QString & filename);
	void copyHeart(QList<ItemBase *> & bases, bool saveBoundingRects, QByteArray & itemData, QList<long> & modelIndexes);
	void pasteHeart(QByteArray & itemData, bool seekOutsideConnections);
	virtual ViewGeometry::WireFlag getTraceFlag();
	void changeBus(ItemBase *, bool connec, const QString & oldBus, const QString & newBus, QList<ConnectorItem *> &, const QString & message);
	const QString & filenameIf();
	void setItemDropOffset(long id, QPointF offset);
	void prepLegBendpointMove(ConnectorItem * from, int index, QPointF oldPos, QPointF newPos, ConnectorItem * to, bool changeConnections);
	void prepLegCurveChange(ConnectorItem * from, int index, const class Bezier * oldB, const class Bezier * newB, bool triggerFirstTime);
	void prepLegBendpointChange(ConnectorItem * from, int oldCount, int newCount, int index, QPointF pos, const class Bezier *, const class Bezier *, const class Bezier *, bool triggerFirstTime);
	void prepLegSelection(ItemBase *);
	void changeWireCurve(long id, const Bezier *);
	void changeLegCurve(long id, const QString & connectorID, int index, const Bezier *);
	void addLegBendpoint(long id, const QString & connectorID, int index, QPointF, const class Bezier *, const class Bezier *);
	void removeLegBendpoint(long id, const QString & connectorID, int index, const class Bezier *);
	void moveLegBendpoint(long id, const QString & connectorID, int index, QPointF);
	bool curvyWires();
	void setCurvyWires(bool);
	bool curvyWiresIndicated(Qt::KeyboardModifiers);
	void triggerRotate(ItemBase *, double degrees);
	void makeWiresChangeConnectionCommands(const QList<Wire *> & wires, QUndoCommand * parentCommand);
	void renamePins(ItemBase *, const QStringList & oldLabels, const QStringList & newLabels, bool singleRow);
	void renamePins(long itemID, const QStringList & labels, bool singleRow);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
	bool dragEnterEventAux(QDragEnterEvent *event);
	virtual bool canDropModelPart(ModelPart * modelPart);

    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    PaletteItem* addPartItem(ModelPart * modelPart, ViewLayer::ViewLayerSpec, PaletteItem * paletteItem, bool doConnectors, bool & ok, ViewIdentifierClass::ViewIdentifier, bool temporary);
	void clearHoldingSelectItem();
	bool startZChange(QList<ItemBase *> & bases);
	void continueZChange(QList<ItemBase *> & bases, int start, int end, bool (*test)(int current, int start), int inc, const QString & text);
	void continueZChangeMax(QList<ItemBase *> & bases, int start, int end, bool (*test)(int current, int start), int inc, const QString & text);
	void continueZChangeAux(QList<ItemBase *> & bases, const QString & text);
	virtual ViewLayer::ViewLayerID getDragWireViewLayerID(ConnectorItem *);
	ViewLayer::ViewLayerID getPartViewLayerID();
	ViewLayer::ViewLayerID getRulerViewLayerID();
	ViewLayer::ViewLayerID getConnectorViewLayerID();
	virtual ViewLayer::ViewLayerID getLabelViewLayerID(ViewLayer::ViewLayerSpec);
	ViewLayer::ViewLayerID getNoteViewLayerID();
	void dragMoveHighlightConnector(QPoint eventPos);

	void addToScene(ItemBase * item, ViewLayer::ViewLayerID viewLayerID);
	ConnectorItem * findConnectorItem(ItemBase * item, const QString & connectorID, ViewLayer::ViewLayerSpec);
	bool checkMoved();

	void changeConnectionAux(long fromID, const QString & fromConnectorID,
						  long toID, const QString & toConnectorID,
						  ViewLayer::ViewLayerSpec,
						  bool connect, bool updateConnections);


	void cutDeleteAux(QString undoStackMessage);
	void deleteAux(QSet<ItemBase *> & deletedItems, QUndoCommand * parentCommand, bool doPush);
	void deleteMiddle(QHash<ItemBase *, SketchWidget *> & otherDeletedItems, QUndoCommand * parentCommand);

	ChangeConnectionCommand * extendChangeConnectionCommand(BaseCommand::CrossViewType, long fromID, const QString & fromConnectorID,
									   long toID, const QString & toConnectorID,
									   ViewLayer::ViewLayerSpec,
									   bool connect, QUndoCommand * parent);
	ChangeConnectionCommand * extendChangeConnectionCommand(BaseCommand::CrossViewType, ConnectorItem * fromConnectorItem, ConnectorItem * toConnectorItem,
										ViewLayer::ViewLayerSpec,
										bool connect, QUndoCommand * parentCommand);


	void keyPressEvent(QKeyEvent *);
	void keyReleaseEvent(QKeyEvent *);
	void clearTemporaries();
	virtual void dragWireChanged(class Wire* wire, ConnectorItem * from, ConnectorItem * to);
	void killDroppingItem();
	ViewLayer::ViewLayerID getViewLayerID(ModelPart *, ViewIdentifierClass::ViewIdentifier, ViewLayer::ViewLayerSpec);
	ItemBase * overSticky(ItemBase *);
	virtual void setNewPartVisible(ItemBase *);
	virtual bool collectFemaleConnectees(ItemBase *, QSet<ItemBase *> &);
	virtual bool checkUnder();
	virtual void findConnectorsUnder(ItemBase * item);

	bool currentlyInfoviewed(ItemBase *item);
	void resizeEvent(QResizeEvent *);

	void addViewLayersAux(const LayerList &layers, float startZ = 1.5);
	void tempConnectWire(Wire * wire, ConnectorItem * from, ConnectorItem * to);
	virtual bool disconnectFromFemale(ItemBase * item, QHash<long, ItemBase *> & savedItems, ConnectorPairHash &, bool doCommand, bool rubberBandLegEnabled, QUndoCommand * parentCommand);
	void clearDragWireTempCommand();
	bool draggingWireEnd();
	void moveItems(QPoint globalPos, bool checkAutoScroll, bool rubberBandLegEnabled);
	virtual ViewLayer::ViewLayerID multiLayerGetViewLayerID(ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier, ViewLayer::ViewLayerSpec, QDomElement & layers, QString & layerName);
	virtual BaseCommand::CrossViewType wireSplitCrossView();
	virtual bool canChainMultiple();
	virtual bool canChainWire(Wire *);
	virtual bool canDragWire(Wire *);
	virtual bool canCreateWire(Wire * dragWire, ConnectorItem * from, ConnectorItem * to);
	virtual bool modifyNewWireConnections(Wire * dragWire, ConnectorItem * fromOnWire, ConnectorItem * from, ConnectorItem * to, QUndoCommand * parentCommand);
	void setupAutoscroll(bool moving);
	void turnOffAutoscroll();
	bool checkAutoscroll(QPoint globalPos);
	virtual void setWireVisible(Wire *);
	virtual void chainVisible(ConnectorItem * fromConnectorItem, ConnectorItem * toConnectorItem, bool connect);
	bool matchesLayer(ModelPart * modelPart);

	QByteArray removeOutsideConnections(const QByteArray & itemData, QList<long> & modelIndexes);
	void addWireExtras(long newID, QDomElement & view, QUndoCommand * parentCommand);
	virtual bool doRatsnestOnCopy();
	virtual const QString & hoverEnterWireConnectorMessage(QGraphicsSceneHoverEvent * event, ConnectorItem * item);
	virtual const QString & hoverEnterPartConnectorMessage(QGraphicsSceneHoverEvent * event, ConnectorItem * item);
	void partLabelChangedAux(ItemBase * pitem,const QString & oldText, const QString &newText);
	void drawBackground( QPainter * painter, const QRectF & rect );
	void handleConnect(QDomElement & connect, ModelPart *, const QString & fromConnectorID, ViewLayer::ViewLayerID, QStringList & alreadyConnected, 
						QHash<long, ItemBase *> & newItems, QUndoCommand * parentCommand, bool seekOutsideConnections);
	void setUpSwapReconnect(ItemBase* itemBase, long newID, const QString & newModuleID, bool master, QUndoCommand * parentCommand);
	bool swappedGender(ConnectorItem * originalConnectorItem, Connector * newConnector);
	void setLastPaletteItemSelected(PaletteItem * paletteItem);
	void setLastPaletteItemSelectedIf(ItemBase * itemBase);
	void prepDragBendpoint(Wire *, QPoint eventPos, bool dragCurve);
	void prepDragWire(Wire *);
	void clickBackground(QMouseEvent *);
	void categorizeDragWires(QSet<Wire *> & wires, QList<ItemBase *> & freeWires);
	void categorizeDragLegs(bool rubberBandLegEnabled);
	void prepMove(ItemBase * originatingItem, bool rubberBandLegEnabled);
	void initBackgroundColor();
	QPointF calcNewLoc(ItemBase * moveBase, ItemBase * detachFrom);
	long findPartOrWire(long itemID);
	virtual AddItemCommand * newAddItemCommand(BaseCommand::CrossViewType crossViewType, 
											   QString moduleID, ViewLayer::ViewLayerSpec, ViewGeometry & viewGeometry, qint64 id, 
											   bool updateInfoView, long modelIndex, QUndoCommand *parent);
	int selectAllItems(QSet<ItemBase *> & itemBases, const QString & msg);
	bool moveByArrow(int dx, int dy, QKeyEvent * );
	double gridSizeInches();
	virtual bool canAlignToTopLeft(ItemBase *);
	virtual void findAlignmentAnchor(ItemBase * originatingItem, QHash<long, ItemBase *> & savedItems, QHash<Wire *, ConnectorItem *> & savedWires);
	void alignLoc(QPointF & loc, const QPointF startPoint, const QPointF newLoc, const QPointF originalLoc);
	void copyAux(QList<ItemBase *> & bases, bool saveBoundingRects);
	void copyDrop();
	void dropItemEvent(QDropEvent *event);
	QString makeWireSVG(Wire * wire, QPointF offset, double dpi, double printerscale, bool blackOnly);
	QString makeWireSVGAux(Wire * wire, double width, const QString & color, QPointF offset, double dpi, double printerScale, bool blackOnly);

	QString makeMoveSVG(double printerScale, double dpi, QPointF & offset); 
	void prepDeleteProps(ItemBase * itemBase, long id, const QString & newModuleID, QUndoCommand * parentCommand);
	void prepDeleteOtherProps(ItemBase * itemBase, long id, const QString & newModuleID, QUndoCommand * parentCommand);
	ViewLayer::ViewLayerSpec getViewLayerSpec(ModelPart * modelPart, QDomElement & instance, QDomElement & view, ViewGeometry &);
	virtual ViewLayer::ViewLayerSpec wireViewLayerSpec(ConnectorItem *);

	virtual bool resizingJumperItemPress(QGraphicsItem * item);
	virtual bool resizingJumperItemRelease();
	virtual bool resizingBoardPress(QGraphicsItem * item);
	virtual bool resizingBoardRelease();
	bool connectedDirectlyTo(ConnectorItem * from, ConnectorItem * to, QList<ConnectorItem *> & byBus);
	bool connectedDirectlyTo(ConnectorItem * from, ConnectorItem * to, QList<ConnectorItem *> & byBus, QList<ConnectorItem *> & visited);
	virtual QPoint calcFixedToCenterItemOffset(const QRect & viewPortRect, const QSizeF & helpSize);
	virtual bool acceptsTrace(const ViewGeometry &);
	virtual ItemBase * placePartDroppedInOtherView(ModelPart *, ViewLayer::ViewLayerSpec, const ViewGeometry & viewGeometry, long id, SketchWidget * dropOrigin);
	void alignOneToGrid(ItemBase * itemBase);
	void showPartLabelsAux(bool show, QList<ItemBase *> & itemBases);
	virtual void changeTrace(Wire * wire, ConnectorItem * from, ConnectorItem * to, QUndoCommand * parentCommand);
	virtual void extraRenderSvgStep(ItemBase *, QPointF offset, double dpi, double printerScale, QString & outputSvg);
	virtual ViewLayer::ViewLayerSpec createWireViewLayerSpec(ConnectorItem * from, ConnectorItem * to);
	virtual Wire * createTempWireForDragging(Wire * fromWire, ModelPart * wireModel, ConnectorItem * connectorItem, ViewGeometry & viewGeometry, ViewLayer::ViewLayerSpec);
	virtual void prereleaseTempWireForDragging(Wire*);
	void checkFit(ModelPart * newModelPart, ItemBase * itemBase, long newID,
								QHash<ConnectorItem *, Connector *> & found, QList<ConnectorItem *> & notFound,
								QHash<ConnectorItem *, ConnectorItem *> & m2f, QHash<ConnectorItem *, Connector *> & byWire,
								QHash<QString, QPolygonF> & legs, 	QHash<QString, ConnectorItem *> & formerLegs, QUndoCommand * parentCommand);
	void checkFitAux(ItemBase * tempItemBase, ItemBase * itemBase, long newID,
								QHash<ConnectorItem *, Connector *> & found, QList<ConnectorItem *> & notFound,
								QHash<ConnectorItem *, ConnectorItem *> & m2f, QHash<ConnectorItem *, Connector *> & byWire,
								QHash<QString, QPolygonF> & legs, QHash<QString, ConnectorItem *> & formerLegs, QUndoCommand * parentCommand);
	void changeLegAux(long fromID, const QString & fromConnectorID, const QPolygonF &, bool reset, bool relative, bool active, const QString & why);
	void moveLegBendpoints(bool undoOnly, QUndoCommand * parentCommand);
	void moveLegBendpointsAux(ConnectorItem * connectorItem, bool undoOnly, QUndoCommand * parentCommand);
	virtual void rotatePartLabels(double degrees, QTransform &, QPointF center, QUndoCommand * parentCommand);


protected:
	static bool lessThan(int a, int b);
	static bool greaterThan(int a, int b);

signals:
	void itemAddedSignal(ModelPart *, ViewLayer::ViewLayerSpec, const ViewGeometry &, long id, SketchWidget * dropOrigin);
	void itemDeletedSignal(long id);
	void clearSelectionSignal();
	void itemSelectedSignal(long id, bool state);
	void wireDisconnectedSignal(long fromID, QString fromConnectorID);
	void wireConnectedSignal(long fromID,  QString fromConnectorID, long toID, QString toConnectorID);
	void changeConnectionSignal(long fromID, QString fromConnectorID,
								long toID, QString toConnectorID,
								ViewLayer::ViewLayerSpec,
								bool connect, bool updateConnections);
	void copyBoundingRectsSignal(QHash<QString, QRectF> &);
	void cleanUpWiresSignal(CleanUpWiresCommand *);
	void selectionChangedSignal();

	void resizeSignal();
	void dropSignal(const QPoint &pos);
	void routingStatusSignal(SketchWidget *, const RoutingStatus &);
	void movingSignal(SketchWidget *, QUndoCommand * parentCommand);
	void selectAllItemsSignal(bool state, bool doEmit);
	void checkStickySignal(long id, bool doEmit, bool checkCurrent, CheckStickyCommand *);
	void disconnectAllSignal(QList<ConnectorItem *>, QHash<ItemBase *, SketchWidget *> & itemsToDelete, QUndoCommand * parentCommand);
	void setResistanceSignal(long itemID, QString resistance, QString pinSpacing, bool doEmit);
	void setPropSignal(long itemID, const QString & prop, const QString & value, bool doRedraw, bool doEmit);
	void setInstanceTitleSignal(long id, const QString & title, bool isUndoable, bool doEmit);
	void statusMessageSignal(QString, int timeout);
	void showLabelFirstTimeSignal(long itemID, bool show, bool doEmit);
	void dropPasteSignal(SketchWidget *);
	void changeBoardLayersSignal(int, bool doEmit);
	void firstTimeHelpHidden();
	void disconnectWireSignal(QSet<ItemBase *> &, QList<long> &, QUndoCommand * parentCommand);
	void deleteTracesSignal(QSet<ItemBase *> & deletedItems, QHash<ItemBase *, SketchWidget *> & otherDeletedItems, QList<long> & deletedIDs, bool isForeign, QUndoCommand * parentCommand);
	void makeDeleteItemCommandPrepSignal(ItemBase * itemBase, bool foreign, QUndoCommand * parentCommand);
	void makeDeleteItemCommandFinalSignal(ItemBase * itemBase, bool foreign, QUndoCommand * parentCommand);
	void warnSMDSignal(const QString &);
	void cursorLocationSignal(double xinches, double yinches);
	void ratsnestConnectSignal(long id, const QString & connectorID, bool connect, bool doEmit);
	void updatePartLabelInstanceTitleSignal(long itemID);
	void filenameIfSignal(QString & filename);

protected slots:
	void itemAddedSlot(ModelPart *, ViewLayer::ViewLayerSpec, const ViewGeometry &, long id, SketchWidget * dropOrigin);
	void itemDeletedSlot(long id);
	void clearSelectionSlot();
	void itemSelectedSlot(long id, bool state);
	void selectionChangedSlot();
	void wireChangedSlot(class Wire*, const QLineF & oldLine, const QLineF & newLine, QPointF oldPos, QPointF newPos, ConnectorItem * from, ConnectorItem * to);
	void wireChangedCurveSlot(class Wire*, const Bezier * oldB, const Bezier * newB, bool triggerFirstTime);
	virtual void wireSplitSlot(class Wire*, QPointF newPos, QPointF oldPos, const QLineF & oldLine);
	void wireJoinSlot(class Wire*, ConnectorItem * clickedConnectorItem);
	void toggleLayerVisibility();
	void wireConnectedSlot(long fromID, QString fromConnectorID, long toID, QString toConnectorID);
	void wireDisconnectedSlot(long fromID, QString fromConnectorID);
	void changeConnectionSlot(long fromID, QString fromConnectorID, long toID, QString toConnectorID, ViewLayer::ViewLayerSpec, bool connect, bool updateConnections);
	void navigatorScrollChange(double x, double y);
	void restartPasteCount();
	void dragIsDoneSlot(class ItemDrag *);
	void statusMessage(QString message, int timeout = 0);
	void cleanUpWiresSlot(CleanUpWiresCommand *);
	void updateInfoViewSlot();
	void spaceBarIsPressedSlot(bool);
	void autoScrollTimeout();
	void dragAutoScrollTimeout();
	void moveAutoScrollTimeout();
	void rememberSticky(long id, QUndoCommand * parentCommand);
	void rememberSticky(ItemBase *, QUndoCommand * parentCommand);
	void copyBoundingRectsSlot(QHash<QString, QRectF> &);
	void disconnectWireSlot(QSet<ItemBase *> &, QList<long> & deletedIDs, QUndoCommand * parentCommand);
	void deleteTracesSlot(QSet<ItemBase *> & deletedItems, QHash<ItemBase *, SketchWidget *> & otherDeletedItems, QList<long> & deletedIDs, bool isForeign, QUndoCommand * parentCommand);
	void vScrollToZero();
	void arrowTimerTimeout();
	void makeDeleteItemCommandPrepSlot(ItemBase * itemBase, bool foreign, QUndoCommand * parentCommand);
	void makeDeleteItemCommandFinalSlot(ItemBase * itemBase, bool foreign, QUndoCommand * parentCommand);
	void updatePartLabelInstanceTitleSlot(long itemID);
	void changePinLabelsSlot(ItemBase * itemBase, bool singleRow);
	void changePinLabels(ItemBase *, bool singleRow);

public slots:
	void changeWireColor(const QString newColor);
	void changeWireWidthMils(const QString newWidth);
 	void selectAllItems(bool state, bool doEmit);
	void setNoteText(long itemID, const QString & newText);
	void setInstanceTitle(long id, const QString & title, bool isUndoable, bool doEmit);
	void incInstanceTitle(long id);
	void showPartLabel(long id, bool showIt);
	void checkSticky(long id, bool doEmit, bool checkCurrent, CheckStickyCommand *);
	void resizeBoard(long id, double w, double h);
	void resizeJumperItem(long id, QPointF pos, QPointF c0, QPointF c1);
	void disconnectAllSlot(QList<ConnectorItem *>, QHash<ItemBase *, SketchWidget *> & itemsToDelete, QUndoCommand * parentCommand);
	void setResistance(long itemID, QString resistance, QString pinSpacing, bool doEmit);
	void setResistance(QString resistance, QString pinSpacing);
	void setProp(long itemID, const QString & prop, const QString & value, bool redraw, bool doEmit);
	void setProp(ItemBase *, const QString & propName, const QString & translatedPropName, const QString & oldValue, const QString & newValue, bool redraw);
	void setSpacing(const QString & spacing);
	void setForm(const QString & form);
	virtual void showLabelFirstTime(long itemID, bool show, bool doEmit);
	void resizeBoard(double w, double h, bool doEmit);
	virtual void changeBoardLayers(int layers, bool doEmit);
	void updateConnectors();
	void ratsnestConnect(long id, const QString & connectorID, bool connect, bool doEmit);

protected:
	enum StatusConnectStatus {
		StatusConnectNotTried,
		StatusConnectSucceeded,
		StatusConnectFailed
	};

protected:
	QPointer<PaletteModel> m_paletteModel;
	QPointer<class ReferenceModel> m_refModel;
	QPointer<SketchModel> m_sketchModel;
	ViewIdentifierClass::ViewIdentifier m_viewIdentifier;
	class WaitPushUndoStack * m_undoStack;
	class SelectItemCommand * m_holdingSelectItemCommand;
	class SelectItemCommand * m_tempDragWireCommand;
	LayerHash m_viewLayers;
	QHash<ViewLayer::ViewLayerID, bool> m_viewLayerVisibility;
	QPointer<Wire> m_connectorDragWire;
	QPointer<Wire> m_bendpointWire;
	ViewGeometry m_bendpointVG;
	QPointer<ConnectorItem> m_connectorDragConnector;
	bool m_droppingWire;
	QPointF m_droppingOffset;
	QPointer<ItemBase> m_droppingItem;
	int m_moveEventCount;
	//QList<QGraphicsItem *> m_lastSelected;  hack for 4.5.something
	ViewLayer::ViewLayerID m_wireViewLayerID;
	ViewLayer::ViewLayerID m_partViewLayerID;
	ViewLayer::ViewLayerID m_rulerViewLayerID;
	ViewLayer::ViewLayerID m_connectorViewLayerID;
	ViewLayer::ViewLayerID m_noteViewLayerID;
	QList<QGraphicsItem *> m_temporaries;
	bool m_chainDrag;
	QPointF m_mousePressScenePos;
	QPointF m_mousePressGlobalPos;
	QTimer m_autoScrollTimer;
	volatile int m_autoScrollX;
	volatile int m_autoScrollY;
	volatile int m_autoScrollCount;
	QPoint m_globalPos;

	QPointer<PaletteItem> m_lastPaletteItemSelected;

	int m_pasteCount;
	QPointF m_pasteOffset;

	// Part Menu
	QMenu *m_itemMenu;
	QMenu *m_wireMenu;

	bool m_infoViewOnHover;

	QHash<long, ItemBase *> m_savedItems;
	QHash<Wire *, ConnectorItem *> m_savedWires;
	QList<ItemBase *> m_additionalSavedItems;
	int m_ignoreSelectionChangeEvents;
	bool m_current;

	class SketchMainHelp * m_fixedToCenterItem;
	QPoint m_fixedToCenterItemOffset;

	QString m_lastColorSelected;

	ConnectorPairHash m_moveDisconnectedFromFemale;
	bool m_spaceBarIsPressed;
	bool m_spaceBarWasPressed;

	QPointer<ConnectorItem> m_lastHoverEnterConnectorItem;
	QPointer<ItemBase> m_lastHoverEnterItem;
	QString m_shortName;
	QPointer<Wire> m_dragBendpointWire;
	bool m_dragCurve;
	QPoint m_dragBendpointPos;
	QColor m_standardBackgroundColor;
	StatusConnectStatus m_statusConnectState;
	QList<QGraphicsItem *> m_inFocus;
	QString m_viewName;
	bool m_movingByArrow;
	int m_arrowTotalX;
	int m_arrowTotalY;
	bool m_movingByMouse;
	bool m_alignToGrid;
	double m_gridSizeInches;
	QPointer<ItemBase> m_alignmentItem;
	QPointF m_alignmentStartPoint;
	double m_zoom;
	bool m_draggingBendpoint;
	QPointer<SizeItem> m_sizeItem;
	int m_autoScrollThreshold;
	bool m_clearSceneRect;
	QPointer<ItemBase> m_moveReferenceItem;
	QPointer<QSvgRenderer> m_movingSVGRenderer;
	QPointF m_movingSVGOffset;
	QPointer<QGraphicsSvgItem> m_movingItem;
	QList< QPointer<ConnectorItem> > m_ratsnestUpdateDisconnect;
	QList< QPointer<ConnectorItem> > m_ratsnestUpdateConnect;
	QList <ItemBase *> m_checkUnder;
	bool m_addDefaultParts;
	QPointer<ItemBase> m_addedDefaultPart;
	float m_z;
	QTimer m_arrowTimer;
	bool m_middleMouseIsPressed;
	QMultiHash<ItemBase *, ConnectorItem *> m_stretchingLegs;
	bool m_curvyWires;
	bool m_rubberBandLegWasEnabled;

public:
	static ViewLayer::ViewLayerID defaultConnectorLayer(ViewIdentifierClass::ViewIdentifier viewId);

protected:
	static QHash<ViewIdentifierClass::ViewIdentifier,QColor> m_bgcolors;
	static const int MoveAutoScrollThreshold;
	static const int DragAutoScrollThreshold;
};

#endif
