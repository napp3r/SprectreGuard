#include "filehistory.h"
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

const QString FileHistory::SETTINGS_KEY = "file_history";

void FileHistory::addEntry(const QString &filePath, const QString &level, const QString &processType) {
    QSettings settings;
    QByteArray data = settings.value(SETTINGS_KEY).toByteArray();
    
    QJsonArray historyArray;
    if (!data.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        historyArray = doc.array();
        qDebug() << "Loaded existing history with" << historyArray.size() << "entries";
    } else {
        qDebug() << "Creating new history";
    }
    
    // Create new entry
    QFileInfo fileInfo(filePath);
    QJsonObject entry;
    entry["filePath"] = filePath;
    entry["fileName"] = fileInfo.fileName();
    entry["level"] = level;
    entry["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    entry["processType"] = processType;
    
    // Add to front of array
    historyArray.prepend(entry);
    qDebug() << "Added entry for file:" << fileInfo.fileName() << "process:" << processType;
    
    // Trim to max size
    while (historyArray.size() > MAX_HISTORY_ENTRIES) {
        historyArray.removeLast();
    }
    
    // Save back to settings
    QJsonDocument doc(historyArray);
    settings.setValue(SETTINGS_KEY, doc.toJson(QJsonDocument::Compact));
    settings.sync(); // Ensure settings are written to disk immediately
    qDebug() << "Saved history with" << historyArray.size() << "entries";
}

QList<HistoryEntry> FileHistory::getEntries(int limit) {
    QSettings settings;
    QByteArray data = settings.value(SETTINGS_KEY).toByteArray();
    
    QList<HistoryEntry> entries;
    
    if (!data.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray historyArray = doc.array();
        qDebug() << "Loading" << historyArray.size() << "history entries";
        
        int count = 0;
        for (const QJsonValue &value : historyArray) {
            if (limit >= 0 && count >= limit) break;
            
            QJsonObject obj = value.toObject();
            HistoryEntry entry;
            entry.filePath = obj["filePath"].toString();
            entry.fileName = obj["fileName"].toString();
            entry.level = obj["level"].toString();
            entry.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
            entry.processType = obj["processType"].toString();
            
            entries.append(entry);
            count++;
        }
        qDebug() << "Loaded" << entries.size() << "history entries";
    } else {
        qDebug() << "No history data found";
    }
    
    return entries;
}

void FileHistory::clear() {
    QSettings settings;
    settings.remove(SETTINGS_KEY);
    settings.sync();
    qDebug() << "History has been cleared";
} 