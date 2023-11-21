#ifndef ASYNC_TASK_H
#define ASYNC_TASK_H

#include <condition_variable>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>

namespace async {

inline int64_t RandomInt(int64_t begin, int64_t end) {
    if (begin > end) return -1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dis(begin, end);
    return dis(gen);
}

class AsyncBase {
   public:
    AsyncBase() {}
    virtual ~AsyncBase() {}
    virtual void Exec() = 0;
};  // class AsyncBase

template <typename F, typename P>
class AsyncTask : public AsyncBase {
   private:
    F m_func;
    P m_param;
    AsyncTask() {}

   public:
    virtual ~AsyncTask() {}

    AsyncTask(const F& f, const P& p) {
        m_func = f;
        m_param = p;
    }

    AsyncTask(const AsyncTask& m) {
        m_func = m.m_func;
        m_param = m.m_param;
    }
    AsyncTask& operator=(const AsyncTask& m) {
        m_func = m.m_func;
        m_param = m.m_param;
        return *this;
    }

    virtual void Exec() override final { m_func(m_param); }
};  // class AsyncTask

class AsyncThread {
   public:
    AsyncThread(uint32_t);
    virtual ~AsyncThread();
    void Do(const std::shared_ptr<AsyncBase>&);

   private:
    AsyncThread(const AsyncThread&) = delete;
    AsyncThread& operator=(const AsyncThread&) = delete;

   private:
    uint32_t m_id;
    bool m_b_stop;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    std::queue<std::shared_ptr<AsyncBase>> m_queue;
    std::shared_ptr<std::thread> m_thread;
};  // class AsyncThread

class AsyncPool {
   public:
    static AsyncPool* Instance() {
        static AsyncPool ins;
        return &ins;
    }
    virtual ~AsyncPool();
    bool Init(uint32_t num);
    void Dispatch(const std::shared_ptr<AsyncBase>&);

   private:
    AsyncPool() = default;
    AsyncPool(const AsyncPool&) = delete;
    AsyncPool& operator=(const AsyncPool&) = delete;

   private:
    std::vector<std::shared_ptr<AsyncThread>> m_pool;
};  // class AsyncPool

}  // namespace async

#endif  // ASYNC_TASK_H
