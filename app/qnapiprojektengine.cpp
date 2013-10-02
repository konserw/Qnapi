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

#include "qnapiprojektengine.h"
#include "qnapisubtitleinfo.h"
#include <QFile>
#include <QIcon>
#include <QPixmap>

const QString QNapiProjektEngine::m_napiDownloadUrlTpl= "http://www.napiprojekt.pl/unit_napisy/dl.php?l=%1&f=%2&t=%3&v=other&kolejka=false&nick=%4&pass=%5&napios=%6";
const QString QNapiProjektEngine::m_napiZipPassword = "iBlm8NTigvru0Jr0";


// konstruktor klasy
QNapiProjektEngine::QNapiProjektEngine(const QString & movieFile, const QString &lang)
    : QNapiAbstractEngine()
{
    m_info = new QNapiSubtitleInfo(movieFile, lang);
}

// destruktor klasy
QNapiProjektEngine::~QNapiProjektEngine()
{
    delete m_info;
}


// zwraca nazwe modulu
QString QNapiProjektEngine::engineName()
{
	return "NapiProjekt";
}

// zwraca informacje nt. modulu
QString QNapiProjektEngine::engineInfo()
{
	return "Modul pobierania napisów z bazy <b>www.napiprojekt.pl</b><br />"
			"Copyright (c) 2008-2009 by Krzemin";
}

// zwraca ikone w formacie XMP
QIcon QNapiProjektEngine::engineIcon()
{
	static const char * const icon[] = {
		"16 16 5 1",
		"   c #FFFFFF",
		".  c #000080",
		"+  c #0052CC",
		"@  c #005CE6",
		"#  c #BFD9FF",
		"           ...  ",
		"         ... .  ",
		"       ... ...  ",
		"     ... ....   ",
		"   ... ....     ",
		" ... ....       ",
		".. ....         ",
		"....+           ",
		" ...............",
		" . . . . . . . .",
		" ...............",
		" .@@####@####@@.",
		" .@@#@@#@#@@#@@.",
		" .@@#@@#@####@@.",
		" .@@#@@#@#@@@@@.",
		" ..............."};
    return QIcon(QPixmap(icon));
}

// oblicza sume kontrolna dla pliku filmowego (md5 z pierwszych 10MB pliku)
bool QNapiProjektEngine::checksum()
{
    QFile file(m_info->moviePath());
    QByteArray fileArray;

    if(!file.open(QIODevice::ReadOnly))
        return false;

    fileArray = file.read(10485760);
    file.close();

    QByteArray b = QCryptographicHash::hash(fileArray, QCryptographicHash::Md5);
    fileArray.clear();

    QString checkSum;
    char next[3];

    for(int i = 0; i < 16; i++)
    {
        snprintf(next, 3, "%.2x", (unsigned char)b[i]);
        checkSum += next;
    }
    m_info->setCheckSum(checkSum);

    return true;
}


bool QNapiProjektEngine::lookForSubtitles()
{
    if(!m_info->hasChecksum()) return false;

	SyncHTTP http;
    QString urlTxt = m_napiDownloadUrlTpl
            .arg(npLangWrapper(m_info->lang()))
            .arg(m_info->checkSum())
            .arg(npFDigest(m_info->checkSum()))
            .arg("NapiProjekt")
            .arg("NapiProjekt")
            .arg("Linux/UNIX");

	QUrl url(urlTxt);

	http.setHost(url.host());
    http.syncGet(url.path() + "?" + url.query(QUrl::EncodeDelimiters | QUrl::EncodeReserved | QUrl::EncodeSpaces | QUrl::EncodeUnicode));

	QByteArray buffer = http.readAll();

	if(buffer.indexOf("NPc") == 0)
		return false;

	QFile file(tmpPackedFile);
	if(file.exists()) file.remove();
	if(!file.open(QIODevice::WriteOnly))
		return false;

	int r = file.write(buffer);
	file.close();
	
	if(!r) return false;
	
    subInfo = new QNapiSubtitleInfo(m_lang,
                                engineName(),
                                urlTxt,
                                QFileInfo(movie).completeBaseName(),
                                "",
                                "txt",
                                SUBTITLE_UNKNOWN);

    return true;
}

// Probuje pobrac napisy do filmu z serwera NAPI
bool QNapiProjektEngine::download()
{
    return (subInfo != 0);
}

// Probuje rozpakowac napisy do filmu
bool QNapiProjektEngine::unpack()
{
    if(!QFile::exists(tmpPackedFile)) return false;
	subtitlesTmp = tmpPath + "/" + checkSum + ".txt";

	if(QFile::exists(subtitlesTmp))
		QFile::remove(subtitlesTmp);

	QStringList args;
    args << "e" << "-y" << ("-p" + m_napiZipPassword) << ("-o" + tmpPath) << tmpPackedFile;

	QProcess p7zip;
    p7zip.start(GlobalConfig().p7zipPath(), args);

	// Rozpakowujemy napisy max w ciagu 5 sekund
	if(!p7zip.waitForFinished(5000)) return false;

    return QFile::exists(subtitlesTmp);
}

void QNapiProjektEngine::cleanup()
{
    if(QFile::exists(info.tmpPackedFile))
        QFile::remove(tmpPackedFile);
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);
    if(QFile::exists(scriptPath))
        QFile::remove(scriptPath);
}
// Tajemnicza funkcja f()
QString QNapiProjektEngine::npFDigest(const QString & input)
{
	if(input.size() != 32) return "";

	int idx[] = {0xe, 0x3, 0x6, 0x8, 0x2},
		mul[] = {2, 2, 5, 4, 3},
		add[] = {0x0, 0xd, 0x10, 0xb, 0x5},
		a, m, i, t, v;

	QString b;
	char tmp[2] = { 0, 0 };

	for(int j = 0; j <= 4; j++)
	{
		a = add[j];
		m = mul[j];
		i = idx[j];

        tmp[0] = input[i].toLatin1();
		t = a + (int)(strtol(tmp, NULL, 16));
		v = (int)(strtol(input.mid(t, 2).toLocal8Bit(), NULL, 16));

		snprintf(tmp, 2, "%x", (v * m) % 0x10);
		b += tmp;
	}

	return b;
}

QString QNapiProjektEngine::npLangWrapper(QString lang)
{
	lang = QNapiLanguage(lang).toTwoLetter().toUpper();

	if(lang == "EN")
		lang = "ENG";

	return lang;
}
