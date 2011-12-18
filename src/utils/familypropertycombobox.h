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

$Revision: 4195 $:
$Author: cohen@irascible.com $:
$Date: 2010-05-21 20:45:42 +0200 (Fri, 21 May 2010) $

********************************************************************/

#ifndef FAMILYPROPERTYCOMBOBOX_H
#define FAMILYPROPERTYCOMBOBOX_H

#include <QComboBox>
#include "../debugdialog.h"

class FamilyPropertyComboBox : public QComboBox 
{
	Q_OBJECT

public:
	FamilyPropertyComboBox(const QString & family, const QString & prop, QWidget * parent = 0) : QComboBox(parent) {
		m_family = family;
		m_prop = prop;
	}
	~FamilyPropertyComboBox() {
	}

        void hidePopup() {
            //DebugDialog::debug(QString("hide popup %1").arg((long) this, 0, 16));
            QComboBox::hidePopup();
        }

	const QString & prop() { return m_prop; }
	const QString & family() { return m_family; }

protected:
	QString m_family;
	QString m_prop;

};

#endif
