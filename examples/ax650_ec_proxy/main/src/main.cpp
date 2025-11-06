#include <iostream>
#include <cstdlib>
#include <modbus.h>
#include "../../include/pzmq.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include "json.hpp"

int condition = 1;

class ax650_ec_prox {
private:
    modbus_t *modbus_ctx;
    std::mutex modbus_mtx_;
    std::unique_ptr<StackFlows::pzmq> zmq_Context;
    std::unique_ptr<StackFlows::pzmq> zmq_but_Context;

public:
    std::string get_fan_speed(StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data)
    {
        // mbpoll -m rtu -b 115200 -P none -a 1 -r 1 -t 3 -c 13 -l 10 /dev/ttyS3
        uint16_t tab_reg[2];
        int rc = modbus_read_input_registers(modbus_ctx, 12, 1, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "读取失败: %s\n", modbus_strerror(errno));
        } else {
            return fmt::format("{{ \"fan_speed\": {} }}", tab_reg[0]);
        }
        return std::string("Error");
    }
    std::string get_fan_pwm(StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data)
    {
        uint16_t tab_reg[2];
        int rc = modbus_read_registers(modbus_ctx, 1, 1, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "读取失败: %s\n", modbus_strerror(errno));
        } else {
            return fmt::format("{{ \"fan_pwm\": {} }}", tab_reg[0]);
        }
        return std::string("Error");
    }
    std::string set_fan_pwm(StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data)
    {
        uint16_t tab_reg[2];
        try {
            nlohmann::json fan_data = nlohmann::json::parse(data->string());
            tab_reg[0]              = (int)fan_data["fan_pwm"];
        } catch (const nlohmann::json::parse_error &e) {
            std::cerr << "解析失败：" << e.what() << std::endl;
            return std::string("Error");
        }
        int rc = modbus_write_register(modbus_ctx, 1, tab_reg[0]);
        if (rc == -1) {
            fprintf(stderr, "写入失败: %s\n", modbus_strerror(errno));
        }
        return std::string("Ok");
    }

    std::string get_rgb_mode(StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data)
    {
        uint16_t tab_reg[2];
        int rc = modbus_read_registers(modbus_ctx, 11, 1, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "读取失败: %s\n", modbus_strerror(errno));
        } else {
            return fmt::format("{{ \"rgb_mode\": {} }}", tab_reg[0]);
        }
        return std::string("Error");
    }

    std::string set_rgb_mode(StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data)
    {
        uint16_t tab_reg[2];
        try {
            nlohmann::json fan_data = nlohmann::json::parse(data->string());
            tab_reg[0]              = (int)fan_data["rgb_mode"];
        } catch (const nlohmann::json::parse_error &e) {
            std::cerr << "解析失败：" << e.what() << std::endl;
            return std::string("Error");
        }
        int rc = modbus_write_register(modbus_ctx, 11, tab_reg[0]);
        if (rc == -1) {
            fprintf(stderr, "写入失败: %s\n", modbus_strerror(errno));
            return std::string("Error");
        }
        return std::string("Ok");
    }

    ax650_ec_prox(/* args */)
    {
        modbus_ctx = modbus_new_rtu("/dev/ttyS3", 115200, 'N', 8, 1);
        if (modbus_ctx == NULL) {
            fprintf(stderr, "Unable to create the context\n");
            exit(-1);
        }
        modbus_set_slave(modbus_ctx, 1);  // 设置 Modbus 从站地址为1
        if (modbus_connect(modbus_ctx) == -1) {
            fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            modbus_free(modbus_ctx);
            exit(-1);
        }
        {
            uint32_t value = 1152000;
            uint16_t tab_reg[2];
            tab_reg[0] = (value >> 16) & 0xFFFF;
            tab_reg[1] = value & 0xFFFF;
            int rc     = modbus_write_registers(modbus_ctx, 3, 2, tab_reg);
            if (rc == -1) {
                fprintf(stderr, "写入失败: %s\n", modbus_strerror(errno));
                exit(-1);
            }
            modbus_close(modbus_ctx);
            modbus_free(modbus_ctx);
            modbus_ctx = NULL;
            sleep(1);
        }
        modbus_ctx = modbus_new_rtu("/dev/ttyS3", 1152000, 'N', 8, 1);
        if (modbus_ctx == NULL) {
            fprintf(stderr, "Unable to create the context\n");
            exit(-1);
        }
        modbus_set_slave(modbus_ctx, 1);  // 设置 Modbus 从站地址为1
        if (modbus_connect(modbus_ctx) == -1) {
            fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            modbus_free(modbus_ctx);
            exit(-1);
        }

        // clang-format off
        zmq_but_Context = std::make_unique<StackFlows::pzmq>("ipc:///tmp/button.ax650_ec_prox.socket", ZMQ_PUB);
        zmq_Context = std::make_unique<StackFlows::pzmq>("ax650_ec_prox.zmq.socket");
        zmq_Context->register_rpc_action("get_fan_speed"    , [this](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data){
            std::unique_lock<std::mutex> lock(this->modbus_mtx_);
            return this->get_fan_speed(_pzmq, data);
        });
        zmq_Context->register_rpc_action("get_fan_pwm"      , [this](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data){
            std::unique_lock<std::mutex> lock(this->modbus_mtx_);
            return this->get_fan_pwm(_pzmq, data);
        });
        zmq_Context->register_rpc_action("set_fan_pwm"      , [this](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data){
            std::unique_lock<std::mutex> lock(this->modbus_mtx_);
            return this->set_fan_pwm(_pzmq, data);
        });
        zmq_Context->register_rpc_action("get_rgb_mode"     , [this](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data){
            std::unique_lock<std::mutex> lock(this->modbus_mtx_);
            return this->get_rgb_mode(_pzmq, data);
        });
        zmq_Context->register_rpc_action("set_rgb_mode"     , [this](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data){
            std::unique_lock<std::mutex> lock(this->modbus_mtx_);
            return this->set_rgb_mode(_pzmq, data);
        });

        // clang-format on
    }
    void loop()
    {
        uint8_t butt = 0;
        while (condition) {
            {
                std::unique_lock<std::mutex> lock(this->modbus_mtx_);
                uint8_t tab_rp_bits[2];
                int rc = modbus_read_input_bits(modbus_ctx, 4, 1, tab_rp_bits);
                if (rc == -1) {
                    fprintf(stderr, "读取失败: %s\n", modbus_strerror(errno));
                } else {
                    if (tab_rp_bits[0] != butt) {
                        if (tab_rp_bits[0]) {
                            zmq_but_Context->send_data("{ \"Butn\": 1 }");
                            std::cout << "Butn: 1" << std::endl;
                        } else {
                            zmq_but_Context->send_data("{ \"Butn\": 0 }");
                            std::cout << "Butn: 0" << std::endl;
                        }
                        butt = tab_rp_bits[0];
                    }
                }
                // std::cout << "butt:" << (int)butt << std::endl;
            }
            usleep(50 * 1000);
        }
    }

    void ax650_ec_prox_exit()
    {
        zmq_but_Context.reset();
        zmq_Context.reset();
        for (int i = 0; i < 3; i++) {
            uint32_t value = 115200;
            uint16_t tab_reg[2];
            tab_reg[0] = (value >> 16) & 0xFFFF;
            tab_reg[1] = value & 0xFFFF;
            int rc     = modbus_write_registers(modbus_ctx, 3, 2, tab_reg);
            if (rc == -1) {
                fprintf(stderr, "写入失败: %s\n", modbus_strerror(errno));
            } else {
                break;
            }
        }
        modbus_close(modbus_ctx);
        modbus_free(modbus_ctx);
        std::cout << "all exit!" << std::endl;
    }

    ~ax650_ec_prox()
    {
    }
};

ax650_ec_prox prox;

void signalHandler(int signum)
{
    condition = 0;
    std::cout << "接收到信号: " << signum << std::endl;
    prox.ax650_ec_prox_exit();
    std::exit(0);  // 这会调用全局对象的析构函数
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    prox.loop();
    return 0;
}
