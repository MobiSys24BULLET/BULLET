/*  
    Operational flow of Trigger
    1. Profile traffic
    2. Get the estimated buffer
*/

#ifndef BULLETTRIGGER_H
#define BULLETTRIGGER_H

#include "ns3/object.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/traced-value.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "nr-mac-short-bsr-ce.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
//#include "nr-mac-scheduler-ns3.h"

#include "ns3/core-module.h"
#include <deque>
#include <complex>
#include <numeric>
#include <ns3/config.h>
#include <cmath>

namespace ns3 
{

class OnlineGaussianEstimator {
private:
    double mean;
    double M2;  // The second raw moment
    int n;  // Number of samples
    std::deque<double> recent_samples = {0.0};
    
public:
    OnlineGaussianEstimator() : mean(0.0), M2(1.0), n(0) {}

    void add_sample(double x) {
        n++;
        double delta = x - mean;
        mean += delta / n;
        M2 += delta * (x - mean);  // The expression used to update M2 is numerically stable
        if (recent_samples.size() > 50) {
            recent_samples.pop_front();  // Remove oldest sample if more than 50 samples
        }
        recent_samples.push_back(x);
        std::cout<<"Mean: " <<mean<<" std: "<<M2<<" "<<recent_samples.size()<<std::endl;
    }

    double get_mean() const {
        return mean;
    }

    double get_variance() const {
        if(n < 2) {
            std::cerr << "At least two samples are needed to calculate variance.\n";
            return 1.0;
        }
        return M2 / (n - 1);
    }

    double get_stddev() const {
        return std::sqrt(get_variance());
    }


    double get_min_recent() const {
        return *std::min_element(recent_samples.begin(), recent_samples.end());
    }

    double get_max_recent() const {
        double return_value = *std::max_element(recent_samples.begin(), recent_samples.end());
        if (return_value == 0) {
            return_value = 100;
        }
        return return_value; 
    }
};

class BulletTrigger: public Object 
{
    public:
        BulletTrigger(uint16_t rnti, int sr_slot, int bsr_slot, int alloc_slot, int ack_slot);
        ~BulletTrigger() override;

        static TypeId GetTypeId ();

        // Logging function
        void TriggerLogging ();

        // Get variables and parameters from mac layer
        void InputVariableTracer (int current_time, int sr, int bsr, bool get_bsr, int alloc, int ack); // called every trigger operation
        void InputParameterTracer (int sr_slot, int bsr_slot, int alloc_slot, int ack_slot); // called at the constructor once
        double GetCurrentSlot (double time);

        // For tracing
        void TriggerTracing ();

        // Process traffic queue and estimated proactive BSR
        void PeriodicProcessor ();
        void Processor ();
        
        struct TriggerInput
        {
            double time = 0;
            int sr = 0;
            int bsr = 0;
            bool get_bsr = 0;
            int alloc = 0;
            int ack = 0;
        };

        struct DetectionOutput
        {
            int arrival = 0;
            int arrival_slot = 0;
        };

        struct ProfileOutput
        {
            int mode;
            double period_app;
        };
        
        struct STFTOutput
        {
            double app_freq;
            bool is_principal;
        };

        double exponential_cdf (double x, double lambda);
        double laplace_cdf (double x, double mu, double b);
        double normal_cdf (double x, double mu, double sigma);
        double truncated_normal_cdf (double x, double mu, double sigma, double lower_bound, double upper_bound);

        // Detect arrival from link-layer information
        DetectionOutput DetectArrival (int current_time, std::deque<int> sr_queue, std::deque<int> bsr_queue, std::deque<bool> get_bsr_queue,
            std::deque<int> alloc_queue);
        ProfileOutput ProfileTraffic (int arrival);
        // Detect ACK to estimate the next traffic arrival in the transport mode
        bool DetectAck (std::deque<int> ack_queue, bool profile_or_estimate);

        // STFT analysis for periodic traffic detection
        void fft (std::complex<double> *x, int n);
        double getMaxFrequency (std::complex<double> *fft_output,  size_t fft_size, double sampling_rate);
        STFTOutput CalculateFFT (std::deque <double> my_queue);

        void UpperLayerTracer (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);
        void MacRxTracer (std::string context, uint32_t size, uint16_t rnti);
        void PdcpTracer (Ipv4Header & ipHeader, TcpHeader & tcpHeader);
        
        // Get proactive BSR and reset it as zero
        int GetProactiveBSR ();

        // Get confident BSR and reset it as zero
        int GetConfidentBSR ();
        
        // Get scheduling mode for comparison
        void SetScheduleMode (int schedulingMode);

        // Get beta value for confidence based resource allocaotr
        void SetBeta (double betaVal);

        // We get the input with struct in Trigger
        TriggerInput t_input;

        // Tracecallback
        typedef void (*PreprocessTracedCallback)(uint16_t rnti, int sr, int bsr, bool get_bsr, int alloc, int ack, int totalBuffer);
        TracedCallback<uint16_t, int, int, bool, int, int, int> m_preprocessTrace; //!< Trace callback for Proactive BSR

        // Tracecallback
        typedef void (*ProfilingTracedCallback)(uint16_t rnti, int arrival, int mode, double period_app);
        TracedCallback<uint16_t, int, int, double> m_profilingTrace; //!< Trace callback for Proactive BSR

        // Tracecallback
        typedef void (*OutputTracedCallback)(uint16_t rnti, int estimated_arrival, int proactive_bsr);
        TracedCallback<uint16_t, int, int> m_outputTrace; //!< Trace callback for Proactive BSR
        
        // Tracecallback
        typedef void (*PeriodTracedCallback)(uint16_t rnti,double Time, double length);
        TracedCallback<uint16_t, double, double> m_onPeriodTrace; //!< Trace callback for on period length
        TracedCallback<uint16_t, double, double> m_offPeriodTrace; //!< Trace callback for off period length

        int m_proactiveBsr = 0;

        int m_totalBuffer = 0;

        double m_startDiff = 0;

        OnlineGaussianEstimator m_estimator;

        double m_betaVal = 0.0;
        double m_zScore = 0.0;

    // Variables of Trigger. Note that we use delay variables as integer since the unit of the time is a 'slot'
    private:

        // identifier
        uint16_t m_rnti;
        uint32_t m_flowAddress = 0;
        uint16_t m_flowPort = 0;

        // logging
        std::ofstream fft_file;
        std::ofstream arrival_file;

        // input variables
        std::deque <int> m_time_queue;
        std::deque <int> m_sr_queue;
        std::deque <int> m_bsr_queue;
        std::deque <bool> m_get_bsr_queue;
        std::deque <int> m_alloc_queue;
        std::deque <int> m_ack_queue;  
        int m_current_slot;

        // input parameters
        double slot_unit = 0.0005;
        int m_sr_slot;
        int m_bsr_slot;
        int m_alloc_slot;
        int m_ack_slot;

        int m_queue_size = 1000;
        int m_initial_input = 50;
        double m_alpha = 0.8;
        double ack_threshold = 0.003;
        int m_fftQueueSize = 400;

        // in-function variables
        double event = 0;
        int mode = 0; // 0: transport mode, 1: periodic app, 2: residual
        bool m_appMode = 0;
        bool m_residualMode = 0;
        bool m_transMode = 1;
        int period_trans = 0;
        int estimated_arrival = 0;
        int fft_arrival = 0;
        double m_period_app = 0;
        std::deque <int> m_arrival_queue;
        std::deque <double> m_app_arrival_queue;
        std::deque <double> m_event_app_queue;
        std::deque <double> m_past_app_queue;
        std::deque <double> m_proactive_queue;
        std::deque <double> m_event_occur_queue;
        int m_residualStack = 0;
        int m_appThreshold = 0;
        // output variable
        int m_scheduleMode = 0;

        bool m_onPeriod = false;
        int avg_arrival = 0;
        double m_onThreshold = 0.005;
        double m_periodStartTime = 0;
        double m_periodEndTime = 0;
        double m_onPeriodLength = 0;
        double m_offPeriodLength = 0;

        // confidence-based resource allocator
        std::map <int, int> slotDemandMap;
        std::map <int, int> slotErrorMap;
        int m_recentGranted;
        int max_sample = 300;
        int m_confidentBsr;
};

}

#endif 
