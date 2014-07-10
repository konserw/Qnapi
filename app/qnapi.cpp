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
#include <QtConcurrent/QtConcurrent>
#include <QProgressDialog>
//#include <QtDebug>
#include "qnapi.h"
#include "forms/frmoptions.h"
#include "qnapiconfig.h"
#include "qnapisubtitleinfo.h"
#include "qnapiabstractengine.h"
#include "qnapiprojektengine.h"
#include "qopensubtitlesengine.h"
#include "forms/frmsummary.h"

QNapiLanguage QNapi::m_lang;

QNapiComplexResult QNapi::bazinga(const QString& movie)
{
    QNapiComplexResult res(movie);

    QNapiProjektEngine np(movie, m_lang);
    res.setNp(np.process());

    //qDebug() << movie << "\t|NapiProjekt:\t" << f;

    if(res.np() != success)
    {
        QOpenSubtitlesEngine os(movie, m_lang);
        res.setOs(os.process());
   //     qDebug() << movie << "\t|OpenSubs:\t" << f;
    }

    return res;
}


QNapi::QNapi(int argc, char **argv)
    : QApplication(argc, argv)
{
    m_showSettings = false;
    m_run = true;

    QStringList args = this->arguments();
/*
    for(int i=0; i<argc; ++i)
    {
        if(i==0)
            continue;
        args.append(QString(argv[i]));
    }
*/
    QString argument;

    for(int i=1; i < args.size(); ++i)
    {
        argument = args[i];
        if(argument.startsWith("file://"))
            argument = argument.remove(0, 7);

        QFileInfo fi(argument);
        if(fi.isDir())
        {
            QFileInfoList l = QDir(argument).entryInfoList(QStringList() << "*.mp4" << "*.avi" << "*.mkv" << "*.mpg" << "*.mov" << "*.vob");
            foreach(QFileInfo i, l)
                m_movies << i.absoluteFilePath();
        }
        else if(fi.isFile())
            m_movies.append(argument);

        if((argument == "-l") || (argument == "--language"))
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
        flag = flag && QFileInfo(QFileInfo(movie).absolutePath()).isWritable();
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
    QFutureWatcher<QNapiComplexResult> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(close()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

    QFuture<QNapiComplexResult> future(QtConcurrent::mapped(m_movies, bazinga));
    futureWatcher.setFuture(future);

    // Display the dialog and start the event loop.
    dialog.exec();
    future.waitForFinished();

    frmSummary sum;

    QFutureIterator<QNapiComplexResult> it(future);
    while(it.hasNext())
    {
        sum.addResult(it.next());
    }

    sum.exec();

    return 0;
}
