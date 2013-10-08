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

#ifndef __QNAPIABSTRACTENGINE__H__
#define __QNAPIABSTRACTENGINE__H__

#include <QString>
#include <QStringList>
#include <QFile>
#ifdef EMBED_PYTHON
#include <QMutex>
#endif
#include "qnapilanguage.h"
#include "QNapiResult.h"

class QNapiSubtitleInfo;
class QNapiSubtitleInfoList;

class QNapiAbstractEngine
{
public:
    //odwala cala czarna robote ;)
    virtual QNapiResult process();

	// destruktor
    virtual ~QNapiAbstractEngine();

	// powinna obliczac i zwracac sume kontrolna pliku filmowego, a takze ustawiac wartosc zmiennej checkSym
    virtual bool checksum() = 0;
	// szuka napisow w podanym jezyku
    virtual bool lookForSubtitles() = 0;
	// powinna pobierac napisy do filmu i zapisywac w jakims pliku tymczasowym
    virtual bool download(const QNapiSubtitleInfo &info) = 0;
    // powinna rozpakowywac pobrane napisy
    virtual bool unpack(const QNapiSubtitleInfo &info) = 0;
    // konwersja na srt
    virtual bool convert(const QNapiSubtitleInfo &info);
    //post proces
    virtual void pp(const QNapiSubtitleInfo &info);
    // usuwa linie z pliku zawierajace conajmniej jedno z podanej listy slow
    bool removeLinesContainingWords(QStringList wordList, const QNapiSubtitleInfo &info);
    // zmienia uprawnienia do pliku z napisami      
    bool changeSubtitlesPermissions(QFile::Permissions permissions, const QNapiSubtitleInfo &info);
protected:
	// konstruktor klasy
    QNapiAbstractEngine(const QString & movieFile, const QNapiLanguage &lang);

    QNapiSubtitleInfoList* m_infoList;

    static const char* const m_convertScript;

#ifdef EMBED_PYTHON
    static const char* m_appPath;
    QMutex m_mutex;
#endif
};

#endif
