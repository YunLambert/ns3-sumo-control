#pragma once

#include "ns3/traci-client.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include <vector>
#include <string>

namespace ns3 {

class Socket;
class Packet;

typedef std::pair<double, double> POS;

class LineControl : public Application {
public:
  static TypeId GetTypeId(void);
  LineControl();
  virtual ~LineControl();

  void StopControlNow() {
    StopApplication();
  }

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void checkScenario(Time interval);

  // From sumo to ns3 : car info
  std::string GetRoadID();
  POS GetPosition();
  double GetVelocity();

  // From ns3 to sumo : command
  void ChangeSpeed(double vel);
  void ChangeMaxSpeed(double vel);

private:
  Time m_interval; // Period check interval
  //EventId m_event; // Event to check
  Ptr<TraciClient> m_client;
  double m_velocity;
};

} // namespace ns3

