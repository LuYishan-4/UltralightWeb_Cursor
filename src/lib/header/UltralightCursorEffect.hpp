

#pragma once



#include <memory>
#include <core/output.h>
#include "UltralightHtmlEffect.hpp"
#include "MouseProvider.hpp"
#include <kwin/effect/effect.h>

namespace KWin
{


class GLTexture;


class UltralightCursorEffect : public Effect
{
    Q_OBJECT
    

public:

    UltralightCursorEffect();

    ~UltralightCursorEffect() override;


   void paintScreen(
    const RenderTarget& renderTarget,
    const RenderViewport& viewport,
    int mask,
    const Region& region,
    LogicalOutput* screen
) override;


    bool isActive() const override;


    int requestedEffectChainPosition() const override
    {
        return 99;
    }


    // 給 factory 巨集用，檢查目前 compositing 環境支不支援這個 effect
    static bool supported();


private:

    void hideCursor();

    void showCursor();


    GLTexture* ensureCursorTexture();


    std::unique_ptr<CursorFX::UltralightHtmlEffect> m_html;

    std::unique_ptr<CursorFX::IMouseProvider> m_mouseProvider;

    std::unique_ptr<GLTexture> m_cursorTexture;

    bool m_isMouseHidden = false;

    QPointF m_cursorPoint;

};


}
