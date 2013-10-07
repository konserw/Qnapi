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

#ifndef __FRMSUMMARY__H__
#define __FRMSUMMARY__H__

#include <QDialog>
#include <QDesktopWidget>
#include <QListWidget>
#include <QFileInfo>

#include "qnapiconfig.h"
#include "ui_frmsummary.h"
#include "QNapiResult.h"

class frmSummary: public QDialog
{
Q_OBJECT

	public:
        frmSummary(QWidget *parent = 0);
        ~frmSummary(){}

        void addResult(const QNapiComplexResult& res);
        int exec();

	private:
		Ui::frmSummary ui;
};

#endif
