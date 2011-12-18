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


#include <QHBoxLayout>
#include <QLabel>

#include "sketchareawidget.h"

const QString SketchAreaWidget::RoutingStateLabelName = "routingStateLabel";

SketchAreaWidget::SketchAreaWidget(SketchWidget *graphicsView, QMainWindow *parent)
	: QFrame(parent)
{
	m_graphicsView = graphicsView;
	graphicsView->setParent(this);

	createLayout();

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_graphicsView);
	layout->addWidget(m_toolbar);
	layout->addWidget(m_statusBarArea);
	m_statusBarArea->setFixedHeight(parent->statusBar()->height()*3/4);
}

SketchAreaWidget::~SketchAreaWidget() {
	// TODO Auto-generated destructor stub
}

ViewIdentifierClass::ViewIdentifier SketchAreaWidget::viewIdentifier() {
	return m_graphicsView->viewIdentifier();
}

SketchWidget *SketchAreaWidget::graphicsView() {
	return m_graphicsView;
}

void SketchAreaWidget::createLayout() {
	m_toolbar = new QFrame(this);
	m_toolbar->setObjectName("sketchAreaToolbar");
	m_toolbar->setFixedHeight(60);

	QFrame *leftButtons = new QFrame(m_toolbar);
	m_buttonsContainer = new QHBoxLayout(leftButtons);
	m_buttonsContainer->setMargin(0);
	m_buttonsContainer->setSpacing(3);

	QFrame *middleButtons = new QFrame(m_toolbar);
	middleButtons->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
	m_labelContainer = new QVBoxLayout(middleButtons);
	m_labelContainer->setSpacing(0);
	m_labelContainer->setMargin(0);

	QFrame *rightButtons = new QFrame(m_toolbar);

	QHBoxLayout *toolbarLayout = new QHBoxLayout(m_toolbar);
	toolbarLayout->setMargin(2);
	toolbarLayout->setSpacing(0);
	toolbarLayout->addWidget(leftButtons);
	toolbarLayout->addWidget(middleButtons);
	toolbarLayout->addWidget(rightButtons);

	m_statusBarArea = new QFrame(this);
	m_statusBarArea->setObjectName("statusBarContainer");
	QVBoxLayout *statusbarlayout = new QVBoxLayout(m_statusBarArea);
	statusbarlayout->setMargin(0);
	statusbarlayout->setSpacing(0);
}

void SketchAreaWidget::setToolbarWidgets(QList<QWidget*> widgets) {
	foreach(QWidget* widget, widgets) {
		if(widget->objectName() != RoutingStateLabelName) {
			m_buttonsContainer->addWidget(widget);
		} else {
			m_labelContainer->addSpacerItem(new QSpacerItem(0,1,QSizePolicy::Maximum));
			m_labelContainer->addWidget(widget);
			m_labelContainer->addSpacerItem(new QSpacerItem(0,1,QSizePolicy::Maximum));
		}
	}
}

void SketchAreaWidget::addStatusBar(QStatusBar *statusBar) {
	m_statusBarArea->layout()->addWidget(statusBar);
}

QWidget *SketchAreaWidget::separator(QWidget* parent) {
	QLabel *separator = new QLabel(parent);
	separator->setPixmap(QPixmap(":/resources/images/toolbar_icons/toolbar_separator.png"));
	separator->setStyleSheet("margin-left: 10px; margin-right: 10px;");
	return separator;
}

void SketchAreaWidget::setRoutingStatusLabel(ExpandingLabel * rsl) {
	m_routingStatusLabel = rsl;
}

ExpandingLabel * SketchAreaWidget::routingStatusLabel() {
	return m_routingStatusLabel;
}

QFrame * SketchAreaWidget::toolbar() {
	return m_toolbar;
}
