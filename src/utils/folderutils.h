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

$Revision: 5444 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-31 06:45:39 +0200 (Wed, 31 Aug 2011) $

********************************************************************/

#ifndef FOLDERUTILS_H
#define FOLDERUTILS_H

#include <QString>
#include <QDir>
#include <QStringList>
#include <QFileDialog>

#include "misc.h"

class FolderUtils
{

public:
	static QString getUserPartsFolder();
	static QDir *getApplicationSubFolder(QString);
	static QString getApplicationSubFolderPath(QString);
	static QString getUserDataStorePath(QString folder = ___emptyString___);
	static const QStringList & getUserDataStoreFolders();
	static bool createFolderAnCdIntoIt(QDir &dir, QString newFolder);
	static bool setApplicationPath(const QString & path);
	static const QString getLibraryPath();
	static QString getOpenFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
	static QStringList getOpenFileNames( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
	static QString getSaveFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
	static void setOpenSaveFolder(const QString& path);
	static void setOpenSaveFolderAux(const QString& path); 
	static const QString openSaveFolder();
	static bool isEmptyFileName(const QString &filename, const QString &unsavedFilename);
	static void rmdir(const QString &dirPath);
	static void rmdir(QDir & dir);
	static bool createZipAndSaveTo(const QDir &dirToCompress, const QString &filename);
	static bool unzipTo(const QString &filepath, const QString &dirToDecompress);
	static void replicateDir(QDir srcDir, QDir targDir);
	static QString getRandText();
	static void initLockedFiles(const QString & prefix, QString & folder, QHash<QString, class QtLockedFile *> & lockedFiles);
	static void releaseLockedFiles(const QString & folder, QHash<QString, class QtLockedFile *> & lockedFiles);
	static void checkLockedFiles(const QString & prefix, QFileInfoList & backupList, QStringList & filters, QHash<QString, class QtLockedFile *> & lockedFiles, bool recurse);
	static void cleanup();
	static void collectFiles(const QDir & parent, QStringList & filters, QStringList & files);

protected:
	FolderUtils();
	~FolderUtils();

	const QStringList & userDataStoreFolders();
	bool setApplicationPath2(const QString & path);
	const QString applicationDirPath();
	const QString libraryPath();

	static bool checkLockedFilesAux(const QDir & parent, QStringList & filters);

public:
	static const QString LockFileName;


protected:
	static FolderUtils* singleton;
	static QString m_openSaveFolder;

protected:
	QStringList m_folders;
	QString m_appPath;
};

#endif
