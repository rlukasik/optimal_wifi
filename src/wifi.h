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

#ifndef WIFI_H
#define WIFI_H

#include <QNetworkSession>
#include <QTimer>
#include "wirelessinterface.h"

class Wifi: public WirelessInterface
{
    Q_OBJECT

public:
    explicit Wifi(bool useSlots = true, QObject *parent = 0);

    ~Wifi();

    void activate();

    void breakScan();

    static bool status(const WirelessInterface::InterfaceProperties &);

    QString name() const;

signals:
    void notRunning();

    void statsUpdate(quint64, quint64);

public slots:
    void onlineStateChanged();

    void onlineStateChangedToNotRunning();

    void stats();

private:
    void session();

    QNetworkSession *m_session;

    QTimer *m_stats;
};

#endif // WIFI_H
