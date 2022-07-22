#ifndef SHAPECORNERS_ADAPTOR_H
#define SHAPECORNERS_ADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>
#include "shapecorners.h"

namespace KWin
{
class EffectAdaptor : public QDBusAbstractAdaptor
{
   Q_OBJECT
   Q_CLASSINFO("D-Bus Interface", "org.kde.kwin")

private:
    ShapeCornersEffect *m_effect;

public:
    EffectAdaptor(ShapeCornersEffect *effect) : QDBusAbstractAdaptor(effect), m_effect(effect) { }

public slots:
    Q_NOREPLY void setRoundness(int r)
    {
        ConfigModel config;
        config.Load();
        if(config.m_dsp) {
            config.m_size = r;
            config.Save();
            m_effect->setConfig(config);
        }
    }
    Q_NOREPLY void configure() { m_effect->reconfigure(KWin::Effect::ReconfigureAll); }
};
}
#endif //SHAPECORNERS_ADAPTOR_H
