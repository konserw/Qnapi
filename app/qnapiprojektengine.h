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
#include "qnapilanguage.h"
#include "synchttp.h"

class QNapiProjektEngine : public QNapiAbstractEngine
{
public:

    QNapiProjektEngine(const QString & movieFile, const QNapiLanguage &lang = QNapiLanguage());
	~QNapiProjektEngine();

    bool checksum();
    bool lookForSubtitles();
    bool download(const QNapiSubtitleInfo &info);
    bool unpack(const QNapiSubtitleInfo &info);

private:
    SyncHTTP http;

	QString npFDigest(const QString & input);
    QString npLangWrapper(QNapiLanguage l);

    static const QString m_napiDownloadUrlTpl;
    static const QString m_napiZipPassword;
};

#endif
