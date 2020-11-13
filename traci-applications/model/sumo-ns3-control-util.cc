#pragma once

#include "ns3/node.h"
#include "sumo-ns3-control-util.h"

namespace ns3 {

class LineControl;
std::map<Ptr<Node>, LineControl*> LineControlMap;  // DEPRECATED 

Ptr<TraciClient> g_traci_client;

void SetTraci(Ptr<TraciClient> m_client) {
   g_traci_client = m_client;
}

double GetVelocity(Ptr<Node> node) {
    if (node == nullptr || g_traci_client == nullptr) return -1;
    const std::string vehicleId = g_traci_client->GetVehicleId(node);
    if (vehicleId == "") return -1;
    double m_velocity = g_traci_client->TraCIAPI::vehicle.getSpeed(vehicleId);
    return m_velocity;
}


void ChangeSpeed(Ptr<Node> node, double vel) {
	if (node == nullptr) return;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") return;
	g_traci_client->TraCIAPI::vehicle.setSpeed(vehicleId, vel);
}


void ChangeMaxSpeed(Ptr<Node> node, double vel) {
	if (node == nullptr) return;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") return;
	g_traci_client->TraCIAPI::vehicle.setMaxSpeed(vehicleId, vel);
}

} // namespace ns3

