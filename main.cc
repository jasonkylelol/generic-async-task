#include <iostream>
#include <sstream>

#include "async_task.h"

using namespace async;

class SumObj {
   public:
    uint32_t a, b;

   public:
    SumObj& operator=(const SumObj& m) {
        a = m.a;
        b = m.b;
        return *this;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "test begin..." << std::endl;

    auto show_str = [&](const std::string& str) {
        std::cout << "show str: " << str << std::endl;
    };
    auto show_num = [&](const uint32_t& num) {
        std::cout << "show num: " << num << std::endl;
    };
    SumObj obj;
    obj.a = 3;
    obj.b = 4;
    auto show_sum = [&](const SumObj& sumobj) {
        std::cout << "show sum a:" << sumobj.a << " b:" << sumobj.b
                  << " equal:" << (sumobj.a + sumobj.b) << std::endl;
    };

    std::shared_ptr<AsyncBase> task_a(
        new AsyncTask<std::function<void(const std::string&)>, std::string>(
            show_str, "HK 00700"));
    std::shared_ptr<AsyncBase> task_b(
        new AsyncTask<std::function<void(const uint32_t&)>, uint32_t>(
            show_num, 356));
    std::shared_ptr<AsyncBase> task_c(
        new AsyncTask<std::function<void(const SumObj& sumobj)>, SumObj>(
            show_sum, obj));
    // task_a.Exec();
    // task_b.Exec();
    std::queue<std::shared_ptr<AsyncBase>> task_q;
    task_q.push(task_a);
    task_q.push(task_b);
    task_q.push(task_c);
    while (!task_q.empty()) {
        auto task = task_q.front();
        task_q.pop();
        task->Exec();
    }

    AsyncPool::Instance()->Init(8);
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ostringstream oss;
        time_t cur_time = time(nullptr);
        oss << "sec:" << uint64_t(cur_time)
            << " date:" << std::string(std::ctime(&cur_time));
        std::shared_ptr<AsyncBase> task(
            new AsyncTask<std::function<void(const std::string&)>, std::string>(
                show_str, oss.str()));
        AsyncPool::Instance()->Dispatch(task);
    }

    std::cout << "test finish..." << std::endl;
    return 0;
}
