#pragma once

#include <QString>
#include <QSettings>
#include <QDir>

class Settings {
public:
    static Settings& instance() {
        static Settings settings;
        return settings;
    }

    QString getOutputDirectory() const {
        return settings.value("output_directory", 
            QDir::homePath() + "/Documents/SpectreGuard").toString();
    }

    QString getExeProtectionDirectory() const {
        return settings.value("exe_protection_directory", 
            getOutputDirectory() + "/exe_protection").toString();
    }

    QString getSourceProtectionDirectory() const {
        return settings.value("source_protection_directory", 
            getOutputDirectory() + "/source_protection").toString();
    }

    void setOutputDirectory(const QString& path) {
        settings.setValue("output_directory", path);
        createDirectories();
    }
    
    void setExeProtectionDirectory(const QString& path) {
        settings.setValue("exe_protection_directory", path);
        QDir dir;
        dir.mkpath(path);
    }
    
    void setSourceProtectionDirectory(const QString& path) {
        settings.setValue("source_protection_directory", path);
        QDir dir;
        dir.mkpath(path);
    }

private:
    Settings() {
        createDirectories();
    }

    void createDirectories() {
        QDir dir;
        dir.mkpath(getExeProtectionDirectory());
        dir.mkpath(getSourceProtectionDirectory());
    }

    QSettings settings{"SpectreGuard", "SpectreGuard"};
}; 