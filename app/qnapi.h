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

#ifndef __QNAPI__H__
#define __QNAPI__H__

#include <QApplication>
#include "qnapilanguage.h"

// globalny menedzer pobierania napisow
class QNapi : public QApplication
{
	public:
        QNapi(int argc, char **argv);
        virtual ~QNapi();

        bool checkAll();
        int exec();
        void showSettings();
        void enqueue(const QString &movie);

        static QPair<bool, QString> bazinga(const QString& movie);

        static QNapiLanguage lang()
        { return m_lang; }
private:
        // lista sciezek do filmow
        QList<QString> m_movies;
        // jezyk napisow
        static QNapiLanguage m_lang;
};

#endif // __QNAPI__H__
