#ifndef QNAPISUBTITLEINFOLIST_H
#define QNAPISUBTITLEINFOLIST_H

#include "qnapilanguage.h"
#include <QList>
#include <QString>

class QNapiSubtitleInfo;

class QNapiSubtitleInfoList
{
public:
    QNapiSubtitleInfoList(const QString& moviePath, const QNapiLanguage &lang);

    bool hasChecksum() const;
    QString checkSum() const;
    void setCheckSum(const QString &checkSum);

    double movieFPS() const;
    QString movieName() const;
    QString movieDir() const;
    QString moviePath() const;

    QNapiLanguage lang() const;
    void setLang(const QNapiLanguage &lang);

    QString tmpPath() const;

    QNapiSubtitleInfo* child() const;
    QList<QNapiSubtitleInfo *> children() const;
    int childCount() const;

    void addChild(const QString& url);

protected:
    // sciezka do pliku filmowego
    QString m_moviePath;
    // sciezka do katalogu tymczasowego
    QString m_tmpPath;
    // suma kontrolna pliku filmowego
    QString m_checkSum;
    // jezyk napisow
    QNapiLanguage m_lang;

    QList<QNapiSubtitleInfo*> m_children;
};

#endif // QNAPISUBTITLEINFOLIST_H
