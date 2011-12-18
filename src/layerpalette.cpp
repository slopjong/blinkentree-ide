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

$Revision: 5510 $:
$Author: cohen@irascible.com $:
$Date: 2011-09-21 05:40:25 +0200 (Wed, 21 Sep 2011) $

********************************************************************/

#include "layerpalette.h"

#include <QAction>


ViewLayerCheckBox::ViewLayerCheckBox(QWidget * parent) : QCheckBox(parent) {
}


ViewLayerCheckBox::~ViewLayerCheckBox()
{
}

void ViewLayerCheckBox::setViewLayer(ViewLayer * viewLayer)
{
	m_viewLayer = viewLayer;
}

ViewLayer * ViewLayerCheckBox::viewLayer() {
	return m_viewLayer;
}

//////////////////////////////////////

LayerPalette::LayerPalette(QWidget * parent) : QScrollArea(parent) 
{
	m_hideAllLayersAct = m_showAllLayersAct = NULL;

	QFrame * frame = new QFrame(this);

	m_mainLayout = new QVBoxLayout();
	m_mainLayout->setSizeConstraint( QLayout::SetMinAndMaxSize );

	for (int i = 0; i < ViewLayer::ViewLayerCount; i++) {
		ViewLayerCheckBox * cb = new ViewLayerCheckBox(this);
		connect(cb, SIGNAL(clicked(bool)), this, SLOT(setLayerVisibility(bool)));
		m_checkBoxes.append(cb);
		cb->setVisible(false);
	}

	m_groupBox = new QGroupBox("");
	QVBoxLayout * groupLayout = new QVBoxLayout();

	m_showAllWidget = new QCheckBox(tr("show all layers"));
	connect(m_showAllWidget, SIGNAL(clicked(bool)), this, SLOT(setAllLayersVisible(bool)));

	groupLayout->addWidget(m_showAllWidget);

	m_groupBox->setLayout(groupLayout);

	m_mainLayout->addWidget(m_groupBox);

	frame->setLayout(m_mainLayout);

	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setWidget(frame);

}

LayerPalette::~LayerPalette() 
{
}

void LayerPalette::resetLayout(LayerHash & viewLayers, LayerList & keys) {
	m_mainLayout->removeWidget(m_groupBox);
	foreach (ViewLayerCheckBox * cb, m_checkBoxes) {
		m_mainLayout->removeWidget(cb);
		cb->setVisible(false);
	}

	int ix = 0;
	foreach (ViewLayer::ViewLayerID key, keys) {
		ViewLayer * viewLayer = viewLayers.value(key);
		//if (viewLayer->parentLayer()) continue;

		ViewLayerCheckBox * cb = m_checkBoxes[ix++];
		cb->setText(viewLayer->action()->text());
		cb->setViewLayer(viewLayer);
		cb->setVisible(true);
		m_mainLayout->addWidget(cb);
	}

	m_mainLayout->addWidget(m_groupBox);
	m_mainLayout->invalidate();

}

void LayerPalette::updateLayerPalette(LayerHash & viewLayers, LayerList & keys)
{
	m_showAllWidget->setEnabled(m_showAllLayersAct->isEnabled() || m_hideAllLayersAct->isEnabled());
	m_showAllWidget->setChecked(!m_showAllLayersAct->isEnabled());

	int ix = 0;
	foreach (ViewLayer::ViewLayerID key, keys) {
		ViewLayer * viewLayer = viewLayers.value(key);
		//if (viewLayer->parentLayer()) continue;

		ViewLayerCheckBox * cb = m_checkBoxes[ix++];
		cb->setChecked(viewLayer->action()->isChecked());
		cb->setEnabled(viewLayer->action()->isEnabled());
	}
}

void LayerPalette::setLayerVisibility(bool) {
	ViewLayerCheckBox * cb = qobject_cast<ViewLayerCheckBox *>(sender());
	if (cb == NULL) return;

	// want to toggle layer individually in this case
	cb->viewLayer()->setIncludeChildLayers(false);
	cb->viewLayer()->action()->trigger();
	cb->viewLayer()->setIncludeChildLayers(true);
}

void LayerPalette::setShowAllLayersAction(QAction * action) 
{
	m_showAllLayersAct = action;
}

void LayerPalette::setHideAllLayersAction(QAction * action) 
{
	m_hideAllLayersAct = action;
}

void LayerPalette::setAllLayersVisible(bool vis) {
	if (vis) {
		if (m_showAllLayersAct) {
			m_showAllLayersAct->trigger();
		}
	}
	else {
		if (m_hideAllLayersAct) {
			m_hideAllLayersAct->trigger();
		}
	}
}

