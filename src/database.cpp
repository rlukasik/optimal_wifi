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

#include "database.h"

const QString DatabaseSingleton::m_JSON = ".json";
QMutex DatabaseSingleton::m_smut;
DatabaseSingleton::DHash_t DatabaseSingleton::m_bases;

std::shared_ptr<DatabaseSingleton> DatabaseSingleton::Instance(const QString &name)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << name;
    //Double-Checked Locking Pattern
    //http://www.drdobbs.com/cpp/c-and-the-perils-of-double-checked-locki/184405726
    DHash_t::iterator i = m_bases.find(name);
    if (m_bases.end() == i) {
        QMutexLocker locker(&m_smut);
        i = m_bases.find(name);
        if (m_bases.end() == i) {
            std::shared_ptr<DatabaseSingleton> base(new DatabaseSingleton(name));
            m_bases[name] = base;
            return base;
        }
    }
    return *i;
}

DatabaseSingleton::DatabaseSingleton(const QString &name) :
    m_name(name)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << m_name;
    QFile loadFile(m_name + m_JSON);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << QTime::currentTime().toString() << "Couldn't open" << m_name + m_JSON << "file.";
        return;
    }
    QByteArray loadData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
    m_db = loadDoc.object();
}

DatabaseSingleton::~DatabaseSingleton()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << m_name;
    QFile saveFile(m_name + m_JSON);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << QTime::currentTime().toString() << "Couldn't open" << m_name + m_JSON << "file.";
        return;
    }
    QJsonDocument saveDoc(m_db);
    saveFile.write(saveDoc.toJson());
}
