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

$Revision: 4583 $:
$Author: cohen@irascible.com $:
$Date: 2010-11-04 13:27:16 +0100 (Thu, 04 Nov 2010) $

********************************************************************/

#ifndef SVGPATHLEXER_H
#define SVGPATHLEXER_H

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QRegExp>
#include <QDomElement>
#include <QMatrix>

class SVGPathLexer
{
public:
    SVGPathLexer(const QString &source);
    ~SVGPathLexer();
    int lex();
	QChar currentCommand();
	double currentNumber();

public:
	static const char FakeClosePathChar = 'x';

protected:
    QChar next();
	QString clean(const QString & source);

protected:
    QString m_source;
    const QChar *m_chars;
    int m_size;
    int m_pos;
    QChar m_current;
	QChar m_currentCommand;
	double m_currentNumber;
};

#endif
