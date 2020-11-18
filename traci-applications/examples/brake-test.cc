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
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

Ptr<Node> car, bus;

void ReceivePacket (Ptr<Socket> socket) {
   NS_LOG_INFO ("Received one packet!");
   Ptr<Packet> packet = socket->Recv ();
   uint8_t buffer[4];
   packet->CopyData(buffer, 4);
   if (int(buffer[0]) == 1 && int(buffer[1]) == 0) {
      ChangeSpeed(car, 0);
      std::cout<<"Car Receive brake and slow down!!!"<<std::endl;  
   }
   if (int(buffer[0]) == 1 && int(buffer[1]) == 1) {
      ChangeSpeed(car, 2);
      std::cout<<"Car restart!!"<<std::endl; 
   }
 }
 
void SendBrake (Ptr<Socket> socket) {
     uint8_t brake_info[4] = {0};
     brake_info[0] = 1;
     ChangeSpeed(bus, 0);
     setSignal(bus, 8); // set brake light
     Ptr<Packet> packet = Create<Packet>(brake_info, 10);
     socket->Send (packet);
     //Simulator::Schedule (Seconds(50), &SendBrake, socket);
}

void SendRestart(Ptr<Socket> socket) {
     uint8_t data_info[4] = {0};
     data_info[0] = 1, data_info[1] = 1;  // TODO use packet-header to make data packet
     ChangeSpeed(bus, 1);
     Ptr<Packet> packet = Create<Packet>(data_info, 10);
     socket->Send (packet);
}




int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);
  //nodes.Create(1);

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
  //g_traci_client->SumoControlStart();
   Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  InternetStackHelper stack;
  stack.Install (nodes);
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink, source;
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 4477);
  InetSocketAddress remote = InetSocketAddress (interfaces.GetAddress (1), 4477); // because i know it;s from 0 -> 2


// callback function for node creation
  std::function<Ptr<Node> ()> setupNewWifiNode = [&] () -> Ptr<Node>
    {
      if (nodeCounter >= nodes.GetN())
        NS_FATAL_ERROR("Node Pool empty!: " << nodeCounter << " nodes created.");
      Ptr<Node> includedNode = nodes.Get(nodeCounter);
      if (nodeCounter == 1) {

        recvSink = Socket::CreateSocket (nodes.Get (1), tid);
        car = nodes.Get(1);
        recvSink->Bind (local);
        recvSink->SetRecvCallback (MakeCallback (ReceivePacket));


UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (140.0));
  serverApps.Stop (Seconds (200.0));
      }
      if (nodeCounter == 0) {
         bus = nodes.Get(0);
         source = Socket::CreateSocket (nodes.Get (0), tid);
         source->Connect(remote);
       UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                   Seconds (120), &SendBrake, source); // make brake event!

  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                   Seconds (150), &SendRestart, source); // make restart event!
  clientApps.Start (Seconds (150.0));
  clientApps.Stop (Seconds (200.0));
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

  //LineControlHelper lineControl;
  //lineControl.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  //lineControl.SetAttribute ("Client", (PointerValue) (sumoClient));


 

  

 

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
