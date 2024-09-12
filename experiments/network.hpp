#ifndef NETWORK_H
#define NETWORK_H

class Network
{
public:
    static double GetLANDelay(size_t payloadBytes)
    {
        return 0.3 * 1e+5 + 1.3 * 1e+2 * payloadBytes;
    }

    static double GetWANDelay(size_t payloadBytes)
    {
        return 0.4 * 1e+8 + 1.7 * 1e+5 * payloadBytes;
    }
};

#endif
