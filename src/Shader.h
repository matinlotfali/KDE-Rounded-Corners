//
// Created by matin on 20/07/22.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_SHADER_H
#define KWIN4_EFFECT_SHAPECORNERS_SHADER_H

#include <QRectF> // for KWin 5.27 compatibility
#include <QVector2D>
#include <memory>

namespace KWin {
    class GLShader;
    class ShaderManager;
}

namespace ShapeCorners {
    class Window;

    inline QRectF operator*(const QRect &r, const qreal scale) {
        return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale};
    }

    inline QRectF operator*(const QRectF &r, const qreal scale) {
        return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale};
    }

    inline QVector2D toVector2D(const QSizeF &s) {
        return {static_cast<float>(s.width()), static_cast<float>(s.height())};
    }

    class Shader {
    public:
        /**
         * \brief Loads the correct shader based on the GL platform in KWin.
         *        Then it assigns the location references for all its uniform variables so they could be written in.
         */
        explicit Shader();

        /**
         * \return True if the shader is valid and loaded
         */
        [[nodiscard]] bool IsValid() const;

        /**
         * \brief This function assigns the required variables to the shader.
         *        Then it pushes the shader to the stack of rendering.
         *        This needs to be called before each window is rendered.
         * \param window The window that the effect will be rendering on
         * \param scale The scale of the screen
         */
        void Bind(const ShapeCorners::Window &window, qreal scale) const;

        /**
         * \brief Pop the shader from the stack of rendering.
         *        This needs to be called after each window is rendered.
         */
        void Unbind() const;

        /**
         * \return A reference to the unique pointer of the loaded shader.
         */
        std::unique_ptr<KWin::GLShader> &GetShader() { return m_shader; }

    private:
        /**
         * \brief A pointer to the loaded shader used to assign the value of uniform variables.
         */
        std::unique_ptr<KWin::GLShader> m_shader;

        /**
         * \brief An instance of `ShaderManager` used to load shader from file and push/pop the shader for each render.
         */
        KWin::ShaderManager *m_manager;

        /**
         * \brief Reference to `uniform vec2 windowSize;`
         *        Containing `window->frameGeometry().size()`
         */
        int m_shader_windowSize = 0;

        /**
         * \brief Reference to `uniform vec2 windowExpandedSize;`
         *        Containing `window->expandedGeometry().size()`
         * \note  When `expandedGeometry = frameGeometry`, it means there is no shadow.
         */
        int m_shader_windowExpandedSize = 0;

        /**
         * \brief Reference to `uniform vec2 windowTopLeft;`
         *        Containing the distance between the top-left of `expandedGeometry` and the top-left of `frameGeometry`.
         * \note  When `windowTopLeft = {0,0}`, it means `expandedGeometry = frameGeometry` and there is no shadow.
         */
        int m_shader_windowTopLeft = 0;

        /**
         * \brief Reference to `uniform bool hasRoundCorners;`
         *        Whether the window has rounded corners.
         */
        int m_shader_hasRoundCorners = 0;

        /**
         * \brief Reference to `uniform float radius;`
         *        Containing the corner radius in pixels specified in settings.
         */
        int m_shader_radius = 0;

        /**
         * \brief Reference to `uniform bool usesNativeShadows;`
         *        Whether the window has custom shadows.
         */
        int m_shader_usesNativeShadows = 0;

        /**
         * \brief Reference to `uniform vec4 shadowColor;`
         *        Containing the RGBA of the shadow color specified in settings.
         */
        int m_shader_shadowColor = 0;

        /**
         * \brief Reference to `uniform float shadowSize;`
         *        Containing the shadow size specified in settings.
         */
        int m_shader_shadowSize = 0;

        /**
         * \brief Reference to `uniform vec4 outlineColor;`
         *        Containing the RGBA of the outline color specified in settings.
         */
        int m_shader_outlineColor = 0;

        /**
         * \brief Reference to `uniform float outlineThickness;`
         *        Containing the thickness of the outline in pixels specified in settings.
         */
        int m_shader_outlineThickness = 0;

        /**
         * \brief Reference to `uniform vec4 secondOutlineColor;`
         *        Containing the RGBA of the outline color specified in settings.
         */
        int m_shader_secondOutlineColor = 0;

        /**
         * \brief Reference to `uniform float secondOutlineThickness;`
         *        Containing the thickness of the outline in pixels specified in settings.
         */
        int m_shader_secondOutlineThickness = 0;

        /**
         * \brief Reference to `uniform sampler2D front;`
         *        Containing the active texture. When assigned to zero, it will contain the painted contents of the window.
         */
        int m_shader_front = 0;
    };
}


#endif //KWIN4_EFFECT_SHAPECORNERS_SHADER_H
