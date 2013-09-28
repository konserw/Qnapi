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

	// Zmiana kodowania pobranych napisow
	if(GlobalConfig().ppChangeEncoding())
	{
		// Jesli automatycznie nie uda mu sie wykryc kodowania, to jako kodowania
		// zrodlowego uzywa kodowania wybranego przez uzytkownika
		if (!GlobalConfig().ppAutoDetectEncoding()
			|| !ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingTo()))
		{
			ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingFrom(),
										GlobalConfig().ppEncodingTo());
		}
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

QString QNapiAbstractEngine::ppDetectEncoding(const QString & fileName, int testBufferSize)
{
	QString from;
	QStringList codecs;

	// Tylko takie kodowania obsługuja polskie litery
	codecs << "windows-1250" << "windows-1257" << "ISO-8859-2"
			<< "ISO-8859-13" << "ISO-8859-16" << "UTF-8";

	QFile f(fileName);
	if(!f.open(QIODevice::ReadOnly))
		return "";

	QByteArray testData = (testBufferSize > 0) ? f.read(testBufferSize) : f.readAll();

	f.close();

	foreach(QString codec, codecs)
	{
		QTextStream ts(testData);
		ts.setCodec(qPrintable(codec));
		QString encodedData = ts.readAll();
		QStringList chars = QString::fromUtf8("ą/ś/ż/ć/ń/ł/ó/ę").split("/");

		int i;
		for (i = 0; i < chars.count(); i++)
		{
			if(!encodedData.contains(chars[i], Qt::CaseInsensitive))
				break;
		}

		if(i == chars.count())
		{
			from = codec;
			break;
		}
	}

	return from;
}

// Konwertuje napisy z jednego kodowania na inne
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & from, const QString & to)
{
	QFile f(subtitles);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QByteArray fileContent = f.readAll();
	
	QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
	f.close();

	if(to.compare("UTF-8", Qt::CaseInsensitive) != 0)
	{
		fileContent = QTextCodec::codecForName(qPrintable(to))
						->fromUnicode(contentStr.constData(), contentStr.size());
	}
	else
	{
		fileContent.clear();
		fileContent.append(contentStr);
	}

	if(!f.open(QIODevice::WriteOnly))
		return false;

	f.write(fileContent);
	f.close();

	return true;
}

// Konwertuje napisy z jednego kodowania na inne, dokonujac proby autodetekcji kodowania zrodlowego
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & to)
{
	if(!QFileInfo(subtitles).exists())
		return false;

	QString from = ppDetectEncoding(subtitles);

	if(from.isEmpty())
		return false;

	return ppChangeSubtitlesEncoding(from, to);
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::ppRemoveLinesContainingWords(QStringList wordList)
{
	if(!QFileInfo(subtitles).exists())
		return false;

	wordList = wordList.filter(QRegExp("^(.+)$"));

	QString fromCodec = ppDetectEncoding(subtitles);

	if(fromCodec.isEmpty())
		fromCodec = GlobalConfig().ppEncodingFrom();

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
		ts.setCodec(qPrintable(fromCodec));
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
