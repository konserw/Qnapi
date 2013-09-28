TEMPLATE = app

QT += network xml widgets

CONFIG += qt release warn_on

#python
#INCLUDEPATH += /usr/include/python3.3m
#LIBS += -lpthread -ldl -lutil -lm -lpython3.3m
#QMAKE_CXXFLAGS += -fms-extensions -std=c++03

#qt http
INCLUDEPATH += qhttp
HEADERS += \
    qhttp/qhttp.h \
    qhttp/qringbuffer_p.h \
    qhttp/qhttpauthenticator_p.h
SOURCES += \
    qhttp/qhttp.cpp \
    qhttp/qhttpauthenticator.cpp

#qnapi
INCLUDEPATH += app

SOURCES += \
app/main.cpp \
app/qnapiconfig.cpp \
app/qnapiapp.cpp \
app/movieinfo.cpp \
app/qmultiparthttprequest.cpp \
app/qnapiabstractengine.cpp \
app/qnapiprojektengine.cpp \
app/qnapicli.cpp \
app/qnapiopendialog.cpp \
app/qnapi.cpp \
app/qopensubtitlesengine.cpp \
app/qnapilanguage.cpp \
xmlrpc/client.cpp \
xmlrpc/request.cpp \
xmlrpc/variant.cpp \
xmlrpc/response.cpp \
forms/frmprogress.cpp \
forms/frmoptions.cpp \
forms/frmupload.cpp \
forms/frmcorrect.cpp \
forms/frmreport.cpp \
forms/frmscan.cpp \
forms/frmcreateuser.cpp \
forms/frmsummary.cpp \
forms/frmnapiprojektconfig.cpp \
forms/frmlistsubtitles.cpp \
forms/frmopensubtitlesconfig.cpp \
qcumber/qinterprocesschannel.cpp \
qcumber/qmanagedrequest.cpp \
qcumber/qmanagedsocket.cpp \
qcumber/qsingleapplication.cpp

HEADERS += \
app/version.h \
app/qnapiconfig.h \
app/qnapiapp.h \
app/movieinfo.h \
app/qmultiparthttprequest.h \
app/qnapithread.h \
app/synchttp.h \
app/qnapiabstractengine.h \
app/qnapiprojektengine.h \
app/qnapicli.h \
app/qnapiopendialog.h \
app/qcheckedlistwidget.h \
app/qnapi.h \
app/qnapisubtitleinfo.h \
app/qopensubtitlesengine.h \
app/qnapilanguage.h \
xmlrpc/qsyncxmlrpcclient.h \
xmlrpc/variant.h \
xmlrpc/client.h \
xmlrpc/request.h \
xmlrpc/response.h \
forms/frmprogress.h \
forms/frmoptions.h \
forms/frmupload.h \
forms/frmcorrect.h \
forms/frmreport.h \
forms/frmscan.h \
forms/frmcreateuser.h \
forms/frmsummary.h \
forms/frmnapiprojektconfig.h \
forms/frmlistsubtitles.h \
forms/frmopensubtitlesconfig.h \
qcumber/qinterprocesschannel.h \
qcumber/qmanagedrequest.h \
qcumber/qmanagedsocket.h \
qcumber/qsingleapplication.h \
qcumber/qcumber.h 

FORMS += \
forms/frmprogress.ui \
forms/frmoptions.ui \
forms/frmupload.ui \
forms/frmcorrect.ui \
forms/frmreport.ui \
forms/frmscan.ui \
forms/frmcreateuser.ui \
forms/frmsummary.ui \
forms/frmnapiprojektconfig.ui \
forms/frmopensubtitlesconfig.ui \
forms/frmlistsubtitles.ui

RESOURCES += resources.qrc

##??????

 INSTALL_PREFIX =  /usr
 target.path =  $${INSTALL_PREFIX}/bin
 doc.path =  $${INSTALL_PREFIX}/share/doc/$${TARGET}
 doc.files =  doc/ChangeLog \
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
 INSTALLS =  target \
  doc \
  man \
  icons \
  desktop \
  dolphin_integration \
  d3lphin_integration \
  konqueror_integration \
  kde4_integration

