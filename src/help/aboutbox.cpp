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

$Revision: 5662 $:
$Author: cohen@irascible.com $:
$Date: 2011-12-04 13:58:39 +0100 (Sun, 04 Dec 2011) $

********************************************************************/

#include <QLabel>
#include <QFont>
#include <QChar>
#include <QTime>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QScrollBar>

#include "aboutbox.h"
#include "../debugdialog.h"
#include "../version/version.h"
#include "../utils/expandinglabel.h"

AboutBox* AboutBox::singleton = NULL;

static const int AboutWidth = 390;
static const int AboutText = 210;

AboutBox::AboutBox(QWidget *parent)
: QWidget(parent)
{
	singleton = this;
	// To make the application not quit when the window closes
	this->setAttribute(Qt::WA_QuitOnClose, FALSE);

	setFixedSize(AboutWidth, 430);

	// the background color
	setStyleSheet("background-color: #E8E8E8");

	// the new Default Font
	QFont smallFont("Droid Sans", 11);
	QFont extraSmallFont("Droid Sans", 9);
	extraSmallFont.setLetterSpacing(QFont::PercentageSpacing, 92);

	// Big Icon
	QLabel *logoShield = new QLabel(this);
	logoShield->setPixmap(QPixmap(":/resources/images/AboutBoxLogoShield.png"));
	logoShield->setGeometry(131, 8, 128, 128);

	// Version String
	QLabel *versionMain = new QLabel(this);
	QString macBuildType;
#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
	macBuildType = " Cocoa";
#else
	macBuildType = " Carbon";
#endif
#endif
	versionMain->setText(tr("Version %1.%2.%3 <small>(%4%5 %6)%7 [Qt %8]</small>")
						 .arg(Version::majorVersion())
						 .arg(Version::minorVersion())
						 .arg(Version::minorSubVersion())
						 .arg(Version::modifier())
						 .arg(Version::revision())
						 .arg(Version::date())
						 .arg(macBuildType)
						 .arg(QT_VERSION_STR) );
	versionMain->setFont(smallFont);
	versionMain->setGeometry(45, 150, 300, 20);
	versionMain->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

	// Link to website
	QLabel *linkToFritzing = new QLabel(this);
	linkToFritzing->setText(tr("<a href=\"http://www.fritzing.org\">www.fritzing.org</a>"));
	linkToFritzing->setOpenExternalLinks(TRUE);
	linkToFritzing->setFont(smallFont);
	linkToFritzing->setGeometry(45, 168, 300, 18);
	linkToFritzing->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);


	// Copyright messages
	

	QLabel *copyrightGNU = new QLabel(this);
	copyrightGNU->setText(tr("<b>GNU GPL v3 on the code and CreativeCommons:BY-SA on the rest"));
	copyrightGNU->setFont(extraSmallFont);
	copyrightGNU->setGeometry(0, 398, AboutWidth, 16);
	copyrightGNU->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

	QLabel *CC = new QLabel(this);
	QPixmap cc(":/resources/images/aboutbox_CC.png");
	CC->setPixmap(cc);
	CC->setGeometry(30, this->height() - cc.height(), cc.width(), cc.height());

	QLabel *FHP = new QLabel(this);
	QPixmap fhp(":/resources/images/aboutbox_FHP.png");
	FHP->setPixmap(fhp);
	FHP->setGeometry(360 - fhp.width(), this->height() - fhp.height(), fhp.width(), fhp.height());

	int w = qMax(fhp.width(), cc.width());

	QLabel *copyrightFHP = new QLabel(this);
	copyrightFHP->setText(tr("<b>2007-%1 Fachhochschule Potsdam</b>").arg(Version::year()));
	copyrightFHP->setFont(extraSmallFont);
	copyrightFHP->setGeometry(30 + w, 414, AboutWidth - 30 - 30 - w - w, 16);
	copyrightFHP->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

	// Scrolling Credits Text

	// moved data out of credits.txt so we could apply translation
	QString data = 
QString("<br /><br /><br /><br /><br /><br /><br /><br /><br />") +
	
"<p>" +
	tr("Fritzing is made by: ") +
	tr("Prof. Reto Wettach, Andr&eacute; Kn&ouml;rig, Myriel Milicevic, ") +
	tr("Zach Eveland, Dirk van Oosterbosch, ") +
	tr("Jonathan Cohen, Marcus Paeschke, Omer Yosha, ") +
	tr("Travis Robertson, Stefan Hermann, Brendan Howell, ") +
	tr("Mariano Crowe, Johannes Landstorfer, ") +
	tr("Jenny Chowdhury, Lionel Michel, Jannis Leidel, ") +
	tr("Bryant Mairs, Uleshka Asher, and Daniel Tzschentke. ") +
"</p>" +

"<p>" +
	tr("Special thanks goes out to: ") +
	tr("Jussi &Auml;ngeslev&auml;, Massimo Banzi, Ayah Bdeir, ") +
	tr("Durrell Bishop, David Cuartielles, Fabian Hemmert, ") +
	tr("Gero Herkenrath, Jeff Hoefs, Tom Hulbert, ") +
	tr("Tom Igoe, Hans-Peter Kadel, Till Savelkoul, ") +
	tr("Jan Sieber, Yaniv Steiner, Olaf Val, ") +
	tr("Michaela Vieser and Julia Werner.") +
"</p>" +

"<p>" +
	tr("Thanks to Kurt Badelt and Miguel Solis for the Spanish translation, ") +
	tr("to Gianluca Urgese for the Italian translation, ") +
	tr("to Nuno Pessanha Santos for the Portuguese (European) translation, ") +
	tr("to Yuelin and Ninjia  for the Chinese (Simplified) translation, ") +
	tr("to Hiroshi Suzuki for the Japanese translation, ") +
	tr("to Robert Lee for the Chinese (Traditional) translation, ") +
	tr("to Vladimir Savinov for the Russian translation, " ) +
	tr("to Steven Noppe for the Dutch translation, " ) +
	tr("and to Arthur Zanona, Nuno Pessanha Santos, and Leandro Nunes for the Portuguese (Brazilian) translation. " ) +
"</p>" +

"<p>" +
	tr("Fritzing is made possible with funding from the ") +
	tr("MWFK Brandenburg, the sponsorship of the Design ") +
	tr("Department of Bauhaus-University Weimar, ") +
	tr("IxDS, an anonymous donor, Parallax, Picaxe, Sparkfun, ") +
	tr("and from each purchase of a Fritzing Starter Kit.") +
"</p>" +

"<p>" +
	tr("Special thanks goes out as well to all the students ") +
	tr("and alpha testers who were brave enough to give ") +
	tr("Fritzing a test spin. ") +
"</p>" +

tr("<br /><br /><br /><br /><br /><br /><br /><br />");

	QPixmap fadepixmap(":/resources/images/aboutbox_scrollfade.png");

	m_expandingLabel = new ExpandingLabel(this, AboutWidth);
	m_expandingLabel->setLabelText(data);
	m_expandingLabel->setFont(smallFont);
	m_expandingLabel->setGeometry(0, AboutText, AboutWidth, fadepixmap.height());
	m_expandingLabel->setStyleSheet("border: 0px; background-color: transparent; margin-top: 0px; margin-bottom: 0px;");
	
	// setAlignment only aligns the "current paragraph"
	// the QTextCursor code aligns all paragraphs
	QTextCursor cursor(m_expandingLabel->document());
	cursor.select(QTextCursor::Document);
	QTextBlockFormat fmt;
	fmt.setAlignment(Qt::AlignCenter);
	cursor.mergeBlockFormat(fmt);
	
	// Add a fade out and a fade in the scrollArea
	QLabel *scrollFade = new QLabel(this);
	scrollFade->setPixmap(fadepixmap);
	scrollFade->setGeometry(0, AboutText, AboutWidth, fadepixmap.height());
	scrollFade->setStyleSheet("background-color: none");
	

	// auto scroll timer initialization
	m_restartAtTop = FALSE;
	m_startTime = QTime::currentTime();
	m_autoScrollTimer = new QTimer(this);
	connect(m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(scrollCredits()));
}

void AboutBox::resetScrollAnimation() {
	// Only called when the window is newly loaded
	m_autoScrollTimer->start(35);
	m_startTime.start();
}

void AboutBox::scrollCredits() {
	if (m_startTime.elapsed() >= 0 ) {
		//int max = m_scrollArea->verticalScrollBar()->maximum();
		//int v = m_scrollArea->widget()->sizeHint().height();
		if (m_restartAtTop) {
			// Reset at the top
			m_startTime.start();
			m_restartAtTop = false;
			m_expandingLabel->verticalScrollBar()->setValue(0);
			return;
		}
		if (m_expandingLabel->verticalScrollBar()->value() >= m_expandingLabel->verticalScrollBar()->maximum()) {
			// go and reset
			// m_startTime.start();
			m_restartAtTop = true;
		} else {
			m_expandingLabel->verticalScrollBar()->setValue(m_expandingLabel->verticalScrollBar()->value() + 1);
		}
	}
}

void AboutBox::hideAbout() {
	//DebugDialog::debug("the AboutBox gets a hide action triggered");
	if (singleton != NULL) {
		singleton->hide();
	}
}

void AboutBox::showAbout() {
	//DebugDialog::debug("the AboutBox gets a show action triggered");
	if (singleton == NULL) {
		new AboutBox();
	}

	// scroll text now to prevent a flash of text if text was visible the last time the about box was open
	singleton->m_expandingLabel->verticalScrollBar()->setValue(0);

	singleton->show();
}

void AboutBox::closeAbout() {
	//DebugDialog::debug("the AboutBox gets a close action triggered");
	// Note: not every close triggers this function. we better listen to closeEvent
	if (singleton != NULL) {
		singleton->close();
	}
}

void AboutBox::closeEvent(QCloseEvent *event) {
	// called when the window is about to close
	//DebugDialog::debug("the AboutBox gets a closeEvent");
	m_autoScrollTimer->stop();
	event->accept();
}

void AboutBox::keyPressEvent ( QKeyEvent * event ) {
	if ((event->key() == Qt::Key_W) && (event->modifiers() & Qt::ControlModifier) ) {
		// We get the ctrl + W / command + W key event
		//DebugDialog::debug("W key!");
		this->closeAbout();
	}
}

void AboutBox::show() {
	QWidget::show();
	m_restartAtTop = true;
	resetScrollAnimation();
}
