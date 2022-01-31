QT += widgets
QT += core gui
QT -= gui
QT += xml
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
    exit.h \
    highlighter.h
SOURCES       = main.cpp \
                exit.cpp \
                highlighter.cpp \
                mainwindow.cpp
#! [0]
RESOURCES     = application.qrc
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/application
INSTALLS += target

DISTFILES += \
    xml-file.xml
