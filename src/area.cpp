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

Location::Area::Area()
{

}

Location::Area::Area(const QGeoCoordinate &coordinate) :
    m_c(coordinate)
{

}

Location::Area::Area(const double &latitude, const double &longitude) :
    m_c(latitude, longitude)
{

}

Location::Area::Area(const QJsonObject &json)
{
    read(json);
}

Location::Area::~Area()
{

}

void Location::Area::read(const QJsonObject &json)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << json;
    m_c.setLatitude(json[JSON_LATITUDE].toDouble());
    m_c.setLongitude(json[JSON_LONGITUDE].toDouble());
}

void Location::Area::write(QJsonObject &json) const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    json[JSON_LATITUDE] = m_c.latitude();
    json[JSON_LONGITUDE] = m_c.longitude();
}
