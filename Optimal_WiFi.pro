# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = Optimal_WiFi

CONFIG += sailfishapp

QT += core dbus positioning

CONFIG += c++11

SOURCES += src/Optimal_WiFi.cpp \
    src/device.cpp \
    src/wifi.cpp \
    src/gsm.cpp \
    src/wirelessinterface.cpp \
    src/location.cpp

OTHER_FILES += qml/Optimal_WiFi.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/Optimal_WiFi.changes.in \
    rpm/Optimal_WiFi.spec \
    rpm/Optimal_WiFi.yaml \
    translations/*.ts \
    Optimal_WiFi.desktop

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/Optimal_WiFi-de.ts

HEADERS += \
    src/device.h \
    src/wifi.h \
    src/gsm.h \
    src/wirelessinterface.h \
    src/location.h \
    src/database.h

