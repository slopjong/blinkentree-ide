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

$Revision: 5535 $:
$Author: cohen@irascible.com $:
$Date: 2011-09-30 17:31:38 +0200 (Fri, 30 Sep 2011) $

********************************************************************/

#include "folderutils.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include <QSettings>
#include <QTextStream>
#include <QUuid>
#include <QCryptographicHash>

#include "../debugdialog.h"
#include "../lib/quazip/quazip.h"
#include "../lib/quazip/quazipfile.h"
#include "../lib/qtlockedfile/qtlockedfile.h"
#include "../lib/qtsysteminfo/QtSystemInfo.h"


FolderUtils* FolderUtils::singleton = NULL;
QString FolderUtils::m_openSaveFolder = "";
const QString FolderUtils::LockFileName = "___lockfile___.txt";

FolderUtils::FolderUtils() {
	m_openSaveFolder = ___emptyString___;
	m_folders << "/bins" 
		<< "/partfactory"
		<< "/parts/user" << "/parts/contrib"
		<< "/parts/svg/user/icon" << "/parts/svg/user/breadboard" << "/parts/svg/user/schematic" << "/parts/svg/user/pcb"
		<< "/parts/svg/contrib/icon" << "/parts/svg/contrib/breadboard" << "/parts/svg/contrib/schematic" << "/parts/svg/contrib/pcb"
		<< "/backup";
}

FolderUtils::~FolderUtils() {
}

// finds the user parts folder based on local desktop (OS) defaults
QString FolderUtils::getUserPartsFolder() {
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/UserCreatedParts";
}

// finds a subfolder of the application directory searching backward up the tree
QDir * FolderUtils::getApplicationSubFolder(QString search) {
	if (singleton == NULL) {
		singleton = new FolderUtils();
	}

	QString path = singleton->applicationDirPath();
    path += "/" + search;
	//DebugDialog::debug(QString("path %1").arg(path) );
    QDir* dir= new QDir(path);
    while (!dir->exists()) {
    	// if we're running from the debug or release folder, go up one to find things
    	dir->cdUp();
		dir->cdUp();
    	if (dir->isRoot()) return NULL;   // didn't find the search folder

		dir->setPath(dir->absolutePath() + "/" + search);
   	}

   	return dir;
}

QString FolderUtils::getApplicationSubFolderPath(QString search) {
	if (singleton == NULL) {
		singleton = new FolderUtils();
	}

	QDir * dir = getApplicationSubFolder(search);
	if (dir == NULL) return "";

	QString result = dir->path();
	delete dir;
	return result;
}

QString FolderUtils::getUserDataStorePath(QString folder) {
	QString settingsFile = QSettings(QSettings::IniFormat,QSettings::UserScope,"Fritzing","Fritzing").fileName();
	return QFileInfo(settingsFile).dir()
        .absolutePath()+(folder!=___emptyString___?QString("/")+folder:QString(""));
}

const QStringList & FolderUtils::getUserDataStoreFolders() {
	if (singleton == NULL) {
		singleton = new FolderUtils();
	}

	return singleton->userDataStoreFolders();
}

bool FolderUtils::createFolderAnCdIntoIt(QDir &dir, QString newFolder) {
	if(!dir.mkdir(newFolder)) return false;
	if(!dir.cd(newFolder)) return false;

	return true;
}

bool FolderUtils::setApplicationPath(const QString & path) 
{
	if (singleton == NULL) {
		singleton = new FolderUtils();
	}

	return singleton->setApplicationPath2(path);
}

void FolderUtils::cleanup() {
	if (singleton) {
		delete singleton;
		singleton = NULL;
	}
}

const QString FolderUtils::getLibraryPath() 
{
	if (singleton == NULL) {
		singleton = new FolderUtils();
	}

	return singleton->libraryPath();
}


const QString FolderUtils::libraryPath() 
{
#ifdef Q_WS_MAC
	// mac plugins are always in the bundle
	return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../lib");
#endif

	return QDir::cleanPath(applicationDirPath() + "/lib");		
}

const QString FolderUtils::applicationDirPath() {
	if (m_appPath.isEmpty()) {
#ifdef Q_WS_WIN
		return QCoreApplication::applicationDirPath();
#else
		// look in standard Fritzing location (applicationDirPath) then in standard linux locations
		QStringList candidates;
		candidates.append(QCoreApplication::applicationDirPath());
#ifdef PKGDATADIR
		candidates.append(QLatin1String(PKGDATADIR));
#else
		candidates.append("/usr/share/fritzing");
		candidates.append("/usr/local/share/fritzing");
#endif
		candidates.append(QDir::homePath() + "/.local/share/fritzing");
		foreach (QString candidate, candidates) {
                        //DebugDialog::debug(QString("candidate:%1").arg(candidate));
			QDir dir(candidate);
                        if (!dir.exists("parts")) continue;

                        if (dir.exists("bins")) {
				m_appPath = candidate;
				return m_appPath;
			}

		}

		m_appPath = QCoreApplication::applicationDirPath();
                DebugDialog::debug("data folders not found");

#endif
	}

	return m_appPath;
}

bool FolderUtils::setApplicationPath2(const QString & path) 
{
	QDir dir(path);
	if (!dir.exists()) return false;

	m_appPath = path;
	return true;
}

const QStringList & FolderUtils::userDataStoreFolders() {
	return m_folders;
}

void FolderUtils::setOpenSaveFolder(const QString& path) {
	setOpenSaveFolderAux(path);
	QSettings settings;
	settings.setValue("openSaveFolder", m_openSaveFolder);
}

void FolderUtils::setOpenSaveFolderAux(const QString& path) 
{
	QFileInfo fileInfo(path);
	if(fileInfo.isDir()) {
		m_openSaveFolder = path;
	} else {
		m_openSaveFolder = fileInfo.path().remove(fileInfo.fileName());
	}
}


const QString FolderUtils::openSaveFolder() {
	if(m_openSaveFolder == ___emptyString___) {
		QSettings settings;
		QString tempFolder = settings.value("openSaveFolder").toString();
		if (!tempFolder.isEmpty()) {
			QFileInfo fileInfo(tempFolder);
			if (fileInfo.exists()) {
				m_openSaveFolder = tempFolder;
				return m_openSaveFolder;
			}
			else {
				settings.remove("openSaveFolder");
			}
		}

		DebugDialog::debug(QString("default save location: %1").arg(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)));
		return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
	} else {
		return m_openSaveFolder;
	}
}


QString FolderUtils::getOpenFileName( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options )
{
	QString result = QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
	if (!result.isNull()) {
		setOpenSaveFolder(result);
	}
	return result;
}

QStringList FolderUtils::getOpenFileNames( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options )
{
	QStringList result = QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
	if (result.count() > 0) {
		setOpenSaveFolder(result.at(0));
	}
	return result;
}

QString FolderUtils::getSaveFileName( QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options )
{
	//DebugDialog::debug(QString("getopenfilename %1 %2 %3 %4").arg(caption).arg(dir).arg(filter).arg(*selectedFilter));
	QString result = QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
	if (!result.isNull()) {
		setOpenSaveFolder(result);
	}
	return result;
}

bool FolderUtils::isEmptyFileName(const QString &fileName, const QString &untitledFileName) {
	return (fileName.isEmpty() || fileName.isNull() || fileName.startsWith(untitledFileName));
}

void FolderUtils::replicateDir(QDir srcDir, QDir targDir) {
	// copy all files from srcDir source to tagDir
	QStringList files = srcDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	for(int i=0; i < files.size(); i++) {
		QFile tempFile(srcDir.path() + "/" +files.at(i));
		DebugDialog::debug(QObject::tr("Copying file %1").arg(tempFile.fileName()));
		QFileInfo fi(files.at(i));
		QString newFilePath = targDir.path() + "/" + fi.fileName();
		if(QFileInfo(tempFile.fileName()).isDir()) {
			QDir newTargDir = QDir(newFilePath);
			newTargDir.mkpath(newTargDir.absolutePath());
			newTargDir.cd(files.at(i));
			replicateDir(QDir(tempFile.fileName()),newTargDir);
		} else {
			if(!tempFile.copy(newFilePath)) {
				DebugDialog::debug(QObject::tr("File %1 already exists: it won't be overwritten").arg(newFilePath));
			}
		}
	}
}

QString FolderUtils::getRandText() {
	QString rand = QUuid::createUuid().toString();
	QString randext = QCryptographicHash::hash(rand.toAscii(),QCryptographicHash::Md4).toHex();
	return randext;
}

/*QString FolderUtils::getBase64RandText() {
	QString rand = QUuid::createUuid().toString();
	QString randext = QCryptographicHash::hash(rand.toAscii(),QCryptographicHash::Md4).toHex();
	return randext;
}*/

// NOTE: This function cannot remove directories that have non-empty name filters set on it.
void FolderUtils::rmdir(const QString &dirPath) {
	QDir dir = QDir(dirPath);
	rmdir(dir);
}

void FolderUtils::rmdir(QDir & dir) {
	//DebugDialog::debug(QString("removing folder: %1").arg(dir.path()));

	QStringList files = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	for(int i=0; i < files.size(); i++) {
		QFile tempFile(dir.path() + "/" +files.at(i));
		//DebugDialog::debug(QString("removing from original folder: %1").arg(tempFile.fileName()));
		if(QFileInfo(tempFile.fileName()).isDir()) {
			QDir dir = QDir(tempFile.fileName());
			rmdir(dir);
		} else {
			tempFile.remove(tempFile.fileName());
		}
	}
	dir.rmdir(dir.path());
}

bool FolderUtils::createZipAndSaveTo(const QDir &dirToCompress, const QString &filepath) {
	DebugDialog::debug("zipping "+dirToCompress.path()+" into "+filepath);

	QString tempZipFile = QDir::temp().path()+"/"+getRandText()+".zip";
	DebugDialog::debug("temp file: "+tempZipFile);
	QuaZip zip(tempZipFile);
	if(!zip.open(QuaZip::mdCreate)) {
		qWarning("zip.open(): %d", zip.getZipError());
		return false;
	}

	QFileInfoList files=dirToCompress.entryInfoList();
	QFile inFile;
	QuaZipFile outFile(&zip);
	char c;

	QString currFolderBU = QDir::currentPath();
	QDir::setCurrent(dirToCompress.path());
	foreach(QFileInfo file, files) {
		if(!file.isFile()||file.fileName()==filepath) continue;

		inFile.setFileName(file.fileName());

		if(!inFile.open(QIODevice::ReadOnly)) {
			qWarning("inFile.open(): %s", inFile.errorString().toLocal8Bit().constData());
			return false;
		}
		if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(inFile.fileName(), inFile.fileName()))) {
			qWarning("outFile.open(): %d", outFile.getZipError());
			return false;
		}

		while(inFile.getChar(&c)&&outFile.putChar(c)){}

		if(outFile.getZipError()!=UNZ_OK) {
			qWarning("outFile.putChar(): %d", outFile.getZipError());
			return false;
		}
		outFile.close();
		if(outFile.getZipError()!=UNZ_OK) {
			qWarning("outFile.close(): %d", outFile.getZipError());
			return false;
		}
		inFile.close();
	}
	zip.close();
	QDir::setCurrent(currFolderBU);

	if(QFileInfo(filepath).exists()) {
		// if we're here the usr has already accepted to overwrite
		QFile::remove(filepath);
	}
	QFile file(tempZipFile);
	file.copy(filepath);
	file.remove();

	if(zip.getZipError()!=0) {
		qWarning("zip.close(): %d", zip.getZipError());
		return false;
	}
	return true;
}


bool FolderUtils::unzipTo(const QString &filepath, const QString &dirToDecompress) {
	QuaZip zip(filepath);
	if(!zip.open(QuaZip::mdUnzip)) {
		qWarning("zip.open(): %d", zip.getZipError());
		return false;
	}
	zip.setFileNameCodec("IBM866");
	DebugDialog::debug(QString("unzipping %1 entries from %2").arg(zip.getEntriesCount()).arg(filepath));
	QuaZipFileInfo info;
	QuaZipFile file(&zip);
	QFile out;
	QString name;
	char c;
	for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
		if(!zip.getCurrentFileInfo(&info)) {
			qWarning("getCurrentFileInfo(): %d\n", zip.getZipError());
			return false;
		}

		if(!file.open(QIODevice::ReadOnly)) {
			qWarning("file.open(): %d", file.getZipError());
			return false;
		}
		name=file.getActualFileName();
		if(file.getZipError()!=UNZ_OK) {
			qWarning("file.getFileName(): %d", file.getZipError());
			return false;
		}

		out.setFileName(dirToDecompress+"/"+name);
		// this will fail if "name" contains subdirectories, but we don't mind that
		if(!out.open(QIODevice::WriteOnly)) {
			qWarning("out.open(): %s", out.errorString().toLocal8Bit().constData());
			return false;
		}

		// Slow like hell (on GNU/Linux at least), but it is not my fault.
		// Not ZIP/UNZIP package's fault either.
		// The slowest thing here is out.putChar(c).
		// TODO: now that out.putChar has been replaced with a buffered write, is it still slow under Linux?

#define BUFFERSIZE 1024
		char buffer[BUFFERSIZE];
		int ix = 0;
		while(file.getChar(&c)) {
			buffer[ix++] = c;
			if (ix == BUFFERSIZE) {
				out.write(buffer, ix);
				ix = 0;
			}
		}
		if (ix > 0) {
			out.write(buffer, ix);
		}

		out.close();
		if(file.getZipError()!=UNZ_OK) {
			qWarning("file.getFileName(): %d", file.getZipError());
			return false;
		}
		if(!file.atEnd()) {
			qWarning("read all but not EOF");
			return false;
		}
		file.close();
		if(file.getZipError()!=UNZ_OK) {
			qWarning("file.close(): %d", file.getZipError());
			return false;
		}
	}
	zip.close();
	if(zip.getZipError()!=UNZ_OK) {
		qWarning("zip.close(): %d", zip.getZipError());
		return false;
	}
	return true;
}

void FolderUtils::initLockedFiles(const QString & prefix, QString & folder, QHash<QString, class QtLockedFile *> & lockedFiles) {
	// first create our own unique folder and lock it
	QDir backupDir(FolderUtils::getUserDataStorePath(prefix));
	QString lockedSubfolder = FolderUtils::getRandText();
	backupDir.mkdir(lockedSubfolder);
	folder = backupDir.absoluteFilePath(lockedSubfolder);
	QtLockedFile * lockFile = new QtLockedFile(folder + "/" + LockFileName);
	lockFile->open(QFile::WriteOnly);
	lockFile->lock(QtLockedFile::WriteLock, true);
	lockedFiles.insert(lockedSubfolder, lockFile);
}


void FolderUtils::releaseLockedFiles(const QString & folder, QHash<QString, class QtLockedFile *> & lockedFiles) 
{
	// remove backup files; this is a clean exit
	QDir backupDir(folder);
	backupDir.cdUp();
	foreach (QString sub, lockedFiles.keys()) {
        QtLockedFile * lockedFile = lockedFiles.value(sub);
		lockedFile->unlock();
		lockedFile->close();
		delete lockedFile;
        FolderUtils::rmdir(backupDir.absoluteFilePath(sub));
	}
	lockedFiles.clear();
}

bool FolderUtils::checkLockedFilesAux(const QDir & parent, QStringList & filters)
{
	QFileInfoList dirList = parent.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);
	foreach (QFileInfo dirInfo, dirList) {
		QDir dir(dirInfo.filePath());
		//DebugDialog::debug(QString("looking in backup dir %1").arg(dir.absolutePath()));
		QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		if (!fileInfoList.isEmpty()) return true;

		if (checkLockedFilesAux(dir, filters)) return true;
	}

	return false;
}

void FolderUtils::collectFiles(const QDir & parent, QStringList & filters, QStringList & files)
{
	QFileInfoList fileInfoList = parent.entryInfoList(filters, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	foreach (QFileInfo fileInfo, fileInfoList) {
		files.append(fileInfo.absoluteFilePath());
	}

	QFileInfoList dirList = parent.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);
	foreach (QFileInfo dirInfo, dirList) {
		QDir dir(dirInfo.filePath());
		//DebugDialog::debug(QString("looking in backup dir %1").arg(dir.absolutePath()));

		collectFiles(dir, filters, files);
	}
}

void FolderUtils::checkLockedFiles(const QString & prefix, QFileInfoList & backupList, QStringList & filters, QHash<QString, class QtLockedFile *> & lockedFiles, bool recurse)
{
	QDir backupDir(FolderUtils::getUserDataStorePath(prefix));
	QFileInfoList dirList = backupDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);
	foreach (QFileInfo dirInfo, dirList) {
		QDir dir(dirInfo.filePath());
		//DebugDialog::debug(QString("looking in backup dir %1").arg(dir.absolutePath()));
		QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		bool gotRecurse = false;
		if (recurse && fileInfoList.isEmpty()) {
			gotRecurse = checkLockedFilesAux(dir, filters);
		}

		if (fileInfoList.isEmpty() && !gotRecurse) {
			// could mean this backup folder is just being created by another process
			// could also mean it's leftover crap.
			// check the date and only delete if it's old

			// don't lock the file, in case this is a race condition:
			// if another instance starting up is about to lock the file, 
			// the date will be recent and we won't delete 
			
			QDateTime lastModified = dirInfo.lastModified();
			if (lastModified < QDateTime::currentDateTime().addSecs(-5 * 60)) {
				// At startup, creating the backup folder, the lock file, and locking it are successive operations
				// so even giving it 5 minutes here is probably way more than we need
				FolderUtils::rmdir(dirInfo.filePath());
			}
			
			continue;
		}
		
		QtLockedFile * otherLockFile = new QtLockedFile(dir.absoluteFilePath(FolderUtils::LockFileName));
		bool isOpen = otherLockFile->open(QFile::WriteOnly);
		if (!isOpen) {
			// somebody else owns the file
			delete otherLockFile;
			continue;
		}

		if (!otherLockFile->lock(QtLockedFile::WriteLock, false)) {
			// somebody else owns the file
			otherLockFile->close();
			delete otherLockFile;
			continue;
		}

		// we own the file
		lockedFiles.insert(dirInfo.fileName(), otherLockFile);		
		foreach (QFileInfo fileInfo, fileInfoList) {
			backupList << fileInfo;
		}
	}
}
