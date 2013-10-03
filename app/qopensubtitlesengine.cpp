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

#include "qopensubtitlesengine.h"

const QString QOpenSubtitlesEngine::openSubtitlesXmlRpcHost  = "www.opensubtitles.org";
const QString QOpenSubtitlesEngine::openSubtitlesXmlRpcPath  = "/xml-rpc";
const int QOpenSubtitlesEngine::openSubtitlesXmlRpcPort  = 80;


// konstruktor klasy
QOpenSubtitlesEngine::QOpenSubtitlesEngine(const QString & movieFile, const QNapiLanguage& lang)
    : QNapiAbstractEngine(movieFile, lang)
{
	rpc.setHost(openSubtitlesXmlRpcHost, openSubtitlesXmlRpcPort, openSubtitlesXmlRpcPath);
}

// destruktor klasy
QOpenSubtitlesEngine::~QOpenSubtitlesEngine()
{
	if(isLogged())
		logout();
}

// oblicza sume kontrolna dla pliku filmowego
bool QOpenSubtitlesEngine::checksum()
{
    QFile file(movie);
	if(!file.open(QIODevice::ReadOnly))
        return false;

    fileSize = movie.size();
	quint64 hash = fileSize;
	quint64 tmp, i;

	for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;
	file.seek(qMax(0, (int)((qint64)fileSize - 65536)));
	for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;

    checkSum = QString("%1").arg(hash, 16, 16, QChar('0'));

    return true;
}

// szuka napisow
bool QOpenSubtitlesEngine::lookForSubtitles()
{
	if(checkSum.isEmpty()) return false;
	if(!isLogged() && !login()) return false;

	subtitlesList.clear();

	QMap<QString, xmlrpc::Variant> paramsMap;
	QList<xmlrpc::Variant> requestList;

    paramsMap["sublanguageid"] = QNapiLanguage(m_lang).toTriLetter();
	paramsMap["moviehash"] = checkSum;
	paramsMap["moviebytesize"] = (uint) fileSize;

	requestList << paramsMap;

	rpc.request("SearchSubtitles", token, requestList);

	QMap<QString, QVariant> responseMap = rpc.result().toMap();

	if(!responseMap.contains("data"))
		return false;

	QList<QVariant> dataList = responseMap["data"].toList();
	QList<QVariant>::iterator i;

	i = dataList.begin();
	while(i != dataList.end())
	{
		responseMap = (*(i++)).toMap();

		if((checkSum != responseMap["MovieHash"]) && (fileSize != responseMap["MovieByteSize"]))
			continue;

		QNapiSubtitleResolution r = SUBTITLE_UNKNOWN;

		if(responseMap["SubBad"].toString() != "0")
		{
			r = SUBTITLE_BAD;
		}
		else if(QFileInfo(movie).completeBaseName() ==
				QFileInfo(responseMap["SubFileName"].toString()).completeBaseName())
		{
			r = SUBTITLE_GOOD;
		}
		else if(QRegExp(QString("^%1").arg(responseMap["MovieReleaseName"].toString()),
						Qt::CaseInsensitive)
				.exactMatch(QFileInfo(movie).completeBaseName()))
		{
			r = SUBTITLE_GOOD;
		}

		QString subtitleName = responseMap["MovieReleaseName"].toString();
		if(subtitleName.isEmpty())
			subtitleName = QFileInfo(movie).completeBaseName();

		subtitlesList << QNapiSubtitleInfo(	responseMap["ISO639"].toString(),
											engineName(),
											responseMap["IDSubtitleFile"].toString(),
											subtitleName,
											responseMap["SubAuthorComment"].toString(),
											QFileInfo(responseMap["SubFileName"].toString()).suffix(),
											r);
	}

	return (subtitlesList.size() > 0);
}

// Probuje pobrac napisy do filmu z serwera OpenSubtitles
bool QOpenSubtitlesEngine::download()
{
    foreach(QNapiSubtitleInfo s, subtitlesList)
    {
        download(s);
    }
}

bool QOpenSubtitlesEngine::download(QNapiSubtitleInfo s)
{
	QList<xmlrpc::Variant> paramsList;
	QList<xmlrpc::Variant> requestList;

	paramsList << s.url;
	requestList << paramsList;

	rpc.request("DownloadSubtitles", token, requestList);

	QMap<QString, QVariant> responseMap = rpc.result().toMap();

	if(!responseMap.contains("data"))
		return false;

	QVariantList responseList;
	responseList << responseMap["data"].toList();

	if(responseList.size() == 0)
		return false;

	QByteArray subtitleContent = QByteArray::fromBase64(responseList.at(0).toMap()["data"].toByteArray());

	QFile file(tmpPackedFile);
	if(file.exists()) file.remove();
	if(!file.open(QIODevice::WriteOnly))
		return false;

	int r = file.write(subtitleContent);
	file.close();

	return (bool)r;
}

// Probuje dopasowac napisy do filmu
bool QOpenSubtitlesEngine::unpack()
{
	if(!QFile::exists(tmpPackedFile)) return false;
	if(!QFile::exists(movie)) return false;	

	QStringList args;
	args << "e" << "-y" << ("-o" + tmpPath) << tmpPackedFile;

	QProcess p7zip;
    p7zip.start(GlobalConfig().p7zipPath(), args);

	if(!p7zip.waitForFinished()) return false;

	QString unpackedTmp = tmpPath + QDir::separator() + QFileInfo(tmpPackedFile).completeBaseName();
    subtitlesTmp = tmpPath + QDir::separator() + generateTmpFileName();

	QFile::copy(unpackedTmp, subtitlesTmp);
	QFile::remove(unpackedTmp);

	return true;
}

bool QOpenSubtitlesEngine::login()
{
	QString userAgent = QString("QNapi v%1").arg(QNAPI_VERSION);

    rpc.request("LogIn", QString(""), QString(""),  m_lang.toLower(), userAgent);
	token = (rpc.result().toMap())["token"].toString();

	return !token.isEmpty();
}

void QOpenSubtitlesEngine::logout()
{
	rpc.request("LogOut", token);
	token = "";
}
