# KDE-Rounded-Corners Class Diagram
```mermaid
classDiagram
    
    namespace KWin {
        class Effect {
            prePaintWindow()
        }
        
        class EffectWindow {
            frameGeometry
            expandedGeometry
            screen
            windowClass
            windowType
            addRepaintFull()
            windowFrameGeometryChanged()
        }
        
        class OffscreenEffect {
            drawWindow()
            setShader()
        }
        
        class GLShader {
            isValid()
            uniformLocation()
            setUniform()
        }
        
        class ShaderManager {
            generateShaderFromFile()
            pushShader()
            popShader()
        }
    }
    
    namespace kwin4_shapecorners_effect {
        class ShapeCornersEffect {
            m_shader_manager
            m_managed
            +prePaintWindow()
            +drawWindow()
            get_window_titles()
            checkTiled()
            checkMaximized()
            windowAdded()
            windowRemoved()
            windowResized()
        }
        
        class ShapeCornersWindow {
            name: QString
            isTiled: bool
            isMaximized: bool
            cornerRadius: float
            shadowSize: float
            outlineSize: float
            m_last_time: miliseconds
            animateProperties()
            isActive()
            hasRoundCorners()
            hasOutline()
            hasEffect()
        }
        
        class ShapeCornersShader {
            m_shader: GLShader
            IsValid()
            GetShader()
            Bind()
            UnBind()
        }
        
        class ShapeCornersColor {
            r: float
            g: float
            b: float
            a: float
            toQColor()
            toString()
            round()
            clamp()
            setAlpha()
        }
    }
    
    namespace kwin4_shapecorners_config {
        class ShapeCornersKCM {
            ui: UI::Form
            +defaults()
            +load()
            +save()
            update_windows()
        }
        
        class ShapeCornersConfig {
            Size: UInt
            ShadowSize: UInt
            ShadowColor: Color
            AnimationEnabled: Bool
            OutlineThickness: Double
            OutlineColor: Color
            .
            .
            .
            Inclusions: StringList
            Exclusions: StringList
            IncludeNormalWindows: Bool
            IncludeDialogs: Bool
            DisableRoundTile: Bool
            DisableRoundMaximize: Bool
        }
    }
    
    class QDBusConnection {
        sessionBus()
        isConnected()
        registerService()
        registerObject()
    }
    
    class KCModule {
        widget: QWidget
        +defaults()
        +load()
        +save()
    }
    
    Effect <|-- OffscreenEffect
    OffscreenEffect <|-- ShapeCornersEffect
    ShapeCornersWindow o-- ShapeCornersColor

    GLShader --o ShaderManager
    ShapeCornersEffect o-- ShapeCornersWindow
    EffectWindow <-- ShapeCornersWindow 
    ShapeCornersEffect --> ShapeCornersShader
    EffectWindow <.. ShapeCornersEffect
    QDBusConnection <.. ShapeCornersEffect
    ShapeCornersWindow --> ShapeCornersConfig
    
    KCModule <|-- ShapeCornersKCM
    QDBusConnection <.. ShapeCornersKCM
    ShapeCornersKCM --> ShapeCornersConfig

    ShapeCornersShader ..> ShapeCornersWindow
    GLShader <-- ShapeCornersShader
    ShaderManager <-- ShapeCornersShader
```