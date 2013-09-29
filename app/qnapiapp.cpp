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

#include "qnapiapp.h"

QNapiApp::QNapiApp(int & argc, char **argv, bool useGui, const QString & appName)
	: QSingleApplication(argc, argv, useGui, appName)
{
	f_progress = 0;
	f_options = 0;
}

QNapiApp::~QNapiApp()
{
	if(f_progress) delete f_progress;
	if(f_options) delete f_options;
}

frmProgress * QNapiApp::progress()
{
	if(!f_progress)
	{
		f_progress = new frmProgress();
		if(!f_progress) abort();
		connect(this, SIGNAL(request(QString)),
				f_progress, SLOT(receiveRequest(QString)));
		connect(this, SIGNAL(downloadFile(const QString &)),
				f_progress, SLOT(receiveRequest(const QString &)));
	}
	return f_progress;
}

void QNapiApp::showSettings()
{
	if(!f_options)
	{
		f_options = new frmOptions();
		f_options->readConfig();
	}

	if(f_options->isVisible())
	{
		f_options->raise();
		return;
	}

	if(f_options->exec() == QDialog::Accepted)
		f_options->writeConfig();

	delete f_options;
	f_options = 0;
}

void QNapiApp::showAbout()
{

    //reserved for future use ;)
}

void QNapiApp::tryQuit()
{
	if(progress()->isVisible() && !progress()->close()) return;
	quit();
}


bool QNapiApp::event(QEvent *ev)
{
	if(ev->type() == QEvent::FileOpen)
		emit downloadFile(static_cast<QFileOpenEvent*>(ev)->file());
	else
		return QApplication::event(ev);

	return true;
}
