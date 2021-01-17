/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "ns3/log.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include <string>
#include <stdlib.h>
#include "ns3/pointer.h"
#include "ns3/trace-source-accessor.h"
#include "line-control.h"

#include <chrono>
#include <ctime>
#include "sumo-ns3-control-util.h"

namespace ns3
{

  NS_LOG_COMPONENT_DEFINE("LineControlDemo");

  NS_OBJECT_ENSURE_REGISTERED(LineControl);

  TypeId
  LineControl::GetTypeId (void)
  {
  static TypeId tid = TypeId ("LineControl")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<LineControl> ()
    .AddAttribute ("Interval",
                   "The time to wait between packets",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&LineControl::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("Velocity", "Last vehicle-velocity value.",
                    UintegerValue (1),
                    MakeUintegerAccessor (&LineControl::m_velocity),
                    MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Client",
                   "TraCI client for SUMO",
                   PointerValue (0),
                   MakePointerAccessor (&LineControl::m_client),
                   MakePointerChecker<TraciClient> ())
  ;
    return tid;
  }

  LineControl::LineControl() {
    NS_LOG_FUNCTION(this);
    m_velocity = 0;
  }
  
  LineControl::~LineControl(){};

  void LineControl::DoDispose (void) {
    NS_LOG_FUNCTION(this);
    Application::DoDispose ();
  }

  void LineControl::StartApplication(void) { 
    checkScenario(Seconds(0.0));
    Simulator::Schedule(Seconds(10.0), &LineControl::checkScenario, this, m_interval);
  }

  void LineControl::StopApplication(void) {
    //Simulator::Cancel(m_event);
  }

  // Period call this function to check the scenario
  void LineControl::checkScenario(Time interval) {
    
    /*
    // test 10 cars
    if (this->GetNode() == nullptr) return;
    if (!flag_test) {
      if (m_all_nodes.size() == 10) {
        using namespace std::chrono;
        auto start = system_clock::now();
        auto s1_ns3 = Simulator::Now().GetSeconds();
        std::cout<<"Nodes number:"<<m_all_nodes.size()<<std::endl;
        for (int i = 0; i < m_all_nodes.size(); ++i) {
          const std::string vehicleId = m_client->GetVehicleId(m_all_nodes[i]);
          if (vehicleId == "") return;
          libsumo::TraCIPosition pos = m_client->TraCIAPI::vehicle.getPosition(vehicleId);
          std::cout<<vehicleId<<" position: ("<<pos.x<<", "<<pos.y<<")"<<std::endl;
        }

        auto end = system_clock::now();
        auto s2_ns3 = Simulator::Now().GetSeconds();
        auto duration = duration_cast<microseconds>(end - start);
	std::cout<<"debug:"<<s1_ns3<<" "<<s2_ns3<<std::endl;
	auto duration2 = s2_ns3 - s1_ns3;
        std::cout <<"Duration in physical time: " << duration.count()<< "ms" << std::endl;
        std::cout<<"Duration in ns3 simulation time: "<<duration2<<"ms"<<std::endl;
        flag_test = true;
      }
    }
    */


    // Check: if veh0(Bus).x is switch from road 1to2 to 2to3, then speed up to 10m/s
    //        if veh0(Bus).x is in [230, 260], then slow down to 3m/s
    //        if veh0(Bus).x is in [260, inf), then speed up to 10m/s again
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());

    if (vehicleId == "") return;

    if (vehicleId == "veh0") {

      if (GetPosition().first >= 230 && GetPosition().first < 260) { // Tips: the coordinate is different between sumo and xml
        std::cout<<"============target area 1=============="<<std::endl;
        //ChangeMaxSpeed(10);
        //ChangeSpeed(3.0);
        cb_("target 1", "aaaaaaaaaaaaaaa");
      }
      else if (GetPosition().first >= 260) {
        std::cout<<"============target area 2=============="<<std::endl;
        //ChangeMaxSpeed(10);
        //ChangeSpeed(10);
        cb_("target 2", "bbbbbbbbbbbbbbbbbb");
      }

      else if (GetRoadID() == "2to3") {
        std::cout<<"===========veh0 switch road==============="<<std::endl;
        //ChangeMaxSpeed(10);
        //ChangeSpeed(10);
        cb_("target 3", "ccccccccccccccccccc");
      }
    }


/*
    // test to get some sumo info
    // POS v_pos = GetPosition();
    // double v_vel = GetVelocity();


    // if (v_pos.first != -1 && v_pos.second != -1 && v_vel != -1) {
    //     std::cout<<vehicleId<<": position ("<<v_pos.first<<", "<<v_pos.second<<") speed "<<v_vel<<"m/s"<<std::endl;
    // }
    // else {
    //     std::cout<<vehicleId<<" may leave the simulation."<<std::endl;
    //     return;
    // }

*/
    

     /*
     // test physical and simualtion time diff
     using namespace std::chrono;

     // Physical time:
      system_clock::time_point now = system_clock::now();
      std::chrono::nanoseconds d = now.time_since_epoch();
      std::chrono::milliseconds millsec = std::chrono::duration_cast<std::chrono::milliseconds>(d);
      std::cout<<"Real Physical Time:"<<millsec.count()<<"ms"<<std::endl;

      // Ns3 simulation time:
      std::cout<<"Ns3 Simulation Time:"<<Simulator::Now().GetMilliSeconds()<<"ms"<<std::endl;
    */

    Simulator::Schedule(interval, &LineControl::checkScenario, this, m_interval);
  }

   std::string LineControl::GetRoadID() {  // like "1to2", "2to3" in xx.edge.xml
    if (this->GetNode() == nullptr) return "";
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());
    if (vehicleId == "") return "";
    std::string roadId = m_client->TraCIAPI::vehicle.getRoadID(vehicleId);
    return roadId;
  }

  POS LineControl::GetPosition() {
    if (this->GetNode() == nullptr) return {-1, -1};
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());
    if (vehicleId == "") return {-1, -1};
    libsumo::TraCIPosition pos = m_client->TraCIAPI::vehicle.getPosition(vehicleId);
    POS t = {pos.x, pos.y};
    return t;
  }

  double LineControl::GetVelocity() {
    if (this->GetNode() == nullptr) return -1;
    //if (m_velocity) return m_velocity;
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());
    if (vehicleId == "") return -1;
    m_velocity = m_client->TraCIAPI::vehicle.getSpeed(vehicleId);
    return m_velocity;
  }


  void LineControl::ChangeSpeed(double vel) {
    if (this->GetNode() == nullptr) return;
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());
    if (vehicleId == "") return;
    m_client->TraCIAPI::vehicle.setSpeed(vehicleId, vel);
    m_velocity = vel;
  }


  void LineControl::ChangeMaxSpeed(double vel) {
    if (this->GetNode() == nullptr) return;
    const std::string vehicleId = m_client->GetVehicleId(this->GetNode());
    if (vehicleId == "") return;
    m_client->TraCIAPI::vehicle.setMaxSpeed(vehicleId, vel);
  }
} // Namespace ns3
