/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
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

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/traci-applications-module.h"
#include "ns3/traci-module.h"
#include <cmath>
#include <map>
#include <functional>
#include <stdlib.h>
#include <iostream>

using namespace ns3;

std::map<Ptr<const MobilityModel>, int> g_id; 
static int cnt = 0;

static void 
PositionInNs3Change (std::string foo, Ptr<const MobilityModel> mobility)
{
  if (g_id.count(mobility) == 0) g_id[mobility] = cnt++;

  Vector pos = mobility->GetPosition ();
  Vector vel = mobility->GetVelocity ();
//   std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
//             << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
//             << ", z=" << vel.z << std::endl;
  std::cout<<"NS3 Car: "<<g_id[mobility]<<" POS=("<<pos.x<<", "<<pos.y<<", "<<pos.z<<")"
           <<"  Velocity="<<std::sqrt(vel.x*vel.x + vel.y*vel.y+vel.z*vel.z)<<std::endl;
}

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  ns3::Time simulationTime (ns3::Seconds(500));
  NodeContainer c;
  c.Create (3);
  uint32_t nodeCounter(0);

  MobilityHelper mobility;

  Ptr<UniformDiscPositionAllocator> positionAlloc = CreateObject<UniformDiscPositionAllocator> ();
  positionAlloc->SetX (320.0);
  positionAlloc->SetY (320.0);
  positionAlloc->SetRho (25.0);
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.InstallAll ();

  Ptr<TraciClient> sumoClient = CreateObject<TraciClient> ();
  sumoClient->SetAttribute ("SumoConfigPath", StringValue ("src/traci/examples/line-simple/line.sumo.cfg"));
  sumoClient->SetAttribute ("SumoBinaryPath", StringValue ("/usr/bin/"));    // use system installation of sumo
  sumoClient->SetAttribute ("SynchInterval", TimeValue (Seconds (0.1)));
  sumoClient->SetAttribute ("StartTime", TimeValue (Seconds (0.0)));
  sumoClient->SetAttribute ("SumoGUI", BooleanValue (true));
  sumoClient->SetAttribute ("SumoPort", UintegerValue (3400));
  sumoClient->SetAttribute ("PenetrationRate", DoubleValue (1.0));  // portion of vehicles equipped with wifi
  sumoClient->SetAttribute ("SumoLogFile", BooleanValue (true));
  sumoClient->SetAttribute ("SumoStepLog", BooleanValue (false));
  sumoClient->SetAttribute ("SumoSeed", IntegerValue (10));
  sumoClient->SetAttribute ("SumoAdditionalCmdOptions", StringValue ("--verbose true"));
  sumoClient->SetAttribute ("SumoWaitForSocket", TimeValue (Seconds (1.0)));

  LineControlHelper lineControl;
  lineControl.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  lineControl.SetAttribute ("Client", (PointerValue) (sumoClient));

// callback function for node creation
  std::function<Ptr<Node> ()> setupNewWifiNode = [&] () -> Ptr<Node>
    {
      if (nodeCounter >= c.GetN())
        NS_FATAL_ERROR("Node Pool empty!: " << nodeCounter << " nodes created.");

      // don't create and install the protocol stack of the node at simulation time -> take from "node pool"
      Ptr<Node> includedNode = c.Get(nodeCounter);
      ++nodeCounter;// increment counter for next node

      // Install Application
      ApplicationContainer LineControlApps = lineControl.Install (includedNode);
      LineControlApps.Start (Seconds (0.0));
      LineControlApps.Stop (simulationTime);

      return includedNode;
    };

  // callback function for node shutdown
  std::function<void (Ptr<Node>)> shutdownWifiNode = [] (Ptr<Node> exNode)
    {
      // stop all applications
      Ptr<LineControl> xlineControl = exNode->GetApplication(0)->GetObject<LineControl>();
      if(xlineControl)
        xlineControl->StopControlNow();

      // set position outside communication range
      Ptr<ConstantPositionMobilityModel> mob = exNode->GetObject<ConstantPositionMobilityModel>();
      //mob->SetPosition(Vector(-100.0+(rand()%25),320.0+(rand()%25),250.0));// rand() for visualization purposes
    };

  // start traci client with given function pointers
  sumoClient->SumoSetup (setupNewWifiNode, shutdownWifiNode);


  //Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
  //                 MakeCallback (&PositionInNs3Change));

  Simulator::Stop (simulationTime);

  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}
