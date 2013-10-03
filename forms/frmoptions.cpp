/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QTextCodec>
#include <QDesktopWidget>
#include "frmoptions.h"
#include "qnapiconfig.h"
#include "qnapilanguage.h"


frmOptions::frmOptions(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

	QString tlcode;
	foreach(QString lang, QNapiLanguage("").listLanguages())
	{
		tlcode = QNapiLanguage(lang).toTwoLetter();
		ui.cbLang->addItem(QIcon(QString(":/languages/%1.gif").arg(tlcode)),
							lang,
							QVariant(tlcode));
	}

	connect(ui.le7zPath, SIGNAL(textChanged(const QString &)), this, SLOT(le7zPathChanged()));
	connect(ui.pb7zPathSelect, SIGNAL(clicked()), this, SLOT(select7zPath()));
	connect(ui.leTmpPath, SIGNAL(textChanged(const QString &)), this, SLOT(leTmpPathChanged()));
	connect(ui.pbTmpPathSelect, SIGNAL(clicked()), this, SLOT(selectTmpPath()));
	connect(ui.pbRestoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2,
		(QApplication::desktop()->height() - height()) / 2);
}

frmOptions::~frmOptions()
{
}

void frmOptions::le7zPathChanged()
{
	QFileInfo f(ui.le7zPath->text());
	ui.le7zPath->setStyleSheet(
		f.isFile() && f.isExecutable()
			? ""
			: "color:red;"
		);
}

void frmOptions::select7zPath()
{
	QString path7z = QFileDialog::getOpenFileName(this, tr("Wskaż ścieżkę do programu 7z"),
													QFileInfo(ui.le7zPath->text()).path());
	if(!path7z.isEmpty())
	{
		if(!QFileInfo(path7z).isExecutable())
			QMessageBox::warning(this, tr("Niepoprawna ścieżka"),
				tr("Wskazana przez Ciebie ścieżka do programu 7z jest niepoprawna. Jeśli nie możesz "
					"odnaleźć programu 7z, spróbuj zainstalować pakiet p7zip-full."));
		else
			ui.le7zPath->setText(path7z);
	}
}

void frmOptions::leTmpPathChanged()
{
	QFileInfo f(ui.leTmpPath->text());
	ui.leTmpPath->setStyleSheet(
		f.isDir() && f.isWritable()
			? ""
			: "color:red;"
		);
}

void frmOptions::selectTmpPath()
{
	QString tmpDir = QFileDialog::getExistingDirectory(this,
									tr("Wskaż katalog tymczasowy"),
									QFileInfo(ui.leTmpPath->text()).path(),
									QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(!tmpDir.isEmpty())
		ui.leTmpPath->setText(QFileInfo(tmpDir).path());
}

void frmOptions::writeConfig()
{
	GlobalConfig().setP7zipPath(ui.le7zPath->text());
	GlobalConfig().setTmpPath(ui.leTmpPath->text());
	GlobalConfig().setLanguage(ui.cbLang->itemData(ui.cbLang->currentIndex()).toString());

	GlobalConfig().setPpRemoveLines(ui.cbRemoveLines->isChecked());
	GlobalConfig().setPpRemoveWords(ui.teRemoveWords->toPlainText().split("\n"));
	GlobalConfig().setPpChangePermissions(ui.cbChangePermissions->isChecked());

	QString permissions = QString("%1%2%3").arg(ui.sbUPerm->value())
										   .arg(ui.sbGPerm->value())
										   .arg(ui.sbOPerm->value());
	GlobalConfig().setPpPermissions(permissions);

	GlobalConfig().save();
}

void frmOptions::readConfig()
{
	GlobalConfig().reload();

	ui.le7zPath->setText(GlobalConfig().p7zipPath());
	ui.leTmpPath->setText(GlobalConfig().tmpPath());
	ui.cbLang->setCurrentIndex(ui.cbLang->findData(QNapiLanguage(GlobalConfig().language()).toTwoLetter()));

	ui.cbRemoveLines->setChecked(GlobalConfig().ppRemoveLines());
	ui.teRemoveWords->setText(GlobalConfig().ppRemoveWords().join("\n"));
	ui.cbChangePermissions->setChecked(GlobalConfig().ppChangePermissions());

	QString permissions = GlobalConfig().ppPermissions();
	unsigned short o, g, u;
    o = permissions.at(0).toLatin1() - '0';
    g = permissions.at(1).toLatin1() - '0';
    u = permissions.at(2).toLatin1() - '0';
	ui.sbUPerm->setValue((o <= 7) ? o : 6);
	ui.sbGPerm->setValue((g <= 7) ? g : 4);
	ui.sbOPerm->setValue((u <= 7) ? u : 4);
}

void frmOptions::restoreDefaults()
{
	GlobalConfig().setP7zipPath("");
	GlobalConfig().setTmpPath(QDir::tempPath());
    GlobalConfig().setLanguage("pl");

    GlobalConfig().setPpRemoveLines(false);
	QStringList words;
	words << "movie info" << "synchro";
	GlobalConfig().setPpRemoveWords(words);
	GlobalConfig().setPpChangePermissions(false);
	GlobalConfig().setPpPermissions("644");

	GlobalConfig().save();

	readConfig();
}




