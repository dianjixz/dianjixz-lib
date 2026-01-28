#ifndef HAL_DEVICE_H
#define HAL_DEVICE_H

#include <list>
#include <memory>
#include <functional>
#include <utility>
#include <string>
#include <ax_engine_api.h>
#include <thread>
#include <functional>
#include <mutex>
class SysGuard {
public:
    SysGuard();
    ~SysGuard();

    SysGuard(const SysGuard&)            = delete;
    SysGuard& operator=(const SysGuard&) = delete;
    SysGuard(SysGuard&&)                 = delete;  // 建议添加
    SysGuard& operator=(SysGuard&&)      = delete;
};



class hal_device {
private:
    std::shared_ptr<SysGuard> sys;
    std::list<std::pair<std::function<void()>, std::string>> work_list;
    std::mutex work_list_mutex;
    std::unique_ptr<std::thread> work_thread;
    int work_flag;
public:
    hal_device()
    {
        sys = std::make_shared<SysGuard>();
    }
    ~hal_device()
    {
    }

    std::shared_ptr<SysGuard> get_sys_guard() const
    {
        return sys;
    }

    AX_ENGINE_NPU_MODE_T get_npu_mode() const
    {
        // 默认返回禁用虚拟NPU模式
        return AX_ENGINE_VIRTUAL_NPU_DISABLE;
    }

    void add_work_list(std::function<void()> func, const std::string& name)
    {
        std::unique_lock<std::mutex> lock(work_list_mutex);
        work_list.emplace_back(std::move(func), name);
    }

    void del_work_list(const std::string& name)
    {
        std::unique_lock<std::mutex> lock(work_list_mutex);
        for (auto it = work_list.begin(); it != work_list.end();) {
            if (it->second == name) {
                it = work_list.erase(it);
            } else {
                ++it;
            }
        }
    }
    void work()
    {
        work_flag  = 1;
        work_thread = std::make_unique<std::thread>(std::bind(&hal_device::work_loop, this));
    }
    void stop_work()
    {
        work_flag = 0;
        if (work_thread) {
            work_thread->join();
            work_thread.reset();
        }
    }
    void work_loop()
    {
        while (work_flag) {
            work_poll();
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    void work_poll()
    {
        std::unique_lock<std::mutex> lock(work_list_mutex);
        for (const auto& work : work_list) {
            try {
                work.first();
            } catch (...) {
                perror("work_poll exception");
            }
        }
    }
};
int get_format_size(int format, int width, int height);
bool endsWith(const std::string& str, const std::string& suffix);
















#endif  // HAL_DEVICE_H