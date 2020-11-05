#ifndef TRAFFIC_INFO_HELPER_H
#define TRAFFIC_INFO_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/traci-client.h"

namespace ns3 {

class LineControlHelper
{
public:

  LineControlHelper ();


  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (Ptr<Node> node) const;

  ApplicationContainer Install (std::string nodeName) const;

  ApplicationContainer Install (NodeContainer c) const;

private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory; //!< Object factory.
};

} // namespace ns3
#endif /* TRAFFIC_INFO_HELPER_H */