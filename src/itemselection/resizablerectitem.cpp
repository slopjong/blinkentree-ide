/*******************************************************************

Part of the Fritzing project - http://fritzing.org
Copyright (c) 2007-2009 Fachhochschule Potsdam - http://fh-potsdam.de

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

$Revision: 3849 $:
$Author: cohen@irascible.com $:
$Date: 2009-12-10 12:50:56 +0100 (Thu, 10 Dec 2009) $

********************************************************************/

#include "resizablerectitem.h"

ResizableRectItem::ResizableRectItem()
{
	m_resizable = false; // not resizable by default
	m_handlers = NULL;
}

void ResizableRectItem::setResizable(bool resizable) {
	m_resizable = resizable;
}

bool ResizableRectItem::isResizable() {
	return m_resizable;
}
