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

$Revision: 5402 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-16 21:53:11 +0200 (Tue, 16 Aug 2011) $

********************************************************************/



#ifndef BREADBOARDSKETCHWIDGET_H
#define BREADBOARDSKETCHWIDGET_H

#include "sketchwidget.h"

class BreadboardSketchWidget : public SketchWidget
{
	Q_OBJECT

public:
    BreadboardSketchWidget(ViewIdentifierClass::ViewIdentifier, QWidget *parent=0);

	void addViewLayers();
	void initWire(Wire *, int penWidth);
	bool canDisconnectAll();
	bool ignoreFemale();
	void addDefaultParts();
	void showEvent(QShowEvent * event);
	double getWireStrokeWidth(Wire *, double wireWidth);
	void getBendpointWidths(class Wire *, double w, double & w1, double & w2, bool & negativeOffsetRect);

protected:
	void setWireVisible(Wire * wire);
	bool collectFemaleConnectees(ItemBase *, QSet<ItemBase *> &);
	void findConnectorsUnder(ItemBase * item);
	bool checkUnder();
	bool disconnectFromFemale(ItemBase * item, QHash<long, ItemBase *> & savedItems, ConnectorPairHash &, bool doCommand, bool rubberBandLegEnabled, QUndoCommand * parentCommand);
	BaseCommand::CrossViewType wireSplitCrossView();
	bool canDropModelPart(ModelPart * modelPart); 
	/*
	void translateToLocalItems(ConnectorPairHash & foreignMoveItems, ConnectorPairHash & moveItems,	QMultiHash<PaletteItemBase *, ConnectorItem *> & bases);
	bool shareBreadboard(ConnectorItem * fromConnectorItem, ConnectorItem * toConnectorItem, ItemBase * & breadboardItemBase);
	*/
	void getLabelFont(QFont &, QColor &, ViewLayer::ViewLayerSpec);
	void setNewPartVisible(ItemBase *);
	double defaultGridSizeInches();
	ViewLayer::ViewLayerID getLabelViewLayerID(ViewLayer::ViewLayerSpec);
	QPoint calcFixedToCenterItemOffset(const QRect & viewPortRect, const QSizeF & helpSize);

};

#endif
