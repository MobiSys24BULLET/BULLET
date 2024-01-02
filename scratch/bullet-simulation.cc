/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/**
 *
 * \file cttc-3gpp-channel-nums-fdm.cc
 * \ingroup examples
 * \brief Frequency division multiplexing example, with TDD and FDD
 *
 * The example is showing how to configure multiple bandwidth parts, in which
 * some of them form a FDD configuration, while others uses TDD. The user
 * can configure the bandwidth and the frequency of these BWPs. Three types
 * of traffic are available: two are DL (video and voice) while one is
 * UL (gaming). Each traffic will be routed to different BWP. Voice will go
 * in the TDD BWP, while video will go in the FDD-DL one, and gaming in the
 * FDD-UL one.
 *
 * The configured spectrum division is the following:
Original
\verbatim
    |------------BandTdd--------------|--------------BandFdd---------------|
    |------------CC0------------------|--------------CC1-------------------|
    |------------BWP0-----------------|------BWP1-------|-------BWP2-------|
\endverbatim
* We will configure BWP0 as TDD, BWP1 as FDD-DL, BWP2 as FDD-UL.
*
* The simulation script is to simulate operational 5G uplink transmission on 3.5GHz frequency band.
* The objective of simulation is to show the inefficiency of current operational 5G uplink scheduling.
* We simulate real-time streaming application over 5G uplink.

* The configured spectrum division is the following:
* 
\verbatim
    |--------------BandTdd---------------| 
    |--------------CC0-------------------|
    |--------------BWP0-------------------|
\endverbatim
 */

#include <filesystem>
#include <queue>
#include <string>
#include <sys/stat.h>

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include <ns3/lte-ue-rrc.h>
#include "ns3/sequence-number.h"

using namespace ns3;
namespace fs = std::filesystem;

NS_LOG_COMPONENT_DEFINE("BULLET-simulation");

// E2E latency component tracing
class LatencyTracer
{
    public:
        LatencyTracer (int index):ue_index(index)
        {     
    
        }

        // ue
        uint64_t m_imsi;
        uint16_t m_rnti;
        int ue_index = -1;

        // app
        int app_frame = -1;
        int prev_app_tx = app_frame;
        int rx_frame = -1;
        int prev_app_rx = rx_frame;
        
        // transport
        int cwnd = 0;
        int inflight = 0;
        int tcp_appSize = 0;
        int prev_tcp_appSize = 0;
        int tcp_appFrameNum = -1;
        int tcp_ack = 0;

        // link
        std::string sr = "-";
        int bsr = 0;
        int link_buffer = 0;
        int tbs = 0;
        int current_ul_frame = -1;
        int prev_phy_frame_num = -1;
        int rlc_frameNum = -1;
        int prev_rlc_frameNum = rlc_frameNum;

        // gNB PDCP
        int pdcp_frame_num = -1;
        int pdcp_sequence_num = 0;
        int pdcp_frame_size = 0;
        std::queue <int> pdcp_complete_frame;

        // app info in link buffer (UE-PHY)
        int frame_num = -1;
        int current_sequence = 0;
        int prev_phy_sequence_num = current_sequence;
        int frame_size = 0;
        double on_device_queueing = 0; // zero
        bool is_frame = false;

        // calculated latency
        double transport_control_latency = 0;
        double link_control_latency = 0;
        double transmission_latency = 0;
        double prop_latency = 0;

        double overall_t_control_latency = 0;
        double overall_l_control_latency = 0;
        double overall_transmission_latency = 0;

        // Old Trigger
        uint32_t trig_BSR = 0;
        int trig_mode = 0;
        int ack = 0;
        double tp_mode = 0;
        double fp_mode = 0;
        double tp_app_timing = 0;
        double fp_app_timing = 0;
        double tp_trans_timing = 0;
        double fp_trans_timing = 0;
        double error_buffer = 0;

        // New Trigger (230416)
        int new_trig_sr = 0; 
        int new_trig_bsr = 0;
        bool new_trig_get_bsr = 0;
        int new_trig_tbs = 0;
        int new_trig_alloc = 0;
        int new_trig_ack = 0;

        int new_trig_arrival = 0;
        int new_trig_mode = 0;
        double new_trig_period_app = 0;

        int new_trig_estimated_arrival = 0;
        int new_trig_proactive_bsr = 0;
        int new_trig_total_bsr = 0;

        int true_traffic_arrival = 0;
        int true_trig_mode = 0;

        // state
        int state = 0; // 0: idle, 1: control_net, 2: control_link, 3: active (on tx)
        int sr_state = 0;
        int prev_sr_state = 0;
        bool frame_done = false;

        // per-frame-log
        Time e2e;
        std::map <int, std::vector<double>> frame_latency_map;
        std::map<int, std::map<std::string, double>> frame_latency_dict;
        std::vector<int> queuing_frames;

        std::string m_folderName;
        std::ofstream latencyStream;
        std::ofstream trig_eval_stream;
        std::ofstream cwndStream;
        std::ofstream rttStream;
        std::ofstream inflgihtStream;
        std::ofstream traceStream;
        std::ofstream resourceStream;
        std::ofstream onPeriodStream;
        std::ofstream offPeriodStream;
        int printed_num = -1;

        void StartLogging () {
            std::string latencyTraceFile = m_folderName + "/bullet_latency" +std::to_string(m_rnti)+".csv";
            latencyStream.open (latencyTraceFile.c_str(), std::ofstream::out);
            latencyStream<<"Frame num\tQueuing\tNet\tLink\tAir\tWireless\tWired\tDL\tE2E\tRespond\tFrame Size\tLink Throughput"<<std::endl;

            std::string triggerTraceFile = m_folderName + "/bullet_trig_eval" +std::to_string(m_rnti)+".csv";
            trig_eval_stream.open (triggerTraceFile.c_str(), std::ofstream::out);
            trig_eval_stream<<"Time\tApp\tTransport buffer\tTrue Arrival\tTrue Mode\tTrue Buffer\tTrue ACK\tSR\tBSR\tAlloc TBS\tUsed TBS\tACK\tArrival\tMode\tMultiplier\tEstimated_Arrival\tProactive BSR\tTotal BSR"<<std::endl;

            std::string cwndTraceFile = m_folderName + "/bullet_cwnd" +std::to_string(m_rnti)+".csv";
            cwndStream.open (cwndTraceFile.c_str(), std::ofstream::out);
            cwndStream<<"Time\tOld\tNew"<<std::endl;

            std::string rttTraceFile = m_folderName + "/bullet_rtt" +std::to_string(m_rnti)+".csv";
            rttStream.open (rttTraceFile.c_str(), std::ofstream::out);
            rttStream<<"Time\tOld\tNew"<<std::endl;

            std::string traceFile = m_folderName + "/bullet_raw_traces_UE" +std::to_string(m_rnti)+".csv";
            traceStream.open (traceFile.c_str(), std::ofstream::out);
            traceStream<<"Time\tApp Tx\tRAN rx\tApp Rx\tCWND\tInflight\tSocket buffer\tTCP ACK RX\tTCP Tx\tRLC Tx\t"<<
            "Link buffer\tBS BSR\tTBS\tUsedTBS\tSR\tBSR\tPHY TX\tPHY sequence\tRAN RX sequence\tRAN frame size\tTx state\tSR state\t"<<
            "OnDeviceQueue\tNet Ctrl\tLink Ctrl\tTx\tAir Tx\t"<<
            "Overall Net Ctrl\tOverall Link Ctrl\tOverall Air Tx\t"<<std::endl;

            std::string resourceTraceFile = m_folderName + "/bullet_resource" +std::to_string(m_rnti)+".csv";
            resourceStream.open (resourceTraceFile.c_str(), std::ofstream::out);
            resourceStream<<"Time\tAlloc RB\t Proactive RB\tUsed RB\tWasted RB\tSE\tTotal TBS"<<std::endl;

            std::string onPeriodTraceFile = m_folderName + "/bullet_onPeriod" +std::to_string(m_rnti)+".csv";
            onPeriodStream.open (onPeriodTraceFile.c_str(), std::ofstream::out);
            onPeriodStream<<"Time\tLength"<<std::endl;

            std::string offPeriodTraceFile = m_folderName + "/bullet_offPeriod" +std::to_string(m_rnti)+".csv";
            offPeriodStream.open (offPeriodTraceFile.c_str(), std::ofstream::out);
            offPeriodStream<<"Time\tLength"<<std::endl;
        }
};

class GnbResourceTracer
{
public:
    GnbResourceTracer();

public:
    int m_gnbIndex;

    uint32_t allocUe;

    uint32_t allocRb;
    uint32_t proactiveRb;
    uint32_t usedRb;
    uint32_t unUsedRb;
    uint32_t deviceBuffer;

    uint32_t needRb;
    uint32_t wastedRb_P;
    uint32_t wastedRb_R;
    uint32_t wastedTBS;

    double totalEffectiveSE = 0;
    double totalEffectiveTBS = 0;
    double totalSE = 0;
    double totalTBS = 0;

    double last_logging = -1;

    std::string m_folderName;
    std::ofstream resourceStream;

    void InitializeResourceStream();
    void Logging ();
};

GnbResourceTracer::GnbResourceTracer()
{
    Simulator::Schedule (MilliSeconds (400), &GnbResourceTracer::InitializeResourceStream, this);
}

void GnbResourceTracer::InitializeResourceStream()
{
    std::string resourceTraceFile = m_folderName + "/gnb_resource.csv";
    resourceStream.open(resourceTraceFile.c_str(), std::ofstream::out);
    resourceStream << "Time\tAlloc RB\t Proactive RB\tUsed RB\tUnused RB\tRequired RB\tWasted RB (P)\tWasted RB (R)\tWasted TBS\tTotal SE\tEffective SE\tTotal TBS\tEffective TBS" << std::endl;
}

void GnbResourceTracer::Logging ()
{
    resourceStream << Simulator::Now ().GetSeconds () << "\t" << allocRb << "\t" << proactiveRb << "\t" << usedRb << "\t" << unUsedRb << "\t" << needRb << 
                "\t" << wastedRb_P << "\t" << wastedRb_R <<"\t"<< wastedTBS << 
                "\t" << totalSE << "\t" << totalEffectiveSE << "\t" << totalTBS << "\t" <<totalEffectiveTBS <<std::endl;
}

void GnbAllocTracer(GnbResourceTracer* gt, std::map<uint16_t, uint32_t> allocRbMap, std::map<uint16_t, uint32_t> proactiveRbMap,
                        std::map<uint16_t, uint32_t> deviceBufferMap, 
                        std::map<uint16_t, uint32_t> transmittedRbMap, std::map<uint16_t, double> spectralEfficiencyMap)
{
    if (Simulator::Now().GetSeconds() > gt->last_logging) {
    }
    else {
        return;
    }

    gt->last_logging = Simulator::Now().GetSeconds();

    // Units: RB
    uint32_t totalAlloc = 0;
    uint32_t totalProc = 0;
    uint32_t totalUsed = 0; 
    uint32_t totalWasteP = 0;
    double totalWastedTBS = 0; // only consider wasted RB from proactive one
    uint32_t totalNeedRb = 0;
    uint32_t totalRemainingBuffer = 0;

    // spectral efficiency and TBS
    double totalEffectiveSE = 0;
    double totalEffectiveTBS = 0;
    double totalSE = 0;
    double totalTBS = 0;

    // Get the number of RB that is allocated and used
    for (auto it = allocRbMap.begin(); it != allocRbMap.end(); it++)
    {
        uint16_t rnti = it->first;
        uint32_t allocRb = it->second;
        uint32_t proactiveRb = proactiveRbMap[rnti];
        uint32_t usedRb = transmittedRbMap[rnti];
        usedRb = std::min(usedRb, allocRb);
        uint32_t deviceBuffer = deviceBufferMap[rnti];
        double  spectralEfficiency = spectralEfficiencyMap[rnti];

        double effectiveTBS = spectralEfficiency*usedRb; 
        double TBS = spectralEfficiency*allocRb;

        uint32_t unusedRb = 0;
        std::cout<<Simulator::Now().GetSeconds()<<" UE alloc process simulator: "<<allocRb<<" "<<usedRb<<std::endl;
        if (allocRb > usedRb) {
            unusedRb = allocRb - usedRb;
        }

        uint32_t wasteRb;
        if (unusedRb > proactiveRb)
        {
            wasteRb = proactiveRb;
        }
        else
        {
            wasteRb = unusedRb;
        }

        uint32_t otherNeedRb = 0;
        // Calculate the number of RB that is needed to eliminate the buffer
        for (auto it = deviceBufferMap.begin(); it != deviceBufferMap.end(); it++)
        {
            if (it->first != rnti)
            {
                uint32_t otherAlloc = allocRbMap[it->first]*spectralEfficiencyMap[it->first];
                uint32_t otherBuffer = it->second;
                std::cout<<Simulator::Now().GetSeconds()<<" RNTI: "<<rnti<<" "<<
                    it->first<<" "<<otherAlloc<<" "<<otherBuffer<<" "<<spectralEfficiencyMap[it->first]<<std::endl;
                if (otherBuffer > otherAlloc) {
                    otherNeedRb += (otherBuffer - otherAlloc)/spectralEfficiencyMap[it->first];
                }
            }
        }
        
        if (otherNeedRb > totalWasteP) {
            otherNeedRb -= totalWasteP; 
        }
        else {
            otherNeedRb = 0;
        }
        

        if (wasteRb > otherNeedRb) {
            wasteRb = otherNeedRb;
        }

        totalAlloc += allocRb;
        totalProc += proactiveRb;
        totalUsed += usedRb;
        totalWasteP += wasteRb;
        totalWastedTBS += wasteRb*spectralEfficiency;
        totalEffectiveTBS += effectiveTBS;
        totalTBS += TBS;
        
        std::cout<<rnti<<" Alloc RB: "<<allocRb<<" Total Alloc: "<<totalAlloc<<" "<<totalWasteP <<" "<<otherNeedRb<<std::endl;
    }

    if (totalUsed != 0) {
        totalEffectiveSE = totalEffectiveTBS/totalUsed;
    }
    else {
        totalEffectiveSE = 0;
    }
    if (totalAlloc != 0) {
        totalSE = totalTBS/totalAlloc;
    }
    else {
        totalSE = 0;
    }

    // Get the number of RB to eliminate buffer
    for (auto it = deviceBufferMap.begin(); it != deviceBufferMap.end(); it++)
    {
        uint16_t rnti = it->first;
        uint32_t deviceBuffer = it->second;

        uint32_t allocRb = allocRbMap[rnti];
        uint32_t usedRb = transmittedRbMap[rnti];
        usedRb = std::min(usedRb, allocRb);
        uint32_t unusedRb = 0;
        std::cout<<Simulator::Now().GetSeconds()<<" UE alloc process simulator: "<<allocRb<<" "<<usedRb<<std::endl;
        if (allocRb > usedRb) {
            unusedRb = allocRb - usedRb;
        }

        double  spectralEfficiency = spectralEfficiencyMap[rnti];

        uint32_t needRb = 0;
        uint32_t deviceTbs = allocRb*spectralEfficiency;
        if (deviceBuffer > deviceTbs) {
            needRb = (deviceBuffer - deviceTbs)/spectralEfficiency;
        }
        totalNeedRb += needRb;
        
        std::cout<<rnti<<" Alloc buffer: "<<deviceBuffer<<" Total Need: "<<totalNeedRb<<" Alloc RB: "<<allocRb<<" Used: "<<usedRb<<std::endl;
    }

    gt->allocRb = totalAlloc;
    gt->proactiveRb = totalProc;
    gt->usedRb = totalUsed;

    if (gt->allocRb < gt->usedRb) {
        std::cout<<"Big error: "<<gt->allocRb<<" "<<gt->proactiveRb<<" "<<gt->usedRb<<std::endl;
        for (auto it = allocRbMap.begin(); it != allocRbMap.end(); it++) {
            uint16_t rnti = it->first;
            uint32_t allocRb = it->second;
            uint32_t proactiveRb = proactiveRbMap[rnti];
            uint32_t usedRb = transmittedRbMap[rnti];

            std::cout<<"Alloc/Used: "<<allocRb<<" "<<usedRb<<" "<<proactiveRb<<std::endl;
        }
    }

    if (totalAlloc > totalUsed) {
        gt->unUsedRb = totalAlloc - totalUsed;
    }
    else {
        gt->unUsedRb = 0;
    }
    gt->needRb = totalNeedRb;
    gt->wastedRb_P = totalWasteP;

    uint32_t remainingRb = (266 - totalAlloc);
    if (totalNeedRb > 0 && remainingRb > 0)
    {
        if (totalNeedRb > remainingRb)
        {
            gt->wastedRb_R = remainingRb;
        }
        else
        {
            gt->wastedRb_R = totalNeedRb;
        }
    }
    else
    {
        gt->wastedRb_R = 0;
    }

    gt->wastedTBS = totalWastedTBS;
    gt->totalEffectiveSE = totalEffectiveSE;
    gt->totalEffectiveTBS = totalEffectiveTBS;
    gt->totalSE = totalSE;
    gt->totalTBS = totalTBS;

    gt->Logging();
}



// TODO: there is error on per-frame-latency analyzer
// TODO: RLC buffer --> get application tag/idle from packet pacing... (de`termine current transmitting frame number with link buffer packet sequence)
void 
LatencyAnalyzer (LatencyTracer* lt)
{
    // TODO 230208: Analyze latency with Tx/Ctrl (link)/Ctrl (transport) and get the ratio of ctrl latency from overall latency (and its overall impact on the app frames)
    // 230217: This function also trace per-frame-latency

    // Ignore SYN packet
    if (lt->app_frame < 0){
        return;
    }
    
    // if sr grant is not exist
    if (lt->sr_state == 0){
        lt->state = 0;
        // there is data to send, but not enough CWND
        // Should be changed
        if (lt->tcp_appSize > 0){
            lt->state = 1;
        }
    }
    // Identify idle
    if (lt->app_frame != lt->prev_app_tx){
        std::map<std::string, double> latency_dict;
        latency_dict["queuing"] = 0.0;
        latency_dict["net"] = 0.0;
        latency_dict["link"] = 0.0;
        latency_dict["air"] = 0.0;
        latency_dict["wired"] = 0.0;
        latency_dict["dl"] = 0.0;
        latency_dict["e2e"] = 0.0;
        latency_dict["respond"] = 0.0;
        lt->frame_latency_dict[lt->app_frame] = latency_dict;

        lt->queuing_frames.push_back(lt->app_frame);

        
        if (lt->state == 0){
            lt->state = 2;
            // Log new uplink transmission
            if (lt->inflight == lt->cwnd){
                lt->state = 1;
            }
        }
        
        if (lt->state != 3){
            std::vector<double> v;
            v.push_back(double(lt->on_device_queueing));
            lt->frame_latency_map[lt->app_frame] = v;
            //lt->frame_latency_dict[lt->app_frame]["queueing"] = lt->on_device_queueing;
            std::cout<<Simulator::Now().GetSeconds()<<" Start from idle: "<<lt->app_frame<<" "<<double(lt->on_device_queueing)<<std::endl;
        }
    }

    // SR is txed
    //if (lt->prev_sr_state == 0 && lt->sr_state == 1 && lt->is_frame){
    if (lt->link_buffer > 0 && lt->sr_state != 2 && lt->is_frame  && lt->tbs < 200){
        lt->state = 2;
    }
    if (lt->sr == "30000" && lt->is_frame  && lt->tbs < 200){
        lt->state = 2;
    }
    // TBS according to the BSR (active)
    if (lt->bsr > 0 && lt->tbs > 200){
        lt->state = 3;
    }


    // Here we should use trace from gNB PDCP
    while (lt->pdcp_complete_frame.size() > 0) {
        // Latency info of current tx frame is not yet put into vector || It is not middle of the first transmission
        int done_frame = lt->pdcp_complete_frame.front();
        lt->pdcp_complete_frame.pop();

        std::vector<double> v = lt->frame_latency_map[lt->pdcp_frame_num];
        v.push_back(lt->transport_control_latency);
        v.push_back(lt->link_control_latency);
        v.push_back(lt->transmission_latency);
    
        std::cout<<Simulator::Now().GetSeconds()<<" "<<done_frame<<"Latency components are pushed back (RAN rx end): "<<lt->transport_control_latency<<" "<<lt->link_control_latency<<" "<<lt->transmission_latency<<std::endl;

        lt->frame_latency_map[lt->pdcp_frame_num] = v;

        lt->transport_control_latency = 0;
        lt->link_control_latency = 0;
        lt->transmission_latency = 0;
        
        lt->queuing_frames.erase(std::remove(lt->queuing_frames.begin(), lt->queuing_frames.end(), lt->pdcp_frame_num), lt->queuing_frames.end());
        lt->printed_num = done_frame;
        lt->frame_done = false;

    }


    if (lt->rlc_frameNum != lt->prev_rlc_frameNum && lt->rlc_frameNum != -1){
        std::vector<double> vv;
        double queuing_latency = lt->on_device_queueing - lt->transport_control_latency - lt->link_control_latency;
        if (queuing_latency < 0){
            queuing_latency = 0;
        }
        vv.push_back(queuing_latency);
        lt->frame_latency_map[lt->frame_num] = vv;

        double queuing = lt->on_device_queueing - lt->frame_latency_dict[lt->frame_num]["net"] 
                                                - lt->frame_latency_dict[lt->frame_num ]["link"] 
                                                - lt->frame_latency_dict[lt->frame_num ]["air"];

        //lt->frame_latency_dict[lt->rlc_frameNum]["queuing"] =

        if (queuing < 0){
            queuing = 0;
        }
        //lt->frame_latency_dict[lt->rlc_frameNum]["queuing"] += queuing_latency;

        std::cout<<Simulator::Now().GetSeconds()<<" "<<lt->rlc_frameNum<<"Latency components are pushed back (RAN tx start): "<<queuing_latency<<std::endl;
    }

    // If the frame is arrived at the receiver
    if (lt->rx_frame != lt->prev_app_rx){
        //std::cout<<lt->rx_frame<<" "<<lt->prev_app_rx<<std::endl;
        std::vector<double> v = lt->frame_latency_map[lt->rx_frame];
        v.push_back(lt->e2e.GetSeconds());
        lt->frame_latency_map[lt->rx_frame] = v;
        //lt->frame_latency_dict[lt->rx_frame]["e2e"] += lt->e2e.GetSeconds();
        std::cout<<Simulator::Now().GetSeconds()<<" "<<lt->rx_frame<<"Latency components are pushed back (app rx end): "<<lt->e2e.GetSeconds()<<std::endl;
    }

    // latency analyzer
    if (lt->printed_num < lt->app_frame){
        if (lt->state == 1){
            lt->transport_control_latency += 0.5/1000;
            lt->overall_t_control_latency += 0.5;
        }
        else if (lt->state == 2){
            lt->link_control_latency += 0.5/1000;
            lt->overall_l_control_latency += 0.5;
        }
        else if (lt->state == 3){
            lt->transmission_latency += 0.5/1000;
            lt->overall_transmission_latency += 0.5;
        }
    }
    
    // per-frame-latency-adder
    if (lt->state == 1 && (lt->printed_num < lt->tcp_appFrameNum)){
        lt->frame_latency_dict[lt->tcp_appFrameNum]["net"] += 0.5/1000;
    }
    else if (lt->state == 2 && (lt->printed_num < lt->rlc_frameNum)){
        lt->frame_latency_dict[lt->rlc_frameNum]["link"] += 0.5/1000;
    }
    else if (lt->state == 3 && (lt->printed_num < lt->frame_num)){
        lt->frame_latency_dict[lt->frame_num]["air"] += 0.5/1000;
    }
    
    // per-frame-queuing-analysis
    for (int i: lt->queuing_frames) {
        bool is_active = (lt->state == 1 && i == lt->tcp_appFrameNum)||(lt->state == 2 && i == lt->rlc_frameNum)||
            (lt->state == 3 && i == lt->frame_num);
        if(!is_active){
            lt->frame_latency_dict[i]["queuing"] += 0.5/1000;
        }
    }

    lt->prev_app_tx = lt->app_frame;
    lt->prev_app_rx = lt->rx_frame;
    lt->prev_sr_state = lt->sr_state;
    lt->prev_phy_frame_num = lt->frame_num;
    lt->prev_phy_sequence_num = lt->current_sequence;
    lt->prev_rlc_frameNum = lt->rlc_frameNum;
}

void TriggerTracer (LatencyTracer* lt) 
{
    if (lt->true_traffic_arrival > 0 && lt->prev_tcp_appSize > 0) {
        lt->true_trig_mode = 0;
    }
    else if (lt->true_traffic_arrival > 0 && lt->prev_tcp_appSize == 0) {
        lt->true_trig_mode = 1;
    }
    /*
    lt->new_trig_total_bsr = lt->new_trig_proactive_bsr + NrMacShortBsrCe::FromLevelToBytes(lt->new_trig_bsr);// - lt->tbs;
    if (lt->new_trig_total_bsr < 0){
        lt->new_trig_total_bsr = 0;
    }
    */
    lt->trig_eval_stream << Simulator::Now().GetSeconds() << "\t" << lt->app_frame << "\t" << lt->tcp_appSize << "\t" << lt->true_traffic_arrival<<"\t"<< lt->true_trig_mode <<"\t" << lt->link_buffer << "\t" << lt->tcp_ack << "\t"<<
                            lt->new_trig_sr << "\t" << NrMacShortBsrCe::FromLevelToBytes(lt->new_trig_bsr) << "\t" << lt->new_trig_tbs << "\t" << lt->new_trig_alloc << "\t" << lt->new_trig_ack << "\t" <<  
                            lt->new_trig_arrival << "\t" << lt->new_trig_mode << "\t" << lt->new_trig_period_app << "\t" << 
                            lt->new_trig_estimated_arrival << "\t"<<  lt->new_trig_proactive_bsr<<"\t"<< lt->new_trig_total_bsr<<std::endl;
 
    lt->true_traffic_arrival = 0;
}

void 
WriteRawTracing (LatencyTracer* lt)
{
    LatencyAnalyzer (lt);    
    //TriggerTracer (lt);

    lt->traceStream << Simulator::Now().GetSeconds() << "\t" <<lt->app_frame << '\t' << lt->printed_num << '\t'<< lt->rx_frame << '\t'
                   << lt->cwnd << '\t' << lt->inflight << '\t' << lt->tcp_appSize <<'\t' << lt->tcp_ack <<'\t'<< lt->tcp_appFrameNum << '\t'
                   << lt->rlc_frameNum << '\t'
                   << lt->link_buffer  << '\t' << lt->trig_BSR <<'\t' << lt->tbs << '\t' << lt->new_trig_alloc << '\t'<< lt->sr << '\t' << lt->bsr<< '\t'
                   << lt->frame_num << '\t' << lt->current_sequence << '\t'<< lt->pdcp_sequence_num << '\t'  << lt->frame_size <<'\t' << lt->state <<'\t' << lt->sr_state <<'\t' 
                   << lt->on_device_queueing << '\t' << lt->transport_control_latency << '\t' << lt->link_control_latency << '\t' << lt->transmission_latency <<'\t'
                   << lt->overall_t_control_latency << '\t' << lt->overall_l_control_latency << '\t' << lt->overall_transmission_latency << std::endl;
    
    //std::cout<<"UE ID: " <<lt->ue_index<<std::endl;
    
    lt->sr = "-";
    lt->bsr = 0;
    lt->new_trig_tbs = lt->tbs;
    lt->tbs = 0;
    lt->tcp_ack = 0;
    lt->prev_tcp_appSize = lt->tcp_appSize;
    
    Simulator::Schedule (MicroSeconds (500), &WriteRawTracing, lt);
}


void 
TraceAppTx (std::ofstream* ofStream, Time time)
{
  *ofStream << Simulator::Now().GetSeconds() <<"\tUE APP Tx"<<std::endl;
}

void 
TraceAppRx (std::ofstream* ofStream, Time time, Address addr)
{
  *ofStream << Simulator::Now().GetSeconds() <<'\t' << time.GetMilliSeconds() <<'\t' << addr<< std::endl;
}

void
TraceTransportTx (std::ofstream* ofStream, Ptr<const Packet> p, const TcpHeader& h, Ptr<const TcpSocketBase> b)
{
  *ofStream << Simulator::Now().GetSeconds() <<"\tUE TCP Tx\t"<<p->GetSize()<<std::endl;
}

void
TraceTransportRx (std::ofstream* ofStream, Ptr<const Packet> p, const TcpHeader& h, Ptr<const TcpSocketBase> b)
{
  *ofStream << Simulator::Now().GetSeconds() <<"\t"<<h.GetSequenceNumber()<<"\t"<<p->GetSize()<<std::endl;
}

void
CwndChange (std::ofstream* ofStream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *ofStream << Simulator::Now().GetSeconds() <<'\t' << oldCwnd << "\t" << newCwnd <<std::endl;
}

void
RttChange (std::ofstream* ofStream, Time oldRtt, Time newRtt)
{
  *ofStream << Simulator::Now().GetSeconds() <<'\t' << oldRtt.GetSeconds() << '\t' << newRtt.GetSeconds() <<std::endl;
}

void
InflightChange (std::ofstream* ofStream, uint32_t oldInflight, uint32_t newInflight)
{
  *ofStream << Simulator::Now().GetSeconds() <<'\t' << oldInflight << "\t" << newInflight <<std::endl;
}

void
TxChange (std::ofstream* ofStream, Ptr<const Packet> p,  const TcpHeader& h, Ptr<const TcpSocketBase> s)
{
  *ofStream << Simulator::Now().GetSeconds() <<'\t' << p->GetSize()<<" "<<h.GetSequenceNumber()<<std::endl;
}

// Functions for latency tracer

void 
LtAppFrameTracer (LatencyTracer* lt, uint32_t newFrameNum)
{
    lt->app_frame = newFrameNum;
}


void 
NewTrial (LatencyTracer* lt, uint32_t respFrameNum, Time responseTime)
{
    // Frame num\tQueuing\tNet\tLink\tAir\tWired\tDL\tE2E\tRespond
    lt->frame_latency_dict[respFrameNum]["dl"] = responseTime.GetSeconds() - lt->frame_latency_dict[respFrameNum]["e2e"];
    lt->frame_latency_dict[respFrameNum]["respond"] = responseTime.GetSeconds();

    double queuing = lt->frame_latency_dict[respFrameNum]["queuing"];
    double net = lt->frame_latency_dict[respFrameNum]["net"];
    double link = lt->frame_latency_dict[respFrameNum]["link"];
    double air = lt->frame_latency_dict[respFrameNum]["air"];

    double dl = lt->frame_latency_dict[respFrameNum]["dl"];
    double e2e = lt->frame_latency_dict[respFrameNum]["e2e"];
    double respond = lt->frame_latency_dict[respFrameNum]["respond"];

    double wireless = queuing + net + link + air;
    lt->frame_latency_dict[respFrameNum]["wired"] = lt->frame_latency_dict[respFrameNum]["e2e"] - wireless;
    double wired = lt->frame_latency_dict[respFrameNum]["wired"];
    uint32_t frameSize = lt->frame_latency_dict[respFrameNum]["size"];

    double l_ul = net + link + air;
    if (l_ul == 0) {
        l_ul = queuing;
    }

    double linkThroughput = frameSize*8 / ((l_ul*1000)*(1024));
    linkThroughput = round(linkThroughput * 100) / 100;

    std::vector<double> v = lt->frame_latency_map[respFrameNum]; 

    lt->latencyStream << respFrameNum <<"\t" << queuing <<"\t" << net <<"\t" << link <<"\t" << air <<"\t"<< wireless << "\t"
                                    <<wired<<"\t"<<dl<<"\t"<<e2e<<"\t"<<respond<<"\t"<<frameSize<<"\t"<<linkThroughput<<std::endl;
    
    std::cout<<Simulator::Now().GetSeconds()<<" "<<respFrameNum<<"Latency components are pushed back (response): "<< respond <<std::endl;
      
}

void 
LtAppRespondTracer (LatencyTracer* lt, uint32_t respFrameNum, Time responseTime)
{
    NewTrial(lt, respFrameNum, responseTime);
    //Legacy(lt, respFrameNum, responseTime);
}

void 
LtAppRxTracer (LatencyTracer* lt_arr [], int ueNum, uint32_t sourceId, uint32_t newFrameNum, Time e2e, uint32_t frameSize)
{
    //std::cout<<"Source Id: "<<sourceId<<" "<<newFrameNum<<" "<<e2e.GetSeconds()<<std::endl;           
    for (int i = 0; i < ueNum; i++) {
        if (sourceId == lt_arr[i]-> ue_index){
            //std::cout<<"Source Id/UE index: "<<sourceId<<" "<<lt_arr[i]-> ue_index<<" "<<newFrameNum<<" "<<e2e.GetSeconds()<<std::endl;
            lt_arr[i]->rx_frame = newFrameNum;
            lt_arr[i]->frame_latency_dict[newFrameNum]["e2e"] = e2e.GetSeconds();
            lt_arr[i]->frame_latency_dict[newFrameNum]["size"] = frameSize;
            break;
        }
    }
}

void
LtCwndTracer (LatencyTracer* lt, uint32_t oldCwnd, uint32_t newCwnd)
{
    lt->cwnd = newCwnd;
    lt->cwndStream << Simulator::Now().GetSeconds() <<'\t' << oldCwnd<< '\t' << newCwnd <<std::endl;
}

void
LtAckTracer (LatencyTracer* lt, SequenceNumber32 oldSeq, SequenceNumber32 newSeq)
{
    lt->tcp_ack = 2500;//newSeq.GetValue();
}

void
LtTxTracer (LatencyTracer* lt, Ptr<const Packet> p,  const TcpHeader& h, Ptr<const TcpSocketBase> s)
{
    lt->true_traffic_arrival += p->GetSize();
}

void 
LtTcpTxBuffer (LatencyTracer* lt, int bufSize, int frameNum)
{
    lt->tcp_appSize = bufSize;
    lt->tcp_appFrameNum = frameNum;
}

void
LtRttTracer (LatencyTracer* lt, Time oldRtt, Time newRtt)
{
  lt->rttStream << Simulator::Now().GetSeconds() <<'\t' << oldRtt.GetSeconds() << '\t' << newRtt.GetSeconds() <<std::endl;
}

void
LtInflightTracer (LatencyTracer* lt, uint32_t oldInflight, uint32_t newInflight)
{
    lt->inflight = newInflight;
}

void
LtRlcBufferTracer (LatencyTracer* lt, int frameNum, double onDeviceQueuing, bool is_frame)
{
    lt->rlc_frameNum = frameNum;
    lt->on_device_queueing = onDeviceQueuing;
    lt->is_frame = is_frame;
}

void
LtSrTracer (LatencyTracer* lt, uint32_t sr)
{
     lt->sr = "30000";
}

void
LtBsrTracer (LatencyTracer* lt, uint32_t bsr, uint32_t tbs)
{
     lt->bsr = bsr;
     lt->tbs = tbs;
}

void
LtBufferTracer (LatencyTracer* lt, uint16_t rnti, uint32_t buffer)
{
     lt->link_buffer = buffer;
}

void
LtSrStateTracer (LatencyTracer* lt, uint32_t sr_state)
{
     lt->sr_state = sr_state;
}

void
LtPhyTracer (LatencyTracer* lt, uint32_t frameNum, uint32_t frameSize, uint32_t currentSeq, Time onDeviceQueueing)
{
     lt->frame_num = frameNum;
     lt->current_sequence = currentSeq;
     lt->frame_size = frameSize;
     //lt->on_device_queueing = onDeviceQueueing.GetSeconds();
}


void
LtTrigPreprocessTracer (LatencyTracer* lt, uint16_t rnti, int sr, int bsr, bool get_bsr, int alloc, int ack, int totalBuffer) {
    if (rnti == lt->m_rnti){
        lt->new_trig_sr = sr;
        lt->new_trig_bsr = bsr;
        lt->new_trig_get_bsr = get_bsr;
        lt->new_trig_alloc = alloc;
        lt->new_trig_ack = ack;
        lt->new_trig_total_bsr = totalBuffer;
    }
}

void
LtTrigProfileTracer (LatencyTracer* lt, uint16_t rnti, int arrival, int mode, double period_app) {
    if (rnti == lt->m_rnti){
        lt->new_trig_arrival = arrival;
        lt->new_trig_mode = mode;
        lt->new_trig_period_app = period_app;
    }
}

void
LtTrigOutputTracer (LatencyTracer* lt, uint16_t rnti, int estimated_arrival, int proactive_bsr) {
    if (rnti == lt->m_rnti){
        lt->new_trig_estimated_arrival = estimated_arrival;
        lt->new_trig_proactive_bsr = proactive_bsr;
        TriggerTracer (lt);
    }
}

void
LtTrigOnPeriodTracer (LatencyTracer* lt, uint16_t rnti, double eventTime, double length) {
    if (rnti == lt->m_rnti){
        lt->onPeriodStream<<eventTime<<"\t"<<length<<std::endl;
    }
}

void
LtTrigOffPeriodTracer (LatencyTracer* lt, uint16_t rnti, double eventTime, double length) {
    if (rnti == lt->m_rnti){
        lt->offPeriodStream<<eventTime<<"\t"<<length<<std::endl;
    }
}

void 
//LtGnbPdcpTracer(LatencyTracer *lt, int frameNum, int sequnceNum)
LtGnbPdcpTracer(LatencyTracer* lt,
                int frameNum,
                int sequenceNum,
                int frameSize)
{
    lt->pdcp_sequence_num = sequenceNum;
    lt->pdcp_frame_size = frameSize;

    if (lt->pdcp_sequence_num == lt->pdcp_frame_size){
        lt->pdcp_frame_num = frameNum;
        lt->frame_done = true;

        lt->pdcp_complete_frame.push(frameNum);
    }
}

void
LtGnbTracer (LatencyTracer* lt_arr [], int ueNum,  std::string folderName, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{

    // We only consider a single cell scenario
    std::ostringstream basePath;
    basePath << context.substr(0, context.rfind('/')) << "/UeMap/" << (uint32_t)rnti;

    int index = -1;
    for (int i = 0; i < ueNum; i++) {
        if (imsi == lt_arr[i]-> m_imsi){
            std::cout<<"Index: "<<i<<std::endl;
            lt_arr[i]->m_folderName = folderName;
            lt_arr[i]->m_rnti = rnti;
            lt_arr[i]->StartLogging();
            index = i;
            break;
        }
    }
    Config::ConnectWithoutContext(basePath.str() + "/DataRadioBearerMap/*/LtePdcp/AppTrace",
                        MakeBoundCallback(&LtGnbPdcpTracer, lt_arr[index]));
    std::cout<<"Context of pdcp tracer for UE: "<<lt_arr[index]->ue_index<<" with RNTI/IMSI: "<<rnti<<" "<<imsi<<" "<<
            ueNum<<" "<<basePath.str() + "/DataRadioBearerMap/*/LtePdcp/AppTrace"<<std::endl;
}

/*
void
LtFrameLatencyTracer (LatencyTracer* lt, uint32_t frameLatency)
{
    lt->frame_latency = frameLatency;
}

void
LtResponseTracer (LatencyTracer* lt, uint32_t responseTime)
{
    lt->response_time = responseTime;
}
*/

std::string GetCurrentTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d-%H%M%S");
    return oss.str();
}

std::string doubleToStringRoundedTwoDecimalPlaces(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

bool folderExists(const std::string& folder_path) {
    struct stat info;
    return stat(folder_path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

uint64_t lastTotalRx[100];
uint16_t c[10];
double t_total[10];
void
TraceConnectionClient (std::ofstream* ofStream1, std::ofstream* ofStream2, std::ofstream* ofStream3, Ptr<const ThreeGppHttpClient> client)
{
  client->GetSocket()->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback (&CwndChange, ofStream1));
  client->GetSocket()->TraceConnectWithoutContext("RTT", MakeBoundCallback (&RttChange, ofStream2));
  client->GetSocket()->TraceConnectWithoutContext("Rx", MakeBoundCallback (&TraceTransportRx, ofStream3));
}

void 
TraceSocket (std::ofstream* ofStream1, std::ofstream* ofStream2, std::ofstream* ofStream3, LatencyTracer* lt, Ptr<Socket> sock)
{
  //sock->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback (&CwndChange, ofStream1));
  //sock->TraceConnectWithoutContext("RTT", MakeBoundCallback (&RttChange, ofStream2));
  //sock->TraceConnectWithoutContext("Tx", MakeBoundCallback (&TxChange, ofStream3));

  sock->TraceConnectWithoutContext("RTT", MakeBoundCallback (&LtRttTracer, lt));
  sock->TraceConnectWithoutContext("BytesInFlight", MakeBoundCallback (&LtInflightTracer, lt));
  sock->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback (&LtCwndTracer, lt));
  sock->TraceConnectWithoutContext("HighestRxAck", MakeBoundCallback (&LtAckTracer, lt));
  sock->TraceConnectWithoutContext("Tx", MakeBoundCallback (&LtTxTracer, lt));
  sock->GetObject<TcpSocketBase>()->GetTxBuffer()->TraceConnectWithoutContext("TxBufferSize", MakeBoundCallback (&LtTcpTxBuffer, lt));
}

void
TraceConnectionServer (std::ofstream* ofStream, Ptr<const ThreeGppHttpServer> server, Ptr<Socket> socket)
{
  socket->TraceConnectWithoutContext("Rx", MakeBoundCallback(&TraceTransportRx, ofStream));
}


int
main(int argc, char* argv[])
{
    uint32_t udpPacketSizeVideo = 100;
    uint32_t udpPacketSizeVoice = 1252;
    uint32_t udpPacketSizeGaming = 50;
    uint32_t lambdaVideo = 50;
    uint32_t lambdaVoice = 100;
    uint32_t lambdaGaming = 30;

    //uint32_t bulletBitrate = 15*1024*1024;

    uint32_t simTimeMs = 10400;
    uint32_t udpAppStartTimeMs = 400;

    double centralFrequencyBand2 = 28.2e9;
    double bandwidthBand2 = 100e6;
    double totalTxPower = 30;
    std::string simTag = "default";
    std::string outputDir = "flow-monitor";
    bool enableVideo = false;
    bool enableVoice = false;
    bool enableGaming = false;
    bool enableBulk = false;
    bool enableStreaming = true;
    
    // TODO --> real-time streaming pattern + frame-level E2E latency
    // First --> main, remaining --> embedded w/ xx ms interval
    // 1213 --> Copa, BBR, Cubic, Vegas experiments! E2E frame delay!!

    // Default 5G    

    // Simulation parameters
    std::string cca = "TcpCopa";
    int scheduleMode = 0;
    uint16_t ueNum = 2;
    //double ueInterval = coverage/ueNum;
    double ueSpeed = 1.4;
    double coverage = 100;
    double ueInterval = 0;
    bool enableBbr = false;
    double beta = 0.9;
    double copaDelta = 0.05;
    bool ideal = false;
    double pfFairness =1;
    double centralFrequencyBand1 = 35e8;
    double bandwidthBand1 = 100e6;

    CommandLine cmd(__FILE__);
    /*
    cmd.AddValue("packetSizeVideo",
                 "packet size in bytes to be used by video traffic",
                 udpPacketSizeVideo);
    cmd.AddValue("packetSizeVoice",
                 "packet size in bytes to be used by voice traffic",
                 udpPacketSizeVoice);
    cmd.AddValue("packetSizeGaming",
                 "packet size in bytes to be used by gaming traffic",
                 udpPacketSizeGaming);
    cmd.AddValue("lambdaVideo",
                 "Number of UDP packets in one second for video traffic",
                 lambdaVideo);
    cmd.AddValue("lambdaVoice",
                 "Number of UDP packets in one second for voice traffic",
                 lambdaVoice);
    cmd.AddValue("lambdaGaming",
                 "Number of UDP packets in one second for gaming traffic",
                 lambdaGaming);
    cmd.AddValue("enableVideo", "If true, enables video traffic transmission (DL)", enableVideo);
    cmd.AddValue("enableVoice", "If true, enables voice traffic transmission (DL)", enableVoice);
    cmd.AddValue("enableGaming", "If true, enables gaming traffic transmission (UL)", enableGaming);
    cmd.AddValue("centralFrequencyBand1",
                 "The system frequency to be used in band 1",
                 centralFrequencyBand1);
    cmd.AddValue("bandwidthBand1", "The system bandwidth to be used in band 1", bandwidthBand1);
    cmd.AddValue("centralFrequencyBand2",
                 "The system frequency to be used in band 2",
                 centralFrequencyBand2);
    cmd.AddValue("bandwidthBand2", "The system bandwidth to be used in band 2", bandwidthBand2);
    cmd.AddValue("totalTxPower",
                 "total tx power that will be proportionally assigned to"
                 " bands, CCs and bandwidth parts depending on each BWP bandwidth ",
                 totalTxPower);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    */

    // Simulation parameters added for BULLET
    cmd.AddValue("CCA", "Congestion control algorithm to be used", cca);
    cmd.AddValue("Schedule", "Scheduling mode to be used", scheduleMode);
    cmd.AddValue("UEs", "Number of UEs to be used", ueNum);
    cmd.AddValue("Coverage", "Interval between UEs", coverage);
    cmd.AddValue("UEInterval", "Interval between UEs", ueInterval);
    cmd.AddValue("UESpeed", "Speed of UEs", ueSpeed);
    cmd.AddValue("enableBbr", "If true, enables buffer-based resource allocation", enableBbr);
    cmd.AddValue("beta", "Beta value for confidence resource allocator", beta);
    cmd.AddValue("copaDelta", "Delta for CoPA", copaDelta);
    cmd.AddValue("ideal", "If true, enables ideal", ideal);
    cmd.AddValue("Fairness", "1 --> PF, 0 --> RR", pfFairness);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("simTimeMs", "Simulation time", simTimeMs);

    cmd.Parse(argc, argv);

    NS_ABORT_IF(centralFrequencyBand1 > 100e9);
    NS_ABORT_IF(centralFrequencyBand2 > 100e9);
    
    ueInterval = coverage/ueNum;

    // Create the folder name
    std::string root_simFolder = "SimResults";
    std::string current_time = GetCurrentTime();
    std::string folder_name = root_simFolder + "/" + current_time +"_" + cca + "_mode:" + std::to_string(scheduleMode) + "_ueNum:" + 
                std::to_string(ueNum) + "_coverage:" + std::to_string(coverage) +"_ueInterval:" + doubleToStringRoundedTwoDecimalPlaces(ueInterval)  + "_ueSpeed:" + 
                doubleToStringRoundedTwoDecimalPlaces(ueSpeed) + "_enableBbr:" + std::to_string(enableBbr) +"_beta:"+std::to_string(beta)+"_fairness:"+doubleToStringRoundedTwoDecimalPlaces(pfFairness) + "_copaDelta:" + doubleToStringRoundedTwoDecimalPlaces(copaDelta) +"_bw: " + doubleToStringRoundedTwoDecimalPlaces(bandwidthBand1/1e6);


    if (ideal) {
        enableBbr = false;
        scheduleMode = 4;
    }

    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(1.5*1024*1024));
    Config::SetDefault("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue(1.5*1024*1024));

    // Uplink scheduilng mode for resource allocation --> 0: reactive, 1: grant-free, 2: proactive, 3: BULLET, 4: Ideal
    Config::SetDefault("ns3::NrMacSchedulerNs3::ScheduleMode", IntegerValue (scheduleMode));
    Config::SetDefault("ns3::NrMacSchedulerNs3::Beta", DoubleValue (beta));

    // Enable buffer-based resource allocation or not
    Config::SetDefault("ns3::NrMacSchedulerOfdma::BBR", BooleanValue (enableBbr));
    Config::SetDefault("ns3::TcpCopa::Latencyfactor", DoubleValue (copaDelta));
    
    // Set PF fairness
    Config::SetDefault("ns3::NrMacSchedulerOfdmaPF::FairnessIndex", DoubleValue (pfFairness));
    // Determine scheduling slot
    //Config::SetDefault("ns3::NrGnbPhy::N1Delay", UintegerValue (0));
    //Config::SetDefault("ns3::NrGnbPhy::N2Delay", UintegerValue (0));

    // Determine SR periodicity
    uint8_t sr_period = 1;

    int64_t randomStream = 1;
    bool fixedUser = false;

    uint16_t gNbNum = 1;

    GridScenarioHelper gridScenario;
    gridScenario.SetRows(gNbNum);
    gridScenario.SetColumns(gNbNum);
    gridScenario.SetHorizontalBsDistance(0);
    gridScenario.SetVerticalBsDistance(0);
    gridScenario.SetBsHeight(30.0);
    gridScenario.SetUtHeight(1.5);
    // must be set before BS number
    gridScenario.SetSectorization(GridScenarioHelper::SINGLE);
    gridScenario.SetBsNumber(gNbNum);
    gridScenario.SetUtNumber(ueNum);
    //gridScenario.SetScenarioLength(0);
    //gridScenario.SetScenarioHeight(100); 
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.SetFixedUserPosition(fixedUser);
    gridScenario.SetSpeed(ueSpeed);
    gridScenario.CreateScenarioWithMobility(ueInterval);


    /*
     * TODO: Add a print, or a plot, that shows the scenario.
     */

    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();

    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    RealisticBfManager::TriggerEvent m_realTriggerEvent {RealisticBfManager::SRS_COUNT};
    Ptr<RealisticBeamformingHelper> realBeamformingHelper = CreateObject<RealisticBeamformingHelper>();
    realBeamformingHelper->SetBeamformingMethod (RealisticBeamformingAlgorithm::GetTypeId());
    // when realistic beamforming used, also realistic beam manager should be set
    // TODO, move this to NrHelper, so user sets BeamformingMethod calling NrHelper
    //nrHelper->SetGnbBeamManagerTypeId (RealisticBfManager::GetTypeId());
    //nrHelper->SetGnbBeamManagerAttribute ("TriggerEvent", EnumValue (m_realTriggerEvent));

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);

    // Trace-driven link parameter setting
    //// Error Model: UE and GNB with same spectrum error model.
    std::string errorModel = "ns3::NrLteMiErrorModel";
    Config::SetDefault("ns3::NrAmc::ErrorModelType", TypeIdValue (TypeId::LookupByName(errorModel)));
    nrHelper->SetUlErrorModel (errorModel);
    nrHelper->SetDlErrorModel (errorModel);

    
    //Config::SetDefault("ns3::NrAmc:AmcModel", EnumValue(NrAmc::ShannonModel));

    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // in this example, both bands have a single CC

    CcBwpCreator::SimpleOperationBandConf bandConfTdd(centralFrequencyBand1,
                                                      bandwidthBand1,
                                                      numCcPerBand,
                                                      BandwidthPartInfo::UMi_StreetCanyon);
    CcBwpCreator::SimpleOperationBandConf bandConfFdd(centralFrequencyBand2,
                                                      bandwidthBand2,
                                                      numCcPerBand,
                                                      BandwidthPartInfo::UMi_StreetCanyon);

    bandConfFdd.m_numBwp = 2; // Here, bandFdd will have 2 BWPs

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo bandTdd = ccBwpCreator.CreateOperationBandContiguousCc(bandConfTdd);
    OperationBandInfo bandFdd = ccBwpCreator.CreateOperationBandContiguousCc(bandConfFdd);

    /*
     * The configured spectrum division is:
     * |------------BandTdd--------------|--------------BandFdd---------------|
     * |------------CC0------------------|--------------CC1-------------------|
     * |------------BWP0-----------------|------BWP1-------|-------BWP2-------|
     *
     * We will configure BWP0 as TDD, BWP1 as FDD-DL, BWP2 as FDD-UL.
     */

    /*
     * Attributes of ThreeGppChannelModel still cannot be set in our way.
     * TODO: Coordinate with Tommaso
     */
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaPF"));

    nrHelper->InitializeOperationBand(&bandTdd);
    nrHelper->InitializeOperationBand(&bandFdd);

    //allBwps = CcBwpCreator::GetAllBwps({bandTdd, bandFdd});
    allBwps = CcBwpCreator::GetAllBwps ({bandTdd});

    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(QuasiOmniDirectPathBeamforming::GetTypeId()));

    // Core latency
    epcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(16));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));
    
    nrHelper->SetGnbPhyAttribute ("TxPower", DoubleValue (43.0));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue (23.0));
    nrHelper->SetGnbPhyAttribute("NoiseFigure", DoubleValue(5.0)); // default value: 5
    nrHelper->SetUePhyAttribute("NoiseFigure", DoubleValue(11.0));
    
    Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (false));

    uint32_t bwpIdForBullet = 0;
    //uint32_t bwpIdForVideo = 1;
    //uint32_t bwpIdForGaming = 2;

    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForBullet));
    //nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO", UintegerValue(bwpIdForVideo));
    //nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_GAMING", UintegerValue(bwpIdForGaming));

    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForBullet));
    //nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO", UintegerValue(bwpIdForVideo));
    //nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_GAMING", UintegerValue(bwpIdForGaming));

    NetDeviceContainer enbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueNetDev =
        nrHelper->InstallUeDevice(gridScenario.GetUserTerminals(), allBwps);

    //std::cout<<"Multi user: "<<gridScenario.GetUserTerminals().GetN()<<std::endl;

    randomStream += nrHelper->AssignStreams(enbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    //NS_ASSERT (enbNetDev.GetN () == 0);

  int numerology = 1;

  // -------------- First GNB:
  // BWP0, the TDD one
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Numerology", UintegerValue (numerology));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Pattern", StringValue ("DL|DL|DL|DL|UL|DL|DL|DL|DL|UL|")); //"DL|S|UL|UL|UL|DL|DL|DL|DL|DL|"
  //nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("TxPower", DoubleValue (10.0));

  /*
  // BWP1, FDD-DL
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Numerology", UintegerValue (numerology));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Pattern", StringValue ("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
  //nrHelper->GetGnbPhy (enbNetDev.Get (0), 1)->SetAttribute ("TxPower", DoubleValue (4.0));

  // BWP2, FDD-UL
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 1)->SetAttribute ("Numerology", UintegerValue (numerology));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 1)->SetAttribute ("Pattern", StringValue ("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
  //nrHelper->GetGnbPhy (enbNetDev.Get (0), 2)->SetAttribute ("TxPower", DoubleValue (0.0));

  // Link the two FDD BWP:
  nrHelper->GetBwpManagerGnb (enbNetDev.Get (0))->SetOutputLink (1, 0);
  
  for (uint32_t i = 0; i < ueNetDev.GetN (); i++)
  {
    nrHelper->GetBwpManagerUe (ueNetDev.Get (i))->SetOutputLink (0, 1);
  }
  */
  
  // -------------- Second GNB:

  // BWP0, the TDD one
  //nrHelper->GetGnbPhy (enbNetDev.Get (1), 0)->SetAttribute ("Numerology", UintegerValue (numerology));
  //nrHelper->GetGnbPhy (enbNetDev.Get (1), 0)->SetAttribute ("Pattern", StringValue ("DL|S|UL|UL|UL|DL|DL|DL|DL|DL|"));
  //nrHelper->GetGnbPhy (enbNetDev.Get (1), 0)->SetAttribute ("TxPower", DoubleValue (4.0));

  /*
  // BWP1, FDD-DL
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 1)->SetAttribute ("Numerology", UintegerValue (1));
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 1)->SetAttribute ("Pattern", StringValue ("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 1)->SetAttribute ("TxPower", DoubleValue (4.0));

  // BWP2, FDD-UL
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 2)->SetAttribute ("Numerology", UintegerValue (1));
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 2)->SetAttribute ("Pattern", StringValue ("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (1), 2)->SetAttribute ("TxPower", DoubleValue (0.0));

  // Link the two FDD BWP:
  nrHelper->GetBwpManagerGnb (enbNetDev.Get (1))->SetOutputLink (2, 1);
  */
  // -------------- Third GNB:

  // BWP0, the TDD one
  //nrHelper->GetGnbPhy (enbNetDev.Get (2), 0)->SetAttribute ("Numerology", UintegerValue (numerology));
  //nrHelper->GetGnbPhy (enbNetDev.Get (2), 0)->SetAttribute ("Pattern", StringValue ("DL|S|UL|UL|UL|DL|DL|DL|DL|DL|"));
  //nrHelper->GetGnbPhy (enbNetDev.Get (2), 0)->SetAttribute ("TxPower", DoubleValue (4.0));

  /*
  // BWP1, FDD-DL
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 1)->SetAttribute ("Numerology", UintegerValue (2));
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 1)->SetAttribute ("Pattern", StringValue ("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 1)->SetAttribute ("TxPower", DoubleValue (4.0));

  // BWP2, FDD-UL
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 2)->SetAttribute ("Numerology", UintegerValue (2));
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 2)->SetAttribute ("Pattern", StringValue ("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (2), 2)->SetAttribute ("TxPower", DoubleValue (0.0));

  // Link the two FDD BWP:
  nrHelper->GetBwpManagerGnb (enbNetDev.Get (2))->SetOutputLink (2, 1);
  */
  // -------------- Fourth GNB:

  // BWP0, the TDD one
  //nrHelper->GetGnbPhy (enbNetDev.Get (3), 0)->SetAttribute ("Numerology", UintegerValue (numerology));
  //nrHelper->GetGnbPhy (enbNetDev.Get (3), 0)->SetAttribute ("Pattern", StringValue ("DL|S|UL|UL|UL|DL|DL|DL|DL|DL|"));
  //nrHelper->GetGnbPhy (enbNetDev.Get (3), 0)->SetAttribute ("TxPower", DoubleValue (4.0));

  /*
  // BWP1, FDD-DL
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 1)->SetAttribute ("Numerology", UintegerValue (3));
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 1)->SetAttribute ("Pattern", StringValue ("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 1)->SetAttribute ("TxPower", DoubleValue (4.0));

  // BWP2, FDD-UL
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 2)->SetAttribute ("Numerology", UintegerValue (3));
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 2)->SetAttribute ("Pattern", StringValue ("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
  nrHelper->GetGnbPhy (enbNetDev.Get (3), 2)->SetAttribute ("TxPower", DoubleValue (0.0));
  
  // Link the two FDD BWP:
  nrHelper->GetBwpManagerGnb (enbNetDev.Get (3))->SetOutputLink (2, 1);
  */

    // Set the UE routing:

    for (uint32_t i = 0; i < ueNetDev.GetN(); i++)
    {
        nrHelper->GetBwpManagerUe(ueNetDev.Get(i))->SetOutputLink(1, 2);
    }

    // When all the configuration is done, explicitly call UpdateConfig ()

    for (auto it = enbNetDev.Begin(); it != enbNetDev.End(); ++it)
    {
        DynamicCast<NrGnbNetDevice>(*it)->UpdateConfig();
    }

    for (auto it = ueNetDev.Begin(); it != ueNetDev.End(); ++it)
    {
        DynamicCast<NrUeNetDevice>(*it)->UpdateConfig();
    }

    // From here, it is standard NS3. In the future, we will create helpers
    // for this part as well.

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.005))); // affects minimum RTT (Round-Trip-Time)
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(gridScenario.GetUserTerminals());

    Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // Fix the attachment of the UEs: UE_i attached to GNB_0
    for (uint32_t i = 0; i < ueNetDev.GetN(); ++i)
    {
        auto enbDev = DynamicCast<NrGnbNetDevice>(enbNetDev.Get(0)); // We only simulate single BS scenario!
        auto ueDev = DynamicCast<NrUeNetDevice>(ueNetDev.Get(i));
        NS_ASSERT(enbDev != nullptr);
        NS_ASSERT(ueDev != nullptr);
        nrHelper->AttachToEnb(ueDev, enbDev);
    }
    /*
     * Traffic part. Install two kind of traffic: low-latency and voice, each
     * identified by a particular source port.
     */
    uint16_t dlPortVideo = 1234;
    uint16_t dlPortVoice = 1235;
    uint16_t ulPortGaming = 1236;
    uint16_t ulPortBullet = 80;
    uint16_t ulPortBulk = 3030;
    uint16_t ulPortStreaming = 5000;

    ApplicationContainer serverApps;

    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSinkVideo(dlPortVideo);
    UdpServerHelper dlPacketSinkVoice(dlPortVoice);
    UdpServerHelper ulPacketSinkVoice(ulPortGaming);
    ThreeGppHttpServerHelper ulPacketSinkBullet (internetIpIfaces.GetAddress(1));
    PacketSinkHelper ulBulkSink("ns3::TcpSocketFactory", InetSocketAddress(internetIpIfaces.GetAddress(1), ulPortBulk));
    VideoStreamClientHelper ulVideoClient (internetIpIfaces.GetAddress(1), ulPortStreaming); // TODO: multi-user service
     // Client should listen and send packets.. but..

    // The server, that is the application which is listening, is installed in the UE
    // for the DL traffic, and in the remote host for the UL traffic
    serverApps.Add(dlPacketSinkVideo.Install(gridScenario.GetUserTerminals()));
    serverApps.Add(dlPacketSinkVoice.Install(gridScenario.GetUserTerminals()));
    serverApps.Add(ulPacketSinkVoice.Install(remoteHost));

    ApplicationContainer BulletServer = ulPacketSinkBullet.Install(remoteHost);
    Ptr <ThreeGppHttpServer> httpServer = BulletServer.Get(0)->GetObject<ThreeGppHttpServer> ();
    serverApps.Add(BulletServer);
    
    ApplicationContainer video_clients = ulVideoClient.Install(remoteHost);                                 
    serverApps.Add(video_clients);

    serverApps.Add(ulBulkSink.Install(remoteHost));


    /*
     * Configure attributes for the different generators, using user-provided
     * parameters for generating a CBR traffic
     *
     * Low-Latency configuration and object creation:
     */
    UdpClientHelper dlClientVideo;
    dlClientVideo.SetAttribute("RemotePort", UintegerValue(dlPortVideo));
    dlClientVideo.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientVideo.SetAttribute("PacketSize", UintegerValue(udpPacketSizeVideo));
    dlClientVideo.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaVideo)));

    // The bearer that will carry low latency traffic
    EpsBearer videoBearer(EpsBearer::GBR_CONV_VIDEO);

    // The filter for the low-latency traffic
    Ptr<EpcTft> videoTft = Create<EpcTft>();
    EpcTft::PacketFilter dlpfVideo;
    dlpfVideo.localPortStart = dlPortVideo;
    dlpfVideo.localPortEnd = dlPortVideo;
    videoTft->Add(dlpfVideo);

    // Voice configuration and object creation:
    UdpClientHelper dlClientVoice;
    dlClientVoice.SetAttribute("RemotePort", UintegerValue(dlPortVoice));
    dlClientVoice.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientVoice.SetAttribute("PacketSize", UintegerValue(udpPacketSizeVoice));
    dlClientVoice.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaVoice)));

    // The bearer that will carry voice traffic
    EpsBearer voiceBearer(EpsBearer::GBR_CONV_VOICE);

    // The filter for the voice traffic
    Ptr<EpcTft> voiceTft = Create<EpcTft>();
    EpcTft::PacketFilter dlpfVoice;
    dlpfVoice.localPortStart = dlPortVoice;
    dlpfVoice.localPortEnd = dlPortVoice;
    voiceTft->Add(dlpfVoice);

    // Gaming configuration and object creation:
    UdpClientHelper ulClientGaming;
    ulClientGaming.SetAttribute("RemotePort", UintegerValue(ulPortGaming));
    ulClientGaming.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    ulClientGaming.SetAttribute("PacketSize", UintegerValue(udpPacketSizeGaming));
    ulClientGaming.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaGaming)));

    // The bearer that will carry gaming traffic
    EpsBearer gamingBearer(EpsBearer::GBR_GAMING);

    // The filter for the gaming traffic
    Ptr<EpcTft> gamingTft = Create<EpcTft>();
    EpcTft::PacketFilter ulpfGaming;
    ulpfGaming.remotePortStart = ulPortGaming;
    ulpfGaming.remotePortEnd = ulPortGaming;
    ulpfGaming.direction = EpcTft::UPLINK;
    gamingTft->Add(ulpfGaming);

    ThreeGppHttpClientHelper clientHelper (internetIpIfaces.GetAddress(1));
    clientHelper.SetAttribute ("RemoteServerPort", UintegerValue (ulPortBullet));
    clientHelper.SetAttribute ("Streaming", BooleanValue (true));

    BulkSendHelper ulBulk ("ns3::TcpSocketFactory",  InetSocketAddress(internetIpIfaces.GetAddress(1), ulPortBulk));// Set the amount of data to send in bytes.  Zero is unlimited.
    ulBulk.SetAttribute("MaxBytes", UintegerValue(0));
    
    VideoStreamServerHelper ulVideoServer (internetIpIfaces.GetAddress(1), ulPortStreaming);
    
    //ulVideoServer.SetAttribute ("MaxPacketSize", UintegerValue (1000));
    //ulVideoServer.SetAttribute ("FrameFile", StringValue ("./scratch/videoStreamer/frameList.csv"));
    /*
     * Let's install the applications!
     */
    ApplicationContainer clientApps;
    
    LatencyTracer *lt_arr [gridScenario.GetUserTerminals().GetN()]; 
    GnbResourceTracer *gnb_tracer = new GnbResourceTracer ();

    for (uint32_t i = 0; i < gridScenario.GetUserTerminals().GetN(); ++i)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(i);
        Ptr<NetDevice> ueDevice = ueNetDev.Get(i);
        Address ueAddress = ueIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        if (enableVoice)
        {
            NS_LOG_UNCOND("Start Voice application");
            dlClientVoice.SetAttribute("RemoteAddress", AddressValue(ueAddress));
            clientApps.Add(dlClientVoice.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, voiceBearer, voiceTft);
        }

        if (enableVideo)
        {
            NS_LOG_UNCOND("Start Video application");
            dlClientVideo.SetAttribute("RemoteAddress", AddressValue(ueAddress));
            clientApps.Add(dlClientVideo.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, videoBearer, videoTft);
        }

        // For the uplink, the installation happens in the UE, and the remote address
        // is the one of the remote host

        if (enableGaming)
        {
            NS_LOG_UNCOND("Start Gaming application for UE "<< i);
            ulClientGaming.SetAttribute("RemoteAddress",
                                        AddressValue(internetIpIfaces.GetAddress(1)));
            clientApps.Add(ulClientGaming.Install(ue));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, gamingBearer, gamingTft);
        }
        if (enableBulk)
        {
            NS_LOG_UNCOND("Start Bulk application for UE " << i);
            clientApps.Add(ulBulk.Install(ue));
            nrHelper->ActivateDedicatedEpsBearer(ueDevice, gamingBearer, gamingTft);
        }
        if (enableStreaming)
        {
            NS_LOG_UNCOND("Start Streaming application for UE " <<i);
            ApplicationContainer StreamServer = ulVideoServer.Install (ue);  
            clientApps.Add(StreamServer);

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, gamingBearer, gamingTft);
        }

        // Tracing
        lt_arr[i] = new LatencyTracer(i);
        LatencyTracer *e2eTracer = lt_arr[i];

            
        if (enableStreaming){ 
            clientApps.Get(i)->SetAttribute("SourceIdentification", UintegerValue(i));
            clientApps.Get(i)->TraceConnectWithoutContext("LatestFrameNum",
                                                            MakeBoundCallback (&LtAppFrameTracer, e2eTracer));
            clientApps.Get(i)->TraceConnectWithoutContext("RespondFrameNum",
                                                            MakeBoundCallback (&LtAppRespondTracer, e2eTracer));
        }
        
        
        // Need to trace RLC buffer
        e2eTracer->m_imsi = ueDevice->GetObject<NrUeNetDevice>()->GetImsi();

        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetMac()->TraceConnectWithoutContext("RlcBuffer",
                                                        MakeBoundCallback (&LtRlcBufferTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetMac()->TraceConnectWithoutContext("SchedulingRequest",
                                                        MakeBoundCallback (&LtSrTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetMac()->TraceConnectWithoutContext("BsrTrace",
                                                        MakeBoundCallback (&LtBsrTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetMac()->TraceConnectWithoutContext("BufferTrace",
                                                        MakeBoundCallback (&LtBufferTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetMac()->TraceConnectWithoutContext("SrState",
                                                        MakeBoundCallback (&LtSrStateTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetCcMap()[0]->GetPhy()->TraceConnectWithoutContext("VideoFrameTrace",
                                                        MakeBoundCallback (&LtPhyTracer, e2eTracer));
                                        
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("TriggerPreprocess",
                                                        MakeBoundCallback (&LtTrigPreprocessTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("TriggerProfile",
                                                        MakeBoundCallback (&LtTrigProfileTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("TriggerOutput",
                                                        MakeBoundCallback (&LtTrigOutputTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("TriggerOnPeriod",
                                                        MakeBoundCallback (&LtTrigOnPeriodTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("TriggerOffPeriod",
                                                        MakeBoundCallback (&LtTrigOffPeriodTracer, e2eTracer));
        ueDevice->GetObject<NrUeNetDevice>()->GetTargetEnb()->GetScheduler(0)->TraceConnectWithoutContext("ResourceBlockTrace",
                                                        MakeBoundCallback (&GnbAllocTracer, gnb_tracer));

        Simulator::Schedule (MilliSeconds (udpAppStartTimeMs), &WriteRawTracing, e2eTracer);

        Ptr<TcpL4Protocol> proto = ue->GetObject<TcpL4Protocol>();
        if (cca == "TcpCopa") {
            proto->SetAttribute("SocketType", TypeIdValue(TcpCopa::GetTypeId()));
        }
        else if (cca == "TcpCubic") {
            proto->SetAttribute("SocketType", TypeIdValue(TcpCubic::GetTypeId()));
        }
        else if (cca == "TcpBbr") {
            proto->SetAttribute("SocketType", TypeIdValue(TcpBbr::GetTypeId()));
        }
        else if (cca == "TcpVegas") {
            proto->SetAttribute("SocketType", TypeIdValue(TcpVegas::GetTypeId()));
        }
        else {
            NS_FATAL_ERROR("No such TCP congestion control algorithm");
        }
    }
                                                                                                           

    // start UDP server and client apps
    clientApps.Start(MilliSeconds(udpAppStartTimeMs));

    
    serverApps.Start(MilliSeconds(udpAppStartTimeMs));
    for (int i = 0; i < clientApps.GetN(); i++){
        clientApps.Get(i)->SetStartTime (MilliSeconds(udpAppStartTimeMs + i*50));
    }

    serverApps.Stop(MilliSeconds(simTimeMs));
    clientApps.Stop(MilliSeconds(simTimeMs));
    
    // remote host.. need to configure tracing source for each transmitter id
    video_clients.Get(0)->TraceConnectWithoutContext("LatestFrameNum",
                                                    MakeBoundCallback (&LtAppRxTracer, lt_arr, gridScenario.GetUserTerminals().GetN())); 

    // Create the folder
    if (!folderExists(root_simFolder)) {
        if (mkdir(root_simFolder.c_str(), 0777) == 0) {
            std::cout << "Root folder created: " << folder_name << std::endl;
        } else {
            std::cerr << "Failed to create root folder: " << folder_name << std::endl;
            return 1;
        }
    }

    if (mkdir(folder_name.c_str(), 0777) == 0) {
        std::cout << "Folder created: " << folder_name << std::endl;
    } else {
        std::cerr << "Failed to create folder: " << folder_name << std::endl;
        return 1;
    }

    
    nrHelper->GetPhyRxTrace()->SetResultsFolder(folder_name+"/");

    // enable the traces provided by the nr module
    nrHelper->EnableTraces();

    // gNB tracing
    gnb_tracer->m_folderName = folder_name;
    Config::Connect(
            "/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionReconfiguration",
            MakeBoundCallback(&LtGnbTracer, lt_arr, gridScenario.GetUserTerminals().GetN(), folder_name));

    // Mobility tracing
    AsciiTraceHelper ascii;
    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream("mobility-trace-example.mob"));

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(gridScenario.GetUserTerminals());

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(MilliSeconds(simTimeMs));
    Simulator::Run();

    /*
     * To check what was installed in the memory, i.e., BWPs of eNb Device, and its configuration.
     * Example is: Node 1 -> Device 0 -> BandwidthPartMap -> {0,1} BWPs -> NrGnbPhy -> Numerology,
    */
    //GtkConfigStore config;
    //config.ConfigureAttributes ();
    

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream outFile;
    std::string filename = folder_name + "/" + simTag;
    if (!fs::is_directory(folder_name))
    {
        fs::create_directories(folder_name);
    }
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                << t.destinationAddress << ":" << t.destinationPort << ") proto "
                << protoStream.str() << "\n";
        outFile << "  Tx Packets: " << i->second.txPackets << "\n";
        outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
        outFile << "  TxOffered:  "
                << i->second.txBytes * 8.0 / ((simTimeMs - udpAppStartTimeMs) / 1000.0) / 1000.0 /
                       1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            // double rxDuration = i->second.timeLastRxPacket.GetSeconds () -
            // i->second.timeFirstTxPacket.GetSeconds ();
            double rxDuration = (simTimeMs - udpAppStartTimeMs) / 1000.0;

            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            outFile << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "  Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            // outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << "
            // Mbps \n";
            outFile << "  Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / stats.size() << "\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();
    return 0;
}
