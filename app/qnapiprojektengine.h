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

#ifndef __QNAPIPROJEKTENGINE__H__
#define __QNAPIPROJEKTENGINE__H__

#include <QCryptographicHash>
#include <QUrl>
#include <QMessageBox>

#include <cmath>

#include "qnapiabstractengine.h"
#include "qmultiparthttprequest.h"
#include "synchttp.h"
#include "movieinfo.h"

#include "qnapilanguage.h"

class QNapiProjektEngine : public QNapiAbstractEngine
{
public:

	enum UploadResult
	{
		NAPI_ADDED_NEW, NAPI_OK, NAPI_FAIL, NAPI_UNKNOWN
	};

	enum ReportResult
	{
		NAPI_REPORTED, NAPI_NO_SUBTITLES, NAPI_NOT_REPORTED
	};

	QNapiProjektEngine(const QString & movieFile = "", const QString & subtitlesFile = "");
	~QNapiProjektEngine();

    bool match()
    { return QNapiAbstractEngine::convert(); }

	// zwraca nazwe modulu
	QString engineName();
	// zwraca informacje nt. modulu
	QString engineInfo();
	// zwraca ikone silnika pobierania
    QIcon engineIcon();

	QString checksum(QString filename = "");
	bool lookForSubtitles(QString lang);
	QList<QNapiSubtitleInfo> listSubtitles();
	bool download(int idx);
    bool unpack();

	QString name()
	{
		return QString("NapiProjekt");
	}

	static bool createUser(const QString & nick, const QString & pass,
							const QString & email, QString * response);
	static bool checkUser(const QString & nick, const QString & pass);
	UploadResult uploadSubtitles(const QString & language, const QString & nick,
									const QString & pass, bool correct = false,
									const QString & comment = "");
	ReportResult reportBad(const QString & language, const QString & nick, const QString & pass,
							const QString & comment, QString *response);

private:

    QString p7zipPath, nick, pass;

	QList<QNapiSubtitleInfo> subtitlesList;

	QString checksum(QString filename, bool limit10M);
	QString npFDigest(const QString & input);
	QString npLangWrapper(QString lang);

    static const unsigned long NAPI_10MB;
    static const QString napiDownloadUrlTpl;
    static const QString napiCheckUserUrlTpl;
    static const QString napiUploadUrlTpl;
    static const QString napiUploadUrlSimpleTpl;
    static const QString napiReportBadUrlTpl;
    static const QString napiCreateUserUrlTpl;
    static const QString napiZipPassword;

};

#endif
