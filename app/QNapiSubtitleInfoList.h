#ifndef QNAPISUBTITLEINFOLIST_H
#define QNAPISUBTITLEINFOLIST_H

#include <QList>
#include <QString>

class QNapiSubtitleInfo;

class QNapiSubtitleInfoList
{
public:
    QNapiSubtitleInfoList(const QString& moviePath, const QString& lang = QString());

    bool hasChecksum() const;
    QString checkSum() const;
    void setCheckSum(const QString &checkSum);

    double movieFPS() const;
    QString movieName() const;
    QString movieDir() const;
    QString moviePath() const;

    QString lang() const;
    void setLang(const QString &lang);

    QString tmpPath() const;

    QList<QNapiSubtitleInfo *> children() const;
    void insertChild(QNapiSubtitleInfo* child);

protected:
    // sciezka do pliku filmowego
    QString m_moviePath;
    // sciezka do katalogu tymczasowego
    QString m_tmpPath;
    // suma kontrolna pliku filmowego
    QString m_checkSum;
    // jezyk napisow
    QString m_lang;

    QList<QNapiSubtitleInfo*> m_children;
};

#endif // QNAPISUBTITLEINFOLIST_H
