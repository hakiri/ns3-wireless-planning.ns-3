/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * You
 *  should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */

#include "ns3/core-module.h"
#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/helper-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/common-module.h" 
#include "ns3/contrib-module.h"

#include "wireless-planning/net-test.h"
#include "wireless-planning/create-network.h"
#include "wireless-planning/network-config.h" 
#include "wireless-planning/print.h"
#include "wireless-planning/netinfo-reader.h"
#include "wireless-planning/report-2-config-data.h"

#include <map>
#include <string>
#include <vector>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("main");

/**
 * @brief network main Script (read from netinfo text file)
 */

int
main (int argc, char *argv[])
{

Time eos = Seconds (12);// End Of Simulation in seconds, necessary to finish some methods.
//g_eos??

double dou = 3;
int inte = 0;

inte = dou;

NS_LOG_DEBUG ("inte/dou " << inte << "/" << dou );

 /*
  * Network Creation
  */
 string netInfoFile = "cusco-wifi-24.txt";
 NS_LOG_INFO ("Getting data in order to create and configure the network...");
 CommandLine cmd;
 cmd.AddValue ("NetInfoFile", "Network Information File", netInfoFile);
 cmd.Parse (argc, argv);
 
 NetDataStruct::NetData netData = NetinfoReader::Read ((netInfoFile));
 Print::Netinfo (netData);
 NetworkConfig::NetworkData networkData = Report2ConfigData::NetData2NetworkData (netData);
 Print::NetworkData (networkData);

 NS_LOG_INFO ("Creating the network...");
 CreateNetwork createNetwork;
 NodeContainer nodes = createNetwork.Create (networkData);
 Print::NodeList (nodes);

 /*
  * Applications
  */
  NetTest netTest;

//  netTest.Echo ("Urcos", "Huiracochan", 1);
//  netTest.Echo ("Huiracochan", "Urcos", 1.5);

  /*
   * WiMAX part
   */
//  netTest.EnablePcap("Josjojauarina 2", 1);
//  netTest.EnablePcap("Josjojauarina 2", 2);
//  netTest.EnablePcap("Kcauri", 1);
  //netTest.EnablePcap("Huiracochan", 1);
  
//  netTest.SetWimaxServiceFlow ("Urcos", "Ccatcca", 1, ServiceFlow::SF_TYPE_RTPS, 17, ServiceFlow::SF_DIRECTION_DOWN);
//  netTest.ApplicationSetup ("Urcos", "Ccatcca", 9 , 1.0, 3.0, "1024kbps", 256, NULL);
////
//  netTest.SetWimaxServiceFlow ("Urcos", "Kcauri", 1, ServiceFlow::SF_TYPE_RTPS, 17, ServiceFlow::SF_DIRECTION_DOWN);
//  netTest.ApplicationSetup ("Urcos", "Kcauri", 9 , 2.0, 4.0, "4096kbps", 256, NULL);

 // OnOff
 /// Short simulations
  string rate = "6Mbps";
 AppState appState1 (AC_VO);
 netTest.ApplicationSetup ("Urcos", "Huiracochan", 1, 10, rate, 1452, &appState1);
// AppState appState2 (AC_VI);
// netTest.ApplicationSetup ("Urcos", "Huiracochan", 2, 10, rate, 1452, &appState2);
// AppState appState3 (AC_VO);
// netTest.ApplicationSetup ("Huiracochan", "Urcos", 3, 10, rate, 1452, &appState3);
// AppState appState4 (AC_BE);
// netTest.ApplicationSetup ("Huiracochan", "Urcos", 4, 10, rate, 1452, &appState4);

 vector < string > flowNames;
 flowNames.push_back ("Urcos-Huiracochan VO 1-10 " + rate);
 flowNames.push_back ("Urcos-Huiracochan VI 2-10 " + rate);
 flowNames.push_back ("Huiracochan-Urcos VO 3-10 " + rate);
 flowNames.push_back ("Huiracochan-Urcos BE 4-10 " + rate);

 /*
  * Setup all the plot system: throughput measurement, gnuplot issues...
  */
 NetMeasure netMeasure (eos, Seconds (0.1));
 netMeasure.SetupPlot ();
 netMeasure.SetFlowNames (flowNames);

 netMeasure.SetFlowMonitor (nodes);
 netMeasure.GetFlowStats ();

 Simulator::Stop (eos);
 NS_LOG_INFO ("Starting simulation...");

 Simulator::Run ();
 Simulator::Destroy ();

 /*
  * After simulation
  */
 NS_LOG_INFO ("Done.");

 return 0;
}
