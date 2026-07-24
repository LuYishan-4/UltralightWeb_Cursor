#include "SettingsBackend.hpp"

#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
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
    if(htmlPath_ == path)
        return;

    htmlPath_ = path;


    QFileInfo info(path);
    QString themeFolder = info.absolutePath();
    QString themeName = QFileInfo(themeFolder).fileName();

    if(!themeFolder.isEmpty() && !themeName.isEmpty())
    {
        config_.uploadTheme(
            themeFolder.toStdString(),
            themeName.toStdString()
        );
    }

    Q_EMIT htmlPathChanged();
}



void SettingsBackend::setSdkPath(
    const QString& path
)
{
    if(sdkPath_ == path)
        return;


    sdkPath_ = path;

    Q_EMIT sdkPathChanged();
}



void SettingsBackend::setEnabled(
    bool value
)
{
    if(enabled_ == value)
        return;


    enabled_ = value;

    Q_EMIT enabledChanged();
}



void SettingsBackend::setCursorWidth(
    int value
)
{
    if(cursorWidth_ == value)
        return;


    cursorWidth_ = value;

    Q_EMIT cursorWidthChanged();
}



void SettingsBackend::setCursorHeight(
    int value
)
{
    if(cursorHeight_ == value)
        return;


    cursorHeight_ = value;

    Q_EMIT cursorHeightChanged();
}



void SettingsBackend::setStatusMessage(
    const QString& msg
)
{
    statusMessage_ = msg;

    Q_EMIT statusMessageChanged();
}





void SettingsBackend::reload()
{
    config_.load();



    htmlPath_ =
        QString::fromStdString(
            config_.readKeyValue("html")
        );



    sdkPath_ =
        QString::fromStdString(
            config_.readKeyValue("sdk")
        );



    enabled_ =config_.readKeyValue("enabled")  == "true";



    blacklist_.clear();


    for(const auto& item :
        config_.getBlacklist())
    {
        blacklist_
            << QString::fromStdString(item);
    }




    currentTheme_ =
        QString::fromStdString(
            config_.currentTheme()
        );




    bool ok=false;



    int width =  std::stoi(config_.readKeyValue("width"));

   cursorWidth_=width;



    int height =std::stoi(config_.readKeyValue("height"));
    cursorHeight_=height;



    loadThemes();



    Q_EMIT htmlPathChanged();
    Q_EMIT sdkPathChanged();
    Q_EMIT enabledChanged();
    Q_EMIT blacklistChanged();
    Q_EMIT currentThemeChanged();
    Q_EMIT cursorWidthChanged();
    Q_EMIT cursorHeightChanged();


    setStatusMessage(
        QStringLiteral("Loaded")
    );
}





void SettingsBackend::save()
{

    config_.setKeyValue(
        "html",
        htmlPath_.toStdString()
    );


    config_.setKeyValue(
        "sdk",
        sdkPath_.toStdString()
    );


    config_.setKeyValue(
        "enabled",
        enabled_ ? "true" : "false"
    );


    config_.setKeyValue(
        "width",
        std::to_string(cursorWidth_)
    );


    config_.setKeyValue(
        "height",
        std::to_string(cursorHeight_)
    );



    if(config_.save()){
        reconfigureKWin();
        setStatusMessage(
            QStringLiteral("Saved")
        );
    }
    else
    {

        setStatusMessage(
            QStringLiteral("Save failed")
        );

    }

}







void SettingsBackend::addBlacklist(
    const QString& app
)
{
    config_.appendBlacklist(
        app.toStdString()
    );


    reload();
}




void SettingsBackend::removeBlacklist(
    const QString& app
)
{
    config_.removeBlacklist(
        app.toStdString()
    );


    reload();
}







void SettingsBackend::loadThemes()
{

    themeList_.clear();



    std::filesystem::path path =
        g_sdkInitialPath /
        "resources";



    if(!std::filesystem::exists(path))
    {
        Q_EMIT themeListChanged();
        return;
    }
    for(auto& item :
        std::filesystem::directory_iterator(path))
    {

        if(item.is_directory())
        {

            themeList_
                << QString::fromStdString(
                    item.path()
                    .filename()
                    .string()
                );

        }

    }



    Q_EMIT themeListChanged();

}








bool SettingsBackend::uploadTheme(const QString& path){


    qDebug() << "uploadTheme path =" << path;

    QDir dir(QDir::cleanPath(path));

    if(!dir.exists())
    {
        setStatusMessage(
            QStringLiteral("Folder not found")
        );
        return false;
    }

    QString name = dir.dirName();


    qDebug() << "uploadTheme name =" << name;

    if(config_.uploadTheme(
        path.toStdString(),
        name.toStdString()))
    {

        loadThemes();


        setStatusMessage(
            QStringLiteral(
                "Theme uploaded"
            )
        );


        return true;

    }




    setStatusMessage(
        QStringLiteral(
            "Upload failed"
        )
    );


    return false;

}






void SettingsBackend::useTheme(
    const QString& name
)
{

    config_.setTheme(
        name.toStdString()
    );


    reload();

}







void SettingsBackend::removeTheme(
    const QString& name
)
{

    std::filesystem::remove_all(

        g_sdkInitialPath /
        "resources" /
        name.toStdString()

    );


    loadThemes();


    reload();

}








void SettingsBackend::openThemeFolder(
    const QString& name
)
{

    QString path =
        QString::fromStdString(

            (g_sdkInitialPath /
             "resources" /
             name.toStdString())
             .string()

        );



    QDesktopServices::openUrl(
        QUrl::fromLocalFile(path)
    );

}







bool SettingsBackend::pathExists(
    const QString& path
) const
{
    return QFileInfo::exists(path);
}








void SettingsBackend::enable()
{

    QDBusInterface effect(

          QStringLiteral("org.kde.KWin"),

          QStringLiteral("/UltralightCursor"),

          QStringLiteral("org.kde.kwin.KWin.UltralightCursorEffect"),

        QDBusConnection::sessionBus()

    );



    auto reply =
        effect.call(  QStringLiteral("enable"));



    if(reply.type()
       == QDBusMessage::ErrorMessage)
    {

        setStatusMessage(
            QStringLiteral("Enable failed")
        );

    }
    else
    {

        enabled_=true;

        Q_EMIT enabledChanged();


        setStatusMessage(
            QStringLiteral("Enabled")
        );

    }

}









void SettingsBackend::disable()
{

    QDBusInterface effect(
          QStringLiteral("org.kde.KWin"),
          QStringLiteral("/UltralightCursor"),
          QStringLiteral("org.kde.kwin.KWin.UltralightCursorEffect"),
            QDBusConnection::sessionBus()
    );



    auto reply = effect.call( QStringLiteral("disable"));



    if(reply.type()
       == QDBusMessage::ErrorMessage)
    {

        setStatusMessage(
            QStringLiteral("Disable failed")
        );

    }
    else
    {

        enabled_=false;

        Q_EMIT enabledChanged();


        setStatusMessage(
            QStringLiteral("Disabled")
        );

    }

}









void SettingsBackend::reconfigureKWin()
{

    QDBusInterface effect(
        QStringLiteral("org.kde.KWin"),
         QStringLiteral("/UltralightCursor"),
        QStringLiteral("org.kde.kwin.KWin.UltralightCursorEffect"),
        QDBusConnection::sessionBus()

    );



    auto reply =
        effect.call(
              QStringLiteral("reloadHtml")
        );



    if(reply.type()
       == QDBusMessage::ErrorMessage)
    {

        setStatusMessage(
            QStringLiteral("Reload failed")
        );

    }
    else
    {

        setStatusMessage(
            QStringLiteral("Reloaded")
        );

    }

}