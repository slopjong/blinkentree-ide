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

#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include <QHttp>
#include <QXmlStreamReader>
#include <QDateTime>

#include "version.h"

// much code borrowed from Qt's rsslisting example

struct AvailableRelease {
	bool interim;
	QString versionString;
	QString link;
	QString summary;
	QDateTime dateTime;
};


class VersionChecker : public QObject {
	Q_OBJECT

public:
	VersionChecker();
	~VersionChecker();

	void setUrl(const QString & url);
	const QList<AvailableRelease *> & availableReleases();
	void stop();
	void ignore(const QString & version, bool interim);

signals:
	void httpError(QHttp::Error statusCode);
	void xmlError(QXmlStreamReader::Error errorCode);
	void releasesAvailable(); 

public slots:
    void fetch();
    void finished(int id, bool error);
    void readData(const QHttpResponseHeader &);

protected:
	void parseXml();
	void parseEntry();

protected:
	QString m_urlString;
    QXmlStreamReader m_xml;
    QHttp m_http;
    int m_connectionId;
	QString m_version;
	int m_depth;
	bool m_inEntry;
	bool m_inTitle;
	bool m_inUpdated;
	bool m_inSummary;
	QList<AvailableRelease *> m_availableReleases;
	QString m_currentLinkHref;
	QString m_currentCategoryTerm;
	QString m_currentTitle;
	QString m_currentUpdated;
	QString m_currentSummary;
	VersionThing m_ignoreMainVersion;
	VersionThing m_ignoreInterimVersion;
	int m_statusCode;
};

#endif
