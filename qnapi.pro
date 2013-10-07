TEMPLATE = app

DEFINES += VERSION=1.7

QT += network xml widgets core  concurrent

CONFIG += qt c++11 release #warn_on

#python
INCLUDEPATH += /usr/include/python2.7
LIBS += -lpthread -ldl -lutil -lm -lpython2.7

#qt http
INCLUDEPATH += qhttp
HEADERS += \
    qhttp/qhttp.h \
    qhttp/qringbuffer_p.h \
    qhttp/qhttpauthenticator_p.h \
    app/QNapiSubtitleInfoList.h \
    app/QNapiResult.h
SOURCES += \
    qhttp/qhttp.cpp \
    qhttp/qhttpauthenticator.cpp \
    app/qnapisubtitleinfo.cpp \
    app/QNapiSubtitleInfoList.cpp \
    app/QNapiResult.cpp

#qnapi
INCLUDEPATH += app

SOURCES += \
    app/main.cpp \
    app/qnapiconfig.cpp \
    app/movieinfo.cpp \
    app/qmultiparthttprequest.cpp \
    app/qnapiabstractengine.cpp \
    app/qnapiprojektengine.cpp \
    #app/qnapicli.cpp \
    app/qnapi.cpp \
    app/qopensubtitlesengine.cpp \
    app/qnapilanguage.cpp \
    xmlrpc/client.cpp \
    xmlrpc/request.cpp \
    xmlrpc/variant.cpp \
    xmlrpc/response.cpp \
    forms/frmoptions.cpp \
    forms/frmsummary.cpp

HEADERS += \
    app/qnapiconfig.h \
    app/movieinfo.h \
    app/qmultiparthttprequest.h \
    app/synchttp.h \
    app/qnapiabstractengine.h \
    app/qnapiprojektengine.h \
    #app/qnapicli.h \
    app/qnapi.h \
    app/qnapisubtitleinfo.h \
    app/qopensubtitlesengine.h \
    app/qnapilanguage.h \
    xmlrpc/qsyncxmlrpcclient.h \
    xmlrpc/variant.h \
    xmlrpc/client.h \
    xmlrpc/request.h \
    xmlrpc/response.h \
    forms/frmoptions.h \
    forms/frmsummary.h

FORMS += \
forms/frmoptions.ui \
forms/frmsummary.ui

RESOURCES += resources.qrc

#Instalation

INSTALL_PREFIX =  /usr
target.path =  $${INSTALL_PREFIX}/bin
doc.path =  $${INSTALL_PREFIX}/share/doc/$${TARGET}
doc.files =  \
    doc/ChangeLog \
    doc/changelog.gz \
    doc/README \
    doc/README.pl \
    doc/LICENSE \
    doc/LICENSE.pl \
    doc/COPYRIGHT \
    doc/qnapi-download.desktop \
    doc/qnapi-download.schemas
man.path =  $${INSTALL_PREFIX}/share/man/man1
man.files =  doc/$${TARGET}.1.gz
icons.path =  /usr/share/icons
icons.files =  res/qnapi.png  res/qnapi-48.png  res/qnapi-128.png  res/qnapi-512.png
desktop.path =  /usr/share/applications
desktop.files =  doc/$${TARGET}.desktop
dolphin_integration.path =  /usr/share/apps/dolphin/servicemenus
dolphin_integration.files =  doc/$${TARGET}-download.desktop
d3lphin_integration.path =  /usr/share/apps/d3lphin/servicemenus
d3lphin_integration.files =  doc/$${TARGET}-download.desktop
konqueror_integration.path =  /usr/share/apps/konqueror/servicemenus
konqueror_integration.files =  doc/$${TARGET}-download.desktop
kde4_integration.path =  /usr/lib/kde4/share/kde4/services/ServiceMenus
kde4_integration.files =  doc/$${TARGET}-download.desktop
INSTALLS =  \
    target \
    doc \
    man \
    icons \
    desktop \
    dolphin_integration \
    d3lphin_integration \
    konqueror_integration \
    kde4_integration
