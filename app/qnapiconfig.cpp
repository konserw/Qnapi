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
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QRegExp>
#include "qnapiconfig.h"

QNapiConfig* QNapiConfig::m_instance = nullptr;

QNapiConfig::QNapiConfig()
{
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
}

QNapiConfig::~QNapiConfig()
{
	if(settings) delete settings;
}

QNapiConfig *QNapiConfig::instance()
{
    if(m_instance == nullptr)
        m_instance = new QNapiConfig;
    return m_instance;
}

void QNapiConfig::reload()
{
	if(settings) delete settings;
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
}

void QNapiConfig::save()
{
    settings->setValue("qnapi/version", VERSION);
	settings->setValue("qnapi/firstrun", false);
	settings->sync();
}

bool QNapiConfig::firstRun()
{
	return settings->value("qnapi/firstrun", true).toBool();
}

QString QNapiConfig::version()
{
	return settings->value("qnapi/version", "").toString();
}

QString QNapiConfig::p7zipPath()
{
	// Odczytujemy z konfiguracji
	QString p7z_path = settings->value("qnapi/7z_path", "").toString();
	if(!p7z_path.isEmpty() && QFileInfo(p7z_path).isExecutable())
		return p7z_path;

	// Przeszukiwanie sciezek systemowych
	QString path = QProcess::systemEnvironment().filter(QRegExp("^PATH=(.*)$")).value(0);
	QStringList paths = path.mid(5).split(":");
	paths.removeAll("");

	if(paths.size() == 0)
		paths << "/usr/bin" << "/usr/local/bin";

	QStringList binaries;
	binaries << "7z" << "7za" << "7zr" << "p7zip";

	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		for(QStringList::iterator j = binaries.begin(); j != binaries.end(); j++)
		{
			p7z_path = (*i) + "/" + (*j);
			if(QFileInfo(p7z_path).isExecutable())
				return p7z_path;
		}
	}


#ifdef Q_WS_WIN
// Pod systemem Windows program 7za.exe musi byc w tym samym katalogu,
// co qnapi.exe
	p7z_path = QFileInfo(QApplication::applicationDirPath() + "/7za.exe").absoluteFilePath();
	if(QFileInfo(p7z_path).isExecutable())
		return p7z_path;
#endif

	// Jesli wszystko inne zawiodlo...;)
	return "7z";
}

void QNapiConfig::setP7zipPath(const QString & path)
{
	settings->setValue("qnapi/7z_path", path);
}

QString QNapiConfig::tmpPath()
{
	return settings->value("qnapi/tmp_path", QDir::tempPath()).toString();
}

void QNapiConfig::setTmpPath(const QString & path)
{
	settings->setValue("qnapi/tmp_path", path);
}

QString QNapiConfig::language()
{
	return settings->value("qnapi/language", "pl").toString();
}

void QNapiConfig::setLanguage(const QString & language)
{
	settings->setValue("qnapi/language", language);
}
bool QNapiConfig::ppRemoveLines()
{
	return settings->value("qnapi/remove_lines", false).toBool();
}

void QNapiConfig::setPpRemoveLines(bool remove)
{
	settings->setValue("qnapi/remove_lines", remove);
}

QStringList QNapiConfig::ppRemoveWords()
{
	QStringList defaultRemoveWords;
	defaultRemoveWords << "movie info" << "synchro ";

	return settings->value("qnapi/remove_words", defaultRemoveWords).toStringList();
}

void QNapiConfig::setPpRemoveWords(const QStringList & words)
{
	settings->setValue("qnapi/remove_words", words);
}

bool QNapiConfig::ppChangePermissions()
{
	return settings->value("qnapi/change_permissions", false).toBool();
}

void QNapiConfig::setPpChangePermissions(bool change)
{
	settings->setValue("qnapi/change_permissions", change);
}

QString QNapiConfig::ppPermissions()
{
	bool ok;
	int perm = settings->value("qnapi/permissions", 644).toInt(&ok);
	if(!ok || perm > 777)
		perm = 644;
	QString str = QString("%1").arg(perm);
	while(str.size() < 3)
		str = QString("0") + str;
	return str;
}

void QNapiConfig::setPpPermissions(const QString & permissions)
{
	bool ok;
	int perm = permissions.toInt(&ok);
	if(ok && perm <= 777)
	{
		QString str = QString("%1").arg(perm);
		while(str.size() < 3)
			str = QString("0") + str;
		settings->setValue("qnapi/permissions", str);
	}
}

QString QNapiConfig::previousDialogPath()
{
	QString path = settings->value("qnapi/prev_dialog_path", "").toString();
	return (QDir(path).exists() ? path : QDir::homePath());
}

void QNapiConfig::setPreviousDialogPath(const QString & path)
{
	settings->setValue("qnapi/prev_dialog_path", path);
}

QNapiConfig & GlobalConfig()
{
    return *QNapiConfig::instance();
}
