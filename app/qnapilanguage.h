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

#ifndef __QNAPILANGUAGE__H__
#define __QNAPILANGUAGE__H__

#include <QString>
#include <QStringList>

class QNapiLanguage
{
public:
    QNapiLanguage();
    QNapiLanguage(QString source);
    QNapiLanguage operator=(const QNapiLanguage& other);

	void setLanguage(QString source);
    bool isValid() const;

    QString toTwoLetter() const;
    QString toTriLetter() const;
    QString toFullName() const;

    QStringList listLanguages() const;

private:
    static const QStringList codes2l, codes3l, names;
    int m_idx;
};

#endif
