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

$Revision: 5619 $:
$Author: cohen@irascible.com $:
$Date: 2011-11-16 16:16:04 +0100 (Wed, 16 Nov 2011) $

********************************************************************/



#ifndef PARTSBINVIEW_H_
#define PARTSBINVIEW_H_

#include "../model/palettemodel.h"
#include "../items/paletteitem.h"
#include "../referencemodel/referencemodel.h"

class PartsBinPaletteWidget;

class PartsBinView {

	public:
		PartsBinView(ReferenceModel *refModel, PartsBinPaletteWidget *parent);
		virtual ~PartsBinView() {};				// removes compiler warnings

		virtual void setPaletteModel(PaletteModel * model, bool clear = false);
		void reloadParts(PaletteModel * model);
		void addPart(ModelPart * model, int position = -1);
		virtual void removePart(const QString &moduleID) = 0;
        virtual void removeParts() = 0;

		virtual ModelPart *selected() = 0;

		bool contains(const QString &moduleID);
		void setInfoViewOnHover(bool infoViewOnHover);

		virtual QList<QObject*> orderedChildren() = 0;
		void dropEventAux(QDropEvent* event, bool justAppend = false);

	protected:
		virtual void doClear();
		virtual void moveItem(int fromIndex, int toIndex) = 0;
		virtual int itemIndexAt(const QPoint& pos, bool &trustIt) = 0;
		virtual void setSelected(int position, bool doEmit=false) = 0;

		void setItem(ModelPart * modelPart);

		void mousePressOnItem(
				const QPoint &dragStartPos,
				const QString &moduleId, const QSize &size,
				const QPointF &dataPoint = QPointF(0,0), const QPoint &hotspot = QPoint(0,0));
		void dragMoveEnterEventAux(QDragMoveEvent* event);

		virtual int setItemAux(ModelPart * modelPart, int position = -1) = 0;

	public:
		static QHash<QString, QString> TranslatedCategoryNames;

	protected:

		ReferenceModel *m_refModel;
		PartsBinPaletteWidget *m_parent;

		QHash<QString /*moduleId*/,ModelPart*> m_partHash;
		bool m_infoViewOnHover;

		QPoint m_dragStartPos;
};

#endif /* PARTSBINVIEW_H_ */
