/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2010 Fachhochschule Potsdam - http://fh-potsdam.de

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

$Revision: 4270 $:
$Author: cohen@irascible.com $:
$Date: 2010-06-17 17:35:25 +0200 (Thu, 17 Jun 2010) $

********************************************************************/



#ifndef CONNECTORSINFOWIDGET_H_
#define CONNECTORSINFOWIDGET_H_

#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>

#include "singleconnectorinfowidget.h"
#include "mismatchingconnectorwidget.h"

class PartsEditorViewsWidget;
class ConnectorsInfoWidget : public QFrame {
	Q_OBJECT
	public:
		ConnectorsInfoWidget(WaitPushUndoStack *undoStack, QWidget *parent=0);
		const QList< QPointer<ConnectorShared> > connectorsShared();
		QCheckBox *showTerminalPointsCheckBox();
		int scrollBarWidth();
		void setViews(PartsEditorViewsWidget* connsView);

		bool connectorsCountChanged();
		bool connectorsRemoved();
		bool connectorAdded();
		bool hasMismatchingConnectors();

	public slots:
		void connectorsFound(QList< QPointer<Connector> >);
		void informConnectorSelection(const QString &);
		void informEditionCompleted();
		void syncNewConnectors(ViewIdentifierClass::ViewIdentifier viewId, const QList< QPointer<Connector> > &conns);
		void emitPaintNeeded();
		void addConnector();
		void removeSelectedConnector();
		void removeConnector(AbstractConnectorInfoWidget* connInfo);
		void removeTerminalPoint(const QString &connId, ViewIdentifierClass::ViewIdentifier vid);

	signals:
		void connectorSelected(const QString &);
		void editionCompleted();
		void existingConnectorSignal(ViewIdentifierClass::ViewIdentifier viewId, const QString &id, Connector* existingConnector, Connector * newConnector);
		void setMismatching(ViewIdentifierClass::ViewIdentifier viewId, const QString &connId, bool mismatching);
		void repaintNeeded();
		void showTerminalPoints(bool show);
		void drawConnector(Connector*);
		void drawConnector(ViewIdentifierClass::ViewIdentifier, Connector*);
		void removeConnectorFrom(const QString &connId, ViewIdentifierClass::ViewIdentifier view);

	protected slots:
		void updateLayout();
		void selectionChanged(AbstractConnectorInfoWidget* selected);
		void deleteAux();
		void connectorSelectedInView(const QString &connId);
		void completeConn(MismatchingConnectorWidget* mcw);

	protected:
		void createScrollArea();
		void createToolsArea();

		void addConnectorInfo(MismatchingConnectorWidget* mcw);
		Connector* addConnectorInfo(QString id);
		void addConnectorInfo(Connector *conn);
		void addMismatchingConnectorInfo(MismatchingConnectorWidget *mcw);
		void addMismatchingConnectorInfo(ViewIdentifierClass::ViewIdentifier viewID, QString connId);
		QVBoxLayout *scrollContentLayout();
		bool eventFilter(QObject *obj, QEvent *event);
		void setSelected(AbstractConnectorInfoWidget * newSelected);
		void selectNext();
		void selectPrev();

		void clearMismatchingForView(ViewIdentifierClass::ViewIdentifier viewId);
		void singleToMismatchingNotInView(ViewIdentifierClass::ViewIdentifier viewId, const QStringList &connIds);

		bool existingConnId(const QString &id);
		MismatchingConnectorWidget* existingMismatchingConnector(const QString &id);
		void removeMismatchingConnectorInfo(MismatchingConnectorWidget* mcw, bool singleShot = true, bool alsoDeleteFromView = false);
		void removeConnectorInfo(SingleConnectorInfoWidget *sci, bool singleShot = true, bool alsoDeleteFromView = false);
		Connector* findConnector(const QString &id);
		SingleConnectorInfoWidget * findSCI(const QString &id);

		int nextConnId();
		void resetType(ViewIdentifierClass::ViewIdentifier viewId, SingleConnectorInfoWidget * sci, Connector * conn);
		void resetName(ViewIdentifierClass::ViewIdentifier viewId, SingleConnectorInfoWidget * sci, Connector * conn);

protected:

		//QHash<QString /*connId*/, QMultiHash<ViewIdentifierClass::ViewIdentifier, SvgIdLayer*> > m_connectorsPins;

		QLabel *m_title;
		QScrollArea *m_scrollArea;

		QFrame *m_scrollContent;
		QFrame *m_mismatchersFrame;
		QFrame *m_mismatchersFrameParent;

		QFrame *m_toolsContainter;
		PartsEditorViewsWidget* m_views;

		AbstractConnectorInfoWidget *m_selected;

		QList<SingleConnectorInfoWidget*> m_connsInfo;
		QList< QPointer<MismatchingConnectorWidget> > m_mismatchConnsInfo;
		QHash<QString /*connId*/, AbstractConnectorInfoWidget*> m_allConnsInfo;

		QStringList m_connIds;

		WaitPushUndoStack *m_undoStack;

		QObject *m_objToDelete;

		bool m_connRemoved;
		bool m_connAdded;
};

#endif /* CONNECTORSINFOWIDGET_H_ */
