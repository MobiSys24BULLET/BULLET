/*
 * Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/lte-pdcp.h"

#include "ns3/log.h"
#include "ns3/lte-pdcp-header.h"
#include "ns3/lte-pdcp-sap.h"
#include "ns3/lte-pdcp-tag.h"
#include "ns3/simulator.h"
#include "ns3/video-stream-tag.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("LtePdcp");

/// LtePdcpSpecificLteRlcSapUser class
class LtePdcpSpecificLteRlcSapUser : public LteRlcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param pdcp PDCP
     */
    LtePdcpSpecificLteRlcSapUser(LtePdcp* pdcp);

    // Interface provided to lower RLC entity (implemented from LteRlcSapUser)
    void ReceivePdcpPdu(Ptr<Packet> p) override;

  private:
    LtePdcpSpecificLteRlcSapUser();
    LtePdcp* m_pdcp; ///< the PDCP
};

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser(LtePdcp* pdcp)
    : m_pdcp(pdcp)
{
}

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser()
{
}

void
LtePdcpSpecificLteRlcSapUser::ReceivePdcpPdu(Ptr<Packet> p)
{
    m_pdcp->DoReceivePdu(p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(LtePdcp);

LtePdcp::LtePdcp()
    : m_pdcpSapUser(nullptr),
      m_rlcSapProvider(nullptr),
      m_rnti(0),
      m_lcid(0),
      m_txSequenceNumber(0),
      m_rxSequenceNumber(0)
{
    NS_LOG_FUNCTION(this);
    m_pdcpSapProvider = new LtePdcpSpecificLtePdcpSapProvider<LtePdcp>(this);
    m_rlcSapUser = new LtePdcpSpecificLteRlcSapUser(this);
}

LtePdcp::~LtePdcp()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LtePdcp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::LtePdcp")
                            .SetParent<Object>()
                            .SetGroupName("Lte")
                            .AddTraceSource("TxPDU",
                                            "PDU transmission notified to the RLC.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_txPdu),
                                            "ns3::LtePdcp::PduTxTracedCallback")
                            .AddTraceSource("RxPDU",
                                            "PDU received.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_rxPdu),
                                            "ns3::LtePdcp::PduRxTracedCallback")
                            .AddTraceSource("AppTrace",
                                            "Uplink packets (including data) received.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_appTrace),
                                            "ns3::LtePdcp::AppTracedCallback")
                            .AddTraceSource("DLTrace",
                                            "Downlink packets (including ACK) received.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_dlTrace),
                                            "ns3::LtePdcp::DlTracedCallback");
    return tid;
}

void
LtePdcp::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete (m_pdcpSapProvider);
    delete (m_rlcSapUser);
}

void
LtePdcp::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << (uint32_t)rnti);
    m_rnti = rnti;
}

void
LtePdcp::SetLcId(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << (uint32_t)lcId);
    m_lcid = lcId;
}

void
LtePdcp::SetLtePdcpSapUser(LtePdcpSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_pdcpSapUser = s;
}

LtePdcpSapProvider*
LtePdcp::GetLtePdcpSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_pdcpSapProvider;
}

void
LtePdcp::SetLteRlcSapProvider(LteRlcSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_rlcSapProvider = s;
}

LteRlcSapUser*
LtePdcp::GetLteRlcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_rlcSapUser;
}

LtePdcp::Status
LtePdcp::GetStatus()
{
    Status s;
    s.txSn = m_txSequenceNumber;
    s.rxSn = m_rxSequenceNumber;
    return s;
}

void
LtePdcp::SetStatus(Status s)
{
    m_txSequenceNumber = s.txSn;
    m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void
LtePdcp::DoTransmitPdcpSdu(LtePdcpSapProvider::TransmitPdcpSduParameters params)
{
    NS_LOG_FUNCTION(this << m_rnti << static_cast<uint16_t>(m_lcid) << params.pdcpSdu->GetSize());
    Ptr<Packet> p = params.pdcpSdu;

    Ptr<Packet> pdcp_p = p->Copy ();
    Ipv4Header ip;
    pdcp_p->RemoveHeader(ip);
    TcpHeader tcp;
    pdcp_p->RemoveHeader(tcp); 

    std::cout<<"PDCP ACK (PDCP) "<<tcp.GetSourcePort()<<" "<<pdcp_p->GetSize()<<" "<<tcp.GetAckNumber()<<std::endl;

    m_dlTrace(ip, tcp);

    // Sender timestamp
    PdcpTag pdcpTag(Simulator::Now());

    LtePdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(m_txSequenceNumber);

    m_txSequenceNumber++;
    if (m_txSequenceNumber > m_maxPdcpSn)
    {
        m_txSequenceNumber = 0;
    }

    pdcpHeader.SetDcBit(LtePdcpHeader::DATA_PDU);

    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    m_txPdu(m_rnti, m_lcid, p->GetSize());

    LteRlcSapProvider::TransmitPdcpPduParameters txParams;
    txParams.rnti = m_rnti;
    txParams.lcid = m_lcid;
    txParams.pdcpPdu = p;

    m_rlcSapProvider->TransmitPdcpPdu(txParams);
}


void
LtePdcp::DoReceivePdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());

    VideoStreamTag app_tag;
    ByteTagIterator i = p->GetByteTagIterator();
    
    TypeId tid = app_tag.GetInstanceTypeId();
    if (p->FindFirstMatchingByteTag(app_tag)){
        while (i.HasNext()){
            ByteTagIterator::Item item = i.Next();
            if (tid == item.GetTypeId())
            {
                item.GetTag(app_tag);
                if (app_tag.m_currentSequence == app_tag.m_byteSequenceNum){
                    m_appTrace((int)app_tag.m_frameNum, (int)app_tag.m_currentSequence, (int)app_tag.m_byteSequenceNum);
                }
            }
        }
    }
    // If current buffer has data of frame
    if (app_tag.m_byteSequenceNum > 0){
        m_appTrace((int)app_tag.m_frameNum, (int)app_tag.m_currentSequence, (int)app_tag.m_byteSequenceNum);
    } 
    ///////////////
    Ipv4Header ipHeader;
    p->PeekHeader(ipHeader);

    TcpHeader tcpHeader;
    p->PeekHeader (tcpHeader);

    // Receiver timestamp
    PdcpTag pdcpTag;
    Time delay;
    p->FindFirstMatchingByteTag(pdcpTag);
    delay = Simulator::Now() - pdcpTag.GetSenderTimestamp();
    m_rxPdu(m_rnti, m_lcid, p->GetSize(), delay.GetNanoSeconds());

    LtePdcpHeader pdcpHeader;
    p->RemoveHeader(pdcpHeader);
    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);

    m_rxSequenceNumber = pdcpHeader.GetSequenceNumber() + 1;
    if (m_rxSequenceNumber > m_maxPdcpSn)
    {
        m_rxSequenceNumber = 0;
    }

    LtePdcpSapUser::ReceivePdcpSduParameters params;
    params.pdcpSdu = p;
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    m_pdcpSapUser->ReceivePdcpSdu(params);
}

} // namespace ns3
