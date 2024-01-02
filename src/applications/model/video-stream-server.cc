/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// Original code from https://github.com/guoxiliu/VideoStream-NS3

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/tcp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include <ns3/tcp-socket-factory.h>
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/video-stream-server.h"
#include "ns3/seq-ts-size-header.h"
#include "ns3/video-stream-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VideoStreamServerApplication");

NS_OBJECT_ENSURE_REGISTERED (VideoStreamServer);

TypeId
VideoStreamServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VideoStreamServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<VideoStreamServer> ()
    .AddAttribute ("Interval", "The time to wait between packets",
                    TimeValue (MilliSeconds (100)),
                    MakeTimeAccessor (&VideoStreamServer::m_interval),
                    MakeTimeChecker ())
    .AddAttribute ("RemoteAddress", "The destination address of the outbound packets",
                    AddressValue (),
                    MakeAddressAccessor (&VideoStreamServer::m_remoteAddress),
                    MakeAddressChecker ())
    .AddAttribute ("RemotePort", "Port on which we listen for incoming packets.",
                    UintegerValue (5000),
                    MakeUintegerAccessor (&VideoStreamServer::m_remotePort),
                    MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MaxPacketSize", "The maximum size of a packet",
                    UintegerValue (1400),
                    MakeUintegerAccessor (&VideoStreamServer::m_maxPacketSize),
                    MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("FrameFile", "The file that contains the video frame sizes",
                    StringValue (""),
                    MakeStringAccessor (&VideoStreamServer::SetFrameFile, &VideoStreamServer::GetFrameFile),
                    MakeStringChecker ())
    .AddAttribute ("VideoLength", "The length of the video in seconds",
                    UintegerValue (60),
                    MakeUintegerAccessor (&VideoStreamServer::m_videoLength),
                    MakeUintegerChecker<uint32_t> ())
     .AddAttribute ("VideoQuality", "The quality of the video (0-5)",
                    UintegerValue (1),
                    MakeUintegerAccessor (&VideoStreamServer::m_videoQuality),
                    MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("AllowedQueuing", "Allowed frames to be queued in the application buffer",
                    UintegerValue (10000000),
                    MakeUintegerAccessor (&VideoStreamServer::m_allowedQueue),
                    MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SourceIdentification", "Index to identify the transmission source",
                    UintegerValue (0),
                    MakeUintegerAccessor (&VideoStreamServer::m_sourceId),
                    MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("ConnectionEstablished",
                     "Connection to the destination of video server has been established.",
                     MakeTraceSourceAccessor (&VideoStreamServer::m_connectionEstablishedTrace),
                     "ns3::ThreeGppHttpClient::TracedCallback")
    .AddTraceSource ("LatestFrameNum",
                     "Connection to the destination of video server has been established.",
                     MakeTraceSourceAccessor (&VideoStreamServer::m_latestFrameNum),
                     "ns3::TracedValueCallback::Uint32")
    .AddTraceSource ("RespondFrameNum",
                     "Recent responded frame's number and latency",
                     MakeTraceSourceAccessor (&VideoStreamServer::m_respondFrameNum),
                     "ns3::VideoStreamServer::RespondFrameNumCallback")
    ;
    return tid;
}

VideoStreamServer::VideoStreamServer ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_frameRate = 30;
  m_currentQueue = 0;
  m_frameSizeList = std::vector<uint32_t>();
  m_cwnd = 14600;
  m_minRtt = 100;
  m_inflight = 0;
  m_targetDelayMs = 100;
  m_bitrateMTU = m_initialBitrateMTU;
  std::cout<<"Initial bitrate: "<<m_bitrateMTU<<std::endl; 
}

VideoStreamServer::~VideoStreamServer ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void 
VideoStreamServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
VideoStreamServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");

      m_socket = Socket::CreateSocket (GetNode (), tid);

      int ret;

      if (Ipv4Address::IsMatchingType (m_remoteAddress))
        {
          ret = m_socket->Bind ();
          NS_LOG_DEBUG (this << " Bind() return value= " << ret
                             << " GetErrNo= " << m_socket->GetErrno () << ".");

          Ipv4Address ipv4 = Ipv4Address::ConvertFrom (m_remoteAddress);
          InetSocketAddress inetSocket = InetSocketAddress (ipv4,
                                                            m_remotePort);
          NS_LOG_INFO (this << " Connecting to " << ipv4
                            << " port " << m_remotePort
                            << " / " << inetSocket << ".");
          ret = m_socket->Connect (inetSocket);
          NS_LOG_DEBUG (this << " Connect() return value= " << ret
                             << " GetErrNo= " << m_socket->GetErrno () << ".");
        }
      else if (Ipv6Address::IsMatchingType (m_remoteAddress))
        {
          ret = m_socket->Bind6 ();
          NS_LOG_DEBUG (this << " Bind6() return value= " << ret
                             << " GetErrNo= " << m_socket->GetErrno () << ".");

          Ipv6Address ipv6 = Ipv6Address::ConvertFrom (m_remoteAddress);
          Inet6SocketAddress inet6Socket = Inet6SocketAddress  (ipv6,
                                                            m_remotePort);
          NS_LOG_INFO (this << " connecting to " << ipv6
                            << " port " << m_remotePort
                            << " / " << inet6Socket << ".");
          ret = m_socket->Connect (inet6Socket);
          NS_LOG_DEBUG (this << " Connect() return value= " << ret
                             << " GetErrNo= " << m_socket->GetErrno () << ".");
        }

  m_socket->SetConnectCallback (MakeCallback (&VideoStreamServer::ConnectionSucceededCallback,
                                                  this),
                                    MakeCallback (&VideoStreamServer::ConnectionFailedCallback,
                                                  this));
}

void
VideoStreamServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);


  if (m_socket != 0)
  {
    m_socket->Close();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    m_socket = 0;
  }

  for (auto iter = m_clients.begin (); iter != m_clients.end (); iter++)
  {
    Simulator::Cancel (iter->second->m_sendEvent);
  }
  
}

void 
VideoStreamServer::SetFrameFile (std::string frameFile)
{
  NS_LOG_FUNCTION (this << frameFile);
  m_frameFile = frameFile;
  if (frameFile != "")
  {
    std::string line;
    std::ifstream fileStream(frameFile);
    while (std::getline (fileStream, line))
    {
      int result = std::stoi(line);
      m_frameSizeList.push_back (result);
    }
  }
  NS_LOG_INFO ("Frame list size: " << m_frameSizeList.size());
}

std::string
VideoStreamServer::GetFrameFile (void) const
{
  NS_LOG_FUNCTION (this);
  return m_frameFile;
}

void
VideoStreamServer::SetMaxPacketSize (uint32_t maxPacketSize)
{
  m_maxPacketSize = maxPacketSize;
}

uint32_t
VideoStreamServer::GetMaxPacketSize (void) const
{
  return m_maxPacketSize;
}

void
VideoStreamServer::SetBitrate ()
{
  int maxQueueMTU = (m_targetDelayMs*m_throughput)/m_maxPacketSize; 
  int inflightMTU = (m_inflight + m_socketBufSize)/m_maxPacketSize;
  double prev_bitrate = m_bitrateMTU;
  if (maxQueueMTU > inflightMTU) {
    m_skipNum = 0;
    if (maxQueueMTU - inflightMTU > 1.1*prev_bitrate) {
      m_bitrateMTU = 1.1*prev_bitrate;
    }
    else if (maxQueueMTU - inflightMTU < 0.8*prev_bitrate){
      m_bitrateMTU = 0.8*prev_bitrate;
    }
    else {
      m_bitrateMTU = maxQueueMTU - inflightMTU;
    }
    if (prev_bitrate == 0) {
      m_bitrateMTU = m_initialBitrateMTU;
    }
  }
  else {
    m_bitrateMTU = 0; //skip
    m_skipNum += 1;
    if (m_skipNum >= 3) {
      m_bitrateMTU = m_initialBitrateMTU;
      m_skipNum = 0;
    }
  }
  
  double maxBitratePerFrame = (m_maxBitrateMbps*1024*1024/8)/m_frameRate;

  if (m_bitrateMTU > maxBitratePerFrame/m_maxPacketSize) {
    m_bitrateMTU = maxBitratePerFrame/m_maxPacketSize;
  }

  std::cout<<"Bitrate at server"<<m_sourceId<<" "<<Simulator::Now().GetSeconds()<<" is "<<m_bitrateMTU<<" "<<prev_bitrate<<" "<<maxQueueMTU<<" "<<inflightMTU<<" "<<m_throughput<<std::endl;
}

// Transmit application video data periodically
void 
VideoStreamServer::Send (uint32_t ipAddress)
{
  NS_LOG_FUNCTION (this);

  uint32_t frameSize, totalFrames;
  ClientInfo *clientInfo = m_clients.at (ipAddress);

  NS_ASSERT (clientInfo->m_sendEvent.IsExpired ());
  // If the frame sizes are not from the text file, and the list is empty
  if (m_frameSizeList.empty ())
  {
    frameSize = m_frameSizes[m_videoQuality];
    totalFrames = m_videoLength * m_frameRate;
  }
  else
  {
    frameSize = m_frameSizeList[clientInfo->m_sent]; // * clientInfo->m_videoLevel;
    totalFrames = m_frameSizeList.size ();
  }

  //uint32_t current_cwnd = m_socket->GetObject<TcpSocket> ()->GetTcb()->m_cWnd;
  //uint32_t minRtt = m_socket->GetObject<TcpSocket> () -> GetTcb()->m_minRtt.GetMilliSeconds();

  //double bitRate = (double)(m_cwnd*1000)/((double)m_minRtt); // per sec
  
  // If the current queued frames are larger than the allowed one, do not send the frame.

  m_lastTx = Simulator::Now();
  SetBitrate ();
  clientInfo->m_frameSize = m_bitrateMTU*m_maxPacketSize;

  m_latestFrameNum (clientInfo->m_sent);

  // 20 is header size to notify the transmission time.
  uint16_t dataSize = m_maxPacketSize;
  uint32_t sequenceNum = 0;
  // the frame might require several packets to send
  for (uint i = 0; i < clientInfo->m_frameSize / dataSize; i++)
  {
    sequenceNum += dataSize;
    SendPacket (clientInfo, dataSize, sequenceNum);
  }

  uint32_t remainder = clientInfo->m_frameSize % dataSize;
  sequenceNum += remainder;
  SendPacket (clientInfo, remainder, sequenceNum);

  NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s at server "<<m_sourceId<<" sent frame " << clientInfo->m_sent << " and " << clientInfo->m_frameSize << " bytes to " << InetSocketAddress::ConvertFrom (clientInfo->m_address).GetIpv4 () << " port " << 
    InetSocketAddress::ConvertFrom (clientInfo->m_address).GetPort ()<< " m_send: "<< clientInfo->m_sent);

  m_currentQueue += 1;
  
  /*
  if (clientInfo->m_sent > totalFrames)
  {
    clientInfo->m_sent = 1;
  }
  */
  uint32_t maxFrameInterval = (1000/m_frameRate);
  // uint32_t skipNum = frameInterval/maxFrameInterval + 1;
  Time next_tx = MilliSeconds(maxFrameInterval);
  clientInfo->m_sent += 1;

  if (clientInfo->m_frameSize > 0) {
    m_appQueue.push_back(clientInfo->m_frameSize);
  }
  clientInfo->m_sendEvent = Simulator::Schedule (next_tx, &VideoStreamServer::Send, this, ipAddress);
}

void 
VideoStreamServer::SendPacket (ClientInfo *client, uint32_t packetSize, uint32_t sequenceNum)
{
  //uint8_t *buffer
  //buffer = new uint8_t[packetSize];
  //sprintf ((char *) buffer, "%u", client->m_sent);
  //Ptr<Packet> p = Create<Packet>(buffer, packetSize);

  uint8_t *dataBuffer;
  dataBuffer = new uint8_t[packetSize];
  memset(dataBuffer, 0, packetSize);
  sprintf ((char *) dataBuffer, "%u", client->m_sent);
  Ptr <Packet> p = Create<Packet> (dataBuffer, packetSize);

  /*
  SeqTsSizeHeader header;
  header.SetSeq(client->m_sent);
  header.SetSize(totalPacketSize);
  p->AddHeader(header);
  */

  VideoStreamTag tag;
  tag.m_sourceId = m_sourceId;
  tag.m_frameNum = client->m_sent;
  tag.m_byteSequenceNum = client->m_frameSize;
  tag.m_currentSequence = sequenceNum;
  tag.m_senderTimestamp = Simulator::Now();
  p->AddByteTag(tag);

  //std::cout<<"APP: "<<tag.m_currentSequence<<std::endl;

  //std::cout<<"App TX: "<<Simulator::Now().GetSeconds()<<" " <<tag.m_byteSequenceNum<<std::endl;

  //std::cout<<" Bytes: "<< remainToSend <<std::endl;
  //NS_LOG_UNCOND(client->m_sent<<" Packet sent " << packetSize <<" "<<p->GetSize());

  if (m_socket->SendTo (p, 0, client->m_address) < 0)
  {
    NS_LOG_UNCOND ("Error while sending " << packetSize << "bytes to " << InetSocketAddress::ConvertFrom (client->m_address).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (client->m_address).GetPort ());
  }
}

void 
VideoStreamServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
  {
    std::cout<<Simulator::Now().GetSeconds()<<" Get end sign "<<from<<std::endl;
    socket->GetSockName (localAddress);
    if (InetSocketAddress::IsMatchingType (from))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (from).GetPort ());

      uint8_t dataBuffer[10];
      packet->CopyData (dataBuffer, 10);

      uint16_t videoLevel;
      sscanf((char *) dataBuffer, "%hu", &videoLevel);
      NS_LOG_UNCOND ("At time " << Simulator::Now ().GetSeconds () << "s server received video level " << videoLevel);
      //m_clients.at (ipAddr)->m_videoLevel = videoLevel;
      if (videoLevel == 99)
      {
        VideoStreamTag end_tag; 
        packet->FindFirstMatchingByteTag(end_tag);
        uint32_t resp_frameNum = end_tag.m_frameNum;
        Time dl = Simulator::Now() - end_tag.m_senderTimestamp;

        std::cout<<"DL: "<<resp_frameNum<<" "<<end_tag.m_senderTimestamp.GetSeconds()<<std::endl;

        m_respondFrameNum(resp_frameNum, dl);

        m_currentQueue -= 1;
      }
      
    }
  }
}

void 
VideoStreamServer::TraceSocket (Ptr<Socket> sock)
{
  sock->TraceConnectWithoutContext("CongestionWindow", MakeCallback (&VideoStreamServer::CwndChange, this));
  sock->TraceConnectWithoutContext("HighestRxAck", MakeCallback (&VideoStreamServer::AckRx, this));
  sock->TraceConnectWithoutContext("BytesInFlight", MakeCallback (&VideoStreamServer::InflightChange, this));
  sock->GetObject<TcpSocketBase>()->GetTxBuffer()->
    TraceConnectWithoutContext("TxBufferSize", MakeCallback (&VideoStreamServer::SocketBufferChange, this));

}

void
VideoStreamServer::CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  m_cwnd = newCwnd;
}

void
VideoStreamServer::AckRx (SequenceNumber32 oldSeq, SequenceNumber32 newSeq)
{
  uint32_t oldSeqValue = oldSeq.GetValue ();
  uint32_t newSeqValue = newSeq.GetValue ();
  uint32_t ackedBytes = newSeqValue - oldSeqValue;

  m_accumulateAck += ackedBytes;

  uint32_t appRemaining = m_appQueue[0];
  if (ackedBytes > appRemaining) {
    m_appQueue.pop_front();
    m_appQueue[0] - (ackedBytes - appRemaining);
  }
  else if (ackedBytes == appRemaining) {
    m_appQueue.pop_front();
    m_graceMs += (1000)/m_frameRate;
  }
  else {
    m_appQueue[0] = m_appQueue[0] - ackedBytes;
  }
}

void
VideoStreamServer::InflightChange (uint32_t oldInflight, uint32_t newInflight)
{
  m_inflight = newInflight;
}

void
VideoStreamServer::SocketBufferChange (int bufSize, int frameNum)
{
  m_socketBufSize = bufSize;
}


void
VideoStreamServer::ConnectionSucceededCallback (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds() <<" " << socket << "Connection done");

  Address remoteAddress;
  socket->GetPeerName(remoteAddress);

  uint32_t ipAddr = InetSocketAddress::ConvertFrom(remoteAddress).GetIpv4 ().Get ();
  // the first time the server connected with the client
  if (m_clients.find (ipAddr) == m_clients.end ())
  {
    ClientInfo *newClient = new ClientInfo();
    newClient->m_sent = 0;
    newClient->m_videoLevel = 3;
    newClient->m_address = remoteAddress;
    // newClient->m_sendEvent = EventId ();
    m_clients[ipAddr] = newClient;
    newClient->m_sendEvent = Simulator::Schedule (Seconds (0.0), &VideoStreamServer::Send, this, ipAddr);
    
    socket->SetRecvCallback (MakeCallback (&VideoStreamServer::HandleRead, this));
    m_connectionEstablishedTrace (socket);
    TraceSocket (socket);
    Simulator::Schedule (MicroSeconds (500), &VideoStreamServer::PeriodicProcessor, this);
  }

}

void
VideoStreamServer::ConnectionFailedCallback (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds() <<" " << socket << "Connection failed");
}

void 
VideoStreamServer::PeriodicProcessor () {
  if (m_accumulateAck > 0) {
    double timeGap = (Simulator::Now().GetMicroSeconds() - m_lastAckTime)/1000;
    timeGap = (timeGap - m_graceMs);
    if (timeGap < 0) {
      timeGap = 0.5;
    }
    double throughput = m_accumulateAck/(timeGap);
    m_throughput = 0.1*throughput + 0.9*m_throughput;
    std::cout<<"TimeGap at server"<<m_sourceId<<" "<<timeGap<<" accumulateACK: "<<m_accumulateAck<<" "<<Simulator::Now().GetMilliSeconds()<<" "<<m_lastAckTime<<" "<<m_graceMs<<" "<<m_throughput<<std::endl;
    m_accumulateAck = 0;
    m_graceMs = 0;
    m_lastAckTime = Simulator::Now().GetMicroSeconds();
  }
  Simulator::Schedule (MicroSeconds (500), &VideoStreamServer::PeriodicProcessor, this);
}


} // namespace ns3