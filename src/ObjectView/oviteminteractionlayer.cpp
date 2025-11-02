#include "oviteminteractionlayer.hpp"

#include "ovinternalscene.hpp"

namespace OVLayers {

OVItemInteractionLayer::OVItemInteractionLayer(QWidget* parent) :
    OVCanvasLayer(parent)
{

}

void OVItemInteractionLayer::setInteractionEnabled(bool isEn)
{
    m_isInteractionEnabled = isEn;
}

} // namespace ObjectItems
