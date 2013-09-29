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
#include "movieinfo.h"
#include <QFlags>
#include <QFile>
#include <QTextStream>

// ustawia sciezke do pliku filmowego
QNapiAbstractEngine::~QNapiAbstractEngine()
{
    cleanup();
}

void QNapiAbstractEngine::cleanup()
{
    if(QFile::exists(tmpPackedFile))
        QFile::remove(tmpPackedFile);
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);
    if(QFile::exists(scriptPath))
        QFile::remove(scriptPath);

}

void QNapiAbstractEngine::setMoviePath(const QString & path)
{
	movie = path;
}

// zwraca sciezke do pliku filmowego
QString QNapiAbstractEngine::moviePath()
{
	return movie;
}

// ustawia sciezke do pliku z napisami
void QNapiAbstractEngine::setSubtitlesPath(const QString & path)
{
	subtitles = path;
}

// zwraca sciezke do pliku z napisami
QString QNapiAbstractEngine::subtitlesPath()
{
	return subtitles;
}

// dopasowuje napisy do pliku z filmem
bool QNapiAbstractEngine::match()
{
	QFileInfo stf(subtitlesTmp);

	if(!stf.exists())
		return false;

	QFileInfo mf(movie);

	subtitles = mf.path() + "/" + mf.completeBaseName() + "." + stf.suffix();

	QFileInfo sf(subtitles);

	if(!QFileInfo(sf.absolutePath()).isWritable())
		return false;

	if(QFile::exists(subtitles))
	{
		if(!noBackup)
		{
			QString newName = mf.path() + QDir::separator() + mf.completeBaseName() + "_kopia." + sf.suffix();

			if(QFile::exists(newName))
				QFile::remove(newName);

			QFile::rename(subtitles, newName);			
		}
		else
			QFile::remove(subtitles);
	}

    return QFile::copy(subtitlesTmp, subtitles);
}

// Dokonuje post-przetwarzania pliku z napisami (na podstawie konfiguracji)
void QNapiAbstractEngine::pp()
{
	// Usuwanie linii z plikow z napisami
	if(GlobalConfig().ppRemoveLines())
	{
		ppRemoveLinesContainingWords(GlobalConfig().ppRemoveWords());
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
			ppChangeSubtitlesPermissions(QFile::Permissions(perm));
		}
    }
}

bool QNapiAbstractEngine::convert()
{
    if(!QFile::exists(subtitlesTmp))
        return false;

    QFileInfo mf(movie);
    subtitles = mf.path() + "/" + mf.completeBaseName() + ".srt";

    QString framerate;
    MovieInfo info(movie);
    if(info.isErr)
        framerate = "FPS_23_976";
    else
    {
        if(info.fps < 24) framerate = "FPS_23_976";
        else if(info.fps < 24.5) framerate = "FPS_24_000";
        else if(info.fps < 27) framerate = "FPS_25_000";
        else framerate = "FPS_29_970";
    }

    QFile f(":/scripts/convert");
    f.open(QFile::ReadOnly);
    QTextStream str(&f);

    QString program = str.readAll().arg(subtitlesTmp).arg(subtitles).arg(framerate);

    scriptPath = qApp->applicationDirPath() + "/convert.py";
    QFile ff(scriptPath);
    if(!ff.open(QFile::WriteOnly | QFile::Truncate))
    {
        return false;
    }
    QTextStream out(&ff);
    out << program;
    ff.close();

    QProcess process;
    process.start("python", QStringList() << scriptPath);
    process.waitForFinished(-1);

    return true;
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::ppRemoveLinesContainingWords(QStringList wordList)
{
	if(!QFileInfo(subtitles).exists())
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
bool QNapiAbstractEngine::ppChangeSubtitlesPermissions(QFile::Permissions permissions)
{
	if(!QFileInfo(subtitles).exists())
		return false;

    return QFile::setPermissions(subtitles, permissions);
}

//konstruktor
QNapiAbstractEngine::QNapiAbstractEngine(const QString &movieFile, const QString &subtitlesFile)
    : movie(movieFile), subtitles(subtitlesFile)
{
    tmpPath = GlobalConfig().tmpPath();
    noBackup = GlobalConfig().noBackup();
}

// generuje nazwe dla pliku tymczasowego
QString QNapiAbstractEngine::generateTmpFileName()
{
	static bool gen_inited;
	if(!gen_inited)
	{
		qsrand(time(0));
		gen_inited = true;
	}
	return QString("QNapi.%1.tmp").arg(qrand());
}
