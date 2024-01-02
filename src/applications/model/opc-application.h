/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef OPC_APPLICATION_H
#define OPC_APPLICATION_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/sequence-number.h"
#include "ns3/internet-module.h"

#include <fstream>
#include <unordered_map>
#include <deque>

namespace ns3 {

class Socket;
class Packet;

  /**
   * @brief A Video Stream Server
   */
  class OpcApplication : public Application
  {
  public:
    /**
     * @brief Get the type ID.
     * 
     * @return the object TypeId
     */
    static TypeId GetTypeId (void);

    OpcApplication ();

    virtual ~OpcApplication ();

    // internet parameters
    Address m_nextAddress;
    Ptr<Socket> m_socekt;
    
    // compute parameters
    double m_weakScale = 1;
    double m_strongScale = 1;
    double m_earlyExitProb = 0.2;
    double m_compressRatio = 0.2;


  protected:
    virtual void DoDispose (void);

  private:

    virtual void StartApplication (void);
    virtual void StopApplication (void);

    /**
     * @brief The information required for each client.
     */

    typedef struct ObjectInfo
    {
        uint32_t m_sent; // counter for object

    }

    /**
     * @brief Send a packet with specified size.
     * 
     * @param packetSize the number of bytes for the packet to be sent
     */
    void SendObject (ClientInfo *client, uint32_t objectSize);
    
    /**
     * @brief Send the video frame to the given ipv4 address.
     * 
     * @param ipAddress ipv4 address
     */
    void Send (uint32_t ipAddress);

    /**
     * @brief Handle a packet reception.
     * 
     * This function is called by lower layers.
     * 
     * @param socket the socket the packet was received to
     */
    void HandleRead (Ptr<Socket> socket);

    void ConnectionSucceededCallback (Ptr <Socket> socket);
    void ConnectionFailedCallback (Ptr <Socket> socket);

    Ptr<Socket> m_txSocket; //!< Socket
    Ptr<Socket> m_rxSocket;

    uint16_t m_remotePort; //!< The port 
    Address m_remoteAddress; //!< Local multicast address

    Address m_localAddress;

    uint32_t m_frameRate; //!< Number of frames per second to be sent
    uint32_t m_videoLength; //!< Length of the video in seconds
    uint32_t m_videoQuality; //!< 
    uint32_t m_allowedQueue; // Number of frames to be queued at the application buffer. If the queue is full, the sent frame will be discarded.
    uint32_t m_sourceId; // id of video source
    uint32_t m_currentQueue;
    std::string m_frameFile; //!< Name of the file containing frame sizes
    std::vector<uint32_t> m_frameSizeList; //!< List of video frame sizes
    
    std::unordered_map<uint32_t, ClientInfo*> m_clients; //!< Information saved for each client
    const uint32_t m_frameSizes[6] = {0, 80000, 345600, 921600, 2073600, 2211840}; //!< Frame size for 360p, 480p, 720p, 1080p and 2K

    uint32_t m_cwnd;
    uint32_t m_inflight = 0;
    uint32_t m_socketBufSize = 0;
    uint32_t m_minRtt;
    Time m_lastTx;

    double m_bitrateMTU;
    double m_initialBitrateMTU = 2;
    double m_maxBitrateMbps = 20;
    int m_skipNum = 0;
    int m_accumulateAck = 0;
    uint32_t m_targetDelayMs;
    double m_throughput = 100; // 1.25KB per ms
    //double m_intervalMs = m_targetDelayMs/10;
    uint32_t m_lastAckedApp = 0;
    std::deque <uint32_t> m_appQueue;
    double m_lastAckTime;
    bool m_nextAppTx = false;
    uint32_t m_ackRxCounter = 0;
    double m_graceMs = 0;

    // TraceSources

      /**
     * Common callback signature for `ConnectionEstablished`, `RxMainObject`, and
     * `RxEmbeddedObject` trace sources.
     * \param httpClient Pointer to this instance of ThreeGppHttpClient,
     *                               which is where the trace originated.
     */
    typedef void (*TracedCallback)(Ptr<const VideoStreamServer> videoServer);

  
    // The `ConnectionEstablished` trace source.
    ns3::TracedCallback<Ptr<Socket>> m_connectionEstablishedTrace;
    ns3::TracedCallback<uint32_t> m_latestFrameNum;
    ns3::TracedCallback<uint32_t, Time> m_respondFrameNum;
  };

} // namespace ns3


#endif /* OPC_APPLICATION_H */