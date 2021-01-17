#pragma once

#include "ns3/node.h"
#include <functional>
#include "sumo-ns3-control-util.h"

namespace ns3 {

class LineControl;
std::map<Ptr<Node>, LineControl*> LineControlMap;

Ptr<TraciClient> g_traci_client;

std::vector<Ptr<Node>> m_all_nodes;

bool flag_test = false;

EventCallback cb_ = nullptr;

void SetMyCallback(EventCallback cb) {
	cb_ = cb;
}

void SetTraci(Ptr<TraciClient> m_client) {
   g_traci_client = m_client;
}

double GetVelocity(Ptr<Node> node) {
    if (node == nullptr) {std::cerr<<"[ERROR] node is null"<<std::endl;return -1;}
    if (g_traci_client == nullptr) {std::cerr<<"[ERROR] traci is null"<<std::endl;return -1;}
    const std::string vehicleId = g_traci_client->GetVehicleId(node);
    if (vehicleId == "") {std::cerr<<"[ERROR] vehicleId is null"<<std::endl;return -1;}
    double m_velocity = g_traci_client->TraCIAPI::vehicle.getSpeed(vehicleId);
    return m_velocity;
}


void ChangeSpeed(Ptr<Node> node, double vel) {
	if (node == nullptr) return;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") {std::cerr<<"[ERROR] vehicleid is null"<<std::endl;return;}
	g_traci_client->TraCIAPI::vehicle.setSpeed(vehicleId, vel);
}


void ChangeMaxSpeed(Ptr<Node> node, double vel) {
	if (node == nullptr) return;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") return;
	g_traci_client->TraCIAPI::vehicle.setMaxSpeed(vehicleId, vel);
}

void setSignal(Ptr<Node> node, int signal_num) {
	if (node == nullptr) return;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") return;
	g_traci_client->TraCIAPI::vehicle.setSignals(vehicleId, signal_num);
}

int getSignal(Ptr<Node> node) {
	if (node == nullptr) return -1;
	const std::string vehicleId = g_traci_client->GetVehicleId(node);
	if (vehicleId == "") return -1;
	return g_traci_client->TraCIAPI::vehicle.getSignals(vehicleId);
}


} // namespace ns3

