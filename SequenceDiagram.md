# KDE-Rounded-Corners Sequence Diagram

```mermaid
sequenceDiagram
    Box beige KWin
        participant KWin
        participant EffectWindow
        participant ShaderManager
        participant OffscreenEffect
    end
    Box kwin4_shapecorners_effect
        participant Effect
        participant WindowManager
        participant Shader
        participant shapecorners.frag
        participant Window
        participant WindowConfig
    end

    loop approximately 60 frames per second
        KWin->>+Effect: prePaintWindow()
        Effect->>WindowManager: findWindow()
        Effect->>+Window: animateProperties()
        Window->>+WindowConfig: read config
        WindowConfig-->>-Window: values
        Note over Window: calculates<br>animation
        Note over Window: stores window<br>properties
        alt animation not finished
            Window-)EffectWindow: addFullRepaint()
        end
        Window-->>-Effect: done
        alt window has round corners
            Effect-)EffectWindow: add paint / remove opaque / set translucent
        end
        Effect->>+OffscreenEffect: OffscreenEffect::<br>prePaintWindow()
        EffectWindow-->>OffscreenEffect: notifies painting parameters 
        OffscreenEffect-->>-Effect: done
        Effect-->>-KWin: done prePaint
        
        alt if painting is required
            KWin->>+Effect: drawWindow()
            Effect->>WindowManager: findWindow()
            Effect->>+Shader: Bind()
            Shader->>+WindowConfig: read properties
            WindowConfig-->>-Shader: values
            Shader->>+shapecorners.frag: setUniform()
            shapecorners.frag-->>-Shader: done
            Shader-)ShaderManager: pushShader()
            Shader-->>-Effect: done
            Effect->>+OffscreenEffect: OffscreenEffect::<br>drawWindow()
            OffscreenEffect->>+ShaderManager: render window
            par parallel render each pixel
                ShaderManager->>+shapecorners.frag: render shader
                shapecorners.frag-->>-ShaderManager: done
            end
            ShaderManager-->>-OffscreenEffect: done
            OffscreenEffect-->>-Effect: done
            Effect->>+Shader: Unbind()
            Shader-)ShaderManager: popShader()
            Shader-->>-Effect: done
            Effect-->>-KWin: done draw
        end
    end
```
