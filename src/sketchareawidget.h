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

$Revision: 4233 $:
$Author: cohen@irascible.com $:
$Date: 2010-06-10 15:20:13 +0200 (Thu, 10 Jun 2010) $

********************************************************************/



#ifndef SKETCHAREAWIDGET_H_
#define SKETCHAREAWIDGET_H_

#include <QFrame>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMainWindow>

#include "sketch/sketchwidget.h"


class SketchAreaWidget : public QFrame {
public:
	SketchAreaWidget(SketchWidget *graphicsView, QMainWindow *parent);
	virtual ~SketchAreaWidget();

	ViewIdentifierClass::ViewIdentifier viewIdentifier();
	SketchWidget* graphicsView();

	void setToolbarWidgets(QList<QWidget*> buttons);
	void addStatusBar(QStatusBar *);
	static QWidget *separator(QWidget* parent);
	class ExpandingLabel * routingStatusLabel();
	void setRoutingStatusLabel(ExpandingLabel *);
	QFrame * toolbar();

protected:
	void createLayout();

public:
	static const QString RoutingStateLabelName;

protected:
	SketchWidget *m_graphicsView;

	QFrame *m_toolbar;
	QHBoxLayout *m_buttonsContainer;
	QVBoxLayout *m_labelContainer;
	QFrame *m_statusBarArea;
	class ExpandingLabel * m_routingStatusLabel;

};

#endif /* SKETCHAREAWIDGET_H_ */
