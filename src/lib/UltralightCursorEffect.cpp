#include "header/UltralightCursorEffect.hpp"
#include "header/KwinMouseProvider.hpp"
#include "header/UserConfig.hpp"

#include "core/rendertarget.h"
#include "core/renderviewport.h"
#include "effect/effecthandler.h"
#include "opengl/glutils.h"

#include <QImage>
#include <iostream>


namespace KWin
{


UltralightCursorEffect::UltralightCursorEffect()
{
    std::cout << "[UltralightCursorEffect][debug] constructor\n";

    m_html = std::make_unique<CursorFX::UltralightHtmlEffect>();

    CursorFX::UserConfig config;
    config.load();

    std::string html_path = config.htmlPath();
    std::string sdk_path = config.sdkPath();


    if(html_path.empty())
    {
        html_path = "/home/luyishan4/Documents/kde-cursor-ani/testHtml/index.html";
        config.setHtmlPath(html_path);

        std::cout << "[UltralightCursorEffect][debug] html Path: " << html_path << "\n";
    }

    if(sdk_path.empty())
    {
        sdk_path = "/home/luyishan4/Documents/kde-cursor-ani/sdk/ultralight-free-sdk-1.4.0-linux-x64";
        config.setSdkPath(sdk_path);

        std::cout << "[UltralightCursorEffect][debug] sdk Path: " << sdk_path << "\n";
    }

    config.save();

    if(!m_html->initialize(html_path, sdk_path))
    {
        std::cerr << "[UltralightCursorEffect][debug] UltralightHtmlEffect initialize 失敗\n";
        m_html.reset();
        return;
    }


    m_mouseProvider = std::make_unique<KwinMouseProvider>();

    m_mouseProvider->setCallback(
        [this](const CursorFX::MousePoint& pt)
        {
            if(!m_html)
                return;

            m_cursorPoint = QPointF(pt.x, pt.y);

            m_html->move(pt.x, pt.y, pt.pressed);

            effects->addRepaintFull();
        }
    );

    if(!m_mouseProvider->initialize())
    {
        std::cerr << "[UltralightCursorEffect][debug] MouseProvider initialize 失敗\n";
    }

    hideCursor();
}


UltralightCursorEffect::~UltralightCursorEffect()
{
    std::cout << "[UltralightCursorEffect][debug] destructor\n";

    showCursor();
}


bool UltralightCursorEffect::supported()
{
    return effects->isOpenGLCompositing();
}


void UltralightCursorEffect::hideCursor()
{
    if(m_isMouseHidden)
        return;

    if(!effects->isOpenGLCompositing())
    {
        std::cerr << "[UltralightCursorEffect][debug] 非 OpenGL compositing，無法顯示自訂游標\n";
        return;
    }

    effects->hideCursor();

    m_isMouseHidden = true;

    std::cout << "[UltralightCursorEffect][debug] 系統游標已隱藏，改用自訂渲染\n";
}


void UltralightCursorEffect::showCursor()
{
    if(!m_isMouseHidden)
        return;

    effects->showCursor();

    m_cursorTexture.reset();

    m_isMouseHidden = false;
}


GLTexture* UltralightCursorEffect::ensureCursorTexture()
{
    if(!m_html)
        return nullptr;

    // 每個 frame 驅動一次 Ultralight（JS 動畫、layout、渲染都在這裡跑）
    m_html->update();

    const uint8_t* pixels = m_html->pixels();

    if(!pixels)
        return nullptr;

    const int width = m_html->width();
    const int height = m_html->height();
    const int stride = m_html->stride();


    QImage image(
        pixels,
        width,
        height,
        stride,
        QImage::Format_ARGB32_Premultiplied
    );

    QImage owned = image.copy();

    m_cursorTexture = GLTexture::upload(owned);

    if(!m_cursorTexture)
    {
        std::cerr << "[UltralightCursorEffect][debug] GLTexture::upload failded\n";
        return nullptr;
    }

    m_cursorTexture->setWrapMode(GL_CLAMP_TO_EDGE);

    return m_cursorTexture.get();
}


void UltralightCursorEffect::paintScreen(
    const RenderTarget& renderTarget,
    const RenderViewport& viewport,
    int mask,
    const Region& region,
    LogicalOutput* screen
)
{
    effects->paintScreen(renderTarget, viewport, mask, region, screen);

    if(!m_isMouseHidden)
        return;

    GLTexture* texture = ensureCursorTexture();

    if(!texture)
    {
        effects->addRepaintFull();
        return;
    }

    const int w = m_html->width();
    const int h = m_html->height();

    const QPointF hotspot(w / 2.0, h / 2.0);

    const QPointF p = effects->cursorPos() - hotspot;

    const auto scale = viewport.scale();

    ShaderBinder binder(ShaderTrait::MapTexture | ShaderTrait::TransformColorspace);
    GLShader* shader = binder.shader();

    shader->setColorspaceUniforms(
        ColorDescription::sRGB,
        renderTarget.colorDescription(),
        RenderingIntent::Perceptual
    );

    QMatrix4x4 mvp = viewport.projectionMatrix();
    mvp.translate(p.x() * scale, p.y() * scale);

    shader->setUniform(GLShader::Mat4Uniform::ModelViewProjectionMatrix, mvp);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // premultiplied alpha

    texture->render(QSizeF(w, h) * scale);

    glDisable(GL_BLEND);

    // 持續要求下一 frame 重繪，讓待機動畫能一直跑
    effects->addRepaintFull();
}


bool UltralightCursorEffect::isActive() const
{
    return m_isMouseHidden;
}


}



KWIN_EFFECT_FACTORY_SUPPORTED(
    KWin::UltralightCursorEffect,
    "metadata.json",
    return KWin::UltralightCursorEffect::supported();
)


#include "UltralightCursorEffect.moc"