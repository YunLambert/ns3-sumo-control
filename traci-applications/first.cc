/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/sumo-ns3-control-util.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);
  nodes.Create(1);

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
  sumoClient->SetAttribute ("SynchInterval", TimeValue (Seconds (0.001)));
  sumoClient->SetAttribute ("StartTime", TimeValue (Seconds (0.0)));
  sumoClient->SetAttribute ("SumoGUI", BooleanValue (true));
  sumoClient->SetAttribute ("SumoPort", UintegerValue (3400));
  sumoClient->SetAttribute ("PenetrationRate", DoubleValue (1.0));  // portion of vehicles equipped with wifi
  sumoClient->SetAttribute ("SumoLogFile", BooleanValue (true));
  sumoClient->SetAttribute ("SumoStepLog", BooleanValue (false));
  sumoClient->SetAttribute ("SumoSeed", IntegerValue (10));
  sumoClient->SetAttribute ("SumoAdditionalCmdOptions", StringValue ("--verbose true"));
  sumoClient->SetAttribute ("SumoWaitForSocket", TimeValue (Seconds (1)));


  SetTraci(sumoClient);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  InternetStackHelper stack;
  stack.Install (nodes);
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

// callback function for node creation
  std::function<Ptr<Node> ()> setupNewWifiNode = [&] () -> Ptr<Node>
    {
      if (nodeCounter >= nodes.GetN())
        NS_FATAL_ERROR("Node Pool empty!: " << nodeCounter << " nodes created.");
      Ptr<Node> includedNode = nodes.Get(nodeCounter);
      
      if (nodeCounter == 1) {
        UdpEchoServerHelper echoServer (9);
        ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
        serverApps.Start (Seconds (0.0));
        serverApps.Stop (Seconds (10.0));
      }
      if (nodeCounter == 0) {

      UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
      echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
      echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
      echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

      ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
      clientApps.Start (Seconds (1.0));
      clientApps.Stop (Seconds (10.0));
      }


      ++nodeCounter;// increment counter for next node
      return includedNode;
    };

// callback function for node shutdown
  std::function<void (Ptr<Node>)> shutdownWifiNode = [] (Ptr<Node> exNode)
    {
      return nullptr;
    };

  // start traci client with given function pointers
  g_traci_client->SumoSetup (setupNewWifiNode, shutdownWifiNode);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
