
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef VIDEO_STREAM_CLIENT_H
#define VIDEO_STREAM_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/seq-ts-size-header.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"

#define MAX_VIDEO_LEVEL 6

namespace ns3 {

class Socket;
class Packet;

/**
 * @brief A Video Stream Client
 */
class VideoStreamClient : public Application
{
public:
/**
 * @brief Get the type ID.
 * 
 * @return the object TypeId
 */
  static TypeId GetTypeId (void);
  VideoStreamClient ();
  virtual ~VideoStreamClient ();

  /**
   * @brief Set the server address and port.
   * 
   * @param ip server IP address
   * @param port server port
   */
  void SetLocal (Address ip, uint16_t port);
  /**
   * @brief Set the server address.
   * 
   * @param addr server address
   */
  void SetLocal (Address addr);
  
  bool ConnectionRequestCallback (Ptr<Socket> socket, const Address &address);
  void NewConnectionCreatedCallback (Ptr<Socket> socket, const Address &address);

  void TraceAppRx (uint32_t frameNum, Time time, uint32_t size);
  
  typedef void (*LatestFrameNumCallback)(uint32_t source_id, uint32_t frameNum, Time e2e, uint32_t frameSize);

protected:
  virtual void DoDispose (void);

private: 
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * @brief Send the packet to the remote server.
   */
  void Send (void);

  /**
   * @brief Read data from the frame buffer. If the buffer does not have 
   * enough frames, it will reschedule the reading event next second.
   * 
   * @return the updated buffer size (-1 if the buffer size is smaller than the fps)
   */
  uint32_t ReadFromBuffer (void);

  /**
   * @brief Handle a packet reception.
   * 
   * This function is called by lower layers.
   * 
   * @param socket the socket the packet was received to
   */
  void HandleRead (Ptr<Socket> socket);

  /**
     * \brief Packet received: assemble byte stream to extract SeqTsSizeHeader
     * \param p received packet
     * \param from from address
     * \param localAddress local address
     *
     * The method assembles a received byte stream and extracts SeqTsSizeHeader
     * instances from the stream to export in a trace source.
     */
    void PacketReceived(Ptr<Socket> socket, const Ptr<Packet>& p, const Address& from, const Address& localAddress);

    /**
     * \brief Hashing for the Address class
     */
    struct AddressHash
    {
        /**
         * \brief operator ()
         * \param x the address of which calculate the hash
         * \return the hash of x
         *
         * Should this method go in address.h?
         *
         * It calculates the hash taking the uint32_t hash value of the IPv4 or IPv6 address.
         * It works only for InetSocketAddresses (IPv4 version) or Inet6SocketAddresses (IPv6
         * version)
         */
        size_t operator()(const Address& x) const
        {
            if (InetSocketAddress::IsMatchingType(x))
            {
                InetSocketAddress a = InetSocketAddress::ConvertFrom(x);
                return Ipv4AddressHash()(a.GetIpv4());
            }
            else if (Inet6SocketAddress::IsMatchingType(x))
            {
                Inet6SocketAddress a = Inet6SocketAddress::ConvertFrom(x);
                return Ipv6AddressHash()(a.GetIpv6());
            }

            NS_ABORT_MSG("PacketSink: unexpected address type, neither IPv4 nor IPv6");
            return 0; // silence the warnings.
        }
    };

  std::unordered_map<Address, Ptr<Packet>, AddressHash> m_buffer; //!< Buffer for received packets
  
  
  /// The `RxDelay` trace source.
  ns3::TracedCallback<const Time &, const Address &>  m_rxDelayTrace;

  Ptr<Socket> m_socket; //!< Socket
  std::list<Ptr<Socket>> m_socketList; //!< the accepted sockets
  Address m_localAddress; //!< Remote peer address
  uint16_t m_localPort; //!< Remote peer port

  uint16_t m_initialDelay; //!< Seconds to wait before displaying the content
  uint16_t m_stopCounter; //!< Counter to decide if the video streaming finishes
  uint16_t m_rebufferCounter; //!< Counter of the rebuffering event
  uint16_t m_videoLevel; //!< The quality of the video from the server
  uint32_t m_frameRate; //!< Number of frames per second to be played
  uint32_t m_frameSize; //!< Total size of packets from one frame
  uint32_t m_lastRecvFrame; //!< Last received frame number
  uint32_t m_lastBufferSize; //!< Last size of the buffer
  uint32_t m_currentBufferSize; //!< Size of the frame buffer

  uint32_t m_rxSize = 0;
  uint32_t m_skipped = 0;

  EventId m_bufferEvent; //!< Event to read from the buffer
  EventId m_sendEvent; //!< Event to send data to the server
  ns3::TracedCallback<uint32_t, uint32_t, Time, uint32_t> m_latestFrameNum;

};

} // namespace ns3

#endif /* VIDEO_STREAM_CLIENT_H */
