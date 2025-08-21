#ifndef CIRCUITSERIALIZER_H
#define CIRCUITSERIALIZER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>

class CircuitSerializer {
public:
    static bool saveToFile(const QString &filename, const QJsonObject &circuitData) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) return false;
        QJsonDocument doc(circuitData);
        file.write(doc.toJson());
        return true;
    }

    static QJsonObject loadFromFile(const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return {};
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        return doc.object();
    }
};

#endif
