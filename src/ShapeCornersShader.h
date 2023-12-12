//
// Created by matin on 20/07/22.
//

#ifndef KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H
#define KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H

#include <kwinglutils.h>
#include <memory>
#include <QWidget>
#include "ShapeCornersConfig.h"

namespace KWin {
    class GLShader;
    class ShaderManager;
}

class ShapeCornersShader {
public:
    ShapeCornersShader();

    static bool IsLegacy();
    bool IsValid() const;
    const std::unique_ptr<KWin::GLShader>& Bind(KWin::EffectWindow *w) const;
    const std::unique_ptr<KWin::GLShader>& Bind(QMatrix4x4 mvp, KWin::EffectWindow *w) const;
    void Unbind() const;
    std::unique_ptr<KWin::GLShader>& GetShader() { return m_shader; }

private:
    std::unique_ptr<KWin::GLShader> m_shader;
    KWin::ShaderManager* m_manager;
    QWidget m_widget;
    int m_shader_windowSize = 0;
    int m_shader_windowExpandedSize = 0;
    int m_shader_windowTopLeft = 0;
    int m_shader_shadowColor = 0;
    int m_shader_shadowSize = 0;
    int m_shader_radius = 0;
    int m_shader_outlineColor = 0;
    int m_shader_outlineThickness = 0;
    int m_shader_front = 0;
};


#endif //KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H
