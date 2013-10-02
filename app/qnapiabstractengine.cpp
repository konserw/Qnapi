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

//#include <QFlags>

//#include <QFileInfo>

const char* const QNapiAbstractEngine::m_convertScript = R"delim(
        import sys
        import aeidon
        from aeidon import Project

        project = aeidon.Project()
        project.open_main("%1", "cp1250")
        project.set_framerate(aeidon.framerates.%3)
        project.save_main(aeidon.files.new(aeidon.formats.SUBRIP,"%2","utf_8"))
        sys.exit(0)
        )delim";

bool QNapiAbstractEngine::process()
{
    if(!checksum())
    {
        return false;
    }
    if(!lookForSubtitles())
    {
        return false;
    }
    if(!download())
    {
        return false;
    }
    if(!unpack())
    {
        return false;
    }
    if(!convert())
    {
        return false;
    }



    return true;
}

QNapiAbstractEngine::~QNapiAbstractEngine()
{
    cleanup();

    delete m_infoList;
}

bool QNapiAbstractEngine::convert(QNapiSubtitleInfo &info)
{
    if(!info.subtitlesTmpExist())
        return false;

    QString framerate;
    double fps = info.parent()->movieFPS();
    if(fps < 24) framerate = "FPS_23_976";
    else if(fps < 24.5) framerate = "FPS_24_000";
    else if(fps < 27) framerate = "FPS_25_000";
    else framerate = "FPS_29_970";

    QString program = QString(m_convertScript).arg(info.subtitlesTmp()).arg(info.subtitlesPath()).arg(framerate);

//    scriptPath = qApp->applicationDirPath() + "/convert_ " + mf.completeBaseName() + ".py";
    QFile ff(info.scriptPath());
    if(!ff.open(QFile::WriteOnly | QFile::Truncate))
    {
        return false;
    }
    QTextStream out(&ff);
    out << program;
    ff.close();
//embed?
    QProcess process;
    process.start("python", QStringList() << info.scriptPath());
    if(!process.waitForFinished(5000))
        return false;

    return (process.exitCode() == 0);
}

void QNapiAbstractEngine::pp(const QNapiSubtitleInfo& info)
{
    // Usuwanie linii z plikow z napisami
    if(GlobalConfig().ppRemoveLines())
    {
        removeLinesContainingWords(GlobalConfig().ppRemoveWords(), info);
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
            changeSubtitlesPermissions(QFile::Permissions(perm), info);
        }
    }
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::removeLinesContainingWords(QStringList wordList, const QNapiSubtitleInfo& info)
{
    if(!info->subtitlesExist())
		return false;

	wordList = wordList.filter(QRegExp("^(.+)$"));

	QFile f(subtitles);
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

QNapiAbstractEngine::QNapiAbstractEngine(const QString &movieFile, const QString &lang)
{
    m_infoList = new QNapiInfoList(movieFile, lang);
}

