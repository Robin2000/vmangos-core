#ifndef MANGOS_MOVEMENT_BROADCASTER_H
#define MANGOS_MOVEMENT_BROADCASTER_H

#include "Platform/Define.h"

#include <atomic>
#include <chrono>
#include <vector>
#include <cstddef>
#include <memory>
#include <thread>
#include <functional>

class PlayerBroadcaster;
class MovementBroadcaster;

class MovementBroadcasterWorker
{
public:
    MovementBroadcasterWorker(int threadId, MovementBroadcaster* broadcaster) : m_threadId(threadId), m_broadcaster(broadcaster) {};
    virtual void run();
    int m_threadId;
    MovementBroadcaster* m_broadcaster;
};

class MovementBroadcaster final
{
    friend class MovementBroadcasterWorker;
    typedef std::set<std::shared_ptr<PlayerBroadcaster> > PlayersBCastSet;

    std::size_t m_num_threads;

    std::atomic_bool m_stop;
    std::vector<std::unique_ptr<std::thread, std::function<void(std::thread *)>>> m_threads;
    std::chrono::milliseconds m_sleep_timer;

    std::vector<PlayersBCastSet> m_thread_players;
    std::vector<std::mutex> m_thread_locks;

    void Work(std::size_t thread_id);
    void BroadcastPackets(std::size_t index, uint32& num_packets);
    uint32 IdentifySlowMap(std::size_t thread_id);

public:
    MovementBroadcaster(std::size_t threads, std::chrono::milliseconds frequency);
    ~MovementBroadcaster();

    void RegisterPlayer(std::shared_ptr<PlayerBroadcaster> const& player);
    void RemovePlayer(std::shared_ptr<PlayerBroadcaster> const& player);

    void StartThreads();
    void UpdateConfiguration(std::size_t new_threads_count, std::chrono::milliseconds new_frequency);
    void Stop();

    struct ThreadUpdateStats
    {
        uint32 update_time;
        uint32 num_packets;
        int32 slow_instance;
    };
    std::vector<ThreadUpdateStats> const& GetStats() const { return m_thread_update_stats; }
    std::chrono::milliseconds GetSleepTimer() const { return m_sleep_timer; }
    std::size_t GetNumThreads() const { return m_num_threads; }
    bool IsMapSlow(uint32 instanceId);
    bool IsEnabled();

protected:
    std::vector<ThreadUpdateStats> m_thread_update_stats;
};

#endif
