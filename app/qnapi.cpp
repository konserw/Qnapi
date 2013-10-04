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
#include <QMessageBox>
#include <QStringList>
#include <QPair>
#include <QtConcurrent/QtConcurrent>
#include <QProgressDialog>
#include "qnapi.h"
#include "forms/frmoptions.h"
#include "qnapiconfig.h"
#include "qnapisubtitleinfo.h"
#include "qnapiabstractengine.h"
#include "qnapiprojektengine.h"
#include "qopensubtitlesengine.h"
#include "forms/frmsummary.h"

QNapiLanguage QNapi::m_lang;

QPair<bool, QString> QNapi::bazinga(const QString& movie)
{
    bool success;

    QNapiProjektEngine np(movie, m_lang);
    success = np.process();

    if(!success)
    {
        QOpenSubtitlesEngine os(movie, m_lang);
        success = os.process();
    }

    return QPair<bool, QString>(success, movie);
}


QNapi::QNapi(int argc, char **argv)
    : QApplication(argc, argv)
{
    m_showSettings = false;
    m_run = true;

    QStringList args;
    for(int i=0; i<argc; ++i)
    {
        if(i==0)
            continue;
        args.append(QString(argv[i]));
    }

    QString p;

    for(int i=0; i < args.size(); ++i)
    {
        p = args[i];
        if(p.startsWith("file://"))
            p = p.remove(0, 7);

        QFileInfo fi(p);
        if(fi.isDir())
        {
            m_movies << QDir(p).entryList(QStringList() << "*.mp4" << "*.avi" << "*.mkv" << "*.mpg" << "*.mov" << "*.vob");
        }
        else if(fi.isFile())
            m_movies.append(p);

        if((p == "-l") || (p == "--language"))
        {
            ++i;
            if(i < args.size())
                m_lang.setLanguage(args[i]);

            if(!m_lang.isValid())
                if(QMessageBox::question(0, "QNapi", QObject::tr("Niepoprawny kod językowy!\nCzy chcesz pobrać napisy w domyślnym języku?"), QMessageBox::Yes | QMessageBox::No)
                    != QMessageBox::Yes)
                    m_run = false;
        }
    }

    if(args.contains("-o") || args.contains("--options") || m_movies.isEmpty())
    {
        m_showSettings = true;
        m_run = false;
    }
    else
    {
        if(GlobalConfig().firstRun())
        {
            if(QMessageBox::question(0, QObject::tr("Pierwsze uruchomienie"),
                    QObject::tr("To jest pierwsze uruchomienie programu QNapi. Czy chcesz go "
                    "teraz skonfigurować?"), QMessageBox::Yes | QMessageBox::No )
                == QMessageBox::Yes )
            {
                m_showSettings = true;
            }
        }
    }
}

QNapi::~QNapi()
{

}

bool QNapi::checkAll()
{
    if(!QFileInfo(GlobalConfig().p7zipPath()).isExecutable())
    {
        QMessageBox::warning(0, tr("Brak programu p7zip!"),
                                tr("Ścieżka do programu p7zip jest nieprawidłowa!"));
        return false;
    }

    QFileInfo f(GlobalConfig().tmpPath());

    if(!(f.isDir() && f.isWritable()))
    {
        QMessageBox::warning(0, tr("Nieprawidłowy katalog tymczasowy!"),
                                tr("Nie można pisać do katalogu tymczasowego! Sprawdź swoje ustawienia."));
        return false;
    }

    bool flag = true;

    foreach(QString movie, m_movies)
    {
        flag = flag && QFileInfo(QDir(movie).absolutePath()).isWritable();
    }

    if(!flag)
        QMessageBox::warning(0, tr("Nieprawidłowy katalog docelowy!"),
                                tr("Nie można pisać do katalogu docelowego! Sprawdź swoje uprawnienia."));

    return flag;
}

void QNapi::enqueue(const QString &movie)
{
    m_movies.append(movie);
}

int QNapi::exec()
{
    if(m_showSettings)
    {
        frmOptions f_options;
        f_options.readConfig();

        if(f_options.exec() == QDialog::Accepted)
            f_options.writeConfig();
    }

    if(!m_run)
        return 0;
    if(!checkAll())
        return 0;

    if(!m_lang.isValid())
        m_lang.setLanguage(GlobalConfig().language());

    QProgressDialog dialog;
    dialog.setLabelText(tr("Szukanie napisow dla %1 filmów. Prosze czekać...").arg(m_movies.size()));
    dialog.setRange(0, m_movies.size());

    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    QFuture<QPair<bool, QString> > future(QtConcurrent::mapped(m_movies, bazinga));
    futureWatcher.setFuture(future);

    // Display the dialog and start the event loop.
    dialog.exec();

    future.waitForFinished();

    QStringList win, fail;

    QFutureIterator<QPair<bool, QString> > it(future);
    while(it.hasNext())
    {
        QPair<bool, QString>  result = it.next();
        if(result.first)
            win.append(result.second);
        else
            fail.append(result.second);
    }

    frmSummary sum;
    sum.setSuccessList(win);
    sum.setFailedList(fail);
    sum.exec();

    return 0;
}
