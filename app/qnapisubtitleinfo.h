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

#ifndef __QNAPISUBTITLEINFO__H__
#define __QNAPISUBTITLEINFO__H__

#include <QList>
#include <QString>

// Orientacyjne okreslenie czy napisy na pewno pasuja lub nie do naszego filmu
enum QNapiSubtitleResolution
{
	// nie wiadomo (lub nie jestesmy pewni)
	SUBTITLE_UNKNOWN,
	// podejrzenie, ze napisy nie pasuja
	// (np. zostaly oznaczone jako nieprawidlowe)
	SUBTITLE_BAD,
	// napisy prawdopodobnie pasuja (np. nazwa pliku czy releasu sie zgadza)
	SUBTITLE_GOOD
};

// struktura opisujaca napisy
class QNapiSubtitleInfo
{
public:
    QNapiSubtitleInfo(const QString& moviePath = "", const QString& lang = "");

    QString movieName() const;
    double movieFPS() const;
    QString moviePath() const;
    void setMoviePath(const QString &moviePath);

    bool subtitlesExist() const;
    QString subtitlesPath() const;
    void setSubtitlesPath(const QString &subtitlesPath);

    bool subtitlesTmpExist() const;
    QString subtitlesTmp() const;
    void setSubtitlesTmp(const QString &subtitlesTmp);

    QString tmpPackedFile() const;
    void setTmpPackedFile(const QString &tmpPackedFile);

    QString tmpPath() const;
    void setTmpPath(const QString &tmpPath);

    QString scriptPath() const;
    void setScriptPath(const QString &scriptPath);

    bool hasChecksum() const;
    QString checkSum() const;
    void setCheckSum(const QString &checkSum);

    QString lang() const;
    void setLang(const QString &lang);

protected:
    //url do napisow
    QString m_url;
    // sciezka do pliku filmowego
    QString m_moviePath;
    // sciezka do napisow (zazwyczaj taka sama jak do napisow z innym rozszerzeniem)
    QString m_subtitlesPath;
    // sciezka do tymczasowego pliku z napisami
    QString m_subtitlesTmp;
    // sciezka do spakowanego pliku napisow
    QString m_tmpPackedFile;
    // sciezka do katalogu tymczasowego
    QString m_tmpPath;
    // sciezka do pliku skryptu konwertujacego
    QString m_scriptPath;
    // suma kontrolna pliku filmowego
    QString m_checkSum;
    // jezyk napisow
    QString m_lang;

    QNapiSubtitleResolution m_resolution;

    static bool m_initialized;
};

typedef QList<QNapiSubtitleInfo> QNapiSubtitleInfoList;

#endif



