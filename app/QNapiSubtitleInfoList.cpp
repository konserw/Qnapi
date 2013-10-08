#include "QNapiSubtitleInfoList.h"
#include "qnapisubtitleinfo.h"
#include "qnapiconfig.h"
#include "movieinfo.h"

#include <QFileInfo>

QNapiSubtitleInfoList::QNapiSubtitleInfoList(const QString &moviePath, const QNapiLanguage &lang)
    : m_moviePath(moviePath), m_lang(lang)
{
    m_tmpPath = GlobalConfig().tmpPath();
}


double QNapiSubtitleInfoList::movieFPS() const
{
    MovieInfo info(m_moviePath);
    if(info.isErr)
        return 0;
    return info.fps;
}

bool QNapiSubtitleInfoList::hasChecksum() const
{
    return !m_checkSum.isEmpty();
}

QString QNapiSubtitleInfoList::movieName() const
{    
    QFileInfo fi(m_moviePath);
    return fi.completeBaseName();
}

QString QNapiSubtitleInfoList::movieDir() const
{
    QFileInfo fi(m_moviePath);
    return fi.absolutePath();
}

QString QNapiSubtitleInfoList::moviePath() const
{
    return m_moviePath;
}

QString QNapiSubtitleInfoList::checkSum() const
{
    return m_checkSum;
}

void QNapiSubtitleInfoList::setCheckSum(const QString &checkSum)
{
    m_checkSum = checkSum;
    m_subtitlesTmp = QString("%1/%2.txt").arg(m_tmpPath, m_checkSum);
#ifndef EMBED_PYTHON
    m_scriptPath = QString("%1/%2.py").arg(m_tmpPath, m_checkSum);
#endif
}

QNapiLanguage QNapiSubtitleInfoList::lang() const
{
    return m_lang;
}

void QNapiSubtitleInfoList::setLang(const QNapiLanguage &lang)
{
    m_lang = lang;
}

QString QNapiSubtitleInfoList::tmpPath() const
{
    return m_tmpPath;
}

QNapiSubtitleInfo *QNapiSubtitleInfoList::child() const
{
    if(m_children.size() != 1)
        return nullptr;
    return m_children[0];
}

QList<QNapiSubtitleInfo *> QNapiSubtitleInfoList::children() const
{
    return m_children;
}

int QNapiSubtitleInfoList::childCount() const
{
    return m_children.size();
}

void QNapiSubtitleInfoList::addChild(const QString &url)
{
    QNapiSubtitleInfo* child = new QNapiSubtitleInfo(this);
    child->setUrl(url);

    QString suffix;
    if(!m_children.isEmpty())
        suffix = QString("_%1").arg(QString::number(m_children.size()).rightJustified(2, '0'));

    child->setSubtitlesPath(QString("%1/%2%3.srt").arg(movieDir(), movieName(), suffix));

    m_children.append(child);
}

bool QNapiSubtitleInfoList::subtitlesTmpExist() const
{
    return QFile::exists(m_subtitlesTmp);
}

QString QNapiSubtitleInfoList::subtitlesTmp() const
{
    return m_subtitlesTmp;
}

void QNapiSubtitleInfoList::setSubtitlesTmp(const QString &subtitlesTmp)
{
    m_subtitlesTmp = subtitlesTmp;
}

#ifndef EMBED_PYTHON
QString QNapiSubtitleInfoList::scriptPath() const
{
    return m_scriptPath;
}

void QNapiSubtitleInfoList::setScriptPath(const QString &scriptPath)
{
    m_scriptPath = scriptPath;
}
#endif
