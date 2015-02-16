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

#include "wifi.h"
#include "device.h"
#include <QTimer>

Wifi::Wifi(bool useSlots, QObject *parent):
    WirelessInterface(QNetworkConfiguration::BearerWLAN, parent)
{
    if (useSlots) {
        initSlots();
    }
}

void Wifi::activate()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    WirelessInterface::activate();
    m_mut.lock();
    m_nmgr->updateConfigurations();

    wait(&m_mut, SECONDS(30));

    m_mut.unlock();
    if (!WirelessInterface::status(Connected)) {
        inactivate();
    }
}

void Wifi::breakScan()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    m_mut.lock();
    wakeAll();
    m_mut.unlock();
}

bool Wifi::status(const WirelessInterface::InterfaceProperties &prop)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    const Wifi wifi(false);
    return wifi.WirelessInterface::status(prop);
}

/* SLOTS */
void Wifi::onlineStateChanged()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (WirelessInterface::status(Connected)) {
        breakScan();
    } else {
        //start a timer before inactivate() because it might be temporary
        QTimer::singleShot(SECONDS(10), this, SLOT(onlineStateChangedToNotRunning()));
    }
}

void Wifi::onlineStateChangedToNotRunning()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (!WirelessInterface::status(Connected)) {
        inactivate();
    }
}
