#include "settings.h"
#include <QDir>
#include <QStandardPaths>

void Settings::loadSettings() {
    if (settings.value("output_directory").toString().isEmpty()) {
        // По умолчанию используем папку Documents/SpectreGuard
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) 
                            + "/SpectreGuard";
        settings.setValue("output_directory", defaultPath);
    }
    
    // Создаем необходимые директории
    QDir dir(getOutputDirectory());
    dir.mkpath(getSourceProtectionDirectory());
    dir.mkpath(getExeProtectionDirectory());
}

QString Settings::getOutputDirectory() const {
    return settings.value("output_directory").toString();
}

void Settings::setOutputDirectory(const QString& path) {
    settings.setValue("output_directory", path);
    
    // Создаем структуру директорий
    QDir dir(path);
    dir.mkpath(getSourceProtectionDirectory());
    dir.mkpath(getExeProtectionDirectory());
} 