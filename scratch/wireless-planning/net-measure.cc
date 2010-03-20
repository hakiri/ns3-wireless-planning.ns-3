/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*-- */
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/on-off-helper.h"
#include "ns3/random-variable.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/data-rate.h"
#include "ns3/uinteger.h"
#include "ns3/object.h"
#include "ns3/ipv4.h"
#include "ns3/names.h"
#include "ns3/flow-monitor.h"

#include <string>
#include <sstream>
#include <map>

#include "net-measure.h"

NS_LOG_COMPONENT_DEFINE ("net-measure");

using namespace std;

namespace ns3
{

 NetMeasure::NetMeasure ()
 : m_eos (Seconds (100)),
 m_interval (Seconds (1))
 {
  NS_LOG_INFO ("EOS " << m_eos << " Interval " << m_interval);
 }

 NetMeasure::NetMeasure (Time eos, Time interval)
 : m_eos (eos),
 m_interval (interval)
 {
  NS_LOG_INFO ("EOS " << m_eos << " Interval " << m_interval);
 }

 NetMeasure::~NetMeasure () { }

 TypeId
 NetMeasure::GetTypeId (void)
 {
  static TypeId tid = TypeId ("ns3::NetMeasure")
          .SetParent<Object > ()
          ;
  return tid;
 }

 void
 NetMeasure::SetupPlot ()
 {
  NS_LOG_INFO ("Setup plot");

  Measurement measurement;
  MeasurementData measData;
  PlotData plotData;

  measurement = "Throughput";
  measData.plotType = TIMEDOMAIN;
  plotData.title = "Throughput";
  plotData.y = "Throughput [Mbps]";
  plotData.x = "Time [sec]";
  measData.plotData = plotData;
  m_measDataSet [measurement] = measData;

  measurement = "Delay";
  measData.plotType = HISTOGRAM;
  plotData.title = "Delay Histogram";
  plotData.x = "Delay [ms]";
  plotData.y = "Frequency [%]";
  measData.plotData = plotData;
  m_measDataSet [measurement] = measData;

  Simulator::Schedule (m_eos, &NetMeasure::EndPlot, this); // call to EndPlot at End of Simulation
 }

 void
 NetMeasure::EndPlot ()
 {
  NS_LOG_INFO ("End Plot");

  for (MeasDataSet::const_iterator it = m_measDataSet.begin (); it != m_measDataSet.end (); ++it)
   {
    string measurement = it->first;
    MeasurementData measData = it->second;

    PlotData plotData = measData.plotData;
    // svg or png
    string ext = "png";
    Gnuplot gnuplot ("net-measure_" + measurement + "." + ext, plotData.title);
    gnuplot.SetTerminal (ext); // png: non detected terminal
    gnuplot.SetLegend (plotData.x, plotData.y);

    switch (measData.plotType)
     {
     case TIMEDOMAIN:
      {
       for (uint16_t i = 0; i < measData.flowsTimeData.size (); i++)
        {
         TimeData timeData = measData.flowsTimeData.at (i);
         double time = timeData.initTime;
         vector < double > data = timeData.data;
         Gnuplot2dDataset plotDataSet;

         if (m_flowNames.size () != measData.flowsTimeData.size ())
          {
           ostringstream flowName;
           flowName << "Flow " << i;
           m_flowNames.push_back (flowName.str ());
          }
         plotDataSet.SetTitle (m_flowNames.at (i));

         plotDataSet.Add (time - m_interval.GetSeconds (), 0); // decoration
         for (uint32_t j = 0; j < data.size (); j++)
          {
           plotDataSet.Add (time, data.at (j));
           time += m_interval.GetSeconds ();
          }
         gnuplot.AddDataset (plotDataSet);
        }
       break;
      }
     case HISTOGRAM:
      {
       // FSTEP see gnuplot help styles fsteps vs histeps
       // style histograms not implemented in gnuplot.cc
       enum Gnuplot2dDataset::Style style = Gnuplot2dDataset::FSTEPS;
       for (uint16_t i = 0; i < measData.flowsHistData.size (); i++)
        {
         HistData hist = measData.flowsHistData.at (i);
         Gnuplot2dDataset plotDataSet;
         plotDataSet.SetStyle (style);

         if (m_flowNames.size () != measData.flowsHistData.size ())
          {
           ostringstream flowName;
           flowName << "Flow " << i;
           m_flowNames.push_back (flowName.str ());
          }
         plotDataSet.SetTitle (m_flowNames.at (i));
         for (uint32_t j = 0; j < hist.data.size (); j++)
          {
           plotDataSet.Add (hist.data.at (j), hist.freq.at (j));
          }
                 plotDataSet.Add (hist.data.back ()  + hist.width, 0); // decoration
         gnuplot.AddDataset (plotDataSet);
        }
       break;
      }
     }
    string file = "net-measure_" + measurement + ".plt";
    ofstream outfile (file.c_str ());
    gnuplot.GenerateOutput (outfile);
   }
 }

 void
 NetMeasure::EndFlowMonitor ()
 {
  m_flowMon->SerializeToXmlFile ("FlowMonitor.xml", true, true);
 }

 void
 NetMeasure::SetFlowMonitor (NodeContainer &nodes)
 {
  NS_LOG_INFO ("Set the flow monitor");
  FlowMonitorHelper flowmonHelper;
  m_flowMon = flowmonHelper.Install (nodes);

  Simulator::Schedule (m_eos, &NetMeasure::EndFlowMonitor, this); // call to EndPlot at End of Simulation
 }

 void
 NetMeasure::InitFlowStats (FlowMonitor::FlowStats &flowStats)
 {
  // alt: flowStat = {0};
  flowStats.delaySum = Seconds (0);
  flowStats.jitterSum = Seconds (0);
  flowStats.lastDelay = Seconds (0);
  flowStats.txBytes = 0;
  flowStats.rxBytes = 0;
  flowStats.txPackets = 0;
  flowStats.rxPackets = 0;
  flowStats.lostPackets = 0;
  flowStats.timesForwarded = 0;
  flowStats.delayHistogram.SetDefaultBinWidth (0.1); // this values is not set propertly; by default 0.001
  //flowStats.jitterHistogram.SetDefaultBinWidth (m_jitterBinWidth);
  //flowStats.packetSizeHistogram.SetDefaultBinWidth (m_packetSizeBinWidth);
 }

 void
 NetMeasure::ExpandFlowStats (uint32_t oldSize, uint32_t newSize)
 {
  NS_LOG_INFO ("Old vs New size " << oldSize << "/" << newSize);
  // FlowStats
  m_vOldFlowStats.resize (newSize);
  for (uint32_t i = oldSize; i < newSize; i++)
   {
    InitFlowStats (m_vOldFlowStats.at (i));
   }

  // Measurements
  for (MeasDataSet::const_iterator it = m_measDataSet.begin (); it != m_measDataSet.end (); ++it)
   {
    Measurement measurement = it->first;
    MeasurementData measData = it->second;
    NS_LOG_DEBUG ("measurement " << measurement);

    if (measData.plotType == TIMEDOMAIN)
     {
      measData.flowsTimeData.resize (newSize);
      Time time = Simulator::Now ();
      double timeD = time.GetSeconds ();
      TimeData timeData;
      timeData.initTime = timeD;
      for (uint32_t i = oldSize; i < newSize; i++)
       {
        measData.flowsTimeData.at (i).initTime = timeD;
       }
     }
    else if (measData.plotType == HISTOGRAM)
     {
      measData.flowsHistData.resize (newSize);
     }
    m_measDataSet [measurement] = measData; // update
   }
 }

 vector < string >
 NetMeasure::GetFlowNames ()
 {
  return m_flowNames;
 }

 void
 NetMeasure::SetFlowNames (vector < string > flowNames)
 {
  m_flowNames = flowNames;
 }

 double
 NetMeasure::CalcThroughput (FlowMonitor::FlowStats news, FlowMonitor::FlowStats olds)
 {
  //Throughput at MAC level: Data + UDP header 8 bytes + IP header 20 bytes
  double rate = ((((news.rxBytes - olds.rxBytes) * 8.0) / 1000000) / m_interval.GetSeconds ());
  NS_LOG_DEBUG (" Throughput " << rate << " Mbps ");

  return rate;
 }

 double
 NetMeasure::CalcMeanDelay (FlowMonitor::FlowStats news)
 {
  double meanDelay = news.delaySum.GetSeconds () / news.rxPackets;
  NS_LOG_DEBUG (" mean delay " << meanDelay << " sec");

  return meanDelay;
 }

 NetMeasure::HistData
 NetMeasure::CalcDelayHist (FlowMonitor::FlowStats stats)
 {
  NS_LOG_INFO ("Calculating delay histogram");
  HistData histData;

  Histogram hist = stats.delayHistogram;
  double delay;
  double freq;
  double width = hist.GetBinWidth (0); // All bins have the same width
  histData.width = width;
  uint32_t rxPackets = stats.rxPackets;

  for (uint32_t index = 0; index < hist.GetNBins (); index++)
   {
    delay = index * width * 10e3;// ms
    double count = hist.GetBinCount (index);
    freq = count / rxPackets * 100; // Normalize to 100

    histData.data.push_back (delay);
    histData.freq.push_back (freq);
    NS_LOG_INFO (" dealy / freq: " << delay << "/" << freq);
   }
  return histData;
 }

 void
 NetMeasure::GetFlowStats ()
 {
  MonStats stats;
  Time time = Simulator::Now ();
  double timeD = time.GetSeconds ();
  NS_LOG_INFO ("Get flow stats at " << timeD << "s");

  m_flowMon->CheckForLostPackets ();
  stats = m_flowMon->GetFlowStats ();

  uint32_t oldSize = m_vOldFlowStats.size ();
  uint32_t newSize = stats.size ();
  if (newSize > oldSize)
   {
    ExpandFlowStats (oldSize, newSize);
   }

  FlowMonitor::FlowStats newStats;
  // Flows
  for (MonStats::const_iterator it = stats.begin (); it != stats.end (); ++it)
   {
    uint32_t i = it->first - 1; // FlowID = uint32_t, beigins from 1.
    newStats = it->second;

    NS_LOG_DEBUG (" FlowID " << it->first);

    FlowMonitor::FlowStats oldStats = m_vOldFlowStats.at (i);

    // Measurements
    for (MeasDataSet::const_iterator ite = m_measDataSet.begin (); ite != m_measDataSet.end (); ++ite)
     {
      Measurement measurement = ite->first;
      MeasurementData measData = ite->second;

      NS_LOG_DEBUG ("measurement " << measurement);

      switch (measData.plotType)
       {
       case TIMEDOMAIN:
        double data;
        if (measurement.compare ("Throughput") == 0)
         {
          data = CalcThroughput (newStats, oldStats);
         }
        else if (measurement.compare ("delaySum") == 0)
         {
          data = CalcMeanDelay (newStats);
         }
        else if (measurement.compare ("lostPackets") == 0)
         {
          data = newStats.lostPackets;
         }
        measData.flowsTimeData.at (i).data.push_back (data);
        break;
       case HISTOGRAM:// Works only in the last iteration
        {
         Time lastIteration = m_eos - m_interval;
         double lastIterationD = lastIteration.GetSeconds (); // errors debugging with timeD
         /// Doesn't work comparing directly time.GetSeconds () == lastIteration.GetSeconds ()
         if (timeD == lastIterationD)
          {
           HistData histData;
           NS_LOG_INFO ("last iteration:  " << timeD);
           if (measurement.compare ("Delay") == 0)
            {
             histData = CalcDelayHist (newStats);
            }
           measData.flowsHistData.at (i) = histData;
          }// end of Last Iteration
         break;
        }
       default:
        NS_LOG_ERROR ("  No correct plot type selected");
        exit (-1);
       }
      m_measDataSet [measurement] = measData;
     }
    // update
    oldStats = newStats;
    // Restore old stats
    m_vOldFlowStats.at (i) = oldStats;
   }
  Simulator::Schedule (m_interval, &NetMeasure::GetFlowStats, this);
 }

} // namespace ns3
