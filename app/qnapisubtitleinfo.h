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

#include <QString>
#include "QNapiSubtitleInfoList.h"

// struktura opisujaca napisy
class QNapiSubtitleInfo
{
public:
    QNapiSubtitleInfo(QNapiSubtitleInfoList* parent);

    bool subtitlesExist() const;
    QString subtitlesPath() const;
    void setSubtitlesPath(const QString &subtitlesPath);

    QString tmpPackedFile() const;
    void setTmpPackedFile(const QString &tmpPackedFile);

   // QNapiSubtitleInfoList *parent() const;

    QString url() const;
    void setUrl(const QString &url);

    //forward to parent

    bool hasChecksum() const
    { return m_parent->hasChecksum(); }
    double movieFPS() const
    { return m_parent->movieFPS(); }
    QString movieName() const
    { return m_parent->movieName(); }
    QString movieDir() const
    { return m_parent->movieDir(); }
    QString moviePath() const
    { return m_parent->moviePath(); }
    QNapiLanguage lang() const
    { return m_parent->lang(); }
    QString tmpPath() const
    { return m_parent->tmpPath(); }
    bool subtitlesTmpExist() const
    { return m_parent->subtitlesTmpExist(); }
    QString subtitlesTmp() const
    { return m_parent->subtitlesTmp(); }
#ifndef EMBED_PYTHON
    QString scriptPath() const
    { return m_parent->scriptPath(); }
#endif
protected:
    //url do napisow
    QString m_url;
    // sciezka do napisow (zazwyczaj taka sama jak do napisow z innym rozszerzeniem)
    QString m_subtitlesPath;
    // sciezka do spakowanego pliku napisow
    QString m_tmpPackedFile;

    QNapiSubtitleInfoList* m_parent;
};

#endif
