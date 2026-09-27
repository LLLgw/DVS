#ifndef BUFFER_STATE_H
#define BUFFER_STATE_H

#include <mutex>
#include <atomic>
#include "GlobalVars.h"

enum class bufferState{
    Free,
    Receiving,
    Ready,
    Processing    
};

struct SharedBuffer
{
    bufferState state[REBUFFERSIZE] {
        bufferState::Free,
        bufferState::Free,
        bufferState::Free
    };
    std::atomic<int> re_currentBuffer{0};
    std::mutex mutex;

    bufferState sendFinalSpectrumState[SEBUFFERSIZE]{
        bufferState::Free
    };
    std::atomic<int> se_currentBuffer{0};
};


#endif