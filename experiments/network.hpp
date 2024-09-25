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

#endif
