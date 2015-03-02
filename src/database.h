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

// JSON database template
//
// class T
// {
// public:
// T(const QJsonObject &json);
// bool write(QJsonObject &) const;
// }

#ifndef DATABASE_H
#define DATABASE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QFile>
#include <QString>
#include <QTime>
#include <QHash>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <memory>

class DatabaseSingleton
{
public:
    static std::shared_ptr<DatabaseSingleton> Instance(const QString &);

    void save(const QString &, const QJsonValue &);

    bool load(const QString &, QJsonValue &) const;

    ~DatabaseSingleton();

protected:
    explicit DatabaseSingleton(const QString &);

private:
    typedef QHash<QString, std::shared_ptr<DatabaseSingleton>> DHash_t;

    static const QString m_JSON;

    static QMutex m_smut;

    const QString m_name;

    QJsonObject m_db;

    static DHash_t m_bases;
};

inline void DatabaseSingleton::save(const QString &key, const QJsonValue &json)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << key;
    m_db.insert(key, json);
}

inline bool DatabaseSingleton::load(const QString &key, QJsonValue &json) const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << key;
    if (m_db[key].isUndefined()) {
        return false;
    }
    json = m_db[key];
    return true;
}

template <class T>
class Database
{
public:
    explicit Database(QList<T> *&, const QString &, const QString &name = "elements");

    explicit Database(T *&, const QString &, const QString &name = "element");

    ~Database();

private:
    void read(const QJsonValue &);

    void write(QJsonValue &) const;

    void load();

    void save() const;

    T m_element;

    QList<T> m_elements;

    QString m_dbName;

    QString m_elementName;

    bool m_areMany;

    std::shared_ptr<DatabaseSingleton> m_db;
};

template<class T>
Database<T>::Database(QList<T> *&elements, const QString &dbName, const QString &elementName):
    m_dbName(dbName),
    m_elementName(elementName),
    m_areMany(true)
{
    m_db = DatabaseSingleton::Instance(m_dbName);
    elements = &m_elements;
    load();
}

template<class T>
Database<T>::Database(T *&element, const QString &dbName, const QString &elementName):
    m_dbName(dbName),
    m_elementName(elementName),
    m_areMany(false)
{
    m_db = DatabaseSingleton::Instance(m_dbName);
    element = &m_element;
    load();
}

template<class T>
Database<T>::~Database()
{
    save();
}

template<class T>
void Database<T>::read(const QJsonValue &json)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << json;
    if (m_areMany) {
        m_elements.clear();
        QJsonArray aArray = json.toArray();
        for (int i = 0; i < aArray.size(); ++i) {
            m_elements.append(T(aArray[i].toObject()));
        }
    } else {
        m_element = T(json.toObject());
    }
}

template<class T>
void Database<T>::write(QJsonValue &json) const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    if (m_areMany) {
        QJsonArray aArray;
        foreach (const T t, m_elements) {
            QJsonObject aObject;
            if (t.write(aObject)) {
                aArray.append(aObject);
            }
        }
        json = aArray;
    } else {
        QJsonObject aObject;
        if (m_element.write(aObject)) {
            json = aObject;
        }
    }
}

template<class T>
void Database<T>::load()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    QJsonValue lValue;
    if (m_db->load(m_elementName, lValue)) {
        read(lValue);
    }
}

template<class T>
void Database<T>::save() const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    QJsonValue lValue;
    write(lValue);
    m_db->save(m_elementName, lValue);
}

#endif // DATABASE_H
