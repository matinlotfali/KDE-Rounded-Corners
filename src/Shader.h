/**
 * @file Shader.h
 * @brief Declares the ShapeCorners::Shader class for managing OpenGL shaders in the KWin effect.
 *
 * This file provides the Shader class, which loads, manages, and binds OpenGL shaders for the ShapeCorners KWin effect.
 */

#pragma once

#include <memory>

namespace KWin
{
    class GLShader;
}

namespace ShapeCorners
{
    class Window;

    /**
     * @class Shader
     * @brief Manages the OpenGL shader program for the ShapeCorners KWin effect.
     *
     * Loads the correct shader based on the GL platform in KWin, assigns uniform variable locations,
     * and provides methods to bind and unbind the shader during rendering.
     */
    class Shader
    {
    public:
        /**
         * \brief Loads the correct shader based on the GL platform in KWin.
         *        Then it assigns the location references for all its uniform variables so they could be written in.
         */
        explicit Shader();

        /**
         * @brief Checks if the shader is valid and loaded.
         * \return True if the shader is valid and loaded
         */
        [[nodiscard]]
        bool IsValid() const;

        /**
         * \brief This function assigns the required variables to the shader.
         *        Then it pushes the shader to the stack of rendering.
         * \note  This needs to be called before each window is rendered.
         * \param window The window that the effect will be rendering on
         * \param scale The scale of the screen
         */
        void Bind(const Window &window, double scale) const;

        /**
         * \brief Pop the shader from the stack of rendering.
         * \note  This needs to be called after each window is rendered.
         */
        void Unbind() const;

        /**
         * @brief Gets a reference to the unique pointer of the loaded shader.
         * \return Reference to the unique pointer of the loaded shader.
         */
        std::unique_ptr<KWin::GLShader> &GetShader() { return m_shader; }

    private:
        /**
         * \brief A pointer to the loaded shader used to assign the value of uniform variables.
         */
        std::unique_ptr<KWin::GLShader> m_shader;

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
         *        Containing the distance between the top-left of `expandedGeometry` and the top-left of
         * `frameGeometry`.
         * \note  When `windowTopLeft = {0,0}`, it means `expandedGeometry = frameGeometry` and there is no shadow.
         */
        int m_shader_windowTopLeft = 0;

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
         *        Containing the active texture. When assigned to zero, it will contain the painted contents of the
         * window.
         */
        int m_shader_front = 0;
    };
} // namespace ShapeCorners
