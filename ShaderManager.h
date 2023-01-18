//
// Created by matin on 20/07/22.
//

#ifndef KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H
#define KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H

#include <kwinglutils.h>
#include <memory>
#include "ConfigModel.h"

namespace KWin {
    class GLShader;
    class ShaderManager;
}

class ShaderManager {
public:
    ShaderManager();

    static bool IsLegacy();
    bool IsValid() const;
    const std::unique_ptr<KWin::GLShader>& Bind(KWin::EffectWindow *w, const ConfigModel& config) const;
    const std::unique_ptr<KWin::GLShader>& Bind(QMatrix4x4 mvp, KWin::EffectWindow *w, const ConfigModel& config) const;
    void Unbind() const;

private:
    std::unique_ptr<KWin::GLShader> m_shader;
    KWin::ShaderManager* m_manager;
    int m_shader_windowActive = 0;
    int m_shader_windowSize = 0;
    int m_shader_windowHasDecoration = 0;
    int m_shader_shadowColor = 0;
    int m_shader_radius = 0;
    int m_shader_outlineColor = 0;
    int m_shader_outlineThickness = 0;
    int m_shader_back = 0;
};


#endif //KWIN4_SHAPECORNERS_CONFIG_SHADERMANAGER_H
