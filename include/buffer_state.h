#ifndef BUFFER_STATE_H
#define BUFFER_STATE_H

#include <mutex>
#include <atomic>

enum class bufferState{
    Free,
    Receiving,
    Ready,
    Processing    
};

struct SharedBuffer
{
    bufferState state[3] {
        bufferState::Free,
        bufferState::Free,
        bufferState::Free,
        //bufferState::Free
    };
    std::atomic<int> m_currentBuffer{0};
    std::mutex mutex;
};


#endif