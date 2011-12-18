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

$Revision: 5442 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-29 08:10:08 +0200 (Mon, 29 Aug 2011) $

********************************************************************/



#ifndef HTMLINFOVIEW_H
#define HTMLINFOVIEW_H

#include <QFrame>
#include <QGraphicsSceneHoverEvent>
#include <QMutex>
#include <QTimer>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>

#include "../items/itembase.h"
#include "../items/wire.h"
#include "../connectors/connectoritem.h"
#include "../referencemodel/referencemodel.h"

struct PropThing {
        QPointer<QLabel> m_name;
        QPointer<QFrame> m_frame;
        QPointer<QLabel> m_value;
        QPointer<QWidget> m_plugin;
        QPointer<QVBoxLayout> m_layout;
};

class TagLabel : public QLabel {
	Q_OBJECT

public:
	TagLabel(QWidget * parent);

protected:
	QSize sizeHint() const;
};

class HtmlInfoView : public QScrollArea
{
Q_OBJECT
public:
	HtmlInfoView(QWidget * parent = 0);
	~HtmlInfoView();

	QSize sizeHint() const;
	void setContent(const QString& html);

	ItemBase *currentItem();
	void reloadContent(class InfoGraphicsView *);

	void viewItemInfo(class InfoGraphicsView *, ItemBase* item, bool swappingEnabled);

	void hoverEnterItem(class InfoGraphicsView *, QGraphicsSceneHoverEvent * event, ItemBase * item, bool swappingEnabled);
	void hoverLeaveItem(class InfoGraphicsView *, QGraphicsSceneHoverEvent * event, ItemBase * item);

	void viewConnectorItemInfo(ConnectorItem* item);
	void hoverEnterConnectorItem(class InfoGraphicsView *, QGraphicsSceneHoverEvent * event, ConnectorItem * item, bool swappingEnabled);
	void hoverLeaveConnectorItem(class InfoGraphicsView *, QGraphicsSceneHoverEvent * event, ConnectorItem * item);

	void unregisterCurrentItem();
	void unregisterCurrentItemIf(long id);

public:
	static const int STANDARD_ICON_IMG_WIDTH;
	static const int STANDARD_ICON_IMG_HEIGHT;

	static void cleanup();
	static QHash<QString, QString> getPartProperties(ModelPart * modelPart, ItemBase * itemBase, bool wantDebug, QStringList & keys);

protected slots:
	void setContent();
	void setInstanceTitle();
	void instanceTitleEnter();
	void instanceTitleLeave();
	void instanceTitleEditable(bool editable);
	void changeLock(bool);

protected:
	void appendStuff(ItemBase* item, bool swappingEnabled); //finds out if it's a wire or something else
	void appendWireStuff(Wire* wire, bool swappingEnabled);
	void appendItemStuff(ItemBase* base, bool swappingEnabled);
	void appendItemStuff(ItemBase * base, ModelPart * modelPart, bool swappingEnabled, bool labelIsVisible = false);

	void setInstanceTitleColors(class FLineEdit * edit, const QColor & base, const QColor & text);

	void setCurrentItem(ItemBase *);
	void setNullContent();
	void setUpTitle(ItemBase *);
	void setUpIcons(ModelPart *);
	void addTags(ModelPart * modelPart);
	void partTitle(const QString & title, const QString & version, const QString & url);
	void displayProps(ModelPart * modelPart, ItemBase * itemBase, bool swappingEnabled);
	void clearPropThingPlugin(PropThing * propThing);
	void clearPropThingPlugin(PropThing * propThing, QWidget * plugin);
	QPixmap * getPixmap(ModelPart * modelPart, ViewIdentifierClass::ViewIdentifier viewIdentifier);

protected:
	QPointer<ItemBase> m_currentItem;
	bool m_currentSwappingEnabled;					// previous item (possibly hovered over)

	QTimer m_setContentTimer;
	QPointer<class InfoGraphicsView> m_infoGraphicsView;
	QPointer<ItemBase> m_lastItemBase;
	bool m_lastSwappingEnabled;						// previous item (selected)
	class FLineEdit * m_titleEdit;
	QLabel * m_icon1;
	QLabel * m_icon2;
	QLabel * m_icon3;
	QLabel * m_partTitle;
	QLabel * m_partUrl;
	QLabel * m_partVersion;
	QLabel * m_tagsTextLabel;
	QLabel * m_connDescr;
	QLabel * m_connName;
	QLabel * m_connType;
	QLabel * m_proplabel;
	QLabel * m_taglabel;
	QLabel * m_connLabel;
	QFrame * m_connFrame;
	QFrame * m_propFrame;
	QCheckBox * m_lockCheckbox;
	QGridLayout * m_propLayout;
	QList <PropThing *> m_propThings;
    QPointer<class InfoGraphicsView> m_pendingInfoGraphicsView;
	QPointer<ItemBase> m_pendingItemBase;
	bool m_pendingSwappingEnabled;

	// note: these m_last items should only be checked for equality and not otherwise accessed
	ItemBase * m_lastTitleItemBase;
	QString m_lastPartTitle;
	QString m_lastPartVersion;
	ModelPart * m_lastTagsModelPart;
	int m_lastConnectorItemCount;
	ConnectorItem * m_lastConnectorItem;
	ModelPart * m_lastIconModelPart;
	ModelPart * m_lastPropsModelPart;
	ItemBase * m_lastPropsItemBase;
	bool m_lastPropsSwappingEnabled;
	// end note

protected:
	static QHash<QString, QPixmap *> m_pixmaps;
};

#endif
