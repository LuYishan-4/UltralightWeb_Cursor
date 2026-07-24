#include "main_settings.hpp"

#include "SettingsBackend.hpp"

#include <QQmlContext>
#include <QQmlEngine>

#include <KLocalizedString>



K_PLUGIN_CLASS_WITH_JSON(UIKCM, "kcm_ultralightwebcursor.json")

UIKCM::UIKCM(
    QObject* parent,
    const KPluginMetaData& metaData
)
    : KQuickConfigModule(parent, metaData)
{
 

    m_backend = new SettingsBackend(this);

 

   setButtons(Help | Apply | Default);

    load();
}


UIKCM::~UIKCM() = default;
QObject* UIKCM::backend() const
{
    return m_backend;
}

void UIKCM::load()
{
    if(!m_backend)
        return;

    m_backend->reload();

    KQuickConfigModule::load();
}


void UIKCM::save()
{
    if(!m_backend)
        return;

    m_backend->save();

    KQuickConfigModule::save();
}


void UIKCM::defaults()
{
    if(!m_backend)
        return;

    m_backend->setEnabled(true);

    m_backend->setCursorWidth(64);
    m_backend->setCursorHeight(64);

    KQuickConfigModule::defaults();
}


// *** 這行是缺的東西：沒有這個巨集，KPluginFactory 根本不知道
//     要怎麼把這個 .so 實例化成 UIKCM 物件，KCM 會直接載入失敗 ***
//
// 巨集第二個參數要對到實際的 metadata.json 檔名


#include "main_settings.moc"