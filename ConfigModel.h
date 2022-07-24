//
// Created by matin on 20/07/22.
//

#ifndef KWIN4_SHAPECORNERS_CONFIG_CONFIGMODEL_H
#define KWIN4_SHAPECORNERS_CONFIG_CONFIGMODEL_H

#include <QColor>

class ConfigModel {
public:
    ConfigModel();
    void Load();
    void Save() const;

    float  m_size,
           m_outlineThickness;
    QColor m_shadowColor,
           m_outlineColor,
	   m_inactiveOutlineColor;
    bool   m_dsp;
};

#endif //KWIN4_SHAPECORNERS_CONFIG_CONFIGMODEL_H
