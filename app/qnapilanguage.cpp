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

#include "qnapilanguage.h"

QNapiLanguage::QNapiLanguage(QString source) : QObject(0)
{
	fill_tables();

	setLanguage(source);
}

void QNapiLanguage::setLanguage(QString source)
{
	idx = -1;

	if(source.length() == 2)
	{
		idx = codes2l.indexOf(source.toLower());
	}
	else if(source.length() == 3)
	{
		idx = codes3l.indexOf(source.toLower());
	}
	else
	{
		idx = names.indexOf(source);
	}	
}

QString QNapiLanguage::toTwoLetter()
{
	if(idx > -1)
	{
		return codes2l.at(idx);
	}
	return "";
}

QString QNapiLanguage::toTriLetter()
{
	if(idx > -1)
	{
		return codes3l.at(idx);
	}
	return "";
}

QString QNapiLanguage::toFullName()
{
	if(idx > -1)
	{
		return names.at(idx);
	}
	return "";
}
	
QStringList QNapiLanguage::listLanguages()
{
	return names;
}

void QNapiLanguage::fill_tables()
{

	codes2l << "en";
	codes3l << "eng";
	names << tr("Angielski");

	codes2l << "pl";
	codes3l << "pol";
	names << tr("Polski");
}
