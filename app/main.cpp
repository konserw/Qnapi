/*****************************************************************************
** QNapi
** 
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
#include <QTextCodec>
#include <QTranslator>
#include <signal.h>
#include <QLibraryInfo>
#include <QtDebug>
#include <QDir>
#include <QFile>
#include "qnapiconfig.h"
#include "qnapi.h"

void regSignal();
void sigHandler(int);
void showSettings();

int main(int argc, char **argv)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	regSignal();
/*
    if(QNapiCli::isCliCall(argc, argv))
    {
        QNapiCli app(argc, argv);
        return app.exec();
    }
    else
	{
    */
        QNapi app(argc, argv);
#ifndef NO_TRANSLATION
		QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
		QTranslator cuteTranslator;
		cuteTranslator.load("qt_" + QLocale::system().name(), resourceDir);
		app.installTranslator(&cuteTranslator);
#endif
        return app.exec();
    //}
}

void regSignal()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigHandler;
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGINT, &sa, 0);
}

void sigHandler(int /*sig*/)
{
//	Q_UNUSED(sig);

	qDebug() << "\nQNapi: usuwanie plików tymczasowych...";

	QString tmpPath = GlobalConfig().tmpPath();

	QStringList filters;
	filters << "QNapi-*-rc";
    filters << "QNapi.*.tmp";

	QDir dir(tmpPath);

	QStringList files = dir.entryList(filters);

	foreach(QString file, files)
	{
		QFile::remove(tmpPath + QDir::separator() + file);
	}

	exit(666);
}


