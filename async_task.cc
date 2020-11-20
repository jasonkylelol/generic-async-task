#include "async_task.h"

namespace async {

AsyncThread::AsyncThread(uint32_t id) : m_b_stop(false), m_id(id) {
    auto func = [&]() {
        for (;;) {
            std::shared_ptr<AsyncBase> task;
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_cond.wait_for(lk, std::chrono::seconds(1));
                if (m_b_stop) return;
                if (!m_queue.empty()) {
                    task = m_queue.front();
                    m_queue.pop();
                }
            }
            if (nullptr != task.get()) {
                task->Exec();
            }
        }
    };
    m_thread.reset(new std::thread(std::bind(func)));
}

AsyncThread::~AsyncThread() {
    m_b_stop = true;
    m_cond.notify_all();
    if (m_thread->joinable()) m_thread->join();
}

void AsyncThread::Do(const std::shared_ptr<AsyncBase>& meta) {
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_queue.push(meta);
    }
    m_cond.notify_one();
}

AsyncPool::~AsyncPool() { m_pool.clear(); }

bool AsyncPool::Init(uint32_t num) {
    for (uint32_t i = 0; i < num; i++) {
        m_pool.emplace_back(new AsyncThread(i));
    }
    return true;
}

void AsyncPool::Dispatch(const std::shared_ptr<AsyncBase>& task) {
    int64_t id = RandomInt(0, m_pool.size() - 1);
    std::cout << "[Dispatch to " << id << "] ";
    m_pool[id]->Do(task);
}

}  // namespace async
