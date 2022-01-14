QT       += core gui axcontainer xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    editdialog.cpp \
    findfiledialog.cpp \
    main.cpp \
    mainwindow.cpp \
    mdparser.cpp \
    mdscanner.cpp \
    mdtoken.cpp \
    styledialog.cpp \
    texparser.cpp \
    worddocument.cpp \
    wordparser.cpp \
    wordxml.cpp

HEADERS += \
    editdialog.h \
    findfiledialog.h \
    mainwindow.h \
    mdparser.h \
    mdscanner.h \
    mdtoken.h \
    styledialog.h \
    texparser.h \
    worddocument.h \
    wordparser.h \
    wordxml.h

FORMS += \
    editdialog.ui \
    findfiledialog.ui \
    mainwindow.ui \
    styledialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    css.qrc
