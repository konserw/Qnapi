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

#ifdef EMBED_PYTHON
#include <QMutexLocker>
#include <QApplication>
#include <Python.h>
const char* QNapiAbstractEngine::m_appPath = nullptr;
#endif

const char* const QNapiAbstractEngine::m_convertScript =
R"delim(
import aeidon
from aeidon import Project
project = aeidon.Project()
project.open_main("%1", "cp1250")
project.set_framerate(aeidon.framerates.%3)
project.save_main(aeidon.files.new(aeidon.formats.SUBRIP,"%2","cp1250"))
)delim";

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
    QNapiResult f = failedDownload;
    foreach(QNapiSubtitleInfo* i, m_infoList->children())
    {
        if(!download(*i))
        {
            continue;
        }
        f = failedUnpack;
        if(!unpack(*i))
        {
            continue;
        }
        f = failedConvert;
        if(!convert(*i))
        {
            QFile::copy(i->subtitlesTmp(), i->subtitlesPath().replace(".srt", ".txt"));
            continue;
        }
        f = success;
        pp(*i);
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
#ifndef EMBED_PYTHON
        if(QFile::exists(i->scriptPath()))
            QFile::remove(i->scriptPath());
#endif
    }

    delete m_infoList;
}

bool QNapiAbstractEngine::convert(const QNapiSubtitleInfo &info)
{
#ifdef NO_CONVERT
    return false;
#endif
    if(!info.subtitlesTmpExist())
        return false;

    QString framerate;
    double fps = info.movieFPS();
    if(fps < 24) framerate = "FPS_23_976";
    else if(fps < 24.5) framerate = "FPS_24_000";
    else if(fps < 27) framerate = "FPS_25_000";
    else framerate = "FPS_29_970";

    QString program = QString(m_convertScript).arg(info.subtitlesTmp()).arg(info.subtitlesPath()).arg(framerate);

#ifdef EMBED_PYTHON
    QMutexLocker lock(&m_mutex);
    Py_SetProgramName(const_cast<char*>(m_appPath));  /* optional but recommended */
    Py_Initialize();
    PyRun_SimpleString(program.toStdString().c_str());
    Py_Finalize();
#else
    QFile ff(info.scriptPath());
    if(!ff.open(QFile::WriteOnly | QFile::Truncate))
    {
        return false;
    }
    QTextStream out(&ff);
    out << program;
    ff.close();

#ifdef SCRIPT_DEBUG
    QFile::copy(info.scriptPath(), info.subtitlesPath().replace(".srt", ".py"));
#endif

    QProcess process;
    process.start("python", QStringList() << info.scriptPath());
    if(!process.waitForFinished(5000))
        return false;
#endif

    return QFile::exists(info.subtitlesPath());
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
#ifdef EMBED_PYTHON
    if(m_appPath == nullptr)
        m_appPath = qApp->applicationFilePath().toStdString().c_str();
#endif
    if(!info.subtitlesExist())
		return false;

    return QFile::setPermissions(info.subtitlesPath(), permissions);
}

QNapiAbstractEngine::QNapiAbstractEngine(const QString &movieFile, const QNapiLanguage &lang)
{
    m_infoList = new QNapiSubtitleInfoList(movieFile, lang);
}

