QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    InCallDialog.cpp \
    IncomingCallDialog.cpp \
    SSPAccount.cpp \
    SSPCall.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CheckableComboBox.hpp \
    GuiHandler.hpp \
    InCallDialog.hpp \
    IncomingCallDialog.hpp \
    LogPopupWindow.hpp \
    MainWindow.hpp \
    ManualTestHandler.hpp \
    ManualTestOpcode.hpp \
    Message.hpp \
    PjManager.hpp \
    Receiver.hpp \
    ResultHandler.hpp \
    SSPAccount.hpp \
    SSPCall.hpp \
    Softphone.hpp \
    SoftphoneArguments.hpp \
    Transmitter.hpp \
    VTCPClient.hpp \
    VTCPManualStatus.hpp \
    VTCPOpcode.hpp

FORMS += \
    InCallDialog.ui \
    IncomingCallDialog.ui \
    MainWindow.ui

#INCLUDEPATH += -l~/Documents/Libraries/pjproject-2.13/pjsip/lib/

#LIBS += -I~/Documents/Libraries/pjproject-2.13/pjsip/include -I/Documents/Libraries/pjproject-2.13/pjlib/include -I~/Documents/Libraries/pjproject-2.13/pjlib-util/include -I~/Documents/Libraries/pjproject-2.13/pjmedia/include -I~/Documents/Libraries/pjproject-2.13/pjnath/include

#DEPENDPATH += ~/Documents/Libraries/pjproject-2.13/pjsip/include

INCLUDEPATH += ~/Documents/Libraries/pjproject-2.13/pjsip/include
LIBS += -L~/Documents/Libraries/pjproject-2.13/pjsip/lib -lpjsua2


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
