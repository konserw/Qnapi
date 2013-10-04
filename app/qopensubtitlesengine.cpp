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
#include <QUrl>
#include <QDir>
#include <QMessageBox>
#include "qnapisubtitleinfo.h"
#include "qopensubtitlesengine.h"
#include "QNapiSubtitleInfoList.h"
#include "synchttp.h"
#include "qnapiconfig.h"

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
    QFile file(m_infoList->moviePath());
	if(!file.open(QIODevice::ReadOnly))
        return false;

    fileSize = file.size();
	quint64 hash = fileSize;
	quint64 tmp, i;

	for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;
	file.seek(qMax(0, (int)((qint64)fileSize - 65536)));
	for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;

    m_infoList->setCheckSum(QString("%1").arg(hash, 16, 16, QChar('0')));

    return true;
}

// szuka napisow
bool QOpenSubtitlesEngine::lookForSubtitles()
{
    if(!m_infoList->hasChecksum()) return false;
	if(!isLogged() && !login()) return false;

	QMap<QString, xmlrpc::Variant> paramsMap;
	QList<xmlrpc::Variant> requestList;

    paramsMap["sublanguageid"] = m_infoList->lang().toTriLetter();
    paramsMap["moviehash"] = m_infoList->checkSum();
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

        if((m_infoList->checkSum() != responseMap["MovieHash"]) && (fileSize != responseMap["MovieByteSize"]))
			continue;

        m_infoList->addChild(responseMap["IDSubtitleFile"].toString());
	}

    return (m_infoList->childCount() > 0);
}

// Probuje pobrac napisy do filmu z serwera OpenSubtitles
bool QOpenSubtitlesEngine::download(const QNapiSubtitleInfo &info)
{
	QList<xmlrpc::Variant> paramsList;
	QList<xmlrpc::Variant> requestList;

    paramsList << info.url();
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

    QFile file(info.tmpPackedFile());
	if(file.exists()) file.remove();
	if(!file.open(QIODevice::WriteOnly))
		return false;

	int r = file.write(subtitleContent);
	file.close();

	return (bool)r;
}

// Probuje rozpakowac napisy do filmu
bool QOpenSubtitlesEngine::unpack(const QNapiSubtitleInfo &info)
{
    if(!QFile::exists(info.tmpPackedFile()))
        return false;

    if(QFile::exists(info.subtitlesTmp()))
        QFile::remove(info.subtitlesTmp());

    QStringList args;
    args << "e" << "-y" << ("-o" + m_infoList->tmpPath()) << info.tmpPackedFile();

    QProcess p7zip;
    p7zip.start(GlobalConfig().p7zipPath(), args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;

    return QFile::exists(info.subtitlesTmp());
}

bool QOpenSubtitlesEngine::login()
{
    QString userAgent = QString("QNapi v%1").arg(VERSION);

    rpc.request("LogIn", QString(""), QString(""),  m_infoList->lang().toTriLetter().toLower(), userAgent);
	token = (rpc.result().toMap())["token"].toString();

	return !token.isEmpty();
}

void QOpenSubtitlesEngine::logout()
{
	rpc.request("LogOut", token);
    token.clear();
}
