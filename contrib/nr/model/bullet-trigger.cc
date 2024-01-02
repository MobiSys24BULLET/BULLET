/*  
    Operational flow of BULLET
    1. Profile traffic
    2. Estimate the actual resource demand
    3. If the base station receives bsr, then compare bsr with previously estimated demand
    4. Update error
    5. For every resource allocation, calculate confident demand with estimated error
    6. Provide confident demand first to multi-user scheduling algorithm, then provide the remaining resource demand if there exists. 
*/

#include "bullet-trigger.h"
#include "ns3/log.h"
#include <boost/math/distributions/normal.hpp>


namespace ns3
{

NS_LOG_COMPONENT_DEFINE("BulletTrigger");

BulletTrigger::BulletTrigger (uint16_t rnti, 
                            int sr_slot, int bsr_slot, int alloc_slot, int ack_slot): // input parameters --> 4, 8, 3, 5 in default 5G setting
m_rnti(rnti)
{
    TriggerLogging ();

    // Get ACK from PDCP & TBS success from MAC (should be modified in srsRAN)
    
    Config::Connect(
            "/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionReconfiguration",
            MakeCallback(&BulletTrigger::UpperLayerTracer, this));
    Config::Connect(
             "/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbMac/RxPdu",
             MakeCallback(&BulletTrigger::MacRxTracer, this));
    
    InputParameterTracer (sr_slot, bsr_slot, alloc_slot, ack_slot);
    PeriodicProcessor();
}

BulletTrigger::~BulletTrigger()
{

}

TypeId
BulletTrigger::GetTypeId()
{
    static TypeId tid = 
        TypeId("ns3::BulletTrigger")
            .SetParent<Object>()
            .AddTraceSource("PreprocessResults",
                            "The intermediate results and outputs of Trigger for every slot",
                            MakeTraceSourceAccessor(&BulletTrigger::m_preprocessTrace),
                            "ns3::BulletTrigger::PreprocessTracedCallback")
            .AddTraceSource("ProfilingResults",
                            "The intermediate results and outputs of Trigger for every slot",
                            MakeTraceSourceAccessor(&BulletTrigger::m_profilingTrace),
                            "ns3::BulletTrigger::ProfilingTracedCallback")
            .AddTraceSource("OutputResults",
                            "The intermediate results and outputs of Trigger for every slot",
                            MakeTraceSourceAccessor(&BulletTrigger::m_outputTrace),
                            "ns3::BulletTrigger::OutputTracedCallback")
            .AddTraceSource("OnPeriodLength",
                            "The length of on period",
                            MakeTraceSourceAccessor(&BulletTrigger::m_onPeriodTrace),
                            "ns3::BulletTrigger::PeriodTracedCallback")
            .AddTraceSource("OffPeriodLength",
                            "The length of off period",
                            MakeTraceSourceAccessor(&BulletTrigger::m_offPeriodTrace),
                            "ns3::BulletTrigger::PeriodTracedCallback");
    return tid;
}

void 
BulletTrigger::TriggerLogging () 
{
    NS_LOG_INFO("Trigger Logging Enabled");
}

void 
BulletTrigger::InputVariableTracer (int current_time, int sr, int bsr, bool get_bsr, int alloc, int ack)
{
    m_time_queue.push_back (current_time);
    m_sr_queue.push_back (sr);
    m_bsr_queue.push_back (bsr);
    m_get_bsr_queue.push_back(get_bsr);
    m_alloc_queue.push_back (alloc);
    m_ack_queue.push_back (ack);

    if (m_time_queue.size() > m_queue_size) {
        m_time_queue.pop_front ();
        m_sr_queue.pop_front ();
        m_bsr_queue.pop_front ();
        m_get_bsr_queue.pop_front ();
        m_alloc_queue.pop_front ();
        m_ack_queue.pop_front ();
    }
}

void 
BulletTrigger::InputParameterTracer (int sr_slot, int bsr_slot, int alloc_slot, int ack_slot)
{
    m_sr_slot = sr_slot;
    m_bsr_slot = bsr_slot;
    m_alloc_slot = alloc_slot;
    m_ack_slot = ack_slot;

    for (int i = 0; i < m_initial_input; i++) {
        m_sr_queue.push_back (0);
        m_bsr_queue.push_back (0);
        m_get_bsr_queue.push_back (0);
        m_time_queue.push_back (0);
        m_alloc_queue.push_back (0);
        m_ack_queue.push_back (0);
    }
}

double
BulletTrigger::GetCurrentSlot (double time) 
{
    // Get current slot number
    return time/slot_unit;
}

void
BulletTrigger::TriggerTracing ()
{
    // Tracing
    //std::cout<<"Trigger Tracing running"<<std::endl;
    m_preprocessTrace (m_rnti, t_input.sr, t_input.bsr, t_input.get_bsr, t_input.alloc, t_input.ack, m_totalBuffer);
    m_profilingTrace (m_rnti, fft_arrival, mode, m_period_app);
    m_outputTrace (m_rnti, estimated_arrival, m_proactiveBsr);
}

void
BulletTrigger::PeriodicProcessor ()
{
    Processor ();
    Simulator::Schedule (MicroSeconds (500), &BulletTrigger::PeriodicProcessor, this);
}   


struct HoltWintersModel {
    double alpha;  // Level smoothing coefficient
    double beta;   // Trend smoothing coefficient
    double gamma;  // Seasonality smoothing coefficient
    int seasonLength;  // Length of the season
    std::vector<double> level;  // Level component
    std::vector<double> trend;  // Trend component
    std::vector<double> seasonality;  // Seasonal component

    HoltWintersModel(double alpha, double beta, double gamma, int seasonLength)
        : alpha(alpha), beta(beta), gamma(gamma), seasonLength(seasonLength) {}
};

void initializeHoltWintersModel(HoltWintersModel& model, const std::vector<double>& data) {
    // Initialize level and trend
    model.level.push_back(data[0]);
    model.trend.push_back(data[1] - data[0]);

    // Initialize seasonality
    for (int i = 0; i < model.seasonLength; ++i) {
        model.seasonality.push_back(data[i] - model.level[0]);
    }
}

double updateAndForecast(HoltWintersModel& model, double newData) {
    int dataSize = model.level.size();
    int lastSeason = (dataSize >= model.seasonLength) ? dataSize - model.seasonLength : 0;

    // Update components
    double newLevel = model.alpha * (newData - model.seasonality[lastSeason]) + (1 - model.alpha) * (model.level.back() + model.trend.back());
    double newTrend = model.beta * (newLevel - model.level.back()) + (1 - model.beta) * model.trend.back();
    double newSeasonality = model.gamma * (newData - newLevel) + (1 - model.gamma) * model.seasonality[lastSeason];

    // Add updated components to the model
    model.level.push_back(newLevel);
    model.trend.push_back(newTrend);
    model.seasonality.push_back(newSeasonality);

    // Forecast next point
    return newLevel + newTrend + model.seasonality[lastSeason + 1];
}

// TODO (230410): DetectArrival, ProfileTraffic, Evaluation through microbenchmark
void 
BulletTrigger::Processor ()
{
    double time = Simulator::Now().GetSeconds();
    int sr = t_input.sr;
    int bsr = t_input.bsr;
    bool get_bsr = t_input.get_bsr;
    int alloc = t_input.alloc;
    int ack = t_input.ack;


    // Actual demand estimator
    // Transform time into slot (should be modified in srsRAN)
    m_current_slot = GetCurrentSlot (time);

    /* Detect traffic arrival to profile*/
    InputVariableTracer (m_current_slot, sr, bsr, get_bsr, alloc, ack);

    DetectionOutput d_out = DetectArrival (m_current_slot, m_sr_queue, m_bsr_queue, m_get_bsr_queue, m_alloc_queue);
    int arrival = d_out.arrival;
    int arrival_slot = d_out.arrival_slot;
    fft_arrival = arrival;
    arrival_slot = std::max(arrival_slot, 1);
    //arrival = arrival/arrival_slot;
    double period_app = m_period_app;

    // Detect on-period and profile traffic arrival pattern
    if (arrival > 0) {
        // Start of on-period
        if (m_onPeriod == false) {
            if (!get_bsr) {
                m_startDiff = m_sr_slot*slot_unit;
            }
            else {
                m_startDiff = m_bsr_slot*slot_unit;
            }

            // Trace off period length
            if (m_periodStartTime != 0) {
                double currentTime = Simulator::Now().GetSeconds();
                double periodLength = currentTime - m_periodStartTime;

                m_offPeriodLength = 0.25*periodLength + 0.75*m_offPeriodLength;
                m_estimator.add_sample(periodLength);
                m_offPeriodTrace (m_rnti, currentTime, periodLength);
            }

            m_periodStartTime = Simulator::Now().GetSeconds() - m_startDiff;
        }
        avg_arrival += arrival;
        m_periodEndTime = Simulator::Now().GetSeconds();
        m_onPeriod = true;
    }
    else {
        if (m_onPeriod == true) {
            double currentTime = Simulator::Now().GetSeconds();
            if (currentTime - m_periodEndTime > m_onThreshold) {
                m_onPeriod = false;
                double periodLength = m_periodEndTime - m_periodStartTime;

                // Trace on period length
                m_onPeriodTrace (m_rnti, currentTime, periodLength);
                m_onPeriodLength = 0.2*periodLength + 0.8*m_onPeriodLength;

                avg_arrival = avg_arrival/(((periodLength)/slot_unit));
                if (estimated_arrival == 0) {
                    estimated_arrival = avg_arrival;
                }
                else {
                    estimated_arrival = 0.2*avg_arrival + 0.8*estimated_arrival;
                    std::cout<<"Estimated arrival: "<<avg_arrival<<" "<<estimated_arrival<<" "<<m_periodEndTime<<" "<<m_periodStartTime<<std::endl;
                }
                avg_arrival = 0;
                m_periodStartTime = m_periodEndTime - m_bsr_slot*slot_unit;
            }
        }
        else {
            if (m_scheduleMode == 5) {
                estimated_arrival = 0.8*estimated_arrival;
            }
        }
    }

    /* Proactive BSR estimation */
    double timeGap = Simulator::Now().GetSeconds() - m_periodStartTime;
    event = 0;

    if (m_onPeriod) {
        if (exponential_cdf(timeGap, m_onPeriodLength) < 0.8) {
            event = 1;
        }
    }
    else {
        double mean = m_estimator.get_mean();
        double std = m_estimator.get_stddev();
        double min = m_estimator.get_min_recent();
        double max = m_estimator.get_max_recent();

        event = truncated_normal_cdf(timeGap, mean, std, min, max);
        //event = exponential_cdf(timeGap, m_offPeriodLength, 0.001)*5;
    }

    //event *= 5;
    if (event < 0) {
        event = 0;
    }

    // we add reactive BSR in the scheduling stage
    if (m_onPeriod) {
        if (!get_bsr) {
            m_proactiveBsr += event*estimated_arrival;
            if (m_proactiveBsr > m_bsr_slot*estimated_arrival) {
                m_proactiveBsr = m_bsr_slot*estimated_arrival;
            }
        }
        else if (get_bsr) {
            //m_proactiveBsr += event*estimated_arrival;
            m_proactiveBsr = event*estimated_arrival;
        }
    }
    else {
        m_proactiveBsr = event*estimated_arrival;
        //std::cout<<"Off period proactive Bsr: "<<event<<" "<<estimated_arrival<<" "<<m_proactiveBsr<<" "<<m_offPeriodLength<<" "<<timeGap<<std::endl;
    }

    // grant_free
    if (m_scheduleMode == 1) {
        m_proactiveBsr = 2000;
    }
    else if (m_scheduleMode == 0 || m_scheduleMode == 4) {
        m_proactiveBsr = 0;
    }
    else if (m_scheduleMode == 3) {
        //m_proactiveBsr = 10000;
    }
    else if (m_scheduleMode == 5) {
        m_proactiveBsr = estimated_arrival;
    }
    else if (m_scheduleMode == 6) {
        m_proactiveBsr = 50000;
    }

    if (m_onPeriod) {
        mode = 1;
    }
    else {
        mode = 0;
    }

    // not period, it is multiplier to estimated arrival rate
    m_period_app = event;

    TriggerTracing ();

    /* Confidence-based resource alloactor */
    int identifiedDemand = static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(bsr));

    slotDemandMap[m_current_slot] = m_proactiveBsr + identifiedDemand;

    // update error
    if (sr || get_bsr) {
        int reportedDelay = 0;
        if (sr) {
            reportedDelay = m_sr_slot;
        }
        else if (get_bsr) {
            reportedDelay = m_bsr_slot;
        }
        int reportedSlot = m_current_slot - reportedDelay;
        if (slotDemandMap.find(reportedSlot) != slotDemandMap.end()) {
            int trueDemand = std::max(static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(bsr)) + m_recentGranted, 0);

            int error =  slotDemandMap[reportedSlot] - trueDemand;
            slotErrorMap[reportedSlot] = error;

            // Get recent error samples
            std::vector<int> values;

            //// Reserve space in the vector for efficiency
            int reserveSize = std::min(static_cast<int>(slotDemandMap.size()), max_sample);
            values.reserve(reserveSize);

            //// Iterate from largest to smallest key, but stop at target or end of map
            int count = 0;
            for (auto it = slotDemandMap.rbegin(); it != slotDemandMap.rend() && count < max_sample; ++it, ++count) {
                values.push_back(it->second); // Store the value
            }

            // Get average and std value
            double mean = 0;
            double standardDeviation = 0;
            if (values.size () > 10) {
                double sum = 0.0;
                for (int value : values) {
                    sum += value;
                }
                mean = sum / values.size();

                double squaredSum = 0.0;
                for (int value : values) {
                    squaredSum += std::pow(value - mean, 2);
                }
                standardDeviation = std::sqrt(squaredSum / values.size());
            }
            m_confidentBsr = m_proactiveBsr + identifiedDemand - (int)mean - (int)(m_zScore*standardDeviation);
            m_confidentBsr = std::max(m_confidentBsr, identifiedDemand);

            std::cout<<"Estimation: "<< m_proactiveBsr + identifiedDemand << " "<<m_confidentBsr<<" "<< identifiedDemand << " "<<m_zScore*standardDeviation<< " "<<mean <<" "<< m_zScore<<std::endl;

        }
    }
    
    // The processor ended. Reset the input 
    t_input.sr = 0;
    //t_input.bsr = 0;
    t_input.get_bsr = 0;
    t_input.alloc = 0;
    t_input.ack = 0;
}



double 
BulletTrigger::exponential_cdf(double x, double scale) {
    return 1.0 - std::exp(-x/scale);
}

double 
BulletTrigger::laplace_cdf(double x, double mu, double b) {
    double z = (x - mu) / b;
    
    if (z < 0) {
        return 0.5 * std::exp(z);
    } else {
        return 1 - 0.5 * std::exp(-z);
    }
}

double 
BulletTrigger::normal_cdf(double x, double mu, double sigma) {
    return 0.5 * (1.0 + std::erf((x - mu) / (sigma * std::sqrt(2.0))));
}

double 
BulletTrigger::truncated_normal_cdf(double x, double mu, double sigma, double lower_bound, double upper_bound) {
    double cdf_value = normal_cdf(x, mu, sigma);
    
    // Normalizing (truncating)
    double cdf_lower = normal_cdf(lower_bound, mu, sigma);
    double cdf_upper = normal_cdf(upper_bound, mu, sigma);

    cdf_value = (cdf_value - cdf_lower) / (cdf_upper - cdf_lower);
    if (cdf_value < 0) {
        cdf_value = 0;
    }
    
    return cdf_value;
}



BulletTrigger::DetectionOutput
BulletTrigger::DetectArrival (int current_time, std::deque<int> sr_queue, std::deque<int> bsr_queue, 
            std::deque<bool> get_bsr_queue, std::deque<int> alloc_queue) 
{
    // Detect arrival only when there is BSR receieved
    DetectionOutput d_out;
    if (sr_queue.back() == true) {
        d_out.arrival += 50;
    }
    if (get_bsr_queue.back() == false) {
        return d_out;
    }
    // Start arrival detection
    else {
        // Detect arrival with minimum bound
        int alloc_sum = 0;
        int add_value = 1;

        int max_arrival = 0;
        int min_arrival = 0;

        int curr_queue_size = get_bsr_queue.size();
        int last_bsr_index = 1;

        int queue_range = std::min(100, curr_queue_size);

        // Get last BSR index
        for (int i = 2; i < queue_range; i++) {
            if (get_bsr_queue[curr_queue_size - i] == true) {
                last_bsr_index = i;
                break;
            }
            else if (sr_queue[curr_queue_size - i - m_bsr_slot + m_sr_slot] > 0) {
                last_bsr_index = i;
                break;
            }
        }
        // Get the sum of allocation
        for (int i = 0; i < last_bsr_index - 1; i++) {
            int temp_index = curr_queue_size - i - 1;
            std::cout<<"Alloc at "<< (m_time_queue[temp_index]/2000.0)<<" "<<alloc_queue[temp_index]<<std::endl;
            alloc_sum += alloc_queue[temp_index];
        }
        m_recentGranted = alloc_sum;

        // Get buffer
        int curr_bsr = bsr_queue[curr_queue_size - 1];
        int last_bsr = bsr_queue[curr_queue_size - last_bsr_index];

        max_arrival = static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(curr_bsr)) - static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(std::max(last_bsr,0)))  + alloc_sum;
        if (max_arrival > 0) {
            min_arrival = static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(std::max(curr_bsr,0))) - static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(last_bsr))  + alloc_sum;
        }
    
        
        last_bsr_index = std::max(last_bsr_index, 1);
        
        std::cout<<"Indexes: "<<last_bsr_index<<std::endl;

        d_out.arrival += max_arrival;
        d_out.arrival_slot = last_bsr_index;


        std::cout<<Simulator::Now().GetSeconds()<<" RNTI: "<<m_rnti<<" Curr buffer: "<<static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(std::max(curr_bsr-1,0)))<<
                    " Last buffer: "<<static_cast<int>(NrMacShortBsrCe::FromLevelToBytes(last_bsr))<<
                    " Alloc: "<<alloc_sum<<" Min arrival: "<<min_arrival/last_bsr_index<<" First alloc time: "<<m_time_queue[curr_queue_size - last_bsr_index - m_bsr_slot - m_alloc_slot]<<std::endl;

        return d_out;
    }
}

BulletTrigger::ProfileOutput
BulletTrigger::ProfileTraffic (int arrival)
{
    ProfileOutput p_out;
    p_out.mode = -1;
    p_out.period_app = -1;

    m_arrival_queue.push_back (arrival);
    mode = 1;
    double temp_period = 0;

    double diff = 100;
    if (!m_proactive_queue.empty()) {
        diff = std::abs(m_proactive_queue.front() - Simulator::Now().GetSeconds());

        if (diff < 5*slot_unit) {
            m_residualMode = 0;
            m_residualStack = 0;
            m_proactive_queue.pop_front();
        }
    }

    m_transMode = false;
    // 1. Check the traffic pattern from ACK packets
    m_transMode = DetectAck (m_ack_queue, true);
    if (m_transMode){
        mode = 0;
    }
    if (m_transMode == false) {
        m_appMode = true;
    }
    else if (m_transMode == true) {
        if (diff < 10*slot_unit) {
            m_appMode = true;
            m_appThreshold = 0;
        }
        else {
            m_appThreshold += 1;
            if (m_appThreshold >= 3) {
                m_appMode = false;
                m_appThreshold = 0;
                m_app_arrival_queue.clear();
            }
        }
    }
    // 2. Do STFT analysis to check the periodicity of the traffic
    if (m_appMode) {
        //m_app_arrival_queue.push_back (arrival);
        // Check the estimation accuracy of proactive BSR on application traffic

        STFTOutput s_out = CalculateFFT (m_app_arrival_queue);
        if (s_out.is_principal == true) {
            //s_out.app_freq = 0.033;
            double next_app_time = Simulator::Now().GetSeconds() - m_bsr_slot*slot_unit + s_out.app_freq*2*slot_unit*1000; // *2 is a compenstation for FFT abs calculation
            mode = 1;
            temp_period = next_app_time; //s_out.app_freq;
            std::cout<<"FFT results for "<<m_rnti<<" "<<Simulator::Now().GetSeconds()<<" "<<m_bsr_slot<<" "<<slot_unit<<" "<<s_out.app_freq<<" "<<next_app_time<<" "<<m_app_arrival_queue.size()<<std::endl;
        }
        else {
            mode = 2;
        }
    }
    
    p_out.mode = mode;
    p_out.period_app = temp_period;

    return p_out;
}

// Need to modified for scheduling slot (2.5ms)
bool
BulletTrigger::DetectAck (std::deque<int> ack_queue, bool profile_or_estimate)
{
    // true: profile
    int ack_threshold_unit  = 0;
    int ack_deliver_time = m_ack_slot;
    if (profile_or_estimate) {
        ack_threshold = 0.002;
        ack_threshold_unit = (ack_threshold/slot_unit + m_bsr_slot);
    }
    else {
        ack_threshold_unit = 4;
        ack_deliver_time = 0;
    }

    // Solution1 --> fixed threshold slots
    if (ack_queue.size() > ack_deliver_time + ack_threshold_unit ) {
        for (int i = 0; i < ack_threshold_unit; i++) {
             if (ack_queue[ack_queue.size() - m_ack_slot - i] > 0) {
                return true;
            }
        }
    }
    /*
    // Solution2 --> dynamic threshold slots
    for (int i = 0; i < dynamic_threshold_unit; i++) {
        if (ack_queue[ack_queue.size() -])
    }
    */

    return false;
}

void
BulletTrigger::fft(std::complex<double> *x, int n)
{
    if (n == 1) return;
    std::complex<double> wn = std::polar(1.0, 2 * M_PI / n);
    std::complex<double> w = 1;
    std::complex<double> u[n / 2], v[n / 2];
    for (int i = 0; i < n / 2; i++) {
        u[i] = x[i * 2];
        v[i] = x[i * 2 + 1];
    }
    fft(u, n / 2);
    fft(v, n / 2);
    for (int i = 0; i < n / 2; i++) {
        x[i] = u[i] + w * v[i];
        x[i + n / 2] = u[i] - w * v[i];
        w *= wn;
    }
}

struct ComplexGreater {
    bool operator()(const std::complex<double>& x, const std::complex<double>& y) const {
        return std::abs(x) > std::abs(y);
    }
};

double 
BulletTrigger::getMaxFrequency (std::complex<double> *fft_output,  size_t fft_size, double sampling_rate) {
    int max_index = 0;
    double max_magnitude = 0;
    
    for (size_t i = 1; i < fft_size / 2; ++i) {
        double magnitude = std::abs(fft_output[i]);
        
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }

    double max_frequency = max_index * (sampling_rate / fft_size);
    std::cout<<"Max index/freq: "<<max_index<<" "<<max_frequency<<std::endl;
    return max_frequency;
}

BulletTrigger::STFTOutput
BulletTrigger::CalculateFFT (std::deque <double> my_queue)
{
    int size = my_queue.size();
    STFTOutput s_out;

    if (size < 100) {
        s_out.app_freq = 0.033; 
        s_out.is_principal = true;
    }
    else {
        std::complex <double> x[size];
        double sum = std::accumulate(my_queue.begin(), my_queue.end(), 0.0);
        double average = sum / my_queue.size();

        for (int i = 0; i < size; i++){
            x[i] =  my_queue[i] - average;
        }
        fft (x, size);

        double app_freq = getMaxFrequency(x, size, 2000);
        app_freq = (1000/app_freq)/1000;
        s_out.app_freq = app_freq;
        s_out.is_principal = true;
    }

    return s_out;
}

void
BulletTrigger::UpperLayerTracer (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti) {
    if (rnti == m_rnti){
        std::ostringstream basePath;
        basePath << context.substr(0, context.rfind('/')) << "/UeMap/" << (uint32_t)rnti;
        Config::ConnectWithoutContext(basePath.str() + "/DataRadioBearerMap/*/LtePdcp/DLTrace",
                            MakeCallback(&BulletTrigger::PdcpTracer, this));    
    }
}

void
BulletTrigger::MacRxTracer (std::string context, uint32_t size, uint16_t rnti) {
    if (rnti == m_rnti){
        t_input.alloc += size;
        std::cout<<"new Trigger Mac RX tracer"<<size<<" "<<rnti<<" "<<t_input.alloc<<std::endl;
    }
}

void
BulletTrigger::PdcpTracer (Ipv4Header & ipHeader, TcpHeader & tcpHeader) {

    Ipv4Address addr = ipHeader.GetDestination();
    uint16_t port = tcpHeader.GetDestinationPort();
    SequenceNumber32 ack = tcpHeader.GetAckNumber();

    // Flow should be identified
    if (m_flowAddress == 0) {
        m_flowAddress = addr.Get(); 
        m_flowPort = port;
    }

    // We only care about the flow we are interested in
    if (m_flowAddress == addr.Get() && m_flowPort == port){
        if (ack > SequenceNumber32(10)) {
            t_input.ack = ack.GetValue();
            //m_outer->m_ackTrace (gt_pdcp_ack, gt_rnti);
        }
    }
    //std::cout<<Simulator"PDCP ACK "<<addr<<" "<<port<<" "<<ack<<" "<<std::endl;
}

int
BulletTrigger::GetProactiveBSR () {
    int returnValue = m_proactiveBsr;
    //m_proactiveBsr = 0;
    //returnValue = 2000;
    return returnValue;
}

int
BulletTrigger::GetConfidentBSR () {
    int returnValue = m_confidentBsr;
    //m_proactiveBsr = 0;
    //returnValue = 2000;
    return returnValue;
}

void
BulletTrigger::SetScheduleMode (int scheduleMode) {
    m_scheduleMode = scheduleMode;
}

void
BulletTrigger::SetBeta (double betaVal) {
    m_betaVal = betaVal;
    
    boost::math::normal_distribution<> dist(0, 1); 
    m_zScore = boost::math::quantile(dist, m_betaVal);
}

}