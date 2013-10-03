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
#include <QObject>
#include "qnapilanguage.h"

const QStringList QNapiLanguage::codes2l = { "en",  "pl" };
const QStringList QNapiLanguage::codes3l = { "eng", "pol"};
const QStringList QNapiLanguage::names = {QObject::tr("Angielski"), QObject::tr("Polski")};

QNapiLanguage::QNapiLanguage()
{
    m_idx = -1;
}

QNapiLanguage::QNapiLanguage(QString source)
{
    setLanguage(source);
}

QNapiLanguage QNapiLanguage::operator=(const QNapiLanguage &other)
{
    m_idx = other.m_idx;
    return *this;
}

void QNapiLanguage::setLanguage(QString source)
{
    m_idx = -1;

	if(source.length() == 2)
	{
        m_idx = codes2l.indexOf(source.toLower());
	}
	else if(source.length() == 3)
	{
        m_idx = codes3l.indexOf(source.toLower());
	}
	else
	{
        m_idx = names.indexOf(source);
    }
}

bool QNapiLanguage::isValid() const
{
    return m_idx > -1;
}

QString QNapiLanguage::toTwoLetter() const
{
    if(m_idx > -1)
	{
        return codes2l.at(m_idx);
	}
	return "";
}

QString QNapiLanguage::toTriLetter() const
{
    if(m_idx > -1)
	{
        return codes3l.at(m_idx);
	}
	return "";
}

QString QNapiLanguage::toFullName() const
{
    if(m_idx > -1)
	{
        return names.at(m_idx);
	}
	return "";
}
	
QStringList QNapiLanguage::listLanguages() const
{
	return names;
}
