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

#ifndef DEVICE_H
#define DEVICE_H

#include <QThread>
#include "wifi.h"
#include "gsm.h"
#include "location.h"

#define SECONDS(x) (x)*1000
#define MINUTES(x) SECONDS(x)*60

class Device : public QThread, public QWaitCondition
{
    Q_OBJECT
    Q_PROPERTY(uint status READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(uint interval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(QString wname READ getWname NOTIFY wnameChanged)

public:
    explicit Device(QObject *parent = 0);

    ~Device();

    void setStatus(const uint &);

    void setInterval(const uint &);

    uint getStatus() const { return m_active; }

    uint getInterval() const { return m_interval; }

    QString getWname() const { return m_wifi->name(); }

signals:
    void statusChanged(uint);

    void intervalChanged(uint);

    void wnameChanged(QString);

    void wifiStateChangedToRunning();

public slots:
    void areaEntered();

    void wifiNotRunning();

protected:
    void run();

private:
    void changeParam();

    uint m_active;

    uint m_interval;

    bool m_abort;

    Wifi *m_wifi;

    Gsm *m_gsm;

    Location *m_loc;

    QMutex m_mut;
};

#endif // DEVICE_H
