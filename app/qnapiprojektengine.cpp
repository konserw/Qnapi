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
#include "QNapiSubtitleInfoList.h"
#include "movieinfo.h"
#include "qnapiconfig.h"
#include "qmultiparthttprequest.h"
#include "synchttp.h"
#include <QUrl>
#include <QMessageBox>
#include <QCryptographicHash>
#include <cmath>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QProcess>

const QString QNapiProjektEngine::m_napiDownloadUrlTpl= "http://www.napiprojekt.pl/unit_napisy/dl.php?l=%1&f=%2&t=%3&v=other&kolejka=false&nick=%4&pass=%5&napios=%6";
const QString QNapiProjektEngine::m_napiZipPassword = "iBlm8NTigvru0Jr0";


// konstruktor klasy
QNapiProjektEngine::QNapiProjektEngine(const QString & movieFile, const QNapiLanguage &lang)
    : QNapiAbstractEngine(movieFile, lang)
{
}

// destruktor klasy
QNapiProjektEngine::~QNapiProjektEngine()
{
}

// oblicza sume kontrolna dla pliku filmowego (md5 z pierwszych 10MB pliku)
bool QNapiProjektEngine::checksum()
{
    QFile file(m_infoList->moviePath());
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
    m_infoList->setCheckSum(checkSum);

    return true;
}


bool QNapiProjektEngine::lookForSubtitles()
{
    if(!m_infoList->hasChecksum()) return false;


    QString urlTxt = m_napiDownloadUrlTpl
            .arg(npLangWrapper(m_infoList->lang()))
            .arg(m_infoList->checkSum())
            .arg(npFDigest(m_infoList->checkSum()))
            .arg("NapiProjekt")
            .arg("NapiProjekt")
            .arg("Linux/UNIX");

	QUrl url(urlTxt);
    SyncHTTP http;
	http.setHost(url.host());
    http.syncGet(url.path() + "?" + url.query(QUrl::EncodeDelimiters | QUrl::EncodeReserved | QUrl::EncodeSpaces | QUrl::EncodeUnicode));

    if(!http.bytesAvailable())
        return false;

    m_infoList->addChild(urlTxt);

    return true;
}

// Probuje pobrac napisy do filmu z serwera NAPI
bool QNapiProjektEngine::download(const QNapiSubtitleInfo &info)
{
    QUrl url(info.url());
    SyncHTTP http;
    http.setHost(url.host());
    http.syncGet(url.path() + "?" + url.query(QUrl::EncodeDelimiters | QUrl::EncodeReserved | QUrl::EncodeSpaces | QUrl::EncodeUnicode));

    if(!http.bytesAvailable())
        return false;

    QByteArray buffer = http.readAll();

    if(buffer.indexOf("NPc") == 0)
        return false;

    QFile file(info.tmpPackedFile());
    if(file.exists()) file.remove();
    if(!file.open(QIODevice::WriteOnly))
        return false;

    int r = file.write(buffer);
    file.close();

    return (r > 0);
}

// Probuje rozpakowac napisy do filmu
bool QNapiProjektEngine::unpack(const QNapiSubtitleInfo &info)
{
    if(!QFile::exists(info.tmpPackedFile()))
        return false;

    if(QFile::exists(info.subtitlesTmp()))
        QFile::remove(info.subtitlesTmp());

    QStringList args;
    args << "e" << "-y" << ("-p" + m_napiZipPassword) << ("-o" + m_infoList->tmpPath()) << info.tmpPackedFile();

    QProcess p7zip;
    p7zip.start(GlobalConfig().p7zipPath(), args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;

    return QFile::exists(info.subtitlesTmp());
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

QString QNapiProjektEngine::npLangWrapper(QNapiLanguage l)
{
    QString lang = l.toTwoLetter().toUpper();

	if(lang == "EN")
		lang = "ENG";

	return lang;
}
