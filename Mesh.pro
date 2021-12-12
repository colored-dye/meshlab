QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    C:/Users/baoyu/Documents/opengl/glm \
    ./vcglib \
    ./vcglib/eigenlib

LIBS += \
    opengl32.lib

SOURCES += \
    MyOpenGLWidget.cpp \
    io/export_all_types.cpp \
    io/import_all_types.cpp \
    main.cpp \
    mainwindow.cpp \
    ./vcglib/wrap/ply/plylib.cpp

HEADERS += \
    Camera.h \
    Mesh.h \
    MyOpenGLWidget.h \
    Shader.h \
    ./io/import_ply.h \
    ./io/io_modeltype.h \
    io/MyVCGMesh.h \
    io/export_all_types.h \
    io/import_all_types.h \
    mainwindow.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-Mesh-Desktop_Qt_5_12_11_MSVC2017_64bit-Debug/debug/shaders/fragEdge.txt \
    ../build-Mesh-Desktop_Qt_5_12_11_MSVC2017_64bit-Debug/debug/shaders/fragFace.txt \
    ../build-Mesh-Desktop_Qt_5_12_11_MSVC2017_64bit-Debug/debug/shaders/fragLighting.txt \
    ../build-Mesh-Desktop_Qt_5_12_11_MSVC2017_64bit-Debug/debug/shaders/fragVert.txt \
    ../build-Mesh-Desktop_Qt_5_12_11_MSVC2017_64bit-Debug/debug/shaders/vert.txt \
    README \

FORMS += \
    mainwindow.ui
