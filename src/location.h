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

#ifndef LOCATION_H
#define LOCATION_H

#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QGeoCoordinate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include "database.h"

#define RADIUS 100 //meters

#define BUFFER_SIZE 100

#define JSON_LOCATION_FILE "location.json"

#define JSON_AREAS      "areas"
#define JSON_LATITUDE   "latitude"
#define JSON_LONGITUDE  "longitude"

class Location : public QObject
{
    Q_OBJECT

public:
    explicit Location(QObject *parent = 0);

    ~Location();

signals:
    void areaEntered();

public slots:
    void positionUpdated(const QGeoPositionInfo &);

    void wifiStateChangedToRunning();

    void timeout();

private:
    class Area
    {
    public:
        explicit Area();

        explicit Area(const QGeoCoordinate &);

        explicit Area(const double &, const double &);

        explicit Area(const QJsonObject &);

        ~Area();

        bool operator ==(const Area &a) const {return RADIUS > m_c.distanceTo(a.m_c);}

        bool operator ==(const QGeoCoordinate &c) const {return RADIUS > m_c.distanceTo(c);}

        friend bool operator ==(const QGeoCoordinate &, const Location::Area &);

        bool operator !=(const Area &a) const {return !(*this == a);}

        bool operator !=(const QGeoCoordinate &c) const {return !(*this == c);}

        friend bool operator !=(const QGeoCoordinate &, const Location::Area &);

        void read(const QJsonObject &);

        void write(QJsonObject &) const;

        void setCoordinate(const QGeoCoordinate &c) {m_c = c;}

        void getCoordinate(QGeoCoordinate &c) const {c = m_c;}

        QGeoCoordinate coordinate() const {return m_c;}

    private:
        QGeoCoordinate m_c;
    };

    friend bool operator ==(const QGeoCoordinate &, const Location::Area &);

    friend bool operator !=(const QGeoCoordinate &, const Location::Area &);

    QGeoPositionInfoSource *m_source;

    Area m_currentArea;

    Database<Area> *m_database;

    QList<Area> *m_areas;

    QMutex m_mut;

    QTimer *m_timer;

    bool m_error;
};

inline bool operator ==(const QGeoCoordinate &c, const Location::Area &a)
{
    return RADIUS > a.m_c.distanceTo(c);
}

inline bool operator !=(const QGeoCoordinate &c, const Location::Area &a)
{
    return !(c == a);
}

#endif // LOCATION_H
