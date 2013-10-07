#ifndef QNAPIRESULT_H
#define QNAPIRESULT_H
#include <QString>

enum QNapiResult
{
    success,
    failedChechsum,
    failedLook,
    failedDownload,
    failedUnpack,
    failedConvert,
    failed,
    notRun
};

class QNapiComplexResult
{
public:
    QNapiComplexResult(QString movie = QString());

    bool succeded() const;
    QString npRes() const;
    QString osRes() const;

    QString movie() const;
    void setMovie(const QString &movie);

    QNapiResult np() const;
    void setNp(const QNapiResult &np);

    QNapiResult os() const;
    void setOs(const QNapiResult &os);

    static QString parseResult(const QNapiResult &res);
private:
    QString m_movie;
    QNapiResult m_np;
    QNapiResult m_os;
};

#endif // QNAPIRESULT_H
