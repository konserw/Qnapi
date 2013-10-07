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

#include "frmsummary.h"

frmSummary::frmSummary(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
         (QApplication::desktop()->height() - height()) / 2);
}

void frmSummary::addResult(const QNapiComplexResult &res)
{
    int  row = ui.tableWidget->rowCount();
    ui.tableWidget->insertRow(row);

    QIcon ico;
    if(res.succeded())
        ico = QIcon(":/ui/accept");
    else
        ico = QIcon(":/ui/exclamation");

    QTableWidgetItem* it = new QTableWidgetItem(ico, res.movie());
    ui.tableWidget->setItem(row, 0, it);

    it = new QTableWidgetItem(res.npRes());
    ui.tableWidget->setItem(row, 1, it);

    it = new QTableWidgetItem(res.osRes());
    ui.tableWidget->setItem(row, 2, it);
}

int frmSummary::exec()
{
    ui.tableWidget->resizeColumnsToContents();
    ui.tableWidget->sortByColumn(0);
    return QDialog::exec();
}
