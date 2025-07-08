// ****************************************************************************************
//Self Organizing Networking Group (SONG) @ Kongu Engineering College (KEC) 

//*****************************************************************************************
// Creating TCP Bulk send Application for AD-HOC scenario (Mobility Model & Energy Included)



#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/energy-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/vht-phy.h"
#include "ns3/aodv-module.h"
#include "ns3/config-store-module.h"
#include "ns3/tcp-header.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/netanim-module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


NS_LOG_COMPONENT_DEFINE ("adhoc-2nodes");

using namespace ns3;

int main (int argc, char *argv[])
{
  bool verbose = true;   
  bool enableFlowMonitor = true;
 std::string traceFile = "scratch/10nodes.dat.ns_movements";
  uint32_t maxBytes = 0;
  double interval = 0.5;
  bool total_energy = true;
  bool node_energy = true;
  CommandLine cmd;
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("maxBytes", "Total number of bytes for application to send", maxBytes);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("total_energy", "enables the energy consumed by network tracing", total_energy);
  cmd.AddValue ("node_energy" , "enables the energy consumed by one single node tracing",
node_energy);
  cmd.Parse (argc, argv);
  
  // Adding error model
 Config::SetDefault("ns3::RateErrorModel::ErrorRate", DoubleValue(0.02));
 Config::SetDefault("ns3::RateErrorModel::ErrorUnit", StringValue("ERROR_UNIT_PACKET"));
 Config::SetDefault("ns3::RateErrorModel::RanVar",StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"));   
 
    // Adding TCP credentials
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpCubic"));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1000));

  if (verbose)
    {
      LogComponentEnable ("TcpCubic", LOG_LEVEL_INFO);
      LogComponentEnable ("TcpCubic", LOG_LEVEL_INFO);
    }

//
// Create the nodes and adding mobility model
//
  NS_LOG_INFO ("Create nodes.");
  Ns2MobilityHelper ns2mobility = Ns2MobilityHelper (traceFile);
    NodeContainer nodes;
  nodes.Create (10);
 //  mobility.Install(nodes); 
  ns2mobility.Install ();



  // Set up WiFi MAC and PHY layer parameters
  WifiMacHelper wifiMac;
  WifiHelper wifi;
  YansWifiPhyHelper wifiPhy;
  
  //Enablling PCAP link Type
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  
  // Propagation Loss Model

  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::RandomPropagationDelayModel");
 
 
  wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel", "Distance0", DoubleValue(1.0), "Distance1", DoubleValue(40.0), "Distance2", DoubleValue(70.0),"Exponent0", DoubleValue(1.0),"Exponent1", DoubleValue(3.0), "Exponent2", DoubleValue(10.0));


  wifiPhy.SetChannel (wifiChannel.Create ());

 // MIMO Antenna
  wifiPhy.Set("Antennas", UintegerValue(3));
  wifiPhy.Set("MaxSupportedTxSpatialStreams", UintegerValue(2));
  wifiPhy.Set("MaxSupportedRxSpatialStreams", UintegerValue(2));
 
 // Set 802.11n standard
 
  wifi.SetStandard(WIFI_STANDARD_80211n);               
  wifiPhy.Set("ChannelSettings", StringValue("{46, 40, BAND_5GHZ, 0}"));

// Rate Control Algorithm

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue("HtMcs7"),
                                "ControlMode",StringValue("HtMcs0"));
// Set the nodes to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");

// NIC Details

  NetDeviceContainer devices;
  devices = wifi.Install (wifiPhy, wifiMac, nodes);

/** Energy Model **/
 /***************************************************************************/
 /* energy source */
 BasicEnergySourceHelper basicSourceHelper;
 // configure energy source
 basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (10.0));
 basicSourceHelper.Set ("BasicEnergySupplyVoltageV",DoubleValue(3.6));
 basicSourceHelper.Set ("PeriodicEnergyUpdateInterval",TimeValue(Seconds(1000000000.0)));
 // install source
 EnergySourceContainer sources = basicSourceHelper.Install (nodes);
 /* device energy model */
 WifiRadioEnergyModelHelper radioEnergyHelper;
 // configure radio energy model (Texas Instruments CC3235s Data sheet)
 radioEnergyHelper.Set ("IdleCurrentA", DoubleValue (0.000710));//710uA
 radioEnergyHelper.Set ("CcaBusyCurrentA", DoubleValue (0.000426));//0.426mA
 radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.223));//223mA
 radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.059));//59mA
 radioEnergyHelper.Set ("SwitchingCurrentA", DoubleValue (0.0000045));//4.5uA
 radioEnergyHelper.Set ("SleepCurrentA", DoubleValue (0.000120));//120uA
 DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources);
 /***************************************************************************/


//  Enable AODV
    AodvHelper aodv;
 
  // Set up internet stack
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv); // has effect on the next Install ()
    internet.Install(nodes);


  // Set up Addresses
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);


// Fixing TCP source and destination pair 1

  Ptr<Node> appSource1 = NodeList::GetNode (0);
  Ptr<Node> appSink1 = NodeList::GetNode (1);
  Ipv4Address remoteAddr1 = appSink1->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();

// Fixing TCP source and destination for Bulk transfer Application

  BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr1, 1));
  source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer apps1 = source1.Install (nodes.Get  (0));
  apps1.Start (Seconds (1));
  apps1.Stop (Seconds (10));
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 1));
  apps1 = sink1.Install (nodes.Get (1));


// Trace devices (pcap)
//  wifiPhy.EnablePcap ("ad-hoc", devices);
   
   AsciiTraceHelper ascii;
   wifiPhy.EnableAsciiAll (ascii.CreateFileStream("adhoc-2nodes.tr"));

  // Flow Monitor
  FlowMonitorHelper flowmonHelper;
	 if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }

//
// Intiating simulation.
//
  AnimationInterface anim ("adhoc-2nodes.xml"); // Mandatory
  Simulator::Stop (Seconds(10.0));
  Simulator::Run ();
  if (enableFlowMonitor)
    {
	   flowmonHelper.SerializeToXmlFile ("adhoc-2nodes.flowmon", true, true);
    }
  Simulator::Destroy ();
  return 0;
}
