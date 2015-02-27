/*
  Copyright (c) 2015, rlukasik
  Contact: Rafal Lukasik <rafal.lukasik@runbox.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the optimal_wifi nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "wirelessinterface.h"
#include <QMap>

const QMap<QString, QVariant> &operator<<(QMap<QString, QVariant> &map, const QDBusArgument &arg)
{
    arg.beginMap();
    map.clear();

    while ( !arg.atEnd() ) {
        QString key;
        QVariant value;
        arg.beginMapEntry();
        arg >> key >> value;
        arg.endMapEntry();
        map.insert( key, value );
    }
    arg.endMap();
    return map;
}

WirelessInterface::WirelessInterface(QNetworkConfiguration::BearerType bearer,
                                     QObject *parent) :
    QObject(parent),
    m_bearer(bearer),
    m_dif(NULL)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << bearer;
    m_dcon = new QDBusConnection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, "connman"));

    if (QNetworkConfiguration::BearerWLAN == m_bearer) {
        m_dif = new QDBusInterface("net.connman",
                                    "/net/connman/technology/wifi",
                                    "net.connman.Technology",
                                    *m_dcon,
                                    this);
    } else if (QNetworkConfiguration::BearerBluetooth == m_bearer) { //Bluetooth not implemented
    } else { /* GSM */
        m_dif = new QDBusInterface("net.connman",
                                    "/net/connman/technology/cellular",
                                    "net.connman.Technology",
                                    *m_dcon,
                                    this);
    }

    m_nmgr = new QNetworkConfigurationManager();
}

WirelessInterface::~WirelessInterface()
{
    delete m_dcon;
    delete m_dif;
    delete m_nmgr;
}

void WirelessInterface::activate()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (!status(Powered)) {
        onOff(true);
    }
}

void WirelessInterface::inactivate()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (status(Powered)) {
        onOff(false);
    }
}

void WirelessInterface::onOff(const bool &var)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << var;
    if (m_dif) {
        if (QNetworkConfiguration::BearerWLAN == m_bearer) {
            m_dif->call(QDBus::Block,
                            "SetProperty",
                            QString("Powered"),
                            QVariant::fromValue(QDBusVariant(var)));
        } else if (QNetworkConfiguration::BearerBluetooth == m_bearer) { //Bluetooth not implemented
        } else { /* GSM */
            m_dif->call(QDBus::Block,
                            "SetProperty",
                            QString("AutoConnect"),
                            QVariant::fromValue(QDBusVariant(var)));
        }
    }
}

bool WirelessInterface::status(const InterfaceProperties &prop) const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << prop;
    if (!m_dif) {
        qWarning() << QTime::currentTime().toString() << QTime::currentTime().toString() << "QDBusInterface m_dif is NULL";
        return false;
    }
    QDBusMessage msg = m_dif->call(QDBus::Block, "GetProperties");
    QList<QVariant> argList = msg.arguments();
    const QDBusArgument &dbusArgs = argList.first().value<QDBusArgument>();
    QMap<QString, QVariant> map;
    QString key;
    map << dbusArgs;
    switch (prop) {
        case Powered:
            key = POWERED;
            break;
        case Connected:
            key = CONNECTED;
            break;
        case Tethering:
            key = TETHERING;
            break;
        default:
            qWarning() << "Wrong key!!!";
            return false;
    }
    return map[key].toBool();
}

void WirelessInterface::initSlots()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    connect(m_nmgr, SIGNAL(updateCompleted()), SLOT(updateCompleted()));
    connect(m_nmgr, SIGNAL(onlineStateChanged(bool)), SLOT(onlineStateChanged()));
}

/* SLOTS */
void WirelessInterface::updateCompleted()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (status(Connected)) {
        m_mut.lock();
        wakeAll();
        m_mut.unlock();
    }
}

void WirelessInterface::onlineStateChanged()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (status(Connected)) {
        m_mut.lock();
        wakeAll();
        m_mut.unlock();
    }
}
