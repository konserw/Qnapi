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

class QIcon;
class QNapiSubtitleInfo;
class QNapiSubtitleInfoList;

class QNapiAbstractEngine
{
public:
    //odwala cala czarna robote ;)
    virtual bool process();

	// destruktor
    virtual ~QNapiAbstractEngine();
    // powinna czyscic pliki tymczasowe itd.
    virtual void cleanup() = 0;


	// powinna zwracac nazwe modulu
	virtual QString engineName() = 0;
	// powinna zwracac informacje nt. modulu (prawa autorskie itp.)
	virtual QString engineInfo() = 0;
	// zwraca ikone silnika pobierania
	virtual QIcon engineIcon() = 0;

	// powinna obliczac i zwracac sume kontrolna pliku filmowego, a takze ustawiac wartosc zmiennej checkSym
    virtual bool checksum() = 0;
	// szuka napisow w podanym jezyku
    virtual bool lookForSubtitles() = 0;
	// powinna pobierac napisy do filmu i zapisywac w jakims pliku tymczasowym
    virtual bool download() = 0;
    // powinna rozpakowywac pobrane napisy
	virtual bool unpack() = 0;
    // konwersja na srt
    virtual bool convert() = 0;
    bool convert(QNapiSubtitleInfo& info);
    // usuwa linie z pliku zawierajace conajmniej jedno z podanej listy slow
    bool removeLinesContainingWords(QStringList wordList, const QNapiSubtitleInfo &info);
    // zmienia uprawnienia do pliku z napisami   
    bool changeSubtitlesPermissions(QFile::Permissions permissions, QNapiSubtitleInfo info);

protected:
	// konstruktor klasy
    QNapiAbstractEngine(const QString & movieFile, const QString &lang = QString());

    QNapiSubtitleInfoList* m_infoList;

    static const char* const m_convertScript;
};

#endif
