#include "QNapiResult.h"
#include <QObject>


QNapiComplexResult::QNapiComplexResult(QString movie)
    : m_movie(movie), m_np(notRun), m_os(notRun)
{
}

bool QNapiComplexResult::succeded() const
{
    return (m_np == success || m_os == success);
}


QString QNapiComplexResult::npRes() const
{
    return parseResult(m_np);
}

QString QNapiComplexResult::osRes() const
{
    return parseResult(m_os);
}

QString QNapiComplexResult::movie() const
{
    return m_movie;
}

void QNapiComplexResult::setMovie(const QString &movie)
{
    m_movie = movie;
}

QNapiResult QNapiComplexResult::np() const
{
    return m_np;
}

void QNapiComplexResult::setNp(const QNapiResult &np)
{
    m_np = np;
}

QNapiResult QNapiComplexResult::os() const
{
    return m_os;
}

void QNapiComplexResult::setOs(const QNapiResult &os)
{
    m_os = os;
}

QString QNapiComplexResult::parseResult(const QNapiResult& res)
{
    switch(res)
    {
    case success:
        return QObject::tr("SUCCESS");
    case failedChechsum:
        return QObject::tr("Failed to calculate checksum");
    case failedLook:
        return QObject::tr("Unable to find subtitles");
    case failedDownload:
        return QObject::tr("Failed to download subtitles");
    case failedUnpack:
        return QObject::tr("Failed to unpack downloaded subtitles");
    case failedPostProcess:
        return QObject::tr("Failed post processing");
    case failed:
        return QObject::tr("Failed, unknown reason");
    case notRun:
        return QObject::tr("Not run");
    }
    return QString();
}

