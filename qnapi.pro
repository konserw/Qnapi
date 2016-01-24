TEMPLATE = app

DEFINES += VERSION=1.7
#DEFINES += EMBED_PYTHON

QT += network xml widgets concurrent #core

CONFIG += qt c++11 warn_on release

DEFINES += NO_TRANSLATION NO_CONVERT
#DEFINES += SCRIPT_DEBUG

#QMAKE_CXXFLAGS += -mno-sse

#python
#contains(DEFINES, EMBED_PYTHON){
#    INCLUDEPATH += /usr/include/python2.7
#    LIBS += -lpthread -ldl -lutil -lm -lpython2.7
#}
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


INSTALLS =  \
    target \
    doc \
    man \
    icons \
    desktop \
