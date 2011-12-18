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

$Revision: 4363 $:
$Author: cohen@irascible.com $:
$Date: 2010-07-21 11:17:20 +0200 (Wed, 21 Jul 2010) $

********************************************************************/

#ifndef SKETCHMODEL_H
#define SKETCHMODEL_H

#include "modelpart.h"
#include "modelbase.h"

#include <QTextStream>
#include <QMultiHash>

class SketchModel : public ModelBase
{
Q_OBJECT
public:
	SketchModel(bool makeRoot);
	SketchModel(ModelPart * root);

	void removeModelPart(ModelPart *);
	ModelPart * findModelPart(const QString & moduleID, long id);

protected:
	ModelPart * findModelPartAux(ModelPart * modelPart, const QString & moduleID, long id);
};

#endif
