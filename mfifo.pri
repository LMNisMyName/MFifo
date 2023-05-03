INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#QT += core gui gui-private
QT += core gui
!build_mfifo_lib:DEFINES += MFIFO_NO_LIB

HEADERS += $$PWD/mfifostats.h \
    $$PWD/mfifostatsui.h \
    $$PWD/mfifo.hpp

SOURCES += $$PWD/mfifostats.cpp \
    $$PWD/mfifostatsui.cpp

FORMS    += $$PWD/mfifostatsui.ui

