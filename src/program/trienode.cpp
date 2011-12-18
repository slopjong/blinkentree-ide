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

$Revision: 4183 $:
$Author: cohen@irascible.com $:
$Date: 2010-05-06 22:30:19 +0200 (Thu, 06 May 2010) $

********************************************************************/



#include "trienode.h"
#include "../debugdialog.h"

#include <QRegExp>
#include <QXmlStreamReader>


TrieLeaf::TrieLeaf()
{
}

TrieLeaf::~TrieLeaf()
{
}

TrieNode::TrieNode(QChar c) 
{
	m_char = c;
	m_leafData = NULL;
	m_isLeaf = false;
	m_caseInsensitive = false;
}

TrieNode::~TrieNode() 
{
	foreach (TrieNode * node, m_children) {
		delete node;
	}
	m_children.clear();
}

void TrieNode::addString(QString & string, bool caseInsensitive, TrieLeaf * leaf) {
	if (string.isEmpty()) {
		m_leafData = leaf;
		m_isLeaf = true;
		return;
	}

	QChar in(string.at(0));
	QString next = string.remove(0, 1);
	QChar c0, c1;
	if (caseInsensitive) {
		c0 = in.toLower();
		c1 = in.toUpper();
	}
	else {
		c0 = c1 = in;
	}

	addStringAux(c0, next, caseInsensitive, leaf);

	if (c0 != c1) {
		addStringAux(c1, next, caseInsensitive, leaf);
	}
}

void TrieNode::addStringAux(QChar c, QString & next, bool caseInsensitive, TrieLeaf * leaf) {
	bool gotc = false;
	foreach (TrieNode * node, m_children) {
		if (node->matchesChar(c)) {
			node->addString(next, caseInsensitive, leaf);
			gotc = true;
		}
	}

	if (!gotc) {
		TrieNode * child = new TrieNode(c);
		m_children.append(child);
		child->addString(next, caseInsensitive, leaf);
	}
}

bool TrieNode::matchesChar(QChar c) {
	return (c == m_char);
}

bool TrieNode::matches(QString & string, TrieLeaf * & leaf) 
{
	if (string.isEmpty()) {
		if (m_isLeaf) {
			leaf = m_leafData;
			return true;
		}

		return false;
	}

	QChar in(string.at(0));
	foreach (TrieNode * node, m_children) {
		if (node->matchesChar(in)) {
			return node->matches(string.remove(0, 1), leaf);
		}
	}

	return false;
}
