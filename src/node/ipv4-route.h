/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 University of Washington
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
 *
 */
#ifndef IPV4_ROUTE_H
#define IPV4_ROUTE_H

#include <list>
#include <vector>
#include <ostream>

#include "ns3/ref-count-base.h"
#include "ipv4-address.h"

namespace ns3 {

class NetDevice;

/**
 *\brief Ipv4 route cache entry (similar to Linux struct rtable)
 *
 * In the future, we will add other entries from struct dst_entry, struct rtable, and struct dst_ops as needed.
 */
class Ipv4Route : public RefCountBase {
public:
  Ipv4Route ();

  void SetDestination (Ipv4Address dest);
  Ipv4Address GetDestination (void) const;

  void SetSource (Ipv4Address src);
  Ipv4Address GetSource (void) const;

  void SetGateway (Ipv4Address gw);
  Ipv4Address GetGateway (void) const;

  // dst_entry.dev
  void SetOutputDevice (Ptr<NetDevice> outputDevice);
  Ptr<NetDevice> GetOutputDevice (void) const;

#ifdef NOTYET
  // rtable.idev
  void SetInputIfIndex (uint32_t iif);
  uint32_t GetInputIfIndex (void) const;
#endif

private:
  Ipv4Address m_dest;
  Ipv4Address m_source;
  Ipv4Address m_gateway;
  Ptr<NetDevice> m_outputDevice;
#ifdef NOTYET
  uint32_t m_inputIfIndex;
#endif
};

std::ostream& operator<< (std::ostream& os, Ipv4Route const& route);

/**
 *\brief Ipv4 multicast route cache entry (similar to Linux struct mfc_cache)
 */
class Ipv4MulticastRoute : public RefCountBase {
public:
  Ipv4MulticastRoute ();

  void SetGroup (const Ipv4Address group);
  Ipv4Address GetGroup (void) const; 

  void SetOrigin (const Ipv4Address group);
  Ipv4Address GetOrigin (void) const; 
  
  void SetParent (uint32_t iif);
  uint32_t GetParent (void) const;

  void SetOutputTtl (uint32_t oif, uint32_t ttl);
  uint32_t GetOutputTtl (uint32_t oif) const;
  
  static const uint32_t MAX_INTERFACES = 16;
  static const uint32_t MAX_TTL = 255;

private:
  Ipv4Address m_group;      // Group 
  Ipv4Address m_origin;     // Source of packet
  uint32_t m_parent;        // Source interface
  std::vector<uint32_t> m_ttls;
};

}//namespace ns3

#endif /* IPV4_ROUTE_H */
