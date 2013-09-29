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

#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <QTextCodec>
#include <QTranslator>
//#include <QLibraryInfo>
//#include <QLocale>
#include <signal.h>

#include "version.h"
#include "qnapiconfig.h"
#include "qnapicli.h"
#include "forms/frmprogress.h"
#include "forms/frmoptions.h"

void regSignal();
void sigHandler(int);
void showSettings();

int main(int argc, char **argv)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	regSignal();

    if(QNapiCli::isCliCall(argc, argv))
    {
        QNapiCli app(argc, argv);
        return app.exec();
    }
    else
	{
        QApplication app(argc, argv);

		QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
		QTranslator cuteTranslator;
		cuteTranslator.load("qt_" + QLocale::system().name(), resourceDir);
		app.installTranslator(&cuteTranslator);


        frmProgress progress;
        progress.show();

        QStringList args = app.arguments();
        QStringList pathList;
        QString batchLang, p;
        bool invalidLang = false;

        for(int i=0; i < args.size(); ++i)
        {
            if(p.startsWith("file://"))
                p = p.remove(0, 7);

            if(QFileInfo(p).isDir())
            {
                pathList << QDir(p).entryList(QStringList() << "*.mp4" << "*.avi" << "*.mkv" << "*.mpg" << "*.mov" << "*.vob");
            }

            if(QFileInfo(p).isFile())
                pathList << p;

            if((p == "-l") || (p == "--language"))
            {
                ++i;
                if(i < args.size())
                {
                    batchLang = QNapiLanguage(args[i]).toTwoLetter();
                    if(batchLang.isEmpty())
                        invalidLang = true;
                }
                else
                    invalidLang = true;
            }

        }


        if(args.contains("-o") || args.contains("--options") || pathList.size() == 0)
		{
            showSettings();
			return 0;
		}
        else // Jesli podano parametry, ustawiamy tzw. batch mode
		{
            if(GlobalConfig().firstRun())
            {
                if(QMessageBox::question(0, QObject::tr("Pierwsze uruchomienie"),
                        QObject::tr("To jest pierwsze uruchomienie programu QNapi. Czy chcesz go "
                        "teraz skonfigurować?"), QMessageBox::Yes | QMessageBox::No )
                    == QMessageBox::Yes )
                {
                    showSettings();
                }
            }

            progress.setBatchMode(true);

			if(invalidLang)
			{
                if(QMessageBox::question(0, "QNapi", QObject::tr("Niepoprawny kod językowy!\n"
                        "Czy chcesz pobrać napisy w domyślnym języku?"), QMessageBox::Yes | QMessageBox::No)
					!= QMessageBox::Yes)
				{
					return 0;
				}
			}

            progress.setBatchLanguage(batchLang);

            progress.enqueueFiles(pathList);
            if(!progress.download())
                return 1;

		}

        return app.exec();
	}
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

void sigHandler(int sig)
{
	Q_UNUSED(sig);

	qDebug() << "\nQNapi: usuwanie plików tymczasowych...";

	QString tmpPath = GlobalConfig().tmpPath();

	QStringList filters;
	filters << "QNapi-*-rc";
	filters << "QNapi.*.tmp";
    filters << "convert.py";

	QDir dir(tmpPath);

	QStringList files = dir.entryList(filters);

	foreach(QString file, files)
	{
		QFile::remove(tmpPath + QDir::separator() + file);
	}

	exit(666);
}

void showSettings()
{
    frmOptions f_options;
    f_options.readConfig();

    if(f_options.exec() == QDialog::Accepted)
        f_options.writeConfig();
}
