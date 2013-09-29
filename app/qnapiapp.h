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

#ifndef __QNAPIAPP__H__
#define __QNAPIAPP__H__

#include <QApplication>
#include <QString>
#include <QEvent>
#include <QFileOpenEvent>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QDesktopServices>

#include "qcumber/qsingleapplication.h"
#include "forms/frmprogress.h"
#include "forms/frmoptions.h"
#include "forms/frmsummary.h"
#include "qnapiconfig.h"


class QNapiApp : public QSingleApplication
{
	Q_OBJECT
	public:
		QNapiApp(int & argc, char **argv, bool useGui, const QString & appName);
		~QNapiApp();

		frmProgress * progress();

	public slots:	
		void showSettings();
		void showAbout();
		void tryQuit();

	signals:
		void downloadFile(const QString & fileName);

	private:
		bool event(QEvent *ev);
		
		frmProgress *f_progress;
        frmOptions *f_options;
};

#endif
