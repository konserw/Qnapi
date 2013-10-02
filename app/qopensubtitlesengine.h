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

#ifndef __QOPENSUBTITLESENGINE__H__
#define __QOPENSUBTITLESENGINE__H__

#include <QUrl>
#include <QDir>
#include <QMessageBox>

#include "qnapiabstractengine.h"
#include "xmlrpc/qsyncxmlrpcclient.h"
#include "synchttp.h"
#include "qnapilanguage.h"

class QOpenSubtitlesEngine : public QNapiAbstractEngine
{
public:

    QOpenSubtitlesEngine(const QString & movieFile, const QString& lang);
	~QOpenSubtitlesEngine();

	// zwraca nazwe modulu
	QString engineName();
	// zwraca informacje nt. modulu
	QString engineInfo();
	// zwraca ikone silnika pobierania
	QIcon engineIcon();

	// oblicza sume kontrolna pliku filmowego
    bool checksum();
	// szuka napisow
    bool lookForSubtitles();
	// probuje pobrac napisy
    bool download();
	// probuje rozpakowac napisy
	bool unpack();

private:
	quint64 fileSize;

	QSyncXmlRpcClient rpc;
	QString token;

	QList<QNapiSubtitleInfo> subtitlesList;

	// sprawdza czy dana instancja klasy jest zalogowana na sewerze
	bool isLogged() { return !token.isEmpty(); }
	// loguje na serwer OpenSubtitles
	bool login();
	// wylogowuje z serwera
	void logout();

    static const QString openSubtitlesXmlRpcHost;
    static const QString openSubtitlesXmlRpcPath;
    static const int openSubtitlesXmlRpcPort;
};

#endif
