//
// Created by matin on 20/07/22.
//

#include <KConfigGroup>
#include <KSharedConfig>
#include "ConfigModel.h"

const QString
    key_size = "roundness",
    key_shadowColor = "shadowColor",
    key_outlineColor = "outlineColor",
    key_inactiveOutlineColor = "inactiveOutlineColor",
    key_outlineThickness = "outlineThickness";

ConfigModel::ConfigModel():
        m_size(10),
        m_outlineThickness(1),
        m_shadowColor(QColor(Qt::black)),
        m_outlineColor(QColor(Qt::black)),
        m_inactiveOutlineColor(QColor(Qt::black))
{
}

void ConfigModel::Load() {
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    m_size = conf.readEntry(key_size, m_size);
    m_shadowColor = conf.readEntry(key_shadowColor, m_shadowColor);
    m_outlineColor = conf.readEntry(key_outlineColor, m_outlineColor);
    m_inactiveOutlineColor = conf.readEntry(key_inactiveOutlineColor,m_inactiveOutlineColor);
    m_outlineThickness = conf.readEntry(key_outlineThickness, m_outlineThickness);
}

void ConfigModel::Save() const {
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    conf.writeEntry(key_size, m_size);
    conf.writeEntry(key_shadowColor, m_shadowColor);
    conf.writeEntry(key_outlineColor, m_outlineColor);
    conf.writeEntry(key_inactiveOutlineColor,m_inactiveOutlineColor);
    conf.writeEntry(key_outlineThickness, m_outlineThickness);
    conf.sync();
}
