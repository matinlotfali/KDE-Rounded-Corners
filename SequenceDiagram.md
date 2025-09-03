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
        participant Animation
        participant Window
        participant WindowConfig
    end

    loop approximately 60 frames per second
        KWin->>+Effect: prePaintWindow()
        Effect->>WindowManager: findWindow()
        Effect->>+Animation: update()
        Animation->>Window: read WindowConfig
        Window->>+WindowConfig:
        WindowConfig-->>-Animation: values
        Note over Animation: calculates<br>animation
        Animation-)Window: set WindowConfig
        alt animation not finished
            Animation-)EffectWindow: addFullRepaint()
        end
        Animation-->>-Effect: done
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
            Shader->>Window: read WindowConfig
            Window->>+WindowConfig:
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
