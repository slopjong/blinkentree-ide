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

$Revision: 5690 $:
$Author: cohen@irascible.com $:
$Date: 2011-12-22 06:15:40 +0100 (Thu, 22 Dec 2011) $

********************************************************************/

#include "translatorlistmodel.h"
#include "../debugdialog.h"

static QVariant emptyVariant;
QHash<QString, QString> TranslatorListModel::m_languages;
QList<QLocale *> TranslatorListModel::m_localeList;

TranslatorListModel::TranslatorListModel(QFileInfoList & fileInfoList, QObject* parent)
: QAbstractListModel(parent) 
{

	if (m_languages.count() == 0) {
        m_languages.insert("english", tr("English - %1").arg("English"));
        m_languages.insert("french", tr("French - %1").arg("Fran�ais"));
        m_languages.insert("german", tr("German - %1").arg("Deutsch"));
        m_languages.insert("spanish", tr("Spanish - %1").arg("Espa�ol"));

		ushort t1[] = { 0x65e5, 0x672c, 0x8a9e, 0 };
		m_languages.insert("japanese", tr("Japanese - %1").arg(QString::fromUtf16(t1)));

        m_languages.insert("portuguese_portugal", tr("Portuguese (European)- %1").arg("Portugu�s (Europeu)"));
        m_languages.insert("portuguese_brazil", tr("Portuguese (Brazilian) - %1").arg("Portugu�s (do Brasil)"));
        m_languages.insert("hungarian", tr("Hungarian - %1").arg("Magyar"));
        m_languages.insert("estonian", tr("Estonian - %1").arg("Eesti keel"));
        m_languages.insert("dutch", tr("Dutch - %1").arg("Nederlands"));

		ushort t2[] = { 0x0420, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, 0 };
        m_languages.insert("russian", tr("Russian - %1").arg(QString::fromUtf16(t2)));

        m_languages.insert("italian", tr("Italian - %1").arg("Italiano"));

 		ushort t3[] = { 0x05e2, 0x05d1, 0x05e8, 0x05d9, 0x05ea, 0 };
        m_languages.insert("hebrew", tr("Hebrew - %1").arg(QString::fromUtf16(t3)));

		ushort t4[] = { 0x0639, 0x0631, 0x0628, 0x064a, 0 };
        m_languages.insert("arabic", tr("Arabic - %1").arg(QString::fromUtf16(t4)));

		ushort t5[] = { 0x0939, 0x093f, 0x0928, 0x094d, 0x0926, 0x0940, 0x0020, 0x0028, 0x092d, 0x093e, 0x0930, 0x0924, 0x0029, 0 };
        m_languages.insert("hindi", tr("Hindi - %1").arg(QString::fromUtf16(t5)));

		ushort t6[] = { 0x4e2d, 0x6587, 0x0020, 0x0028, 0x7b80, 0x4f53, 0x0029, 0 };
        m_languages.insert("chinese_china", tr("Chinese (Simplified) - %1").arg(QString::fromUtf16(t6)));

        ushort t7[] = { 0x6b63, 0x9ad4, 0x4e2d, 0x6587, 0x0020, 0x0028, 0x7e41, 0x9ad4, 0x0029, 0 };
        m_languages.insert("chinese_taiwan", tr("Chinese (Traditional) - %1").arg(QString::fromUtf16(t7)));

        m_languages.insert("polish", tr("Polish - %1").arg("Polski"));

		ushort t8[] = { 0x010C, 0x65, 0x0161, 0x74, 0x69, 0x6E, 0x61, 0 };
        m_languages.insert("czech", tr("Czech - %1").arg(QString::fromUtf16(t8)));

		m_languages.insert("turkish", tr("Turkish - %1").arg("T�rk�e"));
		m_languages.insert("swedish", tr("Swedish - %1").arg("Svenska"));

		ushort t9[] = { 0x52,  0x6F, 0x6D, 0xE2, 0x6E, 0x0103, 0 };
		m_languages.insert("romanian", tr("Romanian - %1").arg(QString::fromUtf16(t9)));

		ushort t10[] = { 0x0E44, 0x0E17, 0x0E22, 0 };
		m_languages.insert("thai", tr("Thai - %1").arg(QString::fromUtf16(t10)));

		ushort t11[] = { 0x0395, 0x03BB, 0x03BB, 0x03B7, 0x03BD, 0x03B9, 0x03BA, 0x03AC, 0};
		m_languages.insert("greek", tr("Greek - %1").arg(QString::fromUtf16(t11)));

		ushort t12[] = { 0x0411, 0x044A, 0x043B, 0x0433, 0x0430, 0x0440, 0x0441, 0x043A, 0x0438, 0 };
		m_languages.insert("bulgarian", tr("Bulgarian - %1").arg(QString::fromUtf16(t12)));


		m_languages.insert("galician", tr("Galician - %1").arg("Galego"));

		ushort t13[] = { 0xD55C,  0xAD6D,  0xC5B4, 0};
		m_languages.insert("korean", tr("Korean - %1").arg(QString::fromUtf16(t13)));

        // More languages written in their own language can be found
        // at http://www.mozilla.com/en-US/firefox/all.html 

		// recipe for translating from mozilla strings into source code via windows:
		//		1. copy the string from the mozilla page into wordpad and save it as a unicode text file
		//		2. open that unicode text file as a binary file (e.g. in msvc)
		//		3. ignore the first two bytes (these are a signal that says "I'm unicode")
		//		4. initialize an array of ushort using the rest of the byte pairs
		//		5. don't forget to reverse the order of the bytes in each pair.

		// to add a new language to fritzing, find its two-letter language code: http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
		// then in the translations.pri file, add a new line translations/fritzing_XX.ts where you substitute the two letters for "XX".
		// If the language has multiple dialects, then you will need to add _YY to distinguish
	}

	if (m_localeList.count() == 0) {
		foreach (QFileInfo fileinfo, fileInfoList) {
			QString name = fileinfo.baseName();
			name.replace("fritzing_", "");
			QStringList names = name.split("_");
			if (names.count() > 1) {
				name = names[0] + "_" + names[1].toUpper();
			}
			QLocale * locale = new QLocale(name);
			m_localeList.append(locale);
		}
	}
}


TranslatorListModel::~TranslatorListModel() {
}

void TranslatorListModel::cleanup() {
	foreach (QLocale * locale, m_localeList) {
		delete locale;
	}
	m_localeList.clear();
}

QVariant TranslatorListModel::data ( const QModelIndex & index, int role) const 
{
	if (role == Qt::DisplayRole && index.row() >= 0 && index.row() < m_localeList.count()) {
		QString languageString = QLocale::languageToString(m_localeList.at(index.row())->language());
		QString countryString = QLocale::countryToString(m_localeList.at(index.row())->country());

        //DebugDialog::debug(QString("language %1 %2").arg(languageString).arg(countryString));

		// QLocale::languageToString() only returns an English string, 
		// so put it through a language-dependent hash table.
		QString trLanguageString = m_languages.value(languageString.toLower(), "");
		if (trLanguageString.isEmpty()) {
			trLanguageString = m_languages.value(languageString.toLower() + "_" + countryString.toLower(), "");
		}

		if (trLanguageString.isEmpty()) {
			foreach (QString key, m_languages.keys()) {
				if (key.startsWith(languageString.toLower())) {
					return m_languages.value(key);
				}
			}

			return languageString;
		}

		return trLanguageString;
	}

	return emptyVariant;
	
}

int TranslatorListModel::rowCount ( const QModelIndex & parent) const 
{
	Q_UNUSED(parent);

	return m_localeList.count();
}

const QLocale * TranslatorListModel::locale( int index) 
{
	if (index < 0 || index >= m_localeList.count()) return NULL;

	return m_localeList.at(index);	
}

int TranslatorListModel::findIndex(const QString & language) {
	int ix = 0;
	foreach (QLocale * locale, m_localeList) {
		//DebugDialog::debug(QString("find index %1 %2").arg(language).arg(locale->name()));
		if (language.compare(locale->name()) == 0) return ix;
		ix++;
	}

	ix = 0;
	foreach (QLocale * locale, m_localeList) {
		if (locale->name().startsWith("en")) return ix;
		ix++;
	}

	return 0;

}

