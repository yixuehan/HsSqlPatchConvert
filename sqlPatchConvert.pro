#-------------------------------------------------
#
# Project created by QtCreator 2017-01-13T14:03:05
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++1y

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sqlPatchConvert
TEMPLATE = app


SOURCES += sqlbusiness.cpp \
    sqlconfig.cpp \
    sqlerror.cpp \
    sqltable.cpp \
    sqlpatch.cpp \
    pubfunc.cpp \
    main.cpp \
    mainwindow.cpp
    
    

HEADERS  += mainwindow.h \
    pubfunc.h \
    sqlpatch.h \
    sqlerror.h \
    sqltable.h \
    sqlconfig.h \
    sqlbusiness.h

FORMS    += mainwindow.ui \
    fieldchioce.ui

INCLUDEPATH += "D:/boost_1_62_0/include/boost-1_62"

CONFIG(debug, debug|release) {
#LIBS += "D:/boost_1_62_0/lib/debug/boost_filesystem-vc140-mt-gd-1_62.dll"
LIBS += "D:/boost_1_62_0/lib/debug/libboost_locale-vc140-mt-gd-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/debug/libboost_regex-vc140-mt-gd-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/debug/libboost_date_time-vc140-mt-gd-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/debug/libboost_filesystem-vc140-mt-gd-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/debug/libboost_system-vc140-mt-gd-1_62.lib"
dst_dir = $$OUT_PWD/debug/conf/
} else {
LIBS += "D:/boost_1_62_0/lib/release/libboost_locale-vc140-mt-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/release/libboost_regex-vc140-mt-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/release/libboost_filesystem-vc140-mt-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/release/libboost_system-vc140-mt-1_62.lib"
LIBS += "D:/boost_1_62_0/lib/release/libboost_date_time-vc140-mt-1_62.lib"
dst_dir = $$OUT_PWD/release/conf
}

src_dir = $$PWD/conf
src_dir ~= s,/,\\,g
dst_dir ~= s,/,\\,g

!exists($$dst_dir){
   system(xcopy $$src_dir $$dst_dir /Y /E /I)
}


