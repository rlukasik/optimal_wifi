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
// void write(QJsonObject &) const;
// }

#ifndef DATABASE_H
#define DATABASE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QFile>
#include <QString>

template <class T>
class Database
{
public:
    explicit Database(QList<T> *&, const QString &, const QString &name = "elements");

    ~Database();

private:
    void read(const QJsonObject &);

    void write(QJsonObject &) const;

    void load();

    void save() const;

    QList<T> m_elements;

    QString m_fileName;

    QString m_name;
};

template<class T>
Database<T>::Database(QList<T> *&elements, const QString &fileName, const QString &name):
    m_fileName(fileName),
    m_name(name)
{
    elements = &m_elements;
    load();
}

template<class T>
Database<T>::~Database()
{
    save();
}

template<class T>
void Database<T>::read(const QJsonObject &json)
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__ << json;
    m_elements.clear();
    QJsonArray aArray = json[m_name].toArray();
    for (int i = 0; i < aArray.size(); ++i) {
        m_elements.append(T(aArray[i].toObject()));
    }
}

template<class T>
void Database<T>::write(QJsonObject &json) const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    QJsonArray aArray;
    foreach (const T t, m_elements) {
        QJsonObject aObject;
        t.write(aObject);
        aArray.append(aObject);
    }
    json[m_name] = aArray;
}

template<class T>
void Database<T>::load()
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    QFile loadFile(m_fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << QTime::currentTime().toString() << "Couldn't open" << m_fileName << "file.";
        return;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    read(loadDoc.object());
}

template<class T>
void Database<T>::save() const
{
    qDebug() << QTime::currentTime().toString() << __FUNCTION__;
    QFile saveFile(m_fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << QTime::currentTime().toString() << "Couldn't open" << m_fileName << "file.";
        return;
    }
    qDebug() << QTime::currentTime().toString() << "Database::save()";
    QJsonObject lObject;
    write(lObject);
    QJsonDocument saveDoc(lObject);
    saveFile.write(saveDoc.toJson());
}

#endif // DATABASE_H
