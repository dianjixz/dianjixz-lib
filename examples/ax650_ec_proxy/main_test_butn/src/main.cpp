#include <iostream>
#include <modbus.h>
#include "../../include/pzmq.hpp"
#include "cmdline.hpp"

int main(int argc, char *argv[])
{
    StackFlows::pzmq Context("ipc:///tmp/button.ax650_ec_prox.socket", ZMQ_SUB,
                             [](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data) {
                                 std::cout << "Received: " << data->string() << std::endl;
                             });
    while (1) {
        sleep(1);
    }

    return 0;
}
