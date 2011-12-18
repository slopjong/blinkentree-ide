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

$Revision: 5479 $:
$Author: cohen@irascible.com $:
$Date: 2011-09-13 13:51:05 +0200 (Tue, 13 Sep 2011) $

********************************************************************/

#include "svgflattener.h"
#include "svgpathlexer.h"
#include "../utils/graphicsutils.h"
#include "../utils/textutils.h"
#include "../debugdialog.h"
#include <QMatrix>
#include <QRegExp>   
#include <qmath.h>

SvgFlattener::SvgFlattener() : SvgFileSplitter()
{
}

void SvgFlattener::flattenChildren(QDomElement &element){

    // recurse the children
    QDomNodeList childList = element.childNodes();

    for(uint i = 0; i < childList.length(); i++){
        QDomElement child = childList.item(i).toElement();
        flattenChildren(child);
    }

    //do translate
    if(hasTranslate(element)){
		QList<double> params = TextUtils::getTransformFloats(element);
		if (params.size() == 2) {
            shiftChild(element, params.at(0), params.at(1), false);
			//DebugDialog::debug(QString("translating %1 %2").arg(params.at(0)).arg(params.at(1)));
		}
		else if (params.size() == 6) {
            shiftChild(element, params.at(4), params.at(5), false);
		}
		else if (params.size() == 1) {
            shiftChild(element, params.at(0), 0, false);
			//DebugDialog::debug(QString("translating %1").arg(params.at(0)));
		}
		else {
			DebugDialog::debug("weird transform found");
		}
    }
    else if(hasOtherTransform(element)) {
        QMatrix transform = TextUtils::transformStringToMatrix(element.attribute("transform"));

        //DebugDialog::debug(QString("rotating %1 %2 %3 %4 %5 %6").arg(params.at(0)).arg(params.at(1)).arg(params.at(2)).arg(params.at(3)).arg(params.at(4)).arg(params.at(5)));
        unRotateChild(element, transform);
    }

    // remove transform
    element.removeAttribute("transform");
}

void SvgFlattener::unRotateChild(QDomElement & element, QMatrix transform) {

	// TODO: missing ellipse element

    if(!element.hasChildNodes()) {

		double scale = qMin(qAbs(transform.m11()), qAbs(transform.m22()));
		if (scale != 1 && transform.m21() == 0 && transform.m12() == 0) {
			QString sw = element.attribute("stroke-width");
			if (!sw.isEmpty()) {
				bool ok;
				double strokeWidth = sw.toDouble(&ok);
				if (ok) {
					element.setAttribute("stroke-width", QString::number(strokeWidth * scale));
				}
			}
		}

		// I'm a leaf node.
		QString tag = element.nodeName().toLower();
		if(tag == "path"){
            QString data = element.attribute("d").trimmed();
            if (!data.isEmpty()) {
                const char * slot = SLOT(rotateCommandSlot(QChar, bool, QList<double> &, void *));
                PathUserData pathUserData;
                pathUserData.transform = transform;
                if (parsePath(data, slot, pathUserData, this, true)) {
                    element.setAttribute("d", pathUserData.string);
                }
            }
        }
		else if ((tag == "polygon") || (tag == "polyline")) {
			QString data = element.attribute("points");
			if (!data.isEmpty()) {
				const char * slot = SLOT(rotateCommandSlot(QChar, bool, QList<double> &, void *));
				PathUserData pathUserData;
				pathUserData.transform = transform;
				if (parsePath(data, slot, pathUserData, this, false)) {
					pathUserData.string.remove(0, 1);			// get rid of the "M"
					element.setAttribute("points", pathUserData.string);
				}
			}
		}
        else if(tag == "rect"){
			float x = element.attribute("x").toFloat();
			float y = element.attribute("y").toFloat();
			float width = element.attribute("width").toFloat();
			float height = element.attribute("height").toFloat();
			QRectF r(x, y, width, height);
			QPolygonF poly = transform.map(r);
			if (GraphicsUtils::isRect(poly)) {
				QRectF rect = GraphicsUtils::getRect(poly);
				element.setAttribute("x", rect.left());
				element.setAttribute("y", rect.top());
				element.setAttribute("width", rect.width());
				element.setAttribute("height", rect.height());
			}
			else {
				element.setTagName("polygon");
				QString pts;
				for (int i = 1; i < poly.count(); i++) {
					QPointF p = poly.at(i);
					pts += QString("%1,%2 ").arg(p.x()).arg(p.y());
				}
				pts.chop(1);
				element.setAttribute("points", pts);
			}
        }
        else if(tag == "circle"){
            float cx = element.attribute("cx").toFloat();
            float cy = element.attribute("cy").toFloat();
            QPointF point = transform.map(QPointF(cx,cy));
            element.setAttribute("cx", point.x());
            element.setAttribute("cy", point.y());
        }
        else if(tag == "line") {
            float x1 = element.attribute("x1").toFloat();
            float y1 = element.attribute("y1").toFloat();
            QPointF p1 = transform.map(QPointF(x1,y1));
            element.setAttribute("x1", p1.x());
            element.setAttribute("y1", p1.y());

            float x2 = element.attribute("x2").toFloat();
            float y2 = element.attribute("y2").toFloat();
            QPointF p2 = transform.map(QPointF(x2,y2));
            element.setAttribute("x2", p2.x());
            element.setAttribute("y2", p2.y());
        }
		else if (tag == "g") {
			// no op
		}
        else {
            DebugDialog::debug("Warning! Can't rotate element: " + tag);
		}
        return;
    }

    // recurse the children
    QDomNodeList childList = element.childNodes();

    for(uint i = 0; i < childList.length(); i++){
        QDomElement child = childList.item(i).toElement();
        unRotateChild(child, transform);
    }

}

bool SvgFlattener::hasTranslate(QDomElement & element)
{
	QString transform = element.attribute("transform");
	if (transform.isEmpty()) return false;
    if (transform.startsWith("translate")) return true;

    if (transform.startsWith("matrix")) {
		QMatrix matrix = TextUtils::transformStringToMatrix(transform);
		matrix.translate(-matrix.dx(), -matrix.dy());
		if (matrix.isIdentity()) return true;
    }

    return false;
}

bool SvgFlattener::hasOtherTransform(QDomElement & element)
{
	QString transform = element.attribute("transform");
	if (transform.isEmpty()) return false;

	// NOTE: doesn't handle multiple transform attributes...
	return (!transform.contains("translate"));
}

void SvgFlattener::rotateCommandSlot(QChar command, bool relative, QList<double> & args, void * userData) {

    Q_UNUSED(relative);			// just normalizing here, so relative is not used

    PathUserData * pathUserData = (PathUserData *) userData;

    pathUserData->string.append(command);
    double x;
    double y;
	QPointF point;

	for (int i = 0; i < args.count(); ) {
		switch(command.toAscii()) {
			case 'v':
			case 'V':
				DebugDialog::debug("'v' and 'V' are now removed by preprocessing; shouldn't be here");
				/*
				y = args[i];			
				x = 0; // what is x, really?
				DebugDialog::debug("Warning! Can't rotate path with V");
				*/
				i++;
				break;
			case 'h':
			case 'H':
				DebugDialog::debug("'h' and 'H' are now removed by preprocessing; shouldn't be here");
				/*
				x = args[i];
				y = 0; // what is y, really?
				DebugDialog::debug("Warning! Can't rotate path with H");
				*/
				i++;
				break;
			case SVGPathLexer::FakeClosePathChar:
				break;
			case 'a':
			case 'A':
				// TODO: test whether this is correct
				for (int j = 0; j < 5; j++) {
					pathUserData->string.append(QString::number(args[j]));
					pathUserData->string.append(',');
				}
				x = args[5];
				y = args[6];
				i += 7;
				point = pathUserData->transform.map(QPointF(x,y));
				pathUserData->string.append(QString::number(point.x()));
				pathUserData->string.append(',');
				pathUserData->string.append(QString::number(point.y()));
				if (i < args.count()) {
					pathUserData->string.append(',');
				}
				break;
			default:
				x = args[i];
				y = args[i+1];
				i += 2;
				point = pathUserData->transform.map(QPointF(x,y));
				pathUserData->string.append(QString::number(point.x()));
				pathUserData->string.append(',');
				pathUserData->string.append(QString::number(point.y()));
				if (i < args.count()) {
					pathUserData->string.append(',');
				}
		}
	}
}

void SvgFlattener::flipSMDSvg(const QString & filename, const QString & svg, QDomDocument & domDocument, const QString & elementID, const QString & altElementID, double printerScale) {
	QString errorStr;
	int errorLine;
	int errorColumn;
	bool result;
	if (filename.isEmpty()) {
		result = domDocument.setContent(svg, &errorStr, &errorLine, &errorColumn);
	}
	else {
		QFile file(filename);
		result = domDocument.setContent(&file, &errorStr, &errorLine, &errorColumn);
	}
	if (!result) {
		domDocument.clear();			// probably redundant
		return;
	}

    QDomElement root = domDocument.documentElement();

	QSvgRenderer renderer;
	bool loaded;
	if (filename.isEmpty()) {
		loaded = renderer.load(svg.toUtf8());
	}
	else {
		loaded = renderer.load(filename);
	}

	if (!loaded) return;

	QDomElement element = TextUtils::findElementWithAttribute(root, "id", elementID);
	if (!element.isNull()) {
		QDomElement altElement = TextUtils::findElementWithAttribute(root, "id", altElementID);
		flipSMDElement(domDocument, renderer, element, elementID, altElement, altElementID, printerScale);
	}

#ifndef QT_NO_DEBUG
	//QString temp = domDocument.toString();
	//Q_UNUSED(temp);
#endif
}

void SvgFlattener::flipSMDElement(QDomDocument & domDocument, QSvgRenderer & renderer, QDomElement & element, const QString & att, QDomElement altElement, const QString & altAtt, double printerScale) 
{
	Q_UNUSED(printerScale);
	Q_UNUSED(att);

	QMatrix m = renderer.matrixForElement(att) * TextUtils::elementToMatrix(element);
	QRectF bounds = renderer.boundsOnElement(att);
	m.translate(bounds.center().x(), bounds.center().y());
	QMatrix mMinus = m.inverted();
    QMatrix cm = mMinus * QMatrix().scale(1, -1) * m;
	QDomElement newElement = element.cloneNode(true).toElement();
	newElement.removeAttribute("id");
	QDomElement pElement = domDocument.createElement("g");
	pElement.appendChild(newElement);
	TextUtils::setSVGTransform(pElement, cm);
	pElement.setAttribute("id", altAtt);
	pElement.setAttribute("flipped", true);
	if (!altElement.isNull()) {
		pElement.appendChild(altElement);
		altElement.removeAttribute("id");
	}
	element.parentNode().appendChild(pElement);
}
