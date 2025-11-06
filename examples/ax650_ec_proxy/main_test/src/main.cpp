#include <iostream>
#include <modbus.h>
#include "../../include/pzmq.hpp"
#include "cmdline.hpp"

int main(int argc, char *argv[])
{
    cmdline::parser a;
    a.add<std::string>("fun", 'f', "call ax650c_ec_proxy function", true);
    a.add<std::string>("data", 'd', "call ax650c_ec_proxy function input data", true);
    a.parse_check(argc, argv);
    const std::string fun = a.get<std::string>("fun");
    const std::string data = a.get<std::string>("data");




    StackFlows::pzmq Context("ax650_ec_prox.zmq.socket");
    Context.call_rpc_action(fun,data,
                            [](StackFlows::pzmq *_pzmq, const std::shared_ptr<StackFlows::pzmq_data> &data) {
                                if (data->size() > 0)
                                {
                                    std::cout << "out data:" << data->string() << std::endl;
                                }else{
                                    std::cout << "call ax650_ec_prox faile!" << std::endl;
                                }
                                
                            });


    return 0;
}
