#include "qnapisubtitleinfo.h"
#include <QFile>

bool QNapiSubtitleInfo::m_initialized = false;

QNapiSubtitleInfo::QNapiSubtitleInfo(QNapiSubtitleInfoList *parent)
    : m_parent(parent)
{
    if(!m_initialized)
    {
        qsrand(time(0));
        m_initialized = true;
    }

    m_tmpPackedFile =  QString("%1/QNapi.%2.tmp").arg(m_parent->tmpPath()).arg(qrand());
}

bool QNapiSubtitleInfo::subtitlesExist() const
{
    return QFile::exists(m_subtitlesPath);
}

QString QNapiSubtitleInfo::subtitlesPath() const
{
    return m_subtitlesPath;
}

void QNapiSubtitleInfo::setSubtitlesPath(const QString &subtitlesPath)
{
    m_subtitlesPath = subtitlesPath;
}

QString QNapiSubtitleInfo::tmpPackedFile() const
{
    return m_tmpPackedFile;
}

void QNapiSubtitleInfo::setTmpPackedFile(const QString &tmpPackedFile)
{
    m_tmpPackedFile = tmpPackedFile;
}

QString QNapiSubtitleInfo::url() const
{
    return m_url;
}

void QNapiSubtitleInfo::setUrl(const QString &url)
{
    m_url = url;
}
