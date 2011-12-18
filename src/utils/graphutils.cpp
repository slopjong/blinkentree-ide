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

$Revision: 5375 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-08 21:38:25 +0200 (Mon, 08 Aug 2011) $

********************************************************************/

#include "graphutils.h"
#include "../fsvgrenderer.h"
#include "../items/wire.h"
#include "../items/jumperitem.h"

#ifdef _MSC_VER 
#pragma warning(push) 
#pragma warning(disable:4100)			// disable scary-looking compiler warnings in Boost library
#pragma warning(disable:4181)			
#endif

#include <boost/config.hpp>
#include <boost/graph/transitive_closure.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>

#ifdef _MSC_VER 
#pragma warning(pop)					// restore warning state
#endif

bool GraphUtils::chooseRatsnestGraph(const QList<ConnectorItem *> & partConnectorItems, ConnectorPairHash & result) {
	using namespace boost;
	typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, double>, property < edge_weight_t, double > > Graph;
	typedef std::pair < int, int >E;

	if (partConnectorItems.count() < 2) return false;

	QList <ConnectorItem *> temp(partConnectorItems);

	//DebugDialog::debug("__________________");
	int tix = 0;
	while (tix < temp.count()) {
		ConnectorItem * connectorItem = temp[tix++];
		//connectorItem->debugInfo("check cross");
		ConnectorItem * crossConnectorItem = connectorItem->getCrossLayerConnectorItem();
		if (crossConnectorItem) {
			// it doesn't matter which one  on which layer we remove
			// when we check equal potential both of them will be returned
			//crossConnectorItem->debugInfo("\tremove cross");
			temp.removeOne(crossConnectorItem);
		}
	}

	QList<QPointF> locs;
	foreach (ConnectorItem * connectorItem, temp) {
		locs << connectorItem->sceneAdjustedTerminalPoint(NULL);
	}

	QList < QList<ConnectorItem *> > wiredTo;

	int num_nodes = temp.count();
	int num_edges = num_nodes * (num_nodes - 1) / 2;
	E * edges = new E[num_edges];
	double * weights = new double[num_edges];
	int ix = 0;
	QVector< QVector<double> > reverseWeights(num_nodes, QVector<double>(num_nodes, 0));
	for (int i = 0; i < num_nodes; i++) {
		ConnectorItem * c1 = temp.at(i);
		//c1->debugInfo("c1");
		for (int j = i + 1; j < num_nodes; j++) {
			edges[ix].first = i;
			edges[ix].second = j;
			ConnectorItem * c2 = temp.at(j);
			if ((c1->attachedTo() == c2->attachedTo()) && (c1->bus() != NULL) && (c1->bus() == c2->bus())) {
				weights[ix++] = 0;
				continue;
			}

			bool already = false;
			bool checkWiredTo = true;
			foreach (QList<ConnectorItem *> list, wiredTo) {
				if (list.contains(c1)) {
					checkWiredTo = false;
					if (list.contains(c2)) {
						weights[ix++] = 0;
						already = true;
					}
					break;
				}
			}
			if (already) continue;

			//c2->debugInfo("\tc2");

			if (checkWiredTo) {
				QList<ConnectorItem *> cwConnectorItems;
				cwConnectorItems.append(c1);
				ConnectorItem::collectEqualPotential(cwConnectorItems, true, ViewGeometry::NotTraceFlags);
				wiredTo.append(cwConnectorItems);
				//foreach (ConnectorItem * cx, cwConnectorItems) {
					//cx->debugInfo("\t\tcx");
				//}
				if (cwConnectorItems.contains(c2)) {
					weights[ix++] = 0;
					continue;
				}
			}

			//DebugDialog::debug("c2 not eliminated");
			double dx = locs[i].x() - locs[j].x();
			double dy = locs[i].y() - locs[j].y();
			weights[ix++] = reverseWeights[i][j] = reverseWeights[j][i] = (dx * dx) + (dy * dy);
		}
	}

	Graph g(edges, edges + num_edges, weights, num_nodes);
	property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, g);

	std::vector < graph_traits < Graph >::vertex_descriptor > p(num_vertices(g));

	prim_minimum_spanning_tree(g, &p[0]);

	for (std::size_t i = 0; i != p.size(); ++i) {
		if (i == p[i]) continue;
		if (reverseWeights[i][p[i]] == 0) continue;

		result.insert(temp[i], temp[p[i]]);
	}

	delete edges;
	delete weights;

	return true;
}

bool GraphUtils::scoreOneNet(QList<ConnectorItem *> & partConnectorItems, RoutingStatus & routingStatus) {
	using namespace boost;

	int num_nodes = partConnectorItems.count();

	typedef property < vertex_index_t, std::size_t > Index;
	typedef adjacency_list < listS, listS, directedS, Index > graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_t;
	typedef graph_traits < graph_t >::edge_descriptor edge_t;

	graph_t G;
	std::vector < vertex_t > verts(num_nodes);
	for (int i = 0; i < num_nodes; ++i) {
		verts[i] = add_vertex(Index(i), G);
	}

	//std::pair<int, int> pair;
	bool gotUserConnection = false;
	for (int i = 0; i < num_nodes; i++) {
		add_edge(verts[i], verts[i], G);
		ConnectorItem * from = partConnectorItems[i];
		for (int j = i + 1; j < num_nodes; j++) {
			ConnectorItem * to = partConnectorItems[j];

			if (from->isCrossLayerConnectorItem(to)) {
				add_edge(verts[i], verts[j], G);
				add_edge(verts[j], verts[i], G);
				continue;
			}

			if (to->attachedTo() != from->attachedTo()) {
				gotUserConnection = true;
				continue;
			}

			if ((to->bus() != NULL) && (to->bus() == from->bus())) {	
				add_edge(verts[i], verts[j], G);
				add_edge(verts[j], verts[i], G);
				continue;
			}

			gotUserConnection = true;
		}
	}

	if (!gotUserConnection) {
		return false;
	}

	routingStatus.m_netCount++;

	for (int i = 0; i < num_nodes; i++) {
		ConnectorItem * fromConnectorItem = partConnectorItems[i];
		if (fromConnectorItem->attachedToItemType() == ModelPart::Jumper) {
			routingStatus.m_jumperItemCount++;				
		}
		foreach (ConnectorItem * toConnectorItem, fromConnectorItem->connectedToItems()) {
			if (toConnectorItem->attachedToItemType() != ModelPart::Wire) {
				continue;
			}

			Wire * wire = qobject_cast<Wire *>(toConnectorItem->attachedTo());
			if (wire == NULL) continue;

			if (!wire->getTrace()) continue;

			QList<Wire *> wires;
			QList<ConnectorItem *> ends;
			wire->collectChained(wires, ends);
			foreach (ConnectorItem * end, ends) {
				if (end == fromConnectorItem) continue;

				int j = partConnectorItems.indexOf(end);
				if (j >= 0) {
					add_edge(verts[i], verts[j], G);
					add_edge(verts[j], verts[i], G);
				}
			}
		}
	}

	adjacency_list <> TC;
	transitive_closure(G, TC);

	QVector<bool> check(num_nodes, true);
	bool anyMissing = false;
	for (int i = 0; i < num_nodes - 1; i++) {
		if (!check[i]) continue;

		check[i] = false;
		bool missingOne = false;
		for (int j = i + 1; j < num_nodes; j++) {
			if (!check[j]) continue;

			if (edge(i, j, TC).second) {
				check[j] = false;
			}
			else {
				// we can minimally span the set with n-1 wires, so even if multiple connections are missing from a given connector, count it as one
				anyMissing = missingOne = true;
				/*
				ConnectorItem * ci = partConnectorItems.at(i);
				ConnectorItem * cj = partConnectorItems.at(j);
				DebugDialog::debug(QString("'%1' id:%2 cid:%3 vlid:%4 to '%5' id:%6 cid:%7 vlid:%8")
					.arg(ci->attachedToTitle())
					.arg(ci->attachedToID())
					.arg(ci->connectorSharedID())
					.arg(ci->attachedToViewLayerID())
					.arg(cj->attachedToTitle())
					.arg(cj->attachedToID())
					.arg(cj->connectorSharedID())
					.arg(cj->attachedToViewLayerID())
					);
				*/
			}
		}
		if (missingOne) {
			routingStatus.m_connectorsLeftToRoute++;
		}
	}

	if (!anyMissing) {
		routingStatus.m_netRoutedCount++;
	}

	return true;
}
