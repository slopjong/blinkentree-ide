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

$Revision: 5375 $:
$Author: cohen@irascible.com $:
$Date: 2011-08-08 21:38:25 +0200 (Mon, 08 Aug 2011) $

********************************************************************/

#include "programwindow.h"
#include "highlighter.h"
#include "syntaxer.h"
#include "programtab.h"

#include "../debugdialog.h"
#include "../waitpushundostack.h"
#include "../utils/folderutils.h"

#include <QFileInfoList>
#include <QFileInfo>
#include <QRegExp>
#include <QtGui>
#include <QSettings>
#include <QFontMetrics>
#include <QTextStream>

// Included for getSerialPort() and a few others
#ifdef Q_WS_WIN
#include "windows.h"
#include <windows.h>
#include <setupapi.h>
#include <dbt.h>
#include <INITGUID.H>

#endif
#ifdef Q_WS_MAC
#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#endif


///////////////////////////////////////////////

PTabWidget::PTabWidget(QWidget * parent) : QTabWidget(parent) {
        m_lastTabIndex = -1;

        connect(tabBar(), SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

QTabBar * PTabWidget::tabBar() {
	return QTabWidget::tabBar();
}

void PTabWidget::tabChanged(int index) {
    // Hide the close button on the old tab
    if (m_lastTabIndex >= 0) {
        QAbstractButton *tabButton = qobject_cast<QAbstractButton *>(tabBar()->tabButton(m_lastTabIndex, QTabBar::LeftSide));
        if (!tabButton) {
			tabButton = qobject_cast<QAbstractButton *>(tabBar()->tabButton(m_lastTabIndex, QTabBar::RightSide));
        }

        if (tabButton) {
            tabButton->hide();
        }
    }

    m_lastTabIndex = index;

    // Show the close button on the new tab
    if (m_lastTabIndex >= 0) {
        QAbstractButton *tabButton = qobject_cast<QAbstractButton *>(tabBar()->tabButton(m_lastTabIndex, QTabBar::LeftSide));
        if (!tabButton) {
            tabButton = qobject_cast<QAbstractButton *>(tabBar()->tabButton(m_lastTabIndex, QTabBar::RightSide));
        }
        if (tabButton) {
            tabButton->show();
        }
    }
}

///////////////////////////////////////////////

static int UntitledIndex = 1;
QHash<QString, QString> ProgramWindow::m_languages;
QHash<QString, class Syntaxer *> ProgramWindow::m_syntaxers;
const QString ProgramWindow::LocateName = "locate";
QString ProgramWindow::NoSerialPortName;

static QHash<QString, QString> ProgrammerNames;

ProgramWindow::ProgramWindow(QWidget *parent)
	: FritzingWindow("", untitledFileCount(), "", parent)
{
    QFile styleSheet(":/resources/styles/programwindow.qss");

    this->setObjectName("programmingWindow");
    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open :/resources/styles/programwindow.qss");
    } else {
        QString ss = styleSheet.readAll();
#ifdef Q_WS_MAC
                int paneLoc = 4;
                int tabBarLoc = 0;
#else
                int paneLoc = -1;
                int tabBarLoc = 5;
#endif
                ss = ss.arg(paneLoc).arg(tabBarLoc);
        this->setStyleSheet(ss);
    }

    if (ProgrammerNames.count() == 0) {
		initProgrammerNames();
	}

	if (m_languages.count() == 0) {
		initLanguages();
	}

	if (NoSerialPortName.isEmpty()) {
		NoSerialPortName = tr("No ports found");
	}

	m_savingProgramTab = NULL;
	UntitledIndex--;						// incremented by FritzingWindow
	ProgramWindow::setTitle();				// set to something weird by FritzingWindow
}

ProgramWindow::~ProgramWindow()
{
}

void ProgramWindow::initLanguages() {
	QDir dir(FolderUtils::getApplicationSubFolderPath("translations"));
	dir.cd("syntax");
	QStringList nameFilters;
	nameFilters << "*.xml";
	QFileInfoList list = dir.entryInfoList(nameFilters, QDir::Files | QDir::NoSymLinks);
	foreach (QFileInfo fileInfo, list) {
		if (fileInfo.baseName().compare("styles") == 0) {
			Highlighter::loadStyles(fileInfo.absoluteFilePath());
		}
		else {
			QString name = Syntaxer::parseForName(fileInfo.absoluteFilePath());
			if (!name.isEmpty()) {		// Arduino is not really available      && !name.contains("arduino", Qt::CaseInsensitive)
				m_languages.insert(name, fileInfo.absoluteFilePath());
			}
		}
	}
}

void ProgramWindow::setup(const QList<LinkedFile *> & linkedFiles, const QString & alternativePath)
{

    resize(500,700);

    setAttribute(Qt::WA_DeleteOnClose, true);
    QFrame * mainFrame = new QFrame(this);

	QFrame * headerFrame = createHeader();
	QFrame * centerFrame = createCenter();

	layout()->setMargin(0);
	layout()->setSpacing(0);

	QGridLayout *layout = new QGridLayout(mainFrame);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(headerFrame,0,0);
	layout->addWidget(centerFrame,1,0);

	setCentralWidget(mainFrame);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	QSettings settings;
	if (!settings.value("programwindow/state").isNull()) {
		restoreState(settings.value("programwindow/state").toByteArray());
	}
	if (!settings.value("programwindow/geometry").isNull()) {
		restoreGeometry(settings.value("programwindow/geometry").toByteArray());
	}

	installEventFilter(this);

    // Setup new menu bar for the programming window
    QMenuBar *menubar = menuBar();

    QMenu *currentMenu = menubar->addMenu(tr("&File"));

    QAction *currentAction = new QAction(tr("New"), this);
    currentAction->setShortcut(tr("Ctrl+N"));
    currentAction->setStatusTip(tr("Create a new program"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(addTab()));
    currentMenu->addAction(currentAction);

    currentAction = new QAction(tr("&Open..."), this);
    currentAction->setShortcut(tr("Ctrl+O"));
    currentAction->setStatusTip(tr("Open a program"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(loadProgramFile()));
    currentMenu->addAction(currentAction);

    currentMenu->addSeparator();

    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(tr("Ctrl+S"));
    m_saveAction->setStatusTip(tr("Save the current program"));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(save()));
    currentMenu->addAction(m_saveAction);

    currentAction = new QAction(tr("Rename"), this);
    currentAction->setStatusTip(tr("Rename the current program"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(rename()));
    currentMenu->addAction(currentAction);

    currentAction = new QAction(tr("Duplicate tab"), this);
    currentAction->setStatusTip(tr("Copies the current program into a new tab"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(duplicateTab()));
    currentMenu->addAction(currentAction);

    currentMenu->addSeparator();

    currentAction = new QAction(tr("Remove tab"), this);
    currentAction->setShortcut(tr("Ctrl+W"));
    currentAction->setStatusTip(tr("Remove the current program from the sketch"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
    currentMenu->addAction(currentAction);

    currentMenu->addSeparator();

    m_printAction = new QAction(tr("&Print..."), this);
    m_printAction->setShortcut(tr("Ctrl+P"));
    m_printAction->setStatusTip(tr("Print the current program"));
    connect(m_printAction, SIGNAL(triggered()), this, SLOT(print()));
    currentMenu->addAction(m_printAction);

    currentMenu->addSeparator();

    currentAction = new QAction(tr("&Quit"), this);
    currentAction->setShortcut(tr("Ctrl+Q"));
    currentAction->setStatusTip(tr("Quit the application"));
    currentAction->setMenuRole(QAction::QuitRole);
    connect(currentAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows2()));
    currentMenu->addAction(currentAction);

    currentMenu = menubar->addMenu(tr("&Edit"));

    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcuts(QKeySequence::Undo);
    m_undoAction->setEnabled(false);
    connect(m_undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    currentMenu->addAction(m_undoAction);

    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcuts(QKeySequence::Redo);
    m_redoAction->setEnabled(false);
    connect(m_redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    currentMenu->addAction(m_redoAction);

    currentMenu->addSeparator();

    m_cutAction = new QAction(tr("&Cut"), this);
    m_cutAction->setShortcut(tr("Ctrl+X"));
    m_cutAction->setStatusTip(tr("Cut selection"));
    m_cutAction->setEnabled(false);
    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    currentMenu->addAction(m_cutAction);

    m_copyAction = new QAction(tr("&Copy"), this);
    m_copyAction->setShortcut(tr("Ctrl+C"));
    m_copyAction->setStatusTip(tr("Copy selection"));
    m_copyAction->setEnabled(false);
    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    currentMenu->addAction(m_copyAction);

    currentAction = new QAction(tr("&Paste"), this);
    currentAction->setShortcut(tr("Ctrl+V"));
    currentAction->setStatusTip(tr("Paste clipboard contents"));
    // TODO: Check clipboard status and disable appropriately here
    connect(currentAction, SIGNAL(triggered()), this, SLOT(paste()));
    currentMenu->addAction(currentAction);

    currentMenu->addSeparator();

    currentAction = new QAction(tr("&Select All"), this);
    currentAction->setShortcut(tr("Ctrl+A"));
    currentAction->setStatusTip(tr("Select all text"));
    connect(currentAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    currentMenu->addAction(currentAction);

    currentMenu = menuBar()->addMenu(tr("&Program"));

    QMenu *languageMenu = new QMenu(tr("Select language"), this);
    currentMenu->addMenu(languageMenu);

	QString currentLanguage = settings.value("programwindow/language", "").toString();
	QStringList languages = getAvailableLanguages();
    QActionGroup *languageActionGroup = new QActionGroup(this);
    foreach (QString language, languages) {
        currentAction = new QAction(language, this);
        currentAction->setCheckable(true);
        m_languageActions.insert(language, currentAction);
        languageActionGroup->addAction(currentAction);
        languageMenu->addAction(currentAction);
		if (!currentLanguage.isEmpty()) {
			if (language.compare(currentLanguage) == 0) {
				currentAction->setChecked(true);
			}
		}
    }

    connect(languageMenu, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)));

    m_serialPortMenu = new QMenu(tr("Select port"), this);
    currentMenu->addMenu(m_serialPortMenu);

    m_serialPortActionGroup = new QActionGroup(this);
	updateSerialPorts();

    connect(m_serialPortMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPort(QAction*)));
	connect(m_serialPortMenu, SIGNAL(aboutToShow()), this, SLOT(updateSerialPorts()), Qt::DirectConnection);

	m_programmerMenu = new QMenu(tr("Select programmer"), this);
    currentMenu->addMenu(m_programmerMenu);

	m_programmerActionGroup = new QActionGroup(this);
	QHash<QString, QString> programmerNames = getProgrammerNames();
	foreach (QString name, programmerNames.keys()) {
		addProgrammer(name, programmerNames.value(name));
	}
	
    connect(m_programmerMenu, SIGNAL(triggered(QAction*)), this, SLOT(setProgrammer(QAction*)));

    currentMenu->addSeparator();

    m_programAction = new QAction(tr("Program"), this);
    m_programAction->setStatusTip(tr("Load the current program onto a microcontroller"));
    m_programAction->setEnabled(false);
    connect(m_programAction, SIGNAL(triggered()), this, SLOT(sendProgram()));
    currentMenu->addAction(m_programAction);

    if (!linkedFiles.isEmpty()) {
        bool firstTime = true;
        foreach (LinkedFile * linkedFile, linkedFiles) {
            ProgramTab * programTab = NULL;
            if (firstTime) {
                firstTime = false;
                programTab = indexWidget(0);
            }
            else {
                programTab = addTab();
            }
            QDir dir(alternativePath);
            QFileInfo fileInfo(linkedFile->filename);
            programTab->loadProgramFile(linkedFile->filename, dir.absoluteFilePath(fileInfo.fileName()), false);
			if (!m_languages.value(linkedFile->language, "").isEmpty()) {
				programTab->setLanguage(linkedFile->language, false);
			}
			else {
				linkedFile->language.clear();
			}
			if (programTab->setProgrammer(linkedFile->programmer)) {
				linkedFile->programmer.clear();
			}
        }
    }
}

QFrame * ProgramWindow::createHeader() {
	QFrame * headerFrame = new QFrame();
	headerFrame->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
	headerFrame->setObjectName("header");
	return headerFrame;
}

QFrame * ProgramWindow::createCenter() {

    QFrame * centerFrame = new QFrame(this);
	centerFrame->setObjectName("center");

	m_tabWidget = new PTabWidget(centerFrame);
	m_tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_tabWidget->setMovable(true);
    m_tabWidget->setTabsClosable(true);
	m_tabWidget->setUsesScrollButtons(false);
	m_tabWidget->setElideMode(Qt::ElideLeft);
     connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	addTab();

	QGridLayout *tabLayout = new QGridLayout(m_tabWidget);
	tabLayout->setMargin(0);
	tabLayout->setSpacing(0);

	QGridLayout *mainLayout = new QGridLayout(centerFrame);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_tabWidget,0,0,1,1);

	return centerFrame;
}

QStringList ProgramWindow::getAvailableLanguages() {
   return m_languages.keys();
}

Syntaxer * ProgramWindow::getSyntaxerForLanguage(QString language) {
	Syntaxer * syntaxer = m_syntaxers.value(language, NULL);
	if (syntaxer == NULL) {
		syntaxer = new Syntaxer();
		if (syntaxer->loadSyntax(m_languages.value(language))) {
			m_syntaxers.insert(language, syntaxer);
		}
	}
    return m_syntaxers.value(language, NULL);
}

void ProgramWindow::cleanUp() {
}

/**
 * eventFilter here is used to catch the keyboard shortcuts that trigger the close event
 * for ProgramWindow. If there are more than one tab widget the standard close shortcut
 * should only close the current tab instead of closing the whole window. Every other
 * case is ignored and handled by closeEvent() like normal.
 */
bool ProgramWindow::eventFilter(QObject * object, QEvent * event) {
        if (object == this && event->type() == QEvent::ShortcutOverride) {
                QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
                if(keyEvent && keyEvent->matches(QKeySequence::Close) && m_tabWidget->count() > 1 ) {
                        return true;
                }
        }
        return QMainWindow::eventFilter(object, event);
}

/**
 * Reimplement closeEvent to save any modified documents before closing.
 */
void ProgramWindow::closeEvent(QCloseEvent *event) {
	if(beforeClosing()) {
		cleanUp();
		QMainWindow::closeEvent(event);
		emit closed();
	} else {
		event->ignore();
	}

	QSettings settings;
	settings.setValue("programwindow/state",saveState());
	settings.setValue("programwindow/geometry",saveGeometry());
}

const QString ProgramWindow::untitledFileName() {
	return "Untitled";
}

const QString ProgramWindow::fileExtension() {
	return "";
}

const QString ProgramWindow::defaultSaveFolder() {
	return FolderUtils::openSaveFolder();
}

bool ProgramWindow::event(QEvent * e) {
	switch (e->type()) {
		case QEvent::WindowActivate:
			emit changeActivationSignal(true, this);
			break;
		case QEvent::WindowDeactivate:
			emit changeActivationSignal(false, this);
			break;
		default:
			break;
	}
	return FritzingWindow::event(e);
}

int & ProgramWindow::untitledFileCount() {
	return UntitledIndex;
}

void ProgramWindow::setTitle() {
    setWindowTitle(tr("Programming Window"));
}

void ProgramWindow::setTitle(const QString & filename) {
        setWindowTitle(tr("Programming Window - %1").arg(filename));
}

/**
 * Create and open a new tab within the PTabWidget child.
 * This function handled connecting all the appropriate signals
 * and setting an appropriate filename.
 */
ProgramTab * ProgramWindow::addTab() {
    QString name = (UntitledIndex == 1) ? untitledFileName() : tr("%1 %2").arg(untitledFileName()).arg(UntitledIndex);
    ProgramTab * programTab = new ProgramTab(name, m_tabWidget);
    connect(programTab, SIGNAL(wantToSave(int)), this, SLOT(tabSave(int)));
    connect(programTab, SIGNAL(wantToSaveAs(int)), this, SLOT(tabSaveAs(int)));
    connect(programTab, SIGNAL(wantToRename(int)), this, SLOT(tabRename(int)));
    connect(programTab, SIGNAL(wantToDelete(int, bool)), this, SLOT(tabDelete(int, bool)), Qt::DirectConnection);
    connect(programTab, 
		SIGNAL(programWindowUpdateRequest(bool, bool, bool, bool, bool, const QString &, const QString &, const QString &, const QString &)), 
		this, 
		SLOT(updateMenu(bool, bool, bool, bool, bool, const QString &, const QString &, const QString &, const QString &)));
	int ix = m_tabWidget->addTab(programTab, name);
    m_tabWidget->setCurrentIndex(ix);
	UntitledIndex++;

	return programTab;
}

/**
 * A function for closing the current displayed tab.
 * I'm using a ProgramWindow method instead of calling deleteTab()
 * directly as I believe it's more apropos.
 */
void ProgramWindow::closeCurrentTab() {
        closeTab(m_tabWidget->currentIndex());
}

void ProgramWindow::closeTab(int index) {
        ProgramTab * pTab = indexWidget(index);
        if (pTab) {
			emit linkToProgramFile(pTab->filename(), "", "", false, true);
                pTab->deleteTab();
        }
}

/**
 * This slot is for updating the tab-dependent menu items.
 *  - program
 *  - undo/redo
 *  - cut/copy
 */
void ProgramWindow::updateMenu(bool programEnable, bool undoEnable, bool redoEnable, bool cutEnable, bool copyEnable, 
							   const QString & language, const QString & port, const QString & programmer, const QString & filename) 
{
	ProgramTab * programTab = currentWidget();
	m_saveAction->setEnabled(programTab->isModified());
    m_programAction->setEnabled(programEnable);
    m_undoAction->setEnabled(undoEnable);
    m_redoAction->setEnabled(redoEnable);
    m_cutAction->setEnabled(cutEnable);
    m_copyAction->setEnabled(copyEnable);
    QAction *lang = m_languageActions.value(language);
    if (lang) {
        lang->setChecked(true);
    }
    QAction *portAction = m_portActions.value(port);
    if (portAction) {
        portAction->setChecked(true);
    }

	QAction *programmerAction = NULL;
	if (programmer == LocateName) {
		foreach (QAction * action, m_programmerActions) {
			if (action->data().toString() == programmer) {
				programmerAction = action;
				break;
			}
		}
	}
	else {
		QFileInfo fileInfo(programmer);
		QString name = fileInfo.fileName();
		programmerAction = m_programmerActions.value(name);
		if (!programmerAction) {
			programmerAction = addProgrammer(name, programmer);
			ProgrammerNames.insert(name, programmer);
			for (int i = 0; i < m_tabWidget->count(); i++) {
				ProgramTab * pt = indexWidget(i);
				if (pt != programTab) {
					pt->updateProgrammers();
				}
			}
		}
	}

    if (programmerAction) {
        programmerAction->setChecked(true);
    }


    setTitle(filename);
}

void ProgramWindow::setLanguage(QAction* action) {
    currentWidget()->setLanguage(action->text(), true);
}

void ProgramWindow::setPort(QAction* action) {
    currentWidget()->setPort(action->text());
}

void ProgramWindow::setProgrammer(QAction* action) {
    currentWidget()->chooseProgrammer(action->data().toString());
}

bool ProgramWindow::beforeClosing(bool showCancel) {
	for (int i = 0; i < m_tabWidget->count(); i++) {
		if (!beforeClosingTab(i, showCancel)) {
			return false;
		}
	}

	return true;
}

bool ProgramWindow::beforeClosingTab(int index, bool showCancel) 
{
	ProgramTab * programTab = indexWidget(index);
	if (programTab == NULL) return true;

	if (!programTab->isModified()) return true;

	QMessageBox::StandardButton reply = beforeClosingMessage(programTab->filename(), showCancel);
	if (reply == QMessageBox::Save) {
		return prepSave(programTab, false);
	} 
	
	if (reply == QMessageBox::Discard) {
		return true;
	}
 		
	return false;
}

void ProgramWindow::print() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        currentWidget()->print(printer);
    } 
#endif
}

bool ProgramWindow::saveAsAux(const QString & fileName) {
	if (!m_savingProgramTab) return false;

    bool result = m_savingProgramTab->save(fileName);
	m_savingProgramTab = NULL;
	return result;
}

void ProgramWindow::tabDelete(int index, bool deleteFile) {
	ProgramTab * programTab = indexWidget(index);
    QString fname = programTab->filename();
	m_tabWidget->removeTab(index);
	if (m_tabWidget->count() == 0) {
		addTab();
	}

	if (deleteFile) {
		QFile file(fname);
		file.remove();
	}
}

void ProgramWindow::tabSave(int index) {
	ProgramTab * programTab =indexWidget(index);
	if (programTab == NULL) return;

    prepSave(programTab, false);
}

void ProgramWindow::tabSaveAs(int index) {
	ProgramTab * programTab = indexWidget(index);
    if (programTab == NULL) return;

    prepSave(programTab, true);
}

void ProgramWindow::tabRename(int index) {
    ProgramTab * programTab = indexWidget(index);
    if (programTab == NULL) return;

    QString oldFileName = programTab->filename();
    if (prepSave(programTab, true)) {
		if (programTab->filename() != oldFileName) {
			QFile oldFile(oldFileName);
			if (oldFile.exists()) {
				oldFile.remove();
				emit linkToProgramFile(oldFileName, "", "", false, true);
			}
		}
    }
}

void ProgramWindow::duplicateTab() {
        ProgramTab * oldTab = currentWidget();
        if (oldTab == NULL) return;

        ProgramTab * newTab = addTab();

        newTab->setText(oldTab->text());
}

void ProgramWindow::tabBeforeClosing(int index, bool & ok) {
	ok = beforeClosingTab(index, true);
}

bool ProgramWindow::prepSave(ProgramTab * programTab, bool saveAsFlag) 
{
	m_savingProgramTab = programTab;				// need this for the saveAsAux call

	bool result = (saveAsFlag) 
		? saveAs(programTab->filename(), programTab->readOnly())
		: save(programTab->filename(), programTab->readOnly());

    if (result) {
		programTab->setClean();
		emit linkToProgramFile(programTab->filename(), programTab->language(), programTab->programmer(), true, true);
	}
	return result;
}

QStringList ProgramWindow::getSerialPorts() {
	QStringList ports = getSerialPortsAux();

	/*
	// on the pc, handy for testing the UI when there are no serial ports
	ports.removeOne("COM0");
	ports.removeOne("COM1");
	ports.removeOne("COM2");
	ports.removeOne("COM3");
	ports.removeOne("COM4");
	ports.removeOne("COM5");
	ports.removeOne("COM6");
	ports.removeOne("COM7");
	ports.removeOne("COM8");
	*/

	if (ports.isEmpty()) {
		ports.append(NoSerialPortName);
	}
	return ports;
}

#ifdef Q_WS_WIN

// faster enumeration code from http://code.google.com/p/qextserialport

/* Gordon Schumacher's macros for TCHAR -> QString conversions and vice versa */
#ifdef UNICODE
    #define QStringToTCHAR(x)     (wchar_t*) x.utf16()
    #define PQStringToTCHAR(x)    (wchar_t*) x->utf16()
    #define TCHARToQString(x)     QString::fromUtf16((ushort*)(x))
    #define TCHARToQStringN(x,y)  QString::fromUtf16((ushort*)(x),(y))
#else
    #define QStringToTCHAR(x)     x.local8Bit().constData()
    #define PQStringToTCHAR(x)    x->local8Bit().constData()
    #define TCHARToQString(x)     QString::fromLocal8Bit((x))
    #define TCHARToQStringN(x,y)  QString::fromLocal8Bit((x),(y))
#endif /*UNICODE*/

// see http://msdn.microsoft.com/en-us/library/ms791134.aspx for list of GUID classes
#ifndef GUID_DEVCLASS_PORTS
    DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 );
#endif

QString getRegKeyValue(HKEY key, LPCTSTR property)
{
    DWORD size = 0;
    DWORD type;
    RegQueryValueEx(key, property, NULL, NULL, NULL, & size);
    BYTE* buff = new BYTE[size];
    QString result;
    if( RegQueryValueEx(key, property, NULL, &type, buff, & size) == ERROR_SUCCESS )
        result = TCHARToQString(buff);
    RegCloseKey(key);
    delete [] buff;
    return result;
}

//static
QString getDeviceProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devData, DWORD property)
{
    DWORD buffSize = 0;
    SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, NULL, 0, & buffSize);
    BYTE* buff = new BYTE[buffSize];
    SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, buff, buffSize, NULL);
    QString result = TCHARToQString(buff);
    delete [] buff;
    return result;
}
#endif

QStringList ProgramWindow::getSerialPortsAux() {
        // TODO: make this call a plugin?
    QStringList ports;
#ifdef Q_WS_WIN
		/*
        for (int i = 1; i < 256; i++)
        {
                QString port = QString("COM%1").arg(i);
                QString sport = QString("\\\\.\\%1").arg(port);
                HANDLE hPort = ::CreateFileA(sport.toLatin1().constData(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
                if (hPort == INVALID_HANDLE_VALUE) {
                        DWORD dwError = GetLastError();
                        if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT) {
                                ports.append(port);
                        }
                }
                else {
                        CloseHandle(hPort);
                        ports.append(port);
                }
        }
		*/

		HDEVINFO devInfo;
		GUID guid = GUID_DEVCLASS_PORTS;
		if( (devInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE)
		{
			SP_DEVINFO_DATA devInfoData;
			devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
			for(int i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devInfoData); i++)
			{
				HKEY devKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				QString value = getRegKeyValue(devKey, TEXT("PortName"));
				ports.append(value);

			}
			SetupDiDestroyDeviceInfoList(devInfo);
		}

        return ports;
#endif
#ifdef Q_WS_MAC
                // see http://developer.apple.com/Mac/library/documentation/DeviceDrivers/Conceptual/WorkingWSerial/WWSerial_SerialDevs/SerialDevices.html

        mach_port_t         masterPort;
        io_iterator_t       matchingServices;

        kern_return_t kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);
        if (KERN_SUCCESS != kernResult)
        {
            DebugDialog::debug(QString("IOMasterPort returned %1").arg(kernResult));
            return ports;
        }

        // Serial devices are instances of class IOSerialBSDClient.
        CFMutableDictionaryRef  classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
        if (classesToMatch == NULL)
        {
            DebugDialog::debug("IOServiceMatching returned a NULL dictionary.");
            return ports;
        }

        CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDRS232Type));

        kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, &matchingServices);
        if (KERN_SUCCESS != kernResult)
        {
            DebugDialog::debug(QString("IOServiceGetMatchingServices returned %1").arg(kernResult));
            return ports;
        }

        io_object_t modemService;
        while ((modemService = IOIteratorNext(matchingServices)))
        {
            CFTypeRef deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                CFSTR(kIOCalloutDeviceKey),
                                kCFAllocatorDefault,
                                0);
            if (deviceFilePathAsCFString)
            {
                char deviceFilePath[1024];
                Boolean result = CFStringGetCString((CFStringRef) deviceFilePathAsCFString,
                                            deviceFilePath,
                                            1024,
                                            kCFStringEncodingASCII);
                CFRelease(deviceFilePathAsCFString);
                if (result)
                {
                    ports.append(deviceFilePath);
                }
            }

            // Release the io_service_t now that we are done with it.
            (void) IOObjectRelease(modemService);
        }

        return ports;
#endif
#ifdef Q_WS_X11
        QProcess * process = new QProcess(this);
        process->setProcessChannelMode(QProcess::MergedChannels);
        process->setReadChannel(QProcess::StandardOutput);

        connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(portProcessFinished(int, QProcess::ExitStatus)));
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(portProcessReadyRead()));

        process->start("dmesg");
        bool result = process->waitForFinished(3000);			// hate to block here, but a better approach will take some time;
        if (!result) return ports;

        return m_ports;
#endif

}

const QHash<QString, QString> ProgramWindow::getProgrammerNames()
{
	return ProgrammerNames;
}

void ProgramWindow::initProgrammerNames()
{
	ProgrammerNames.insert(tr("Locate..."), LocateName);

	// TODO: eventually save a list of programmer names (a la recent files)

	QSettings settings;
	QString programmer = settings.value("programwindow/programmer", "").toString();
	if (!programmer.isEmpty()) {
		QFileInfo fileInfo(programmer);
		if (fileInfo.exists()) {
			ProgrammerNames.insert(fileInfo.fileName(), programmer);
		}
	}
}

QAction * ProgramWindow::addProgrammer(const QString & name, const QString & path)
{
    QAction * currentAction = new QAction(name, this);
    currentAction->setCheckable(true);
	currentAction->setData(path);
    m_programmerActions.insert(name, currentAction);
    m_programmerMenu->addAction(currentAction);
    m_programmerActionGroup->addAction(currentAction);
	return currentAction;
}

void ProgramWindow::loadProgramFile() {
	currentWidget()->loadProgramFile();
	
}

void ProgramWindow::loadProgramFileNew() {
	ProgramTab * programTab = addTab();
	if (programTab) {
		if (!programTab->loadProgramFile()) {
			delete programTab;
		}
	}
}

void ProgramWindow::rename() {
	 currentWidget()->rename();
}

void ProgramWindow::undo() {
	 currentWidget()->undo();
}

void ProgramWindow::redo() {
	 currentWidget()->redo();
}

void ProgramWindow::cut() {
	 currentWidget()->cut();
}

void ProgramWindow::copy() {
	 currentWidget()->copy();
}

void ProgramWindow::paste() {
	 currentWidget()->paste();
}

void ProgramWindow::selectAll() {
	 currentWidget()->selectAll();
}

void ProgramWindow::sendProgram() {
	 currentWidget()->sendProgram();
}

ProgramTab * ProgramWindow::currentWidget() {
	return qobject_cast<ProgramTab *>(m_tabWidget->currentWidget());
}

ProgramTab * ProgramWindow::indexWidget(int index) {
	return qobject_cast<ProgramTab *>(m_tabWidget->widget(index));
}

bool ProgramWindow::alreadyHasProgram(const QString & filename) {
	for (int i = 0; i < m_tabWidget->count(); i++) {
		ProgramTab * tab = indexWidget(i);
		if (tab->filename() == filename) {
			m_tabWidget->setCurrentIndex(i);
			return true;
		}
	}

	return false;
}

QString ProgramWindow::getExtensionString() {
	ProgramTab * pt = currentWidget();
	if (pt == NULL) return "";

	return pt->extensionString();
}

QStringList ProgramWindow::getExtensions() {
	ProgramTab * pt = currentWidget();
	if (pt == NULL) return QStringList();

	return pt->extensions();
}

void ProgramWindow::updateSerialPorts() {
	QStringList ports = getSerialPorts();
	QStringList newPorts;
	foreach (QString port, ports) {
		if (m_portActions.value(port, NULL) == NULL) {
			newPorts.append(port);
		}
	}
	QStringList obsoletePorts;
	foreach (QString port, m_portActions.keys()) {
		if (!ports.contains(port)) {
			obsoletePorts.append(port);
		}
	}

	foreach (QString port, newPorts) {
        QAction * action = new QAction(port, this);
        action->setCheckable(true);
        m_portActions.insert(port, action);
        m_serialPortMenu->addAction(action);
        m_serialPortActionGroup->addAction(action);
    }

	foreach (QString port, obsoletePorts) {
		QAction * action = m_portActions.value(port, NULL);
		if (action == NULL) continue;			// shouldn't happen

		if (action->isChecked()) {
			// TODO:  what?
		}

		m_portActions.remove(port);
		m_serialPortActionGroup->removeAction(action);
		m_serialPortMenu->removeAction(action);
	}
}

void ProgramWindow::updateLink(const QString & filename, const QString & language, const QString & programmer, bool addlink, bool strong)
{
	emit linkToProgramFile(filename, language, programmer, addlink, strong);
}

void ProgramWindow::portProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	DebugDialog::debug(QString("process finished %1 %2").arg(exitCode).arg(exitStatus));

	// parse the text and update the combo box

	sender()->deleteLater();
}

void ProgramWindow::portProcessReadyRead() {
	m_ports.clear();

	QByteArray byteArray = qobject_cast<QProcess *>(sender())->readAllStandardOutput();
    QTextStream textStream(byteArray, QIODevice::ReadOnly);
    while (true) {
        QString line = textStream.readLine();
        if (line.isNull()) break;

        if (!line.contains("tty")) continue;
        if (!line.contains("serial", Qt::CaseInsensitive)) continue;

        QStringList candidates = line.split(" ");
        foreach (QString candidate, candidates) {
            if (candidate.contains("tty")) {
                m_ports.append(candidate);
                break;
            }
        }
    }
}

