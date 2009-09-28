/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef IPV6_ROUTING_HELPER_H
#define IPV6_ROUTING_HELPER_H

#include "ns3/ptr.h"

namespace ns3 {

class Ipv6RoutingProtocol;
class Node;

/**
 * \brief A factory to create ns3::Ipv6RoutingProtocol objects
 *
 * For each new routing protocol created as a subclass of 
 * ns3::Ipv6RoutingProtocol, you need to create a subclass of 
 * ns3::Ipv6RoutingHelper which can be used by 
 * ns3::InternetStackHelper::SetRoutingHelper and 
 * ns3::InternetStackHelper::Install.
 */
class Ipv6RoutingHelper
{
public:
  virtual ~Ipv6RoutingHelper ();
  /**
   * \param node the node within which the new routing protocol will run
   * \returns a newly-created routing protocol
   */
  virtual Ptr<Ipv6RoutingProtocol> Create (Ptr<Node> node) const = 0;
};

} // namespace ns3


#endif /* IPV6_ROUTING_HELPER_H */

