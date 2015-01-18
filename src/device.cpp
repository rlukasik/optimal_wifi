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

#include "device.h"

Device::Device(QObject *parent) :
    QThread(parent),
    m_active(0),
    m_interval(15),
    m_abort(false)
{
    m_wifi = new Wifi(this);
    m_gsm = new Gsm(this);
    m_loc = new Location(this);

    connect(m_loc, SIGNAL(areaEntered()), SLOT(areaEntered()));

    if (!isRunning()) {
        start(LowPriority);
    }
}

Device::~Device()
{    
    m_wifi->breakScan();
    m_mut.lock();
    m_abort = true;
    wakeAll();
    m_mut.unlock();
    QThread::wait();

    m_gsm->activate();

    delete m_wifi;
    delete m_gsm;
    delete m_loc;
}

void Device::changeParam()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    m_wifi->breakScan();
    m_mut.lock();
    wakeAll();
    m_mut.unlock();
}

void Device::setStatus(const uint& status)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << status;
    if (status != m_active) {
        m_active = status;
        changeParam();
        emit statusChanged(status);
    }

}

void Device::setInterval(const uint& interval)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << interval;
    if ((interval != 0) && (interval != m_interval)) {
        m_interval = interval;
        changeParam();
    }
    emit intervalChanged(m_interval);
}

void Device::run()
{
    forever {
        m_mut.lock();
        if (m_abort) {
            m_mut.unlock();
            return;
        }
        if (m_active &&
           (!m_wifi->status(WirelessInterface::Connected))) {
            m_gsm->inactivate();
            m_wifi->activate();
            if (m_wifi->status(WirelessInterface::Connected)) {
                emit wifiStateChangedToRunning();
            } else {
                m_gsm->activate();
            }
        }
        QWaitCondition::wait(&m_mut, MINUTES(m_interval));
        m_mut.unlock();
    }
}

/* SLOTS */
void Device::areaEntered()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    m_wifi->breakScan();
    m_mut.lock();
    wakeAll();
    m_mut.unlock();
}
