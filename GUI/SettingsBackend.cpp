#include "SettingsBackend.hpp"

#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDBusInterface>
#include <QDBusConnection>

#include <filesystem>

using namespace UltralightWebCursorM;

SettingsBackend::SettingsBackend(QObject* parent)
    : QObject(parent)
{
    reload();
}

QString SettingsBackend::htmlPath() const
{
    return htmlPath_;
}

QString SettingsBackend::sdkPath() const
{
    return sdkPath_;
}

bool SettingsBackend::enabled() const
{
    return enabled_;
}

QString SettingsBackend::statusMessage() const
{
    return statusMessage_;
}

QStringList SettingsBackend::blacklist() const
{
    return blacklist_;
}

QStringList SettingsBackend::themeList() const
{
    return themeList_;
}

QString SettingsBackend::currentTheme() const
{
    return currentTheme_;
}

int SettingsBackend::cursorWidth() const
{
    return cursorWidth_;
}

int SettingsBackend::cursorHeight() const
{
    return cursorHeight_;
}

void SettingsBackend::setHtmlPath(const QString& path)
{
    if (htmlPath_ == path)
        return;

    htmlPath_ = path;
    Q_EMIT htmlPathChanged();
}

void SettingsBackend::setSdkPath(const QString& path)
{
    if (sdkPath_ == path)
        return;

    sdkPath_ = path;
    Q_EMIT sdkPathChanged();
}

void SettingsBackend::setEnabled(bool value)
{
    if (enabled_ == value)
        return;

    enabled_ = value;
    Q_EMIT enabledChanged();
}

void SettingsBackend::setCursorWidth(int value)
{
    if (cursorWidth_ == value)
        return;

    cursorWidth_ = value;
    Q_EMIT cursorWidthChanged();
}

void SettingsBackend::setCursorHeight(int value)
{
    if (cursorHeight_ == value)
        return;

    cursorHeight_ = value;
    Q_EMIT cursorHeightChanged();
}

void SettingsBackend::setStatusMessage(const QString& msg)
{
    statusMessage_ = msg;
    Q_EMIT statusMessageChanged();
}

void SettingsBackend::reload()
{
    config_.load();

    htmlPath_ = QString::fromStdString(
        config_.readKeyValue("html")
    );

    sdkPath_ = QString::fromStdString(
        config_.readKeyValue("sdk")
    );

    blacklist_.clear();

    for (auto& item : config_.getBlacklist()) {
        blacklist_ << QString::fromStdString(item);
    }

    currentTheme_ = QString::fromStdString(
        config_.currentTheme()
    );

    bool ok = false;

    int width = QString::fromStdString(
        config_.readKeyValue("width")
    ).toInt(&ok);

    if (ok)
        cursorWidth_ = width;

    int height = QString::fromStdString(
        config_.readKeyValue("height")
    ).toInt(&ok);

    if (ok)
        cursorHeight_ = height;

    loadThemes();

    Q_EMIT htmlPathChanged();
    Q_EMIT sdkPathChanged();
    Q_EMIT blacklistChanged();
    Q_EMIT currentThemeChanged();
    Q_EMIT cursorWidthChanged();
    Q_EMIT cursorHeightChanged();

    reconfigureKWin();
    setStatusMessage(QStringLiteral("Reload"));
}

void SettingsBackend::save()
{
    config_.setKeyValue("html", htmlPath_.toStdString());
    config_.setKeyValue("sdk", sdkPath_.toStdString());

    /*
        UserConfig 管理 theme
        不需要額外保存
    */

    config_.setKeyValue("width", std::to_string(cursorWidth_));
    config_.setKeyValue("height", std::to_string(cursorHeight_));

    if (config_.save()) {
        setStatusMessage(QStringLiteral("Saved"));
    } else {
        setStatusMessage(QStringLiteral("Save failed"));
    }
}

void SettingsBackend::addBlacklist(const QString& app)
{
    config_.appendBlacklist(app.toStdString());
    reload();
}

void SettingsBackend::removeBlacklist(const QString& app)
{
    config_.removeBlacklist(app.toStdString());
    reload();
}

void SettingsBackend::loadThemes()
{
    /*
        UserConfig 沒有提供 themeList
        所以 SettingsBackend 自己讀取
    */

    themeList_.clear();

    std::filesystem::path path = g_sdkInitialPath / "resources";

    if (!std::filesystem::exists(path))
        return;

    for (auto& item : std::filesystem::directory_iterator(path)) {
        if (item.is_directory()) {
            themeList_ << QString::fromStdString(
                item.path().filename().string()
            );
        }
    }

    Q_EMIT themeListChanged();
}

bool SettingsBackend::uploadTheme(const QString& path)
{
    QFileInfo info(path);

    if (!info.exists() || !info.isDir()) {
        setStatusMessage(QStringLiteral("Folder not found"));
        return false;
    }

    QString name = info.fileName();

    if (config_.uploadTheme(path.toStdString(), name.toStdString())) {
        loadThemes();
        setStatusMessage(QStringLiteral("Theme uploaded"));
        return true;
    }

    setStatusMessage(QStringLiteral("Upload failed"));
    return false;
}

void SettingsBackend::useTheme(const QString& name)
{
    config_.setTheme(name.toStdString());
    reload();
}

void SettingsBackend::removeTheme(const QString& name)
{
    std::filesystem::remove_all(
        g_sdkInitialPath / "resources" / name.toStdString()
    );

    loadThemes();
}

void SettingsBackend::openThemeFolder(const QString& name)
{
    /*
        UserConfig 沒有提供開啟資料夾
    */

    QString path = QString::fromStdString(
        (g_sdkInitialPath / "resources" / name.toStdString()).string()
    );

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(path)
    );
}

bool SettingsBackend::pathExists(const QString& path) const
{
    return QFileInfo::exists(path);
}

void SettingsBackend::enable()
{
    QDBusInterface effect(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/UltralightCursor"),
        QStringLiteral("org.kde.KWin"),
        QDBusConnection::sessionBus()
    );

    QDBusMessage reply =
        effect.call(
            QStringLiteral("enable")
        );


    if(reply.type() == QDBusMessage::ErrorMessage)
    {
        setStatusMessage(
            QStringLiteral("enable failed")
        );
    }
    else
    {
        setStatusMessage(
            QStringLiteral("enable")
        );
    }
}

void SettingsBackend::disable()
{
    QDBusInterface effect(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/UltralightCursor"),
        QStringLiteral("org.kde.KWin"),
        QDBusConnection::sessionBus()
    );

   QDBusMessage reply =
        effect.call(
            QStringLiteral("disable")
        );


    if(reply.type() == QDBusMessage::ErrorMessage)
    {
        setStatusMessage(
            QStringLiteral("disable failed")
        );
    }
    else
    {
        setStatusMessage(
            QStringLiteral("disable")
        );
    }
}


void SettingsBackend::reconfigureKWin()
{
    QDBusInterface effect(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/UltralightCursor"),
        QStringLiteral("org.kde.KWin")
    );


    QDBusMessage reply =
        effect.call(
            QStringLiteral("reloadHtml")
        );


    if(reply.type() == QDBusMessage::ErrorMessage)
    {
        setStatusMessage(
            QStringLiteral("Reload HTML failed")
        );
    }
    else
    {
        setStatusMessage(
            QStringLiteral("HTML reloaded")
        );
    }
}