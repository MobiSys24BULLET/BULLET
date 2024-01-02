/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "video-stream-client.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4.h"
#include "ns3/video-stream-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VideoStreamClientApplication");

NS_OBJECT_ENSURE_REGISTERED (VideoStreamClient);

TypeId
VideoStreamClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VideoStreamClient")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<VideoStreamClient> ()
    .AddAttribute ("LocalAddress", "The destination address of the outbound packets",
                    AddressValue (),
                    MakeAddressAccessor (&VideoStreamClient::m_localAddress),
                    MakeAddressChecker ())
    .AddAttribute ("LocalPort", "The destination port of the outbound packets",
                    UintegerValue (5000),
                    MakeUintegerAccessor (&VideoStreamClient::m_localPort),
                    MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("RxDelay",
                     "A packet has been received with delay information.",
                     MakeTraceSourceAccessor (&VideoStreamClient::m_rxDelayTrace),
                     "ns3::Application::DelayAddressCallback")
    .AddTraceSource ("LatestFrameNum",
                     "Connection to the destination of video server has been established.",
                     MakeTraceSourceAccessor (&VideoStreamClient::m_latestFrameNum),
                     "ns3::VideoStreamClient::LatestFrameNumCallback")
  ;
  return tid;
}

VideoStreamClient::VideoStreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_initialDelay = 3;
  m_lastBufferSize = 0;
  m_currentBufferSize = 0;
  m_frameSize = 0;
  m_frameRate = 30;
  m_videoLevel = 3;
  m_stopCounter = 0;
  m_lastRecvFrame = 1e6;
  m_rebufferCounter = 0;
  m_bufferEvent = EventId();
  m_sendEvent = EventId();

  m_skipped = 0;

}

VideoStreamClient::~VideoStreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void
VideoStreamClient::SetLocal (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_localAddress = ip;
  m_localPort = port;
}

void 
VideoStreamClient::SetLocal (Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_localAddress = addr;
}

void
VideoStreamClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
VideoStreamClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
        {
          // Find the current default MTU value of TCP sockets.
          Ptr<const ns3::AttributeValue> previousSocketMtu;
          const TypeId tcpSocketTid = TcpSocket::GetTypeId ();
          for (uint32_t i = 0; i < tcpSocketTid.GetAttributeN (); i++)
            {
              struct TypeId::AttributeInformation attrInfo = tcpSocketTid.GetAttribute (i);
              if (attrInfo.name == "SegmentSize")
                {
                  previousSocketMtu = attrInfo.initialValue;
                }
            }

          // Creating a TCP socket to connect to the server.
          m_socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());
          //m_socket->SetAttribute ("SegmentSize", UintegerValue (m_maxPacketSize));

          int ret;

          if (Ipv4Address::IsMatchingType (m_localAddress))
            {
              const Ipv4Address ipv4 = Ipv4Address::ConvertFrom (m_localAddress);
              const InetSocketAddress inetSocket = InetSocketAddress (ipv4,
                                                                      m_localPort);
              NS_LOG_INFO (this << " Binding on " << ipv4
                                << " port " << m_localPort
                                << " / " << inetSocket << ".");
              ret = m_socket->Bind (inetSocket);
              NS_LOG_DEBUG (this << " Bind() return value= " << ret
                                 << " GetErrNo= "
                                 << m_socket->GetErrno () << ".");
            }
          else if (Ipv6Address::IsMatchingType (m_localAddress))
            {
              const Ipv6Address ipv6 = Ipv6Address::ConvertFrom (m_localAddress);
              const Inet6SocketAddress inet6Socket = Inet6SocketAddress (ipv6,
                                                                         m_localPort);
              NS_LOG_INFO (this << " Binding on " << ipv6
                                << " port " << m_localPort
                                << " / " << inet6Socket << ".");
              ret = m_socket->Bind (inet6Socket);
              NS_LOG_DEBUG (this << " Bind() return value= " << ret
                                 << " GetErrNo= "
                                 << m_socket->GetErrno () << ".");
            }

          ret = m_socket->Listen ();
          NS_LOG_DEBUG (this << " Listen () return value= " << ret
                             << " GetErrNo= " << m_socket->GetErrno ()
                             << ".");

          //NS_UNUSED (ret);

        } // end of `if (m_socket == 0)`

  //m_socket->SetAllowBroadcast (true);
  m_socket->SetRecvCallback (MakeCallback (&VideoStreamClient::HandleRead, this));
  m_socket->SetAcceptCallback (MakeCallback (&VideoStreamClient::ConnectionRequestCallback,
                                                        this),
                                          MakeCallback (&VideoStreamClient::NewConnectionCreatedCallback,
                                                        this));
  
  //HandleRead (m_socket);
}

void
VideoStreamClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
  {
    m_socket->Close ();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    m_socket = 0;
  }

  Simulator::Cancel (m_bufferEvent);
}

void
VideoStreamClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  uint8_t dataBuffer[10];
  sprintf((char *) dataBuffer, "%hu", 0);
  Ptr<Packet> firstPacket = Create<Packet> (dataBuffer, 10);
  m_socket->Send (firstPacket);

  if (Ipv4Address::IsMatchingType (m_localAddress))
  {
    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent 10 bytes to " <<
                  Ipv4Address::ConvertFrom (m_localAddress) << " port " << m_localPort);
  }
  else if (Ipv6Address::IsMatchingType (m_localAddress))
  {
    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent 10 bytes to " <<
                  Ipv6Address::ConvertFrom (m_localAddress) << " port " << m_localPort);
  }
  else if (InetSocketAddress::IsMatchingType (m_localAddress))
  {
    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent 10 bytes to " <<
                  InetSocketAddress::ConvertFrom (m_localAddress).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (m_localAddress).GetPort ());
  }
  else if (Inet6SocketAddress::IsMatchingType (m_localAddress))
  {
    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent 10 bytes to " <<
                  Inet6SocketAddress::ConvertFrom (m_localAddress).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (m_localAddress).GetPort ());
  }
  
}

uint32_t 
VideoStreamClient::ReadFromBuffer (void)
{
  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << " s, last buffer size: " << m_lastBufferSize << ", current buffer size: " << m_currentBufferSize);
  if (m_currentBufferSize < m_frameRate) 
  {

    if (m_lastBufferSize == m_currentBufferSize)
    {
      m_stopCounter++;
      // If the counter reaches 3, which means the client has been waiting for 3 sec, and no packets arrived.
      // In this case, we think the video streaming has finished, and there is no need to schedule the event.
      if (m_stopCounter < 3)
      {
        m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
      }
    }
    else
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << " s: Not enough frames in the buffer, rebuffering!");
      m_stopCounter = 0;  // reset the stopCounter
      m_rebufferCounter++;
      m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    }

    m_lastBufferSize = m_currentBufferSize;
    return (-1);
  }
  else
  {
    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << " s: Play video frames from the buffer");
    if (m_stopCounter > 0) m_stopCounter = 0;    // reset the stopCounter
    if (m_rebufferCounter > 0) m_rebufferCounter = 0;   // reset the rebufferCounter
    m_currentBufferSize -= m_frameRate;

    m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    m_lastBufferSize = m_currentBufferSize;
    return (m_currentBufferSize);
  }
}

void
VideoStreamClient::PacketReceived(Ptr<Socket> socket, const Ptr<Packet>& p, const Address& from, const Address& localAddress)
{
    //NS_LOG_UNCOND(Simulator::Now()<< " Packet received at client: "<<p->GetSize());

    //SeqTsSizeHeader header;
    VideoStreamTag tag;
    Ptr<Packet> buffer;

    auto itBuffer = m_buffer.find(from);
    if (itBuffer == m_buffer.end())
    {
        NS_LOG_UNCOND("Add new packet to the buffer");
        itBuffer = m_buffer.insert(std::make_pair(from, Create<Packet>(0))).first;
    }

    buffer = itBuffer->second;
    buffer->AddAtEnd(p);
    //buffer->PeekHeader(header);
    p->FindFirstMatchingByteTag(tag);

    std::cout<<"APP RX: "<<tag.m_frameNum <<" "<<Simulator::Now().GetSeconds()<<" "<< tag.m_currentSequence<<" "<<buffer->GetSize() << std::endl;

    
    ByteTagIterator i = p->GetByteTagIterator();
    TypeId tid = tag.GetInstanceTypeId();

    if (p->FindFirstMatchingByteTag(tag)){
        while(i.HasNext()){
            ByteTagIterator::Item item = i.Next();
            if (tid == item.GetTypeId())
            {
                item.GetTag(tag);
                if (tag.m_byteSequenceNum == 0)
                {
                    NS_LOG_INFO("Wrong tag");
                    continue;
                }
                else if (buffer->GetSize() >= tag.m_byteSequenceNum)
                {
                    //NS_LOG_UNCOND("Removing packet of seq: " <<header.GetSeq()<< " size: "<< header.GetSize() << " from buffer of size "
                    //                                        << buffer->GetSize()<< " last frame num: "<< m_lastRecvFrame);
                    Ptr<Packet> complete = buffer->CreateFragment(0, static_cast<uint32_t>(tag.m_byteSequenceNum));
                    buffer->RemoveAtStart(static_cast<uint32_t>(tag.m_byteSequenceNum));

                    //complete->RemoveHeader(header);

                    uint32_t sourceId = tag.m_sourceId;
                    uint32_t frameNum = tag.m_frameNum;
                    
                    std::cout<<"Client get tag size: "<<tag.m_byteSequenceNum<<" complete: "<<complete->GetSize()<<" source: "<<sourceId<<" curr recv num: "<<frameNum<<" last recv num: "<<m_lastRecvFrame<<std::endl;
                   
                    // Frame number counter
                    if (0)//(frameNum == m_lastRecvFrame)
                    {
                      m_frameSize += complete->GetSize ();
                    }
                    else
                    {
                      if (frameNum >= 0)
                      {
                        m_skipped += (frameNum - m_lastRecvFrame) - 1;
                        Time ts = tag.m_senderTimestamp;
                        Time e2e = Simulator::Now() - ts;
                        //TraceAppRx(frameNum, e2e, m_frameSize);
                        m_latestFrameNum(sourceId, frameNum, e2e, tag.m_byteSequenceNum); // need to add tx source
            
                        uint8_t dataBuffer[10];
                        sprintf((char *) dataBuffer, "%hu", 99); // 99--> end sign
                        Ptr<Packet> txEndPacket = Create<Packet> (dataBuffer, 10);
                        VideoStreamTag end_tag;
                        end_tag.m_frameNum = frameNum;
                        end_tag.m_senderTimestamp = Simulator::Now();
                        txEndPacket->AddByteTag(tag);

                        socket->SendTo (txEndPacket, 0, from);
                        NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds() << "s: transmit end sign "<< ts.GetSeconds());

                        //NS_LOG_UNCOND ("Frame number: "<< frameNum - 1 << "/ E2E latency: " << 
                        //Simulator::Now().GetMilliSeconds() - ts.GetMilliSeconds()  << "ms/ Frame size: "<< m_frameSize << " bytes/ From " <<  InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (from).GetPort ());
                      }
                      m_currentBufferSize++;
                      m_lastRecvFrame = frameNum;
                      std::cout<<"Client get tag size: "<<tag.m_byteSequenceNum<<" complete: "<<complete->GetSize()<<" source2: "<<sourceId<<" curr recv num: "<<frameNum<<" last recv num: "<<m_lastRecvFrame<<std::endl;
                   
                      m_frameSize = complete->GetSize ();
                    }
                }
            }
        }
    }
    else{
      NS_LOG_INFO("There is no tag to evaluate the performance.. is it the intended operation?");
    }
}

void 
VideoStreamClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
  {
    socket->GetSockName (localAddress);
    if (InetSocketAddress::IsMatchingType (from))
    {

      uint8_t recvData[packet->GetSize()];
      packet->CopyData (recvData, packet->GetSize ());

      PacketReceived(socket, packet, from, localAddress);
    }
  }
}

void 
VideoStreamClient::TraceAppRx (uint32_t frameNum, Time time, uint32_t size)
{
}

bool
VideoStreamClient::ConnectionRequestCallback (Ptr<Socket> socket,
                                               const Address &address)
{
  NS_LOG_FUNCTION (this << socket << address<< "connection done");
  return true; // Unconditionally accept the connection request.
}


void  
VideoStreamClient::NewConnectionCreatedCallback (Ptr<Socket> socket,
                                               const Address &address)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds()<<" "<<this << socket << address<< "new connection done");

  socket->SetRecvCallback (MakeCallback (&VideoStreamClient::HandleRead, this));
  m_socketList.push_back (socket);
  //m_connectionEstablishedTrace (socket);
}


} // namespace ns3