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


#ifndef HIGHLIGHTER_H_
#define HIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QPointer>
#include <QString>
#include <QChar>

class Highlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
	Highlighter(QTextEdit * parent);
	~Highlighter();

	void setSyntaxer(class Syntaxer *);
	class Syntaxer * syntaxer();

public:
	static void loadStyles(const QString & filename);

protected:
	void highlightBlock(const QString & text);
	bool isWordChar(QChar c);
	void highlightTerms(const QString & text);
	void highlightStrings(int startStringIndex, QString & text);

protected:
	QPointer<class Syntaxer> m_syntaxer;

	static QHash<QString, QTextCharFormat *> m_styleFormats;
};

#endif /* HIGHLIGHTER_H_ */
