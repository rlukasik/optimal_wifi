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

#include "location.h"
#include "device.h"
#include "config.h"

Location::Location(QObject *parent) :
    QObject(parent),
    m_source(NULL),
    m_database(NULL),
    m_areas(NULL),
    m_timer(NULL),
    m_error(false)
{
    m_source = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_source) {
        m_database = new Database<Area>(m_areas, DATABASE, JSON_AREAS);
        m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
        m_source->setUpdateInterval(SECONDS(10));
        connect(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)), SLOT(positionUpdated(QGeoPositionInfo)));
        connect(dynamic_cast<Device*>(parent), SIGNAL(wifiStateChangedToRunning()), SLOT(wifiStateChangedToRunning()));
        m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
        m_source->startUpdates();
    }
}

Location::~Location()
{
    delete m_database;
    delete m_timer;
}

/* SLOTS */
void Location::positionUpdated(const QGeoPositionInfo &update)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
/*WORKAROUND: reject first position*/
    static int counter = 0;
    if (!counter) {
        ++counter;
        m_error = true;
        return;
    }
/*WORKAROUND*/
    m_mut.lock();
    m_timer->stop();
    m_error = false;
    QGeoCoordinate uc = update.coordinate();
    qDebug() << QTime::currentTime().toString() << "Current position is" << uc.toString();
    m_currentArea = Area(uc);
    int i = m_areas->indexOf(m_currentArea);
    if (i > -1) {
        if (Wifi::status(WirelessInterface::Connected)) {
            m_areas->move(i, m_areas->size()-1);
        } else {
            emit areaEntered();
        }
    } else if (Wifi::status(WirelessInterface::Connected)) {
        qDebug() << QTime::currentTime().toString() << "Current possition added to the list";
        m_areas->append(m_currentArea);
        if (BUFFER_SIZE < m_areas->size()) {
            m_areas->removeFirst();
        }
    }
    //start Timer
    m_timer->start(SECONDS(31));
    m_mut.unlock();
}

void Location::wifiStateChangedToRunning()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    m_mut.lock();
    if (m_currentArea.coordinate().isValid() && m_areas && !m_error) {
        int i = m_areas->indexOf(m_currentArea);
        if (i > -1) {
            m_areas->move(i, m_areas->size()-1);
        } else {
            qDebug() << QTime::currentTime().toString() << "Current possition added to the list";
            m_areas->append(m_currentArea);
            if (BUFFER_SIZE < m_areas->size()) {
                m_areas->removeFirst();
            }
        }
    }
    m_mut.unlock();
}

void Location::timeout()
{
    m_mut.lock();
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    m_error = true;
    m_mut.unlock();
}
