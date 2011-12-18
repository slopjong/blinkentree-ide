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
																		
$Revision: 5309 $:
$Author: cohen@irascible.com $:
$Date: 2011-07-30 21:17:22 +0200 (Sat, 30 Jul 2011) $
		
********************************************************************/

// much code borrowed from Qt's rsslisting example

#include "versionchecker.h"	
#include "version.h"
#include "../debugdialog.h"

#include <QUrl>
								
VersionChecker::VersionChecker() : QObject() 
{
    connect(&m_http, SIGNAL(readyRead(const QHttpResponseHeader &)), 
		this, SLOT(readData(const QHttpResponseHeader &)));
    connect(&m_http, SIGNAL(requestFinished(int, bool)), 
		this, SLOT(finished(int, bool)));

	m_depth = 0;
	m_inSummary = m_inUpdated = m_inTitle = m_inEntry = false;
	m_ignoreInterimVersion.ok = m_ignoreMainVersion.ok = false;
}

VersionChecker::~VersionChecker() {
	foreach (AvailableRelease * availableRelease, m_availableReleases) {
		delete availableRelease;
	}

	m_availableReleases.clear();
}

void VersionChecker::fetch()
{
	m_statusCode = 200;
    m_xml.clear();
    QUrl url(m_urlString);
    m_http.setHost(url.host());
    m_connectionId = m_http.get(url.path());
}


void VersionChecker::readData(const QHttpResponseHeader &resp)
{
	m_statusCode = resp.statusCode();
	if (resp.statusCode() != 200) {
        m_http.abort();
		return;
	}

	QString temp = m_http.readAll();
    m_xml.addData(temp);
    parseXml();
}


void VersionChecker::finished(int id, bool error)
{	
	if (id != m_connectionId) {
		return;
	}

	if (error) {
		if (m_statusCode != 200) {
			emit httpError(m_http.error());
			return;
		}
	}

	emit releasesAvailable();
}

void VersionChecker::parseXml()
{
    while (!m_xml.atEnd()) {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
			QString elementName = m_xml.name().toString();
			if (elementName.compare("entry") == 0) {
				m_inEntry = true;
				m_inSummary = m_inTitle = m_inUpdated = false;
				m_currentLinkHref = "";
				m_currentCategoryTerm = "";
				m_currentTitle = "";
				m_currentUpdated = "";
			}
			else if (m_inEntry && elementName.compare("title") == 0) {
				m_inTitle = true;
			}
			else if (m_inEntry && elementName.compare("link") == 0) {
				if (m_xml.attributes().value("rel").toString().compare("enclosure") == 0) {
					m_currentLinkHref = m_xml.attributes().value("href").toString();
				}
			}
			else if (m_inEntry && elementName.compare("category") == 0) {
				m_currentCategoryTerm = m_xml.attributes().value("term").toString();
			}
			else if (m_inEntry && elementName.compare("updated") == 0) {
				m_inUpdated = true;
			}
			else if (m_inEntry && elementName.compare("summary") == 0) {
				m_inSummary = true;
			}

			//DebugDialog::debug(QString("%1<%2>").arg(QString(m_depth * 4, ' ')).arg(elementName));
			m_depth++;
			//foreach (QXmlStreamAttribute attribute, m_xml.attributes()) {
				//DebugDialog::debug(QString("%1%2: %3").arg(QString((m_depth + 1) * 4, ' ')).arg(attribute.name().toString()).arg(attribute.value().toString()));
			//}
		} 
		else if (m_xml.isEndElement()) {
			QString elementName = m_xml.name().toString();
			if (m_inEntry && elementName.compare("entry") == 0) {
				m_inEntry = false;
				parseEntry();
			}
			else if (m_inTitle && elementName.compare("title") == 0) {
				m_inTitle = false;
			}
			else if (m_inUpdated && elementName.compare("updated") == 0) {
				m_inUpdated = false;
			}
			else if (m_inSummary && elementName.compare("summary") == 0) {
				m_inSummary = false;
			}
			m_depth--;
			//DebugDialog::debug(QString("%1</%2>").arg(QString(m_depth * 4, ' ')).arg(elementName));
        } 
		else if (m_xml.isCharacters() && !m_xml.isWhitespace()) {
			QString t = m_xml.text().toString();
			t.replace(QRegExp("[\\s]+"), " ");
			//DebugDialog::debug(QString("%1%2").arg(QString((m_depth + 1) * 4, ' ')).arg(t));
			if (m_inTitle) {
				m_currentTitle = m_xml.text().toString();
			}
			else if (m_inUpdated) {
				m_currentUpdated = m_xml.text().toString();
			}
			else if (m_inSummary) {
				m_currentSummary = m_xml.text().toString();
			}
        }
    }
    if (m_xml.error() && m_xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
		emit xmlError(m_xml.error());
        m_http.abort();
		return;
    }

}

void VersionChecker::setUrl(const QString & url)
{
	m_urlString = url;
}

void VersionChecker::parseEntry() {
	if (m_currentLinkHref.isEmpty()) return;
	if (m_currentCategoryTerm.isEmpty()) return;
	if (m_currentUpdated.isEmpty()) return;

#ifndef QT_NO_DEBUG
	// hack for testing
	if (m_currentCategoryTerm.compare("main") == 0) {
		if (m_currentTitle.compare("0.11b") == 0) {
			m_currentTitle = "0.1.11b";
		}
		else if (m_currentTitle.compare("0.1b") == 0) {
			m_currentTitle = "0.1.1b";
		}
	}
#endif

	VersionThing entryVersionThing;
	Version::toVersionThing(m_currentTitle, entryVersionThing);
	if (!entryVersionThing.ok) {
		// older versions that don't conform, bail
		m_http.abort();
		return;
	}

	if (!Version::candidateGreaterThanCurrent(entryVersionThing)) {
		m_http.abort();
		return;
	}

	bool interim = (m_currentCategoryTerm.compare("interim") == 0);
	if (interim) {
		if (m_ignoreInterimVersion.ok) {
			if (!Version::greaterThan(m_ignoreInterimVersion, entryVersionThing)) {
				return;						// candidate version is earlier or the same as interim version already in prefs
			}
		}
	}
	else {
		if (m_ignoreMainVersion.ok) {
			if (!Version::greaterThan(m_ignoreMainVersion, entryVersionThing)) {
				return;						// candidate version is earlier or the same as main version already in prefs
			}
		}
	}

	AvailableRelease * availableRelease = new AvailableRelease();
	availableRelease->versionString = m_currentTitle;
	availableRelease->link = m_currentLinkHref;
	availableRelease->interim = interim;
	availableRelease->summary = m_currentSummary;
	QStringList temp = m_currentUpdated.split('+');											// conversion is confused by the +timezone suffix from the site xml
	availableRelease->dateTime = QDateTime::fromString(temp[0], "yyyy-MM-ddThh:mm:ss");     
	m_availableReleases.append(availableRelease);
}

const QList<AvailableRelease *> & VersionChecker::availableReleases()
{
	return m_availableReleases;
}

void VersionChecker::stop() {
    disconnect(&m_http, SIGNAL(readyRead(const QHttpResponseHeader &)), 
		this, SLOT(readData(const QHttpResponseHeader &)));
    disconnect(&m_http, SIGNAL(requestFinished(int, bool)), 
		this, SLOT(finished(int, bool)));
	m_http.abort();

}

void VersionChecker::ignore(const QString & version, bool interim) {
	if (interim) {
		Version::toVersionThing(version, m_ignoreInterimVersion);
	}
	else {
		Version::toVersionThing(version, m_ignoreMainVersion);
	}
}
