#ifndef FILEHISTORY_H
#define FILEHISTORY_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QSettings>

struct HistoryEntry {
    QString filePath;
    QString fileName;
    QString level;
    QDateTime timestamp;
    QString processType; // "Obfuscation" or "Encryption"
};

class FileHistory {
public:
    static void addEntry(const QString &filePath, const QString &level, const QString &processType);
    static QList<HistoryEntry> getEntries(int limit = -1);
    static void clear();
    
private:
    static const QString SETTINGS_KEY;
    static const int MAX_HISTORY_ENTRIES = 50;
};

#endif // FILEHISTORY_H 