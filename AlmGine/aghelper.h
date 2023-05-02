#pragma once

#include <QString>
#include <QUrl>
#include <QSharedPointer>
#include <QIODevice>

class AGHelper
{
public:
    static QSharedPointer<QIODevice> getStreamForFile(const QString &inPath, QString *outPath = nullptr);
};
