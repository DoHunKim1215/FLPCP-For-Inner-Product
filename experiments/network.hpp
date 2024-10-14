#ifndef NETWORK_H
#define NETWORK_H

/*
  # Approximate network latency model

  It considers propagation delay and transmission delay.
  Processing delay doesn't have a important effect in these days.
  Queuing delay tends to be effected by many factor, so inevitably it is excluded from consideration.
  The following code assume the same network settings as the experiment from 
  https://www.usenix.org/conference/usenixsecurity23/presentation/li-yun.
*/
class Network
{
public:
    // LAN Assumption : 23Gbps bandwidth + 30 microsec. propagation delay
    static double GetLANDelay(size_t payloadBytes)
    {
        return 0.3 * 1e+5 + 0.3478 * payloadBytes;
    }

    // WAN Assumption : 8Mbps bandwidth + 40 millisec. propagation delay
    static double GetWANDelay(size_t payloadBytes)
    {
        return 0.4 * 1e+8 + 1e+2 * payloadBytes;
    }

    static double GetLANPayloadDelay(size_t payloadBytes)
    {
        return 0.3478 * payloadBytes;
    }

    static double GetWANPayloadDelay(size_t payloadBytes)
    {
        return 1e+2 * payloadBytes;
    }

    static constexpr double LANBaseDelayMs = 0.3 * 1e+5 * 1e-6;
    static constexpr double WANBaseDelayMs = 0.4 * 1e+8 * 1e-6;
};

class FLPCPMeasurement
{
public:
    size_t proofLength;
    size_t nQueries;
    double proverTime;
    double verifierTime;
    bool isVaild;

    FLPCPMeasurement& operator+=(const FLPCPMeasurement& obj)
    {
        this->proverTime += obj.proverTime;
        this->verifierTime += obj.verifierTime;
        return *this;
    }

    FLPCPMeasurement& operator/=(const size_t& divisor)
    {
        this->proverTime /= (double)divisor;
        this->verifierTime /= (double)divisor;
        return *this;
    }
};

struct FLIOPMeasurement
{
    size_t proofLength;
    size_t nQueries;
    double proverTime;
    double verifierTime;
    double LANTime;
    double WANTime;
    bool isVaild;

    FLIOPMeasurement& operator+=(const FLIOPMeasurement& obj)
    {
        this->proverTime += obj.proverTime;
        this->verifierTime += obj.verifierTime;
        this->LANTime += obj.LANTime;
        this->WANTime += obj.WANTime;
        return *this;
    }

    FLIOPMeasurement& operator/=(const size_t& divisor)
    {
        this->proverTime /= (double)divisor;
        this->verifierTime /= (double)divisor;
        this->LANTime /= (double)divisor;
        this->WANTime /= (double)divisor;
        return *this;
    }
};

struct OneRoundMeasurement
{
    double proverTimeNs;
    double verifierTimeNs;
    double communicationTimeNsInLAN;
    double communicationTimeNsInWAN;
    size_t totalPayloadSize;

    OneRoundMeasurement& operator+=(const OneRoundMeasurement& obj)
    {
        this->proverTimeNs += obj.proverTimeNs;
        this->verifierTimeNs += obj.verifierTimeNs;
        this->communicationTimeNsInLAN += obj.communicationTimeNsInLAN;
        this->communicationTimeNsInWAN += obj.communicationTimeNsInWAN;
        this->totalPayloadSize = obj.totalPayloadSize;
        return *this;
    }

    OneRoundMeasurement& operator/=(const size_t& divisor)
    {
        this->proverTimeNs /= (double)divisor;
        this->verifierTimeNs /= (double)divisor;
        this->communicationTimeNsInLAN /= (double)divisor;
        this->communicationTimeNsInWAN /= (double)divisor;
        return *this;
    }
};

struct IOPSchedule
{
    double time;
    std::vector<size_t> lambdas;
    std::vector<OneRoundMeasurement> trace;
};

#endif
