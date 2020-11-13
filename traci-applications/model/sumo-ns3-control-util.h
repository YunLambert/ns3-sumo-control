#pragma once

#include "ns3/node.h"
#include <map>
#include "ns3/traci-client.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/node.h"
#include <vector>
#include <string>

namespace ns3 {

class LineControl;
extern std::map<Ptr<Node>, LineControl*> LineControlMap;  // DEPRECATED: Now it is not completely removed
extern Ptr<TraciClient> g_traci_client;


// Make the interface public and global, so in other module, we can use it easily.
// TODO: Is there any better idea replace the global function ?
void SetTraci(Ptr<TraciClient> m_client);
double GetVelocity(Ptr<Node> node);
void ChangeSpeed(Ptr<Node> node, double vel);
void ChangeMaxSpeed(Ptr<Node> node, double vel);


} // namespace ns3

