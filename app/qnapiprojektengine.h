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




#include "qnapiabstractengine.h"
#include "qmultiparthttprequest.h"
#include "synchttp.h"

class QNapiProjektEngine : public QNapiAbstractEngine
{
public:

    QNapiProjektEngine(const QString & movieFile, const QString &lang = QString());
	~QNapiProjektEngine();

	// zwraca nazwe modulu
	QString engineName();
	// zwraca informacje nt. modulu
	QString engineInfo();
	// zwraca ikone silnika pobierania
    QIcon engineIcon();

    bool checksum();
    bool lookForSubtitles();
    bool download();
    bool unpack();

    void cleanup();

private:
    SyncHTTP http;

	QString npFDigest(const QString & input);
    QString npLangWrapper(QString m_lang);

    static const QString m_napiDownloadUrlTpl;
    static const QString m_napiZipPassword;
};

#endif
