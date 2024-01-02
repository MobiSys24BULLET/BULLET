/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


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
#include "ns3/opc-application.h"
#include "ns3/seq-ts-size-header.h"
#include "ns3/video-stream-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OpcApplication");

NS_OBJECT_ENSURE_REGISTERED (OpcApplication);



void
OpcApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");

    m_txSocket = Socket::CreateSocket (GetNode (), tid);

    int ret;

    if (Ipv4Address::IsMatchingType (m_remoteAddress))
    {
        ret = m_txSocket->Bind ();
        NS_LOG_DEBUG (this << " Bind() return value= " << ret
                            << " GetErrNo= " << m_txSocket->GetErrno () << ".");

        Ipv4Address ipv4 = Ipv4Address::ConvertFrom (m_remoteAddress);
        InetSocketAddress inetSocket = InetSocketAddress (ipv4,
                                                        m_remotePort);
        NS_LOG_INFO (this << " Connecting to " << ipv4
                        << " port " << m_remotePort
                        << " / " << inetSocket << ".");
        ret = m_txSocket->Connect (inetSocket);
        NS_LOG_DEBUG (this << " Connect() return value= " << ret
                            << " GetErrNo= " <<m_txSocket->GetErrno () << ".");
    }
    else if (Ipv6Address::IsMatchingType (m_remoteAddress))
    {
        ret = m_txSocket->Bind6 ();
        NS_LOG_DEBUG (this << " Bind6() return value= " << ret
                            << " GetErrNo= " << m_txSocket->GetErrno () << ".");

        Ipv6Address ipv6 = Ipv6Address::ConvertFrom (m_remoteAddress);
        Inet6SocketAddress inet6Socket = Inet6SocketAddress  (ipv6,
                                                        m_remotePort);
        NS_LOG_INFO (this << " connecting to " << ipv6
                        << " port " << m_remotePort
                        << " / " << inet6Socket << ".");
        ret = m_txSocket->Connect (inet6Socket);
        NS_LOG_DEBUG (this << " Connect() return value= " << ret
                            << " GetErrNo= " << m_txSocket->GetErrno () << ".");
    }

    m_txSocket->SetConnectCallback (MakeCallback (&OpcApplication::TxConnectionEstablishedCallback,
                                                  this),
                                    MakeCallback (&OpcApplication::TxConnectionFailedCallback,
                                                  this));

    if (m_rxSocket == 0)
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
          m_rxSocket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());

          int ret;

          if (Ipv4Address::IsMatchingType (m_localAddress))
            {
              const Ipv4Address ipv4 = Ipv4Address::ConvertFrom (m_localAddress);
              const InetSocketAddress inetSocket = InetSocketAddress (ipv4,
                                                                      m_localPort);
              NS_LOG_INFO (this << " Binding on " << ipv4
                                << " port " << m_localPort
                                << " / " << inetSocket << ".");
              ret = m_rxSocket->Bind (inetSocket);
              NS_LOG_DEBUG (this << " Bind() return value= " << ret
                                 << " GetErrNo= "
                                 << m_rxSocket->GetErrno () << ".");
            }
          else if (Ipv6Address::IsMatchingType (m_localAddress))
            {
              const Ipv6Address ipv6 = Ipv6Address::ConvertFrom (m_localAddress);
              const Inet6SocketAddress inet6Socket = Inet6SocketAddress (ipv6,
                                                                         m_localPort);
              NS_LOG_INFO (this << " Binding on " << ipv6
                                << " port " << m_localPort
                                << " / " << inet6Socket << ".");
              ret = m_rxSocket->Bind (inet6Socket);
              NS_LOG_DEBUG (this << " Bind() return value= " << ret
                                 << " GetErrNo= "
                                 << m_rxSocket->GetErrno () << ".");
            }

          ret = m_rxSocket->Listen ();
          NS_LOG_DEBUG (this << " Listen () return value= " << ret
                             << " GetErrNo= " << m_rxSocket->GetErrno ()
                             << ".");


        } 

  m_rxSocket->SetRecvCallback (MakeCallback (&OpcApplication::HandleRead, this));
  m_rxSocket->SetAcceptCallback (MakeCallback (&OpcApplication::ConnectionRequestCallback, this),
                                 MakeCallback (&OpcApplication::RxConnectionEstablishedCallback, this));
  
}

void
OpcApplication::StopApplication ()
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
OpcApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
OpcApplication::TxConnectionEstablishedCallback (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds() <<" " << socket << "TX Connection estabilished");

  Address remoteAddress;
  socket->GetPeerName(remoteAddress);

  uint32_t ipAddr = InetSocketAddress::ConvertFrom(remoteAddress).GetIpv4 ().Get ();
  // TODO: change code to fit in the OPC 
  if (m_clients.find (ipAddr) == m_clients.end ())
  {
    ClientInfo *newClient = new ClientInfo();
    newClient->m_sent = 0;
    newClient->m_videoLevel = 3;
    newClient->m_address = remoteAddress;
    // newClient->m_sendEvent = EventId ();
    m_clients[ipAddr] = newClient;
    newClient->m_sendEvent = Simulator::Schedule (Seconds (0.0), &OpcApplication::Send, this, ipAddr);
    
    m_connectionEstablishedTrace (socket);
    TraceSocket (socket);
  }

}

void
OpcApplication::TxConnectionFailedCallback (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds() <<" " << socket << " TX Connection failed");
}

bool
OpcApplication::ConnectionRequestCallback (Ptr<Socket> socket,
                                               const Address &address)
{
  NS_LOG_FUNCTION (this << socket << address<< "connection done");
  return true; // Unconditionally accept the connection request.
}

void  
OpcApplication::RxConnectionEstablishedCallback (Ptr<Socket> socket,
                                               const Address &address)
{
  NS_LOG_FUNCTION (Simulator::Now().GetSeconds()<<" "<<this << socket << address<< "new RX connection established");

  socket->SetRecvCallback (MakeCallback (&OpcApplication::HandleRead, this));
  m_socketList.push_back (socket);
  //m_connectionEstablishedTrace (socket);
}

}