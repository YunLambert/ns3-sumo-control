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

using EventCallback = std::function<void(std::string event_type, std::string msg)>;

class LineControl;
extern std::map<Ptr<Node>, LineControl*> LineControlMap;  // TODO: Should not use golbal variable, may be have some tricky methods.
extern Ptr<TraciClient> g_traci_client;
extern std::vector<Ptr<Node>> m_all_nodes;

extern bool flag_test;

extern EventCallback cb_;

void SetMyCallback(EventCallback cb);

void SetTraci(Ptr<TraciClient> m_client);
double GetVelocity(Ptr<Node> node);
void ChangeSpeed(Ptr<Node> node, double vel);
void ChangeMaxSpeed(Ptr<Node> node, double vel);
void setSignal(Ptr<Node> node, int signal_num);
int getSignal(Ptr<Node> node);


} // namespace ns3

