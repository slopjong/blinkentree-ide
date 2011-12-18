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

$Revision: 4346 $:
$Author: cohen@irascible.com $:
$Date: 2010-07-09 08:30:53 +0200 (Fri, 09 Jul 2010) $

********************************************************************/

#ifndef RECOVERYDIALOG_H
#define RECOVERYDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QString>
#include <QFileInfoList>
#include <QStyleOptionViewItem>

class CenteredTreeWidget : public QTreeWidget {
	Q_OBJECT

public:
	CenteredTreeWidget(QWidget *parent = 0);

protected:
	QStyleOptionViewItem viewOptions () const;
};

class RecoveryDialog : public QDialog {
        Q_OBJECT
public:
        RecoveryDialog(QFileInfoList fileList, QWidget *parent = 0, Qt::WindowFlags flags = 0);
        QList<QTreeWidgetItem*> getFileList();

protected:
        QString getOriginalFileName(const QString & path);

        QList<QTreeWidgetItem*> m_fileList;
        QTreeWidget *m_recoveryList;
        QPushButton *m_recover;
        QPushButton *m_ignore;

protected slots:
        void updateRecoverButton();
};

#endif // RECOVERYDIALOG_H
