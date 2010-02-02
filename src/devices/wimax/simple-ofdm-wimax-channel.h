/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
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
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 *                              <amine.ismail@udcast.com>
 */

#ifndef SIMPLE_OFDM_WIMAX_CHANNEL_H
#define SIMPLE_OFDM_WIMAX_CHANNEL_H

#include <list>
#include "wimax-channel.h"
#include "bvec.h"
#include "wimax-phy.h"
#include "ns3/propagation-loss-model.h"
#include "simple-ofdm-send-param.h"

namespace ns3 {

class Packet;
class PacketBurst;
class SimpleOfdmWimaxPhy;

class SimpleOfdmWimaxChannel : public WimaxChannel
{
public:
  SimpleOfdmWimaxChannel (void);
  ~SimpleOfdmWimaxChannel (void);

  enum PropModel
  {
    RANDOM_PROPAGATION,
    FRIIS_PROPAGATION,
    LOG_DISTANCE_PROPAGATION,
    COST231_PROPAGATION
  };

  SimpleOfdmWimaxChannel (PropModel propModel);

  void Send (Time BlockTime, const bvec &fecBlock,
             uint32_t burstSize, Ptr<WimaxPhy> phy, bool isFirstBlock,
             uint64_t frequency, WimaxPhy::ModulationType modulationType,
             uint8_t direction, uint32_t symbolIndex, double txPowerDbm);

  void SetPropagationModel (PropModel propModel);

private:
  void DoAttach (Ptr<WimaxPhy> phy);
  std::list<Ptr<SimpleOfdmWimaxPhy> > m_phyList;
  uint32_t DoGetNDevices (void) const;
  void EndSend (Ptr<SimpleOfdmWimaxPhy> rxphy, simpleOfdmSendParam * param);
  Ptr<NetDevice> DoGetDevice (uint32_t i) const;
  Ptr<PropagationLossModel> m_loss;
};

} // namespace ns3

#endif /* SIMPLE_OFDM_WIMAX_CHANNEL_H */