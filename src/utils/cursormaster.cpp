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

$Revision: 5615 $:
$Author: cohen@irascible.com $:
$Date: 2011-11-14 14:41:31 +0100 (Mon, 14 Nov 2011) $

********************************************************************/

#include "cursormaster.h"
#include "../debugdialog.h"


#include <QApplication>
#include <QBitmap>
#include <QString>
#include <QKeyEvent>
#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

QCursor * CursorMaster::BendpointCursor = NULL;
QCursor * CursorMaster::NewBendpointCursor = NULL;
QCursor * CursorMaster::MakeWireCursor = NULL;
QCursor * CursorMaster::MakeCurveCursor = NULL;
QCursor * CursorMaster::RubberbandCursor = NULL;
QCursor * CursorMaster::MoveCursor = NULL;
QCursor * CursorMaster::BendlegCursor = NULL;
QCursor * CursorMaster::RotateCursor = NULL;

//static QTimer timer;

CursorMaster CursorMaster::TheCursorMaster;
static QList<QObject *> Listeners;

static QHash<QGraphicsScene *, QGraphicsPixmapItem *> CursorItems;

CursorMaster::CursorMaster() : QObject()
{
}

void CursorMaster::initCursors()
{
	if (BendpointCursor == NULL) {
		//timer.setSingleShot(true);
		//timer.setInterval(0);
		//connect(&timer, SIGNAL(timeout()), &TheCursorMaster, SLOT(moveCursor()));

		QBitmap bitmap1(":resources/images/cursor/bendpoint.bmp");
		QBitmap bitmap1m(":resources/images/cursor/bendpoint_mask.bmp");
		BendpointCursor = new QCursor(bitmap1, bitmap1m, 0, 0);

		QBitmap bitmap2(":resources/images/cursor/new_bendpoint.bmp");
		QBitmap bitmap2m(":resources/images/cursor/new_bendpoint_mask.bmp");
		NewBendpointCursor = new QCursor(bitmap2, bitmap2m, 0, 0);

		QBitmap bitmap3(":resources/images/cursor/make_wire.bmp");
		QBitmap bitmap3m(":resources/images/cursor/make_wire_mask.bmp");
		MakeWireCursor = new QCursor(bitmap3, bitmap3m, 0, 0);

		QBitmap bitmap4(":resources/images/cursor/curve.bmp");
		QBitmap bitmap4m(":resources/images/cursor/curve_mask.bmp");
		MakeCurveCursor = new QCursor(bitmap4, bitmap4m, 0, 0);

		QBitmap bitmap5(":resources/images/cursor/rubberband_move.bmp");
		QBitmap bitmap5m(":resources/images/cursor/rubberband_move_mask.bmp");
		RubberbandCursor = new QCursor(bitmap5, bitmap5m, 0, 0);

		QBitmap bitmap6(":resources/images/cursor/part_move.bmp");
		QBitmap bitmap6m(":resources/images/cursor/part_move_mask.bmp");
		MoveCursor = new QCursor(bitmap6, bitmap6m, 0, 0);

		QBitmap bitmap7(":resources/images/cursor/bendleg.bmp");
		QBitmap bitmap7m(":resources/images/cursor/bendleg_mask.bmp");
		BendlegCursor = new QCursor(bitmap7, bitmap7m, 0, 0);

		QBitmap bitmap8(":resources/images/cursor/rotate.bmp");
		QBitmap bitmap8m(":resources/images/cursor/rotate_mask.bmp");
		RotateCursor = new QCursor(bitmap8, bitmap8m, 0, 0);

		QApplication::instance()->installEventFilter(instance());
	}
}

CursorMaster * CursorMaster::instance()
{
	return &TheCursorMaster;
}

void CursorMaster::addCursor(QObject * object, const QCursor & cursor)
{
	if (object == NULL) return;


	/*
	QGraphicsItem * item = dynamic_cast<QGraphicsItem *>(object);
	if (item == NULL) return;

	QGraphicsScene * scene = item->scene();
	if (scene == NULL) return;

	QGraphicsView * view = dynamic_cast<QGraphicsView *>(scene->parent());
	if (view == NULL) return;

	QGraphicsPixmapItem * pixmapItem = CursorItems.value(scene, NULL);
	if (pixmapItem == NULL) {
		pixmapItem = new QGraphicsPixmapItem();
		pixmapItem->setZValue(10000);			// always on top
		pixmapItem->setVisible(true);
		pixmapItem->setAcceptedMouseButtons(0);
		pixmapItem->setAcceptDrops(false);
		pixmapItem->setAcceptTouchEvents(false);
		pixmapItem->setAcceptHoverEvents(false);
		pixmapItem->setEnabled(false);
		pixmapItem->setFlags(QGraphicsItem::ItemIgnoresTransformations);
		CursorItems.insert(scene, pixmapItem);
		scene->addItem(pixmapItem);
		scene->installEventFilter(this);
	}

	pixmapItem->setPixmap(*cursor.mask());
	pixmapItem->setPos(view->mapToScene(view->mapFromGlobal(QCursor::pos())) + cursor.hotSpot());
	*/

	if (Listeners.contains(object)) {
		if (Listeners.first() != object) {
			Listeners.removeOne(object);
			Listeners.push_front(object);
		}
		//DebugDialog::debug(QString("changing cursor %1").arg((long) object));
		QApplication::changeOverrideCursor(cursor);
		return;
	}

	Listeners.push_front(object);
	connect(object, SIGNAL(destroyed(QObject *)), this, SLOT(deleteCursor(QObject *)));
	QApplication::setOverrideCursor(cursor);
	//DebugDialog::debug(QString("addding cursor %1").arg((long) object));
}

void CursorMaster::removeCursor(QObject * object)
{
	if (object == NULL) return;

	if (Listeners.contains(object)) {
		disconnect(object, SIGNAL(destroyed(QObject *)), this, SLOT(deleteCursor(QObject *)));
		Listeners.removeOne(object);
		QApplication::restoreOverrideCursor();
		//DebugDialog::debug(QString("removing cursor %1").arg((long) object));
	}


	/*
	QGraphicsItem * item = dynamic_cast<QGraphicsItem *>(object);
	if (item == NULL) return;

	QGraphicsScene * scene = item->scene();
	if (scene == NULL) return;

	QGraphicsPixmapItem * pixmapItem = CursorItems.value(scene, NULL);
	if (pixmapItem == NULL) return;

	pixmapItem->hide();
	*/
}

void CursorMaster::deleteCursor(QObject * object)
{
	removeCursor(object);
}

bool CursorMaster::eventFilter(QObject * object, QEvent * event)
{
	QApplication * application = NULL;
	//QGraphicsScene * scene = NULL;

	switch (event->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			application = dynamic_cast<QApplication *>(object);
			if (application) {
				QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
				foreach (QObject * listener, Listeners) {
					if (listener) {
						dynamic_cast<CursorKeyListener *>(listener)->cursorKeyEvent(keyEvent->modifiers());
						break;
					}
				}
			}
			break;
/*
		case QEvent::GraphicsSceneMouseMove:
			
			scene = dynamic_cast<QGraphicsScene *>(object);
			if (scene) {
				QGraphicsPixmapItem * pixmapItem = CursorItems.value(scene, NULL);
				if (pixmapItem) {
					timer.setProperty("loc", ((QGraphicsSceneMouseEvent *) event)->scenePos());
					timer.setUserData(1, (QObjectUserData *) pixmapItem);
				}
			}
			break;

		case QEvent::Leave:
			scene = dynamic_cast<QGraphicsScene *>(object);
			if (scene) {
				QGraphicsPixmapItem * pixmapItem = CursorItems.value(scene, NULL);
				if (pixmapItem) {
					//DebugDialog::debug("pos", ((QGraphicsSceneMouseEvent *) event)->scenePos());
					pixmapItem->hide();
				}
			}
			break;
*/
		default:
			break;
	}
	

	return false;
}

void CursorMaster::moveCursor() {
	//QObject * t = sender();
	//if (t == NULL) return;

	//QPointF p = t->property("loc").toPointF();
	//QGraphicsPixmapItem * item = (QGraphicsPixmapItem *) t->userData(1);

	//DebugDialog::debug("move", p);
	//item->setPos(p);   // + cursor->hotspot
	//item->show();
}
