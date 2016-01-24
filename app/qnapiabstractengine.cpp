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
#include "qnapiabstractengine.h"
#include "qnapiconfig.h"
#include "qnapisubtitleinfo.h"
#include "QNapiSubtitleInfoList.h"

#include <QTextCodec>
#include <QTextStream>
#include <QProcess>

QNapiResult QNapiAbstractEngine::process()
{
    if(!checksum())
    {
        return failedChechsum;
    }
    if(!lookForSubtitles())
    {
        return failedLook;
    }
    QNapiResult f = failed;
    foreach(QNapiSubtitleInfo* i, m_infoList->children())
    {
        f = failedDownload;
        if(!download(*i))
            continue;
        f = failedUnpack;
        if(!unpack(*i))
            continue;
        move(*i);
        f = failedPostProcess;
        if(!pp(*i))
            continue;
        f = success;
    }

    return f;
}

QNapiAbstractEngine::~QNapiAbstractEngine()
{
    foreach(QNapiSubtitleInfo* i, m_infoList->children())
    {
        if(QFile::exists(i->tmpPackedFile()))
            QFile::remove(i->tmpPackedFile());
        if(QFile::exists(i->subtitlesTmp()))
            QFile::remove(i->subtitlesTmp());
    }

    delete m_infoList;
}

bool QNapiAbstractEngine::move(const QNapiSubtitleInfo& info)
{
    QFile::copy(info.subtitlesTmp(), info.subtitlesPath());

  //  qDebug() << info.subtitlesTmp() << " -> " << info.subtitlesPath();
    return info.subtitlesExist();
}

bool QNapiAbstractEngine::pp(const QNapiSubtitleInfo& info)
{
    bool success = true;
    // Usuwanie linii z plikow z napisami
    if(GlobalConfig().ppRemoveLines())
    {
        success = removeLinesContainingWords(GlobalConfig().ppRemoveWords(), info);
    }

    // Zmiana uprawnien do pliku
    if(GlobalConfig().ppChangePermissions())
    {
        bool validPermissions;
        int permInt = GlobalConfig().ppPermissions().toInt(&validPermissions, 8);

        if(validPermissions)
        {
            int perm = 0;
            perm |= (permInt & 0700) << 2;
            perm |= (permInt & 0070) << 1;
            perm |= (permInt & 0007);
            success = success && changeSubtitlesPermissions(QFile::Permissions(perm), info);
        }
    }
    return success;
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::removeLinesContainingWords(QStringList wordList, const QNapiSubtitleInfo& info)
{
    if(!info.subtitlesExist())
		return false;

	wordList = wordList.filter(QRegExp("^(.+)$"));

    QFile f(info.subtitlesPath());
	if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QList<QByteArray> lines = f.readAll().split('\n');
	QList<QByteArray> out;

	foreach(QByteArray line, lines)
	{
		int i;
		while((i = line.indexOf('\r')) >= 0)
			line.remove(i, 1);

        QTextStream ts(line);
		QString encLine = ts.readAll();

		if(encLine.isEmpty())
		{
			out << line;
			continue;
		}

		bool found = false;
		foreach(QString word, wordList)
		{
			if(encLine.contains(word, Qt::CaseInsensitive))
			{
				found = true;
				break;
			}
		}
		if(found) continue;

		out << line;
	}

	f.close();

	if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		return false;

	foreach(QByteArray line, out)
	{
		f.write(line);
		f.write("\n", 1);
	}
	f.close();

	return true;
}

// Zmienia uprawnienia do pliku z napisami
bool QNapiAbstractEngine::changeSubtitlesPermissions(QFile::Permissions permissions, const QNapiSubtitleInfo& info)
{
    if(!info.subtitlesExist())
		return false;

    return QFile::setPermissions(info.subtitlesPath(), permissions);
}

QNapiAbstractEngine::QNapiAbstractEngine(const QString &movieFile, const QNapiLanguage &lang)
{
    m_infoList = new QNapiSubtitleInfoList(movieFile, lang);
}

