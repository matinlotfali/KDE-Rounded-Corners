# KDE-Rounded-Corners Class Diagram
```mermaid
classDiagram
    
    namespace KWin {
        class KWinEffect {
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
        class Effect {
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
        
        class Window {
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
        
        class Shader {
            m_shader: GLShader
            IsValid()
            GetShader()
            Bind()
            UnBind()
        }
        
        class Color {
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
        class KCM {
            ui: UI::Form
            +defaults()
            +load()
            +save()
            update_windows()
        }
        
        class Config {
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
    
    KWinEffect <|-- OffscreenEffect
    OffscreenEffect <|-- Effect
    Window o-- Color

    GLShader --o ShaderManager
    Effect o-- Window
    EffectWindow <-- Window 
    Effect --> Shader
    EffectWindow <.. Effect
    QDBusConnection <.. Effect
    Window --> Config
    
    KCModule <|-- KCM
    QDBusConnection <.. KCM
    KCM --> Config

    Shader ..> Window
    GLShader <-- Shader
    ShaderManager <-- Shader
```