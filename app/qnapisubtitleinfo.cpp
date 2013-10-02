#include "qnapisubtitleinfo.h"
#include "qnapiconfig.h"
#include "movieinfo.h"

bool QNapiSubtitleInfo::m_initialized = false;

QNapiSubtitleInfo::QNapiSubtitleInfo(const QString &moviePath, const QString &lang)
    : m_moviePath(moviePath)
{
    if(!m_initialized)
    {
        qsrand(time(0));
        m_initialized = true;
    }

    m_tmpPath = GlobalConfig().tmpPath();
    m_tmpPackedFile =  QString("%1/QNapi.%2.tmp").arg(m_tmpPath).arg(qrand());

    if(lang.isEmpty())
        m_lang = GlobalConfig().language();
    else
        m_lang = lang;
}

double QNapiSubtitleInfo::movieFPS() const
{
    MovieInfo info(m_moviePath);
    if(info.isErr)
        return 0;
    return info.fps;
}

bool QNapiSubtitleInfo::hasChecksum() const
{
    return !m_checkSum.isEmpty();
}

QString QNapiSubtitleInfo::movieName() const
{
    return QString();//placeholder
}

QString QNapiSubtitleInfo::moviePath() const
{
    return m_moviePath;
}

void QNapiSubtitleInfo::setMoviePath(const QString &moviePath)
{
    m_moviePath = moviePath;
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

bool QNapiSubtitleInfo::subtitlesTmpExist() const
{
    return QFile::exists(m_subtitlesTmp);
}

QString QNapiSubtitleInfo::subtitlesTmp() const
{
    return m_subtitlesTmp;
}

void QNapiSubtitleInfo::setSubtitlesTmp(const QString &subtitlesTmp)
{
    m_subtitlesTmp = subtitlesTmp;
}

QString QNapiSubtitleInfo::tmpPackedFile() const
{
    return m_tmpPackedFile;
}

void QNapiSubtitleInfo::setTmpPackedFile(const QString &tmpPackedFile)
{
    m_tmpPackedFile = tmpPackedFile;
}
QString QNapiSubtitleInfo::tmpPath() const
{
    return m_tmpPath;
}

void QNapiSubtitleInfo::setTmpPath(const QString &tmpPath)
{
    m_tmpPath = tmpPath;
}
QString QNapiSubtitleInfo::scriptPath() const
{
    return m_scriptPath;
}

void QNapiSubtitleInfo::setScriptPath(const QString &scriptPath)
{
    m_scriptPath = scriptPath;
}
QString QNapiSubtitleInfo::checkSum() const
{
    return m_checkSum;
}

void QNapiSubtitleInfo::setCheckSum(const QString &checkSum)
{
    m_checkSum = checkSum;
}
QString QNapiSubtitleInfo::lang() const
{
    return m_lang;
}

void QNapiSubtitleInfo::setLang(const QString &lang)
{
    m_lang = lang;
}
