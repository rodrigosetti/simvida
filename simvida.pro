######################################################################
# Automatically generated by qmake (2.01a) qua ago 13 22:37:05 2008
######################################################################

# conf.pri
PREFIX = /usr/local
BINDIR = /usr/local/bin
DATADIR = /usr/local/share

TEMPLATE = app
DEPENDPATH += src src/core graphs src/gui src/gui/forms
INCLUDEPATH += src src/gui src/core
INSTALLS = target simvida_pt_BR.qm

MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = src/gui/ui

QT += xml

# Input
HEADERS += src/core/biota.h \
           src/core/definicoes.h \
           src/core/mundo.h \
           src/core/vetor.h \
           src/gui/ajuda.h \
           src/gui/grafico.h \
           src/gui/info.h \
           src/gui/main-window.h \
           src/gui/mundoqt.h \
           src/gui/opcoes.h \
           src/gui/stat.h
FORMS += src/gui/forms/ajuda.ui \
         src/gui/forms/info.ui \
         src/gui/forms/main-window.ui \
         src/gui/forms/opcoes.ui \
         src/gui/forms/stat.ui
SOURCES += src/main.cpp \
           src/core/biota.cpp \
           src/core/mundo.cpp \
           src/gui/ajuda.cpp \
           src/gui/grafico.cpp \
           src/gui/info.cpp \
           src/gui/main-window.cpp \
           src/gui/mundoqt.cpp \
           src/gui/opcoes.cpp \
           src/gui/stat.cpp
RESOURCES += graphs/recursos.qrc

RC_FILE = graphs/winicon.rc

TRANSLATIONS += simvida_pt_BR.ts
