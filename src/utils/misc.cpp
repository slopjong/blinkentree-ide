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

$Revision: 4225 $:
$Author: cohen@irascible.com $:
$Date: 2010-06-08 11:46:48 +0200 (Tue, 08 Jun 2010) $

********************************************************************/

// misc Fritzing utility functions
#include "misc.h"
#include "../debugdialog.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include <QSet>

static QStringList ___fritzingExtensions___;

const QStringList & fritzingExtensions() {
	if (___fritzingExtensions___.count() == 0) {
		___fritzingExtensions___
			<< FritzingSketchExtension << FritzingBinExtension
			<< FritzingPartExtension
			<< FritzingBundleExtension << FritzingBundledPartExtension
			<< FritzingBundledBinExtension;
	}

	return ___fritzingExtensions___;
}

bool isParent(QObject * candidateParent, QObject * candidateChild) {
	QObject * parent = candidateChild->parent();
	while (parent) {
		if (parent == candidateParent) return true;

		parent = parent->parent();
	}

	return false;
}

Qt::KeyboardModifier altOrMetaModifier() {
#ifdef LINUX_32
	return Qt::MetaModifier;
#endif
#ifdef LINUX_64
	return Qt::MetaModifier;
#endif
	return Qt::AltModifier;
}
