#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "aghelper.h"

QSharedPointer<QIODevice> AGHelper::getStreamForFile(const QString &inPath, QString *outPath)
{
    QFile *file = nullptr;
    QString tryPath = inPath.startsWith(QLatin1String("qrc:/")) ? inPath.mid(3) : inPath;
    QFileInfo fi(tryPath);
    bool found = fi.exists();
    if (!found && fi.isNativePath()) {
        tryPath.prepend(QLatin1String(":/"));
        fi.setFile(tryPath);
        found = fi.exists();
    }
    if (found) {
        QString filePath = fi.canonicalFilePath();
        file = new QFile(filePath);
        if (file->open(QIODevice::ReadOnly)) {
            if (outPath)
                *outPath = filePath;
        } else {
            delete file;
            file = nullptr;
        }
    }
    return QSharedPointer<QIODevice>(file);
}
