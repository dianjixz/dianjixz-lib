#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include "../ui/ui.h"
#include "subprocess.h"

#include "linux_uart/linux_uart.h"
#include <any.hpp>
#include <unordered_map>
#include <functional>
#include <semaphore.h>
#include <json.hpp>
#include <regex>

// // static int Debug_s = 1;

// // #define LOG_PRINT(fmt, ...)                                                                       \
// //     do {                                                                                          \
// //         if (Debug_s) {                                                                            \
// //             printf(fmt "  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); \
// //         }                                                                                         \
// //     } while (0);
static int cap_flage  = 0;
static int load_flage = 0;
std::string sample_unescapeString(const std::string &input)
{
    std::string unescaped;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '\\' && i + 1 < input.length()) {
            switch (input[i + 1]) {
                case 'n' :unescaped += '\n';++i;break;
                case 't' :unescaped += '\t';++i;break;
                case '\\':unescaped += '\\';++i;break;
                case '\"':unescaped += '\"';++i;break;
                case 'r' :unescaped += '\r';++i;break;
                case 'b' :unescaped += '\b';++i;break;
                default  :unescaped += input[i];break;
            }
        } else {
            unescaped += input[i];
        }
    }
    return unescaped;
}
std::vector<std::string> split_string(const std::string& content, const std::string& delimiter)
{
    std::vector<std::string> result;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = content.find(delimiter);
    while (std::string::npos != pos2) {
        result.push_back(content.substr(pos1, pos2 - pos1));
        pos1 = pos2 + delimiter.size();
        pos2 = content.find(delimiter, pos1);
    }
    if (pos1 != content.length()) {
        result.push_back(content.substr(pos1));
    }
    return result;
}
std::string sample_json_str_get(const std::string &json_str, const std::string &json_key)
{
    std::string key_val;
    std::string format_val;
    // SLOGD("json_str: %s json_key:%s\n", json_str.c_str(), json_key.c_str());
    std::string find_key = "\"" + json_key + "\"";
    int subs_start       = json_str.find(find_key);
    if (subs_start == std::string::npos) {
        return key_val;
    }
    int status    = 0;
    char last_c   = '\0';
    int obj_flage = 0;
    for (auto c : json_str.substr(subs_start + find_key.length())) {
        switch (status) {
            case 0: {
                switch (c) {
                    case '"': {
                        status = 100;
                    } break;
                    case '{': {
                        key_val.push_back(c);
                        obj_flage = 1;
                        status    = 10;
                    } break;
                    case ':':
                        obj_flage = 1;
                        break;
                    case ',':
                    case '}': {
                        obj_flage = 0;
                        status    = -1;
                    } break;
                    case ' ':
                        break;
                    default: {
                        if (obj_flage) {
                            key_val.push_back(c);
                        }
                    } break;
                }
            } break;
            case 10: {
                key_val.push_back(c);
                if (c == '{') {
                    obj_flage++;
                }
                if (c == '}') {
                    obj_flage--;
                }
                if (obj_flage == 0) {
                    if (!key_val.empty()) {
                        status = -1;
                    }
                }
            } break;
            case 100: {
                if ((c == '"') && (last_c != '\\')) {
                    obj_flage = 0;
                    status    = -1;
                } else {
                    key_val.push_back(c);
                }
            } break;
            default:
                break;
        }
        last_c = c;
    }
    if (obj_flage != 0) {
        key_val.clear();
    }
    // SLOGD("key_val:%s\n", key_val.c_str());
    return key_val;
}

void str_parse_line(const std::string &da, std::string &lines, int &flage,
                    std::function<void(const std::string &)> call_fun) {
    for (auto c : da) {
        if (flage) {
            if (c != '\\') {
                lines += c;
            } else {
                flage = 0;
            }
        } else {
            if (c == 'r') {
                lines += "\r";
            } else if (c == 'n') {
                lines += "\n";
                call_fun(lines);
                lines.clear();
            } else {
                lines += "\\";
            }
            flage = 1;
        }
    }
}

class llm_uart_exec {
   private:
    int uart_fd;
    pthread_mutex_t uart_mutex;
    std::unordered_map<int, std::function<void(const std::string &)>> key_sql;
    int alloc_commit;
    int exit_flage;

   public:
    llm_uart_exec(const std::string &dev_name = "/dev/ttySTM0") {
        exit_flage   = 0;
        alloc_commit = 1;
        pthread_mutex_init(&uart_mutex, NULL);
        uart_t uart_parm;
        memset(&uart_parm, 0, sizeof(uart_parm));
        uart_parm.baud      = 115200;
        uart_parm.data_bits = 8;
        uart_parm.stop_bits = 1;
        uart_parm.parity    = 'n';
        uart_fd             = linux_uart_init((char *)dev_name.c_str(), &uart_parm);
        std::thread t(&llm_uart_exec::uart_read_loop, this);
        t.detach();
    }

    int sample_json_get_request_id(const std::string &json_str) {
        std::string key_val;
        std::string json_key = "request_id";
        int subs_start       = json_str.find(json_key);
        if (subs_start == std::string::npos) {
            return -1;
        }
        int flage   = 0;
        char last_c = '\0';
        // printf("json_str.substr:%s\n", json_str.substr(subs_start).c_str());
        for (auto c : json_str.substr(subs_start)) {
            if (flage == 2) key_val.push_back(c);
            if ((c == '"') && (last_c != '\\')) {
                flage++;
            }
            if (flage == 3) {
                key_val.pop_back();
                break;
            }
            last_c = c;
        }
        // printf("key_val:%s\n", key_val.c_str());
        return std::stoi(key_val);
    }

    void clear_request_id(int request_id) {
        pthread_mutex_lock(&uart_mutex);
        try {
            key_sql.erase(request_id);
        } catch (...) {
        }
        pthread_mutex_unlock(&uart_mutex);
    }

    int get_request_id() {
        int request_id;
        pthread_mutex_lock(&uart_mutex);
        request_id = alloc_commit++;
        pthread_mutex_unlock(&uart_mutex);
        return request_id;
    }
    void exec(const std::string &cmd_str, std::function<void(const std::string &)> call_fun, int commit) {
        pthread_mutex_lock(&uart_mutex);
        key_sql[commit] = call_fun;
        linux_uart_write(uart_fd, cmd_str.length(), (void *)cmd_str.c_str());
        pthread_mutex_unlock(&uart_mutex);
    }
    char _cmd_str[1024];
    int bashexec(const std::string &cmd_str, std::function<void(const std::string &)> call_fun) {
        int commit;
        pthread_mutex_lock(&uart_mutex);
        commit          = alloc_commit++;
        int len         = sprintf(_cmd_str,
                                  "{\"request_id\": \"%d\", \"work_id\": \"sys\",\"action\": \"bashexec\",\"object\": "
                                          "\"sys.utf-8.stream\",\"data\":{\"index\":0,\"delta\":\"%s\",\"finish\":true}}",
                                  commit, cmd_str.c_str());
        key_sql[commit] = call_fun;
        // printf("linux_uart_write:%.*s\n", len, _cmd_str);
        linux_uart_write(uart_fd, len, _cmd_str);
        pthread_mutex_unlock(&uart_mutex);
        return commit;
    }
    int sys_hwinfo(std::function<void(const std::string &)> call_fun) {
        int commit;
        char json_str[72];
        pthread_mutex_lock(&uart_mutex);
        commit  = alloc_commit++;
        int len = sprintf(json_str, "{\"request_id\": \"%d\", \"work_id\": \"sys\",\"action\": \"hwinfo\"}", commit);
        key_sql[commit] = call_fun;
        // printf("linux_uart_write:%.*s\n", len, json_str);
        linux_uart_write(uart_fd, len, json_str);
        pthread_mutex_unlock(&uart_mutex);
        return commit;
    }
    void close_led() {
        pthread_mutex_lock(&uart_mutex);
        int len = sprintf(
            _cmd_str,
            "{\"request_id\": \"0\", \"work_id\": \"sys\",\"action\": \"bashexec\",\"object\": "
            "\"sys.utf-8.stream\",\"data\":{\"index\":0,\"delta\":\"/usr/sbin/i2cset -y -f 4 0x30 0x02 0x00 ; "
            "/usr/sbin/i2cset -y -f 4 0x30 0x03 0x00 ; /usr/sbin/i2cset -y -f 4 0x30 0x04 0x00 ; \",\"finish\":true}}");
        // printf("linux_uart_write:%.*s\n", len, _cmd_str);
        linux_uart_write(uart_fd, len, _cmd_str);
        pthread_mutex_unlock(&uart_mutex);
    }
    void uart_read_loop() {
        char read_buff[256];
        int flage = 0;
        std::string json_str;
        while (!exit_flage) {
            int len = linux_uart_read(uart_fd, 256, &read_buff);
            for (size_t i = 0; i < len; i++) {
                json_str += read_buff[i];
                if (read_buff[i] == '{') flage++;
                if (read_buff[i] == '}') flage--;
                if (flage == 0) {
                    if (json_str[0] == '{') {
                        // printf("2reace %s\n", json_str.c_str());
                        std::function<void(const std::string &)> call_fun;
                        pthread_mutex_lock(&uart_mutex);
                        int request_id = sample_json_get_request_id(json_str);
                        if (request_id > 0) {
                            call_fun = key_sql[request_id];
                        }
                        pthread_mutex_unlock(&uart_mutex);
                        if (request_id > 0) {
                            try {
                                call_fun(json_str);
                            } catch (...) {
                                // 可能存在命令执行超时的情况，此时call_fun被删除，所以出现了call
                                // false的现象，在此捕捉一下
                            }
                        }
                    }
                    json_str.clear();
                }
                if (flage < 0) {
                    flage = 0;
                    json_str.clear();
                }
            }
        }
    }
    void reset_com() {
        const char *endstr = "}";
        pthread_mutex_lock(&uart_mutex);
        linux_uart_write(uart_fd, 1, (char *)endstr);
        pthread_mutex_unlock(&uart_mutex);
    }
    int ping_sync() {
        int return_cal = -1;
        sem_t sem;
        int commit;
        sem_init(&sem, 0, 0);
        pthread_mutex_lock(&uart_mutex);
        commit          = alloc_commit++;
        key_sql[commit] = [&sem](const std::string &json_body) { sem_post(&sem); };
        char send_buf[128];
        int mk = sprintf(send_buf, "{\"request_id\": \"%d\", \"work_id\": \"sys\",\"action\": \"ping\"}", commit);
        linux_uart_write(uart_fd, mk, send_buf);
        pthread_mutex_unlock(&uart_mutex);

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;
        int ret = sem_timedwait(&sem, &ts);
        if (ret == 0) {
            return_cal = 0;
        }
        pthread_mutex_lock(&uart_mutex);
        key_sql.erase(commit);
        pthread_mutex_unlock(&uart_mutex);

        sem_destroy(&sem);
        return return_cal;
    }

    ~llm_uart_exec() {
        exit_flage = 1;
        linux_uart_deinit(uart_fd);
    }
};
llm_uart_exec uart_exec;
subprocess_s process;

// int Debug_s = 1;

// #define LOG_PRINT(fmt, ...)                                                                       \
//     do {                                                                                          \
//         if (Debug_s) {                                                                            \
//             printf(fmt "  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); \
//         }                                                                                         \
//     } while (0);

#include "hv/icmp.h"
// #include "llm_exec_tcp.hpp"

extern pthread_mutex_t ui_thread_mutex;
extern int ax_llm_connect_flage;

#define ui_lock() pthread_mutex_lock(&ui_thread_mutex)

#define ui_unlock() pthread_mutex_unlock(&ui_thread_mutex)

// // void update_chart(lv_chart_series_t * ser, int32_t * new_data, uint32_t data_len)
// // {
// //     int32_t min_value = INT32_MAX;
// //     int32_t max_value = INT32_MIN;
// //     for(uint32_t i = 0; i < data_len; i++) {
// //         ser->y_points[i] = new_data[i];
// //         if(new_data[i] < min_value) min_value = new_data[i];
// //         if(new_data[i] > max_value) max_value = new_data[i];
// //     }
// //     lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, min_value, max_value);
// //     lv_chart_refresh(ui_Chart1);
// // }

void ax_cmmc_test_task() {
    // int ret;
    lv_obj_t ** ui_Button = &ui_Button2;
    lv_obj_t ** ui_TextArea = &ui_TextArea1;
    lv_obj_t ** ui_Chart = &ui_Chart1;
    lv_obj_t ** ui_TabPage = &ui_TabPage1;

    lv_obj_t ** R_ui_Label = &ui_Label9;
    lv_obj_t ** W_ui_Label = &ui_Label10;

    std::string lines;
    lv_chart_series_t *ui_Chart1_series_1;
    lv_chart_series_t *ui_Chart1_series_2;
    lv_coord_t *ui_Chart1_series_1_array;
    lv_coord_t *ui_Chart1_series_2_array;
    float Ravg_s       = 0.0;
    float Wavg_s       = 0.0;
    int arrar_count[2] = {0};
    int summ[2]        = {0};
    ui_lock();
    lv_textarea_add_text(*ui_TextArea, "test llm emmc.\n");
    // lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_SECONDARY_Y, 0, 200);
    ui_Chart1_series_1       = lv_chart_get_series_next(*ui_Chart, NULL);
    ui_Chart1_series_2       = lv_chart_get_series_next(*ui_Chart, ui_Chart1_series_1);
    ui_Chart1_series_1_array = lv_chart_get_y_array(*ui_Chart, ui_Chart1_series_1);
    ui_Chart1_series_2_array = lv_chart_get_y_array(*ui_Chart, ui_Chart1_series_2);
    memset(ui_Chart1_series_1_array, 0, sizeof(lv_coord_t) * 10);
    memset(ui_Chart1_series_2_array, 0, sizeof(lv_coord_t) * 10);
    lv_chart_set_ext_y_array(*ui_Chart, ui_Chart1_series_1, ui_Chart1_series_1_array);
    lv_chart_set_ext_y_array(*ui_Chart, ui_Chart1_series_2, ui_Chart1_series_2_array);

    ui_unlock();
    int success_count = 0;
    {
        int request_id_num = -1;
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec(
            "mkdir -p /root/fsiotest ; "
            "cd /root/fsiotest ; "
            "sysbench fileio --file-total-size=512M prepare ; "
            "sysbench fileio --file-total-size=512M --file-test-mode=seqrd --report-interval=1 --max-time=11 "
            "--file-extra-flags=direct run ; "
            "sysbench fileio --file-total-size=512M --file-test-mode=seqwr --report-interval=1 --max-time=11 "
            "--file-extra-flags=direct run ; "
            "sysbench fileio --file-total-size=512M cleanup ; "
            "cd /root ; rm /root/fsiotest -rf ; "
            "sync ; ",
            [&](const std::string &json_body) {
                // printf("json_body:%s\n", json_body.c_str());
                int true_s = json_body.find("true");
                if (true_s != std::string::npos) {
                    std::string request_id = sample_json_str_get(json_body, "request_id");
                    sem_post(&sem);
                    uart_exec.clear_request_id(std::stoi(request_id));
                } else {
                    if (request_id_num == -1) {
                        request_id_num = std::stoi(sample_json_str_get(json_body, "request_id"));
                    }
                    std::string delta = sample_json_str_get(json_body, "delta");
                    static int flage  = 1;
                    str_parse_line(delta, lines, flage, [&](const std::string &_lin) {
                        if (_lin.find("Creating") != std::string::npos) {
                            ui_lock();
                            lv_textarea_add_text(*ui_TextArea, ".");
                            ui_unlock();
                        }
                        if (_lin.find("latency") != std::string::npos) {
                            int find_index;
                            double writes, reads;
                            std::string writess, readss;
                            find_index = _lin.find("reads: ");
                            if (find_index != std::string::npos) {
                                for (size_t i = find_index + 7; i < _lin.length(); i++) {
                                    // printf("%c",_lin[i]);
                                    if (_lin[i] != ' ') {
                                        readss += _lin[i];
                                    } else {
                                        break;
                                    }
                                }
                                // printf("readss:%s<\n", readss.c_str());
                                reads = std::stod(readss);
                            }
                            find_index = _lin.find("writes: ");
                            if (find_index != std::string::npos) {
                                for (size_t i = find_index + 8; i < _lin.length(); i++) {
                                    // printf("%c",_lin[i]);
                                    if (_lin[i] != ' ') {
                                        writess += _lin[i];
                                    } else {
                                        break;
                                    }
                                }
                                // printf("writess:%s<\n", readss.c_str());
                                writes = std::stod(writess);
                            }
                            ui_lock();

                            // printf("get writes:%f reads:%f\n", writes, reads);
                            if (reads > 0) {
                                if (arrar_count[0] < 10) {
                                    ui_Chart1_series_1_array[arrar_count[0]++] = (int)reads;
                                    summ[0] += (int)reads;
                                    Ravg_s = (float)summ[0] / (float)arrar_count[0];
                                }
                                std::string lab_tmp = std::to_string((int)Ravg_s) + "M/s";
                                lv_label_set_text(*R_ui_Label, lab_tmp.c_str());
                                lv_chart_set_ext_y_array(*ui_Chart, ui_Chart1_series_1, ui_Chart1_series_1_array);
                                {
                                    lv_coord_t min_value = 1000;
                                    lv_coord_t max_value = 0;
                                    for (uint32_t i = 0; i < 10; i++) {
                                        if (ui_Chart1_series_1_array[i] < min_value)
                                            min_value = ui_Chart1_series_1_array[i];
                                        if (ui_Chart1_series_1_array[i] > max_value)
                                            max_value = ui_Chart1_series_1_array[i];
                                    }
                                    min_value = min_value > 25 ? min_value - 25 : 0;
                                    max_value += 25;
                                    lv_chart_set_range(*ui_Chart, LV_CHART_AXIS_SECONDARY_Y, min_value, max_value);
                                    // lv_chart_refresh(ui_Chart1);
                                }
                            }
                            if (writes > 0) {
                                if (arrar_count[1] < 10) {
                                    ui_Chart1_series_2_array[arrar_count[1]++] = (int)writes;
                                    summ[1] += (int)writes;
                                    Wavg_s = (float)summ[1] / (float)arrar_count[1];
                                }
                                std::string lab_tmp = std::to_string((int)Wavg_s) + "M/s";
                                lv_label_set_text(*W_ui_Label, lab_tmp.c_str());
                                lv_chart_set_ext_y_array(*ui_Chart, ui_Chart1_series_2, ui_Chart1_series_2_array);
                                {
                                    lv_coord_t min_value = 1000;
                                    lv_coord_t max_value = 0;
                                    for (int i = 0; i < 10; i++) {
                                        if (ui_Chart1_series_2_array[i] < min_value)
                                            min_value = ui_Chart1_series_2_array[i];
                                        if (ui_Chart1_series_2_array[i] > max_value)
                                            max_value = ui_Chart1_series_2_array[i];
                                    }
                                    min_value = min_value > 15 ? min_value - 15 : 0;
                                    max_value += 15;
                                    lv_chart_set_range(*ui_Chart, LV_CHART_AXIS_PRIMARY_Y, min_value, max_value);
                                    // lv_chart_refresh(ui_Chart1);
                                }
                            }
                            if (lv_obj_has_flag(*ui_Chart, LV_OBJ_FLAG_HIDDEN))
                                lv_obj_clear_flag(*ui_Chart, LV_OBJ_FLAG_HIDDEN);
                            if (!lv_obj_has_flag(*ui_TextArea, LV_OBJ_FLAG_HIDDEN))
                                lv_obj_add_flag(*ui_TextArea, LV_OBJ_FLAG_HIDDEN);
                            ui_unlock();
                        }
                    });
                }
            });
        // sem_wait(&sem);
        struct timespec ts;
        int WaitTime = 0;
        while (1) {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
            int ret = sem_timedwait(&sem, &ts);
            if (ret == 0) {
                break;
            }
            WaitTime = ax_llm_connect_flage ? 0 : WaitTime + 1;
            if (WaitTime > 3) {
                ui_lock();
                lv_textarea_add_text(*ui_TextArea, "emmc test timeout!\n");
                lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                ui_unlock();
                printf("dev disconnect!\n");
                uart_exec.clear_request_id(request_id_num);
                break;
            }
        }
        sem_destroy(&sem);
    }

cmmc_task_e:
    int error = 0;
    // for (size_t i = 0; i < 10; i++)
    // {
    //     if(ui_Chart1_series_1_array[i] < 80)
    //     {
    //         error = 1;
    //         break;
    //     }
    //     if(ui_Chart1_series_2_array[i] < 70)
    //     {
    //         error = 1;
    //         break;
    //     }
    // }
    if (Ravg_s < 80) {
        error = 1;
    }
    if (Wavg_s < 70) {
        error = 1;
    }
    if (error) {
        ui_lock();
        lv_textarea_add_text(*ui_TextArea, "emmc test false!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    } else {
        ui_lock();
        lv_textarea_add_text(*ui_TextArea, "emmc test success!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    }

    ui_lock();
    lv_obj_add_flag(*ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(*ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(*ui_TextArea, LV_OBJ_FLAG_CLICKABLE);

    ui_unlock();
}


void ax_sd_test_task() {
    lv_obj_t ** ui_Button = &ui_Button3;
    lv_obj_t ** ui_TextArea = &ui_TextArea3;
    lv_obj_t ** ui_Chart = &ui_Chart3;
    lv_obj_t ** ui_TabPage = &ui_TabPage2;

    lv_obj_t ** R_ui_Label = &ui_Label14;
    lv_obj_t ** W_ui_Label = &ui_Label15;


    int ret;
    std::string lines;
    // llm_TcpClient llm_exec("m5stack-LLM", 5555);
    int arrar_count[2] = {0};
    int summ[2]        = {0};
    lv_chart_series_t *ui_Chart8_series_1;
    lv_chart_series_t *ui_Chart8_series_2;
    lv_coord_t *ui_Chart8_series_1_array;
    lv_coord_t *ui_Chart8_series_2_array;
    float Ravg_s = 0.0;
    float Wavg_s = 0.0;
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm sd.\n");
    ui_Chart8_series_1       = lv_chart_get_series_next(* ui_Chart, NULL);
    ui_Chart8_series_2       = lv_chart_get_series_next(* ui_Chart, ui_Chart8_series_1);
    ui_Chart8_series_1_array = lv_chart_get_y_array(* ui_Chart, ui_Chart8_series_1);
    ui_Chart8_series_2_array = lv_chart_get_y_array(* ui_Chart, ui_Chart8_series_2);
    memset(ui_Chart8_series_1_array, 0, sizeof(lv_coord_t) * 10);
    memset(ui_Chart8_series_2_array, 0, sizeof(lv_coord_t) * 10);
    lv_chart_set_ext_y_array(* ui_Chart, ui_Chart8_series_1, ui_Chart8_series_1_array);
    lv_chart_set_ext_y_array(* ui_Chart, ui_Chart8_series_2, ui_Chart8_series_2_array);
    // lv_chart_set_range(ui_Chart8, LV_CHART_AXIS_SECONDARY_Y, 0, 40);
    // lv_chart_set_range(ui_Chart8, LV_CHART_AXIS_PRIMARY_Y, 0, 60);
    ui_unlock();
    {
        int request_id_num = -1;
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec(
            "[ -d /mnt/mmcblk1p2/fsiotest ] || { echo \\\"mmcblk1p2_not_exists\\\" ; exit 1 ; } ; "
            "cd /mnt/mmcblk1p2/fsiotest ; "
            "sysbench fileio --file-total-size=512M prepare ; "
            "sysbench fileio --file-total-size=512M --file-test-mode=seqrd --report-interval=1 --max-time=11 "
            "--file-extra-flags=direct run ; "
            "sysbench fileio --file-total-size=512M --file-test-mode=seqwr --report-interval=1 --max-time=11 "
            "--file-extra-flags=direct run ; "
            "sysbench fileio --file-total-size=512M cleanup ; "
            "sync ; ",
            [&](const std::string &json_body) {
                // printf("json_body:%s\n", json_body.c_str());
                if (json_body.find("mmcblk1p2_not_exists") != std::string::npos) {
                    ui_lock();
                    lv_textarea_add_text(* ui_TextArea, "mmcblk1p2 not exists!\n");
                    lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    ui_unlock();
                }
                int true_s = json_body.find("true");
                if (true_s != std::string::npos) {
                    std::string request_id = sample_json_str_get(json_body, "request_id");
                    sem_post(&sem);
                    uart_exec.clear_request_id(std::stoi(request_id));
                } else {
                    if (request_id_num == -1) {
                        request_id_num = std::stoi(sample_json_str_get(json_body, "request_id"));
                    }
                    std::string delta = sample_json_str_get(json_body, "delta");
                    static int flage  = 1;
                    str_parse_line(delta, lines, flage, [&](const std::string &_lin) {
                        if (_lin.find("Creating") != std::string::npos) {
                            ui_lock();
                            lv_textarea_add_text(* ui_TextArea, ".");
                            ui_unlock();
                        }
                        if (_lin.find("latency") != std::string::npos) {
                            int find_index;
                            double writes, reads;
                            std::string writess, readss;
                            find_index = _lin.find("reads: ");
                            if (find_index != std::string::npos) {
                                for (size_t i = find_index + 7; i < _lin.length(); i++) {
                                    // printf("%c",_lin[i]);
                                    if (_lin[i] != ' ') {
                                        readss += _lin[i];
                                    } else {
                                        break;
                                    }
                                }
                                // printf("readss:%s<\n", readss.c_str());
                                reads = std::stod(readss);
                            }
                            find_index = _lin.find("writes: ");
                            if (find_index != std::string::npos) {
                                for (size_t i = find_index + 8; i < _lin.length(); i++) {
                                    // printf("%c",_lin[i]);
                                    if (_lin[i] != ' ') {
                                        writess += _lin[i];
                                    } else {
                                        break;
                                    }
                                }
                                // printf("writess:%s<\n", readss.c_str());
                                writes = std::stod(writess);
                            }
                            ui_lock();

                            // printf("get writes:%f reads:%f\n", writes, reads);
                            if (reads > 0) {
                                if (arrar_count[0] < 10) {
                                    ui_Chart8_series_1_array[arrar_count[0]++] = (int)reads;
                                    summ[0] += (int)reads;
                                    Ravg_s = (float)summ[0] / (float)arrar_count[0];
                                }
                                std::string lab_tmp = std::to_string((int)Ravg_s) + "M/s";
                                lv_label_set_text(* R_ui_Label, lab_tmp.c_str());
                                lv_chart_set_ext_y_array(* ui_Chart, ui_Chart8_series_1, ui_Chart8_series_1_array);
                                {
                                    lv_coord_t min_value = 1000;
                                    lv_coord_t max_value = 0;
                                    for (uint32_t i = 0; i < 10; i++) {
                                        if (ui_Chart8_series_1_array[i] < min_value)
                                            min_value = ui_Chart8_series_1_array[i];
                                        if (ui_Chart8_series_1_array[i] > max_value)
                                            max_value = ui_Chart8_series_1_array[i];
                                    }
                                    min_value = min_value > 20 ? min_value - 20 : 0;
                                    max_value += 5;
                                    lv_chart_set_range(* ui_Chart, LV_CHART_AXIS_SECONDARY_Y, min_value, max_value);
                                    // lv_chart_refresh(ui_Chart8);
                                }
                            }
                            if (writes > 0) {
                                if (arrar_count[1] < 10) {
                                    ui_Chart8_series_2_array[arrar_count[1]++] = (int)writes;
                                    summ[1] += (int)writes;
                                    Wavg_s = (float)summ[1] / (float)arrar_count[1];
                                }
                                std::string lab_tmp = std::to_string((int)Wavg_s) + "M/s";
                                lv_label_set_text(* W_ui_Label, lab_tmp.c_str());
                                lv_chart_set_ext_y_array(* ui_Chart, ui_Chart8_series_2, ui_Chart8_series_2_array);
                                {
                                    lv_coord_t min_value = 1000;
                                    lv_coord_t max_value = 0;
                                    for (int i = 0; i < 10; i++) {
                                        if (ui_Chart8_series_2_array[i] < min_value)
                                            min_value = ui_Chart8_series_2_array[i];
                                        if (ui_Chart8_series_2_array[i] > max_value)
                                            max_value = ui_Chart8_series_2_array[i];
                                    }
                                    min_value = min_value > 20 ? min_value - 20 : 0;
                                    max_value += 5;
                                    lv_chart_set_range(* ui_Chart, LV_CHART_AXIS_PRIMARY_Y, min_value, max_value);
                                    // lv_chart_refresh(ui_Chart1);
                                }
                            }
                            if (lv_obj_has_flag(* ui_Chart, LV_OBJ_FLAG_HIDDEN))
                                lv_obj_clear_flag(* ui_Chart, LV_OBJ_FLAG_HIDDEN);
                            if (!lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN))
                                lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
                            ui_unlock();
                        }
                    });
                }
            });
        // sem_wait(&sem);
        struct timespec ts;
        int WaitTime = 0;
        while (1) {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
            int ret = sem_timedwait(&sem, &ts);
            if (ret == 0) {
                break;
            }
            WaitTime = ax_llm_connect_flage ? 0 : WaitTime + 1;
            if (WaitTime > 3) {
                ui_lock();
                lv_textarea_add_text(* ui_TextArea, "sd test timeout!\n");
                lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                ui_unlock();
                printf("dev disconnect!\n");
                uart_exec.clear_request_id(request_id_num);
                break;
            }
        }
        sem_destroy(&sem);
    }
sd_task_e:
    int error = 0;
    // for (size_t i = 0; i < 10; i++)
    // {
    //     if(ui_Chart8_series_1_array[i] < 10)
    //     {
    //         error = 1;
    //         break;
    //     }
    //     if(ui_Chart8_series_2_array[i] < 2)
    //     {
    //         error = 1;
    //         break;
    //     }
    // }
    if (Ravg_s < 30) {
        error = 1;
    }
    if (Wavg_s < 7) {
        error = 1;
    }
    if (error) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "sd test false!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    } else {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "sd test success!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    }
    ui_lock();
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(*ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);

    ui_unlock();
}

void wait_bash_exec_timeout(sem_t &sem, std::function<void(void)> call_fun, int WaitTime = 3)
{
    struct timespec ts;
    while (1) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        int ret = sem_timedwait(&sem, &ts);
        if (ret == 0) {
            break;
        }
        WaitTime --;
        if (WaitTime <= 0) {
            call_fun();
            break;
        }
    }
}

void ax_i2c_test_task() {
    lv_obj_t ** ui_Button = &ui_Button5;
    lv_obj_t ** ui_TextArea = &ui_TextArea5;
    lv_obj_t ** ui_TabPage = &ui_TabPage4;

    int error = 0;
    int ret;
    std::string lines;
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm i2c.\n");
    ui_unlock();
    {
        int request_id_num = -1;
        sem_t sem;
        int true_s;
        sem_init(&sem, 0, 0);
        std::string reace_buff;
        uart_exec.bashexec(R"(i2cget -y -f 1 0x68 0x00 ; [ \"$?\" != \"0\" ] && echo \"BMI270 ERROR\" ; i2cget -y -f 1 0x49 0x00 ; [ \"$?\" != \"0\" ] && echo \"POWER IC ERROR\" ; i2cget -y -f 1 0x57 0x00 ; [ \"$?\" != \"0\" ] && echo \"Unit Heart ERROR\" ; i2cget -y -f 1 0x29 0x00 ; [ \"$?\" != \"0\" ] && echo \"Unit TOF ERROR\" ; i2cget -y 1 0x55 0x00 ; [ \"$?\" != \"0\" ] && echo \"Batty ERROR\" ; )", [&](const std::string &json_body) {
            true_s = json_body.find("true");
            if (true_s != std::string::npos) {
                std::string request_id = sample_json_str_get(json_body, "request_id");
                sem_post(&sem);
                uart_exec.clear_request_id(std::stoi(request_id));
            } else {
                lines = sample_unescapeString(sample_json_str_get(json_body, "delta"));
                reace_buff += lines;
                ui_lock();
                lv_textarea_add_text(* ui_TextArea, lines.c_str());
                ui_unlock();
            }
            // printf("%s \n", json_body.c_str());
        });
        wait_bash_exec_timeout(sem, [&]() {
            printf("wait_bash_exec_timeout\n");
            error = 1;
        });
        if(reace_buff.find("Error: Read failed") != std::string::npos)
        {
            error = 1;
        }
    }
    if (error) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "i2c test false!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    } else {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "i2c test success!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    }
    ui_lock();
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(*ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    ui_unlock();
}

void ax_otg_test_task() {
    lv_obj_t ** ui_Button = &ui_Button4;
    lv_obj_t ** ui_TextArea = &ui_TextArea4;
    lv_obj_t ** ui_TabPage = &ui_TabPage3;
    const char *_cmd_str[] = {"/usr/bin/lsusb", NULL};
    const char* enable_switch = R"([ -d '/sys/class/gpio/PF3' ] || { echo 83 > /sys/class/gpio/export ; echo out > /sys/class/gpio/PF3/direction ; echo 0 > /sys/class/gpio/PF3/value ; } )";
    system(enable_switch);
    
    // 检测从机设备
    std::string lines;
    int exit_code;
    int len;
    char lsusb_cmd_buff[1024];

    int lsusb_num[2] = {0};

    int ret = system("echo 1 > /sys/class/gpio/PF3/value");
    if (ret) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "otg switch connect false!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
        goto otg_task_e;
    }
    sleep(3);
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm otg master mode.\n");
    ui_unlock();

    {
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec("/usr/bin/lsusb", [&](const std::string &json_body) {
            int true_s = json_body.find("true");
            if (true_s != std::string::npos) {
                std::string request_id = sample_json_str_get(json_body, "request_id");
                sem_post(&sem);
                uart_exec.clear_request_id(std::stoi(request_id));
            } else {
                std::string delta = sample_json_str_get(json_body, "delta");
                static int flage  = 1;
                str_parse_line(delta, lines, flage, [&lsusb_num, ui_TextArea](const std::string &_lin) {
                    if (_lin.find("Bus") != std::string::npos) {
                        ui_lock();
                        lsusb_num[0]++;
                        lv_textarea_add_text(*ui_TextArea, _lin.c_str());
                        ui_unlock();
                    }
                });
            }
        });
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        int ret = sem_timedwait(&sem, &ts);
        if (ret != 0) {
            ui_lock();
            lv_textarea_add_text(*ui_TextArea, "otg test timeout\n");
            ui_unlock();
        }
        sem_destroy(&sem);
    }

    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm otg device mode.\n");
    lv_textarea_add_text(* ui_TextArea, "Detected USB device:\n");
    ui_unlock();

    ret = system("echo 0 > /sys/class/gpio/PF3/value");
    if (ret) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "otg switch connect false!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
        goto otg_task_e;
    }
    sleep(3);
    memset(&process, 0, sizeof(subprocess_s));
    if (subprocess_create(_cmd_str, subprocess_option_combined_stdout_stderr, &process)) {
        fprintf(stderr, "Failed to create subprocess\n");
        goto otg_task_e;
    }

    subprocess_join(&process, &exit_code);
    printf("Process exited with code %d\n", exit_code);
    len = subprocess_read_stdout(&process, lsusb_cmd_buff, 1024);
    // printf("lsusb:%s\n", lsusb_cmd_buff);
    lines.clear();
    for (size_t i = 0; i < len; i++) {
        if (lsusb_cmd_buff[i] != '\r' && lsusb_cmd_buff[i] != '\n') {
            lines += lsusb_cmd_buff[i];
        } else {
            if (lines.find("Bus") != std::string::npos) {
                lsusb_num[1]++;
            }
            lines.clear();
        }
    }
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, lsusb_cmd_buff);
    ui_unlock();

    subprocess_destroy(&process);

otg_task_e:
    if ((lsusb_num[0] < 3) || (lsusb_num[1] < 4)) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "otg test false!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    } else {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "otg test success!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    }

    ui_lock();
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    ui_unlock();
    system("echo 1 > /sys/class/gpio/PF3/value");
}



void ax_wifi_test_task() {
    lv_obj_t ** ui_Button = &ui_Button7;
    lv_obj_t ** ui_TextArea = &ui_TextArea7;
    lv_obj_t ** ui_TabPage = &ui_TabPage6;

    int error = 1;
    int ret;
    std::string lines;
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm wifi.\n");
    ui_unlock();
    {
        int request_id_num = -1;
        sem_t sem;
        int true_s;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec("if ! grep -q \\\"esp32_sdio\\\" /proc/modules ; then /usr/local/m5stack/bin/S03espc6wifi start ; sleep 5 ; fi ; wpa_cli -i wlan0 scan && wpa_cli -i wlan0 scan_result ; ", [&](const std::string &json_body) {
            true_s = json_body.find("true");
            if (true_s != std::string::npos) {
                std::string request_id = sample_json_str_get(json_body, "request_id");
                sem_post(&sem);
                uart_exec.clear_request_id(std::stoi(request_id));
            } else {
                lines = sample_unescapeString(sample_json_str_get(json_body, "delta"));
                ui_lock();
                lv_textarea_add_text(* ui_TextArea, lines.c_str());
                ui_unlock();
                if(lines.find("[WPA-PSK-CCMP]") != std::string::npos)
                {
                    error = 0;
                }
            }
            printf("%s \n", json_body.c_str());
        });
        wait_bash_exec_timeout(sem, [&]() {
            printf("wait_bash_exec_timeout\n");
        }, 30);
    }
    if (error) {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "wifi test false!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    } else {
        ui_lock();
        lv_textarea_add_text(* ui_TextArea, "wifi test success!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_unlock();
    }
    ui_lock();
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(*ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    ui_unlock();
}
// void ax_eth_test_task() {
//     int ret;
//     std::string lines;
//     llm_TcpClient llm_exec("m5stack-LLM", 5555);
//     int arrar_count[2]    = {0};
//     int summ[2]           = {0};
//     static int eth_status = 0;
//     lv_chart_series_t *ui_Chart3_series_1;
//     lv_chart_series_t *ui_Chart3_series_2;
//     lv_coord_t *ui_Chart3_series_1_array;
//     lv_coord_t *ui_Chart3_series_2_array;
//     ret = llm_exec.sendMessage(
//         "iperf3 -c 192.168.5.110 -t 12 ; "
//         "sleep 1 ; "
//         "iperf3 -c 192.168.5.110 -t 12 -R ; "
//         "echo '----execend' ; \r");
//     // ret = llm_exec.sendMessage("echo '----execend';\r");
//     if (ret) {
//         ui_lock();
//         lv_textarea_add_text(ui_TextArea4, "ax_llm connect false!\n");
//         lv_obj_set_style_bg_color(ui_Button3, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         ui_unlock();
//         goto eth_task_e;
//     }

//     ui_lock();
//     lv_textarea_add_text(ui_TextArea4, "start eth0 test\n");
//     ui_Chart3_series_1       = lv_chart_get_series_next(ui_Chart3, NULL);
//     ui_Chart3_series_2       = lv_chart_get_series_next(ui_Chart3, ui_Chart3_series_1);
//     ui_Chart3_series_1_array = lv_chart_get_y_array(ui_Chart3, ui_Chart3_series_1);
//     ui_Chart3_series_2_array = lv_chart_get_y_array(ui_Chart3, ui_Chart3_series_2);
//     memset(ui_Chart3_series_1_array, 0, sizeof(lv_coord_t) * 10);
//     memset(ui_Chart3_series_2_array, 0, sizeof(lv_coord_t) * 10);
//     lv_chart_set_ext_y_array(ui_Chart3, ui_Chart3_series_1, ui_Chart3_series_1_array);
//     lv_chart_set_ext_y_array(ui_Chart3, ui_Chart3_series_2, ui_Chart3_series_2_array);
//     ui_unlock();

//     while (1) {
//         std::string data = llm_exec.receiveMessage();
//         if (data.length() == 0) {
//             goto eth_task_e;
//         }
//         // std::cout << "date:" << data;
//         {
//             for (auto c : data) {
//                 if (c != '\n') {
//                     lines.push_back(c);
//                 } else {
//                     // std::cout << "line:" << lines << "\n";
//                     if ((lines.find("----execend") != std::string::npos) && (lines.find("echo") == std::string::npos)) {
//                         goto eth_task_e;
//                     }
//                     if ((lines.find("Connection refused") != std::string::npos)) {
//                         goto eth_task_e;
//                     }

//                     if ((lines.find("[ ID]") != std::string::npos)) {
//                         eth_status++;
//                     }
//                     switch (eth_status) {
//                         case 0:
//                             break;
//                         case 1: {
//                             if ((lines.find("[ ID]") != std::string::npos)) {
//                                 ui_lock();
//                                 if (lv_obj_has_flag(ui_Chart3, LV_OBJ_FLAG_HIDDEN))
//                                     lv_obj_clear_flag(ui_Chart3, LV_OBJ_FLAG_HIDDEN);
//                                 if (!lv_obj_has_flag(ui_TextArea4, LV_OBJ_FLAG_HIDDEN))
//                                     lv_obj_add_flag(ui_TextArea4, LV_OBJ_FLAG_HIDDEN);
//                                 ui_unlock();
//                                 eth_status++;
//                             }
//                         } break;

//                         case 2: {
//                             std::list<std::string> bleak_str;
//                             std::string tmp;
//                             for (auto l_c : lines) {
//                                 if (l_c != ' ') {
//                                     tmp.push_back(l_c);
//                                 } else {
//                                     bleak_str.push_back(tmp);
//                                     tmp.clear();
//                                 }
//                             }
//                             double Bitrate;
//                             int __star = 0;
//                             for (auto it : bleak_str) {
//                                 if (__star == 1) {
//                                     if (it.length() > 0) {
//                                         Bitrate = std::stod(it);
//                                         break;
//                                     }
//                                 }
//                                 if (it == "MBytes") {
//                                     __star = 1;
//                                 }
//                             }

//                             float avg_s;
//                             if (arrar_count[0] < 10) {
//                                 ui_Chart3_series_1_array[arrar_count[0]++] = (int)Bitrate;
//                                 summ[0] += (int)Bitrate;
//                                 avg_s = (float)summ[0] / (float)arrar_count[0];
//                             }
//                             std::string lab_tmp = "S:" + std::to_string((int)avg_s) + "M/s";
//                             lv_label_set_text(ui_Label28, lab_tmp.c_str());
//                             lv_chart_set_ext_y_array(ui_Chart3, ui_Chart3_series_1, ui_Chart3_series_1_array);
//                         } break;
//                         case 3:
//                             break;
//                         case 4: {
//                             eth_status++;
//                         } break;
//                         case 5: {
//                             std::list<std::string> bleak_str;
//                             std::string tmp;
//                             for (auto l_c : lines) {
//                                 if (l_c != ' ') {
//                                     tmp.push_back(l_c);
//                                 } else {
//                                     bleak_str.push_back(tmp);
//                                     tmp.clear();
//                                 }
//                             }
//                             double Bitrate;
//                             int __star = 0;
//                             for (auto it : bleak_str) {
//                                 if (__star == 1) {
//                                     if (it.length() > 0) {
//                                         Bitrate = std::stod(it);
//                                         break;
//                                     }
//                                 }
//                                 if (it == "MBytes") {
//                                     __star = 1;
//                                 }
//                             }
//                             float avg_s;
//                             if (arrar_count[1] < 10) {
//                                 ui_Chart3_series_2_array[arrar_count[1]++] = (int)Bitrate;
//                                 summ[1] += (int)Bitrate;
//                                 avg_s = (float)summ[1] / (float)arrar_count[1];
//                             }
//                             std::string lab_tmp = "R:" + std::to_string((int)avg_s) + "M/s";
//                             lv_label_set_text(ui_Label29, lab_tmp.c_str());
//                             lv_chart_set_ext_y_array(ui_Chart3, ui_Chart3_series_2, ui_Chart3_series_2_array);
//                         } break;
//                         default: {
//                             if ((lines.find("iperf Done") != std::string::npos)) {
//                                 goto eth_task_e;
//                             }
//                         } break;
//                     }
//                     lines.clear();
//                 }
//             }
//         }
//     }

// eth_task_e:
//     eth_status = 0;
//     ui_lock();
//     lv_obj_add_flag(ui_TabPage4, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_clear_state(ui_Button3, LV_STATE_DISABLED);
//     lv_obj_add_flag(ui_TextArea4, LV_OBJ_FLAG_CLICKABLE);

//     ui_unlock();
// }
// static int ax_uart_test_task_flage = 0;

// subprocess_s process;

// void ax_uart_master_send() {
//     char buffer[512];
//     size_t n;

//     int send_count = 0;
//     struct timeval timeout;
//     fd_set readfd;
//     FD_ZERO(&readfd);
//     timeout.tv_sec  = 0;
//     timeout.tv_usec = 0;

//     while (ax_uart_test_task_flage) {
//         int fd = fileno(process.stdout_file);
//         FD_SET(fd, &readfd);
//         select(fd + 1, &readfd, NULL, NULL, &timeout);
//         if (FD_ISSET(fd, &readfd)) {
//             const ssize_t bytes_read = read(fd, buffer, 512);
//             ui_lock();
//             lv_textarea_add_text(ui_TextArea3, buffer);
//             ui_unlock();
//         }
//         if ((send_count % 10) == 0) {
//             // printf("send\n");
//             int count = sprintf(buffer, "CT%d\r\n", (int)(send_count / 10));
//             int fdw   = fileno(process.stdin_file);
//             write(fdw, buffer, count);
//         }

//         //  printf("send---\n");
//         usleep(100 * 1000);
//         send_count++;
//     }
//     subprocess_terminate(&process);
//     int exit_code;
//     subprocess_join(&process, &exit_code);
//     printf("Process exited with code %d\n", exit_code);
//     subprocess_destroy(&process);
// }

// // void ax_uart_test_task() {
// //     int ret;
// //     std::string lines;
// //     int send_count         = 0;
// //     const char *_cmd_str[] = {"/usr/bin/picocom", "/dev/ttySTM2", "-b115200", NULL};
// //     llm_TcpClient llm_exec("m5stack-LLM", 5555);
// //     ret = llm_exec.sendMessage("picocom /dev/ttyS1 -b 115200 \r");
// //     if (ret) {
// //         ui_lock();
// //         lv_textarea_add_text(ui_TextArea3, "ax_llm connect false!\n");
// //         lv_obj_set_style_bg_color(ui_Button5, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
// //         ui_unlock();
// //         goto uart_task_e;
// //     }
// //     memset(&process, 0, sizeof(subprocess_s));
// //     if (subprocess_create(_cmd_str, subprocess_option_combined_stdout_stderr, &process)) {
// //         fprintf(stderr, "Failed to create subprocess\n");
// //         goto uart_task_e;
// //     }
// //     ui_lock();
// //     lv_textarea_set_text(ui_TextArea3, "");
// //     // lv_textarea_set_text(ui_TextArea9, "");
// //     ui_unlock();
// //     {
// //         std::thread t(ax_uart_master_send);
// //         t.detach();
// //     }
// //     while (ax_uart_test_task_flage) {
// //         std::string data;
// //         int ret = llm_exec.receiveMessage_pool(data);
// //         if (ret < 0) {
// //             goto uart_task_e;
// //         }
// //         if (ret > 0) {
// //             // std::cout << "len:" << ret << "date:" << data;
// //             {
// //                 for (auto c : data) {
// //                     if (c != '\n') {
// //                         lines.push_back(c);
// //                     } else {
// //                         lines.push_back('\n');
// //                         // std::cout << "line:" << lines << "\n";
// //                         ui_lock();
// //                         // lv_textarea_add_text(ui_TextArea9, lines.c_str());
// //                         ui_unlock();
// //                         lines.clear();
// //                     }
// //                 }
// //             }
// //         }
// //         if ((send_count % 10) == 0) {
// //             std::string send_data = "MT" + std::to_string((int)(send_count / 10)) + "\r\n";
// //             llm_exec.sendMessage(send_data);
// //         }
// //         usleep(100 * 1000);
// //         send_count++;
// //     }
// // uart_task_e:
// //     ui_lock();
// //     lv_obj_add_flag(ui_TabPage6, LV_OBJ_FLAG_SCROLL_CHAIN);
// //     // lv_obj_clear_state(ui_Button5, LV_STATE_DISABLED);
// //     lv_obj_add_flag(ui_TextArea3, LV_OBJ_FLAG_CLICKABLE);
// //     // lv_obj_add_flag(ui_TextArea9, LV_OBJ_FLAG_CLICKABLE);
// //     ui_unlock();
// // }

// void ax_otg_test_task() {
//     // 检测从机设备
//     std::string lines;
//     int exit_code;
//     int len;
//     char lsusb_cmd_buff[1024];
//     const char *_cmd_str[] = {"/usr/bin/lsusb", NULL};

//     int lsusb_num[2] = {0};

//     int ret = system("i2cset -y -f 1 0x26 0x11 0x00");
//     if (ret) {
//         ui_lock();
//         lv_textarea_add_text(ui_TextArea6, "otg switch connect false!\n");
//         lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         ui_unlock();
//         goto otg_task_e;
//     }
//     sleep(3);
//     ui_lock();
//     lv_textarea_add_text(ui_TextArea6, "test llm otg master mode.\n");
//     ui_unlock();

//     {
//         sem_t sem;
//         sem_init(&sem, 0, 0);
//         uart_exec.bashexec("/usr/bin/lsusb", [&](const std::string &json_body) {
//             int true_s = json_body.find("true");
//             if (true_s != std::string::npos) {
//                 std::string request_id = sample_json_str_get(json_body, "request_id");
//                 sem_post(&sem);
//                 uart_exec.clear_request_id(std::stoi(request_id));
//             } else {
//                 std::string delta = sample_json_str_get(json_body, "delta");
//                 static int flage  = 1;
//                 str_parse_line(delta, lines, flage, [&lsusb_num](const std::string &_lin) {
//                     if (_lin.find("Bus") != std::string::npos) {
//                         ui_lock();
//                         lsusb_num[0]++;
//                         lv_textarea_add_text(ui_TextArea6, _lin.c_str());
//                         ui_unlock();
//                     }
//                 });
//             }
//         });
//         struct timespec ts;
//         clock_gettime(CLOCK_REALTIME, &ts);
//         ts.tv_sec += 1;
//         int ret = sem_timedwait(&sem, &ts);
//         if (ret != 0) {
//             ui_lock();
//             lv_textarea_add_text(ui_TextArea6, "otg test timeout\n");
//             ui_unlock();
//         }
//         sem_destroy(&sem);
//     }

//     ui_lock();
//     lv_textarea_add_text(ui_TextArea6, "test llm otg device mode.\n");
//     lv_textarea_add_text(ui_TextArea6, "Detected USB device:\n");
//     ui_unlock();

//     ret = system("i2cset -y -f 1 0x26 0x11 0xff");
//     if (ret) {
//         ui_lock();
//         lv_textarea_add_text(ui_TextArea6, "otg switch connect false!\n");
//         lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         ui_unlock();
//         goto otg_task_e;
//     }
//     sleep(1);
//     system(
//         "[ -d /sys/class/gpio/PC13 ] || echo 45 > /sys/class/gpio/export ; [ \"$(cat /sys/class/gpio/PC13/direction)\" "
//         "= \"out\" ] || echo out > /sys/class/gpio/PC13/direction ; echo 0 > /sys/class/gpio/PC13/value");
//     sleep(1);
//     system("echo 1 > /sys/class/gpio/PC13/value");
//     sleep(1);
//     memset(&process, 0, sizeof(subprocess_s));
//     if (subprocess_create(_cmd_str, subprocess_option_combined_stdout_stderr, &process)) {
//         fprintf(stderr, "Failed to create subprocess\n");
//         goto otg_task_e;
//     }

//     subprocess_join(&process, &exit_code);
//     printf("Process exited with code %d\n", exit_code);
//     len = subprocess_read_stdout(&process, lsusb_cmd_buff, 1024);
//     // printf("lsusb:%s\n", lsusb_cmd_buff);
//     lines.clear();
//     for (size_t i = 0; i < len; i++) {
//         if (lsusb_cmd_buff[i] != '\r' && lsusb_cmd_buff[i] != '\n') {
//             lines += lsusb_cmd_buff[i];
//         } else {
//             if (lines.find("Bus") != std::string::npos) {
//                 lsusb_num[1]++;
//             }
//             lines.clear();
//         }
//     }
//     ui_lock();
//     lv_textarea_add_text(ui_TextArea6, lsusb_cmd_buff);
//     ui_unlock();

//     subprocess_destroy(&process);

// otg_task_e:
//     if ((lsusb_num[0] < 3) || (lsusb_num[1] < 4)) {
//         ui_lock();
//         lv_textarea_add_text(ui_TextArea6, "otg test false!\n");
//         lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         ui_unlock();
//     } else {
//         ui_lock();
//         lv_textarea_add_text(ui_TextArea6, "otg test success!\n");
//         lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
//         ui_unlock();
//     }

//     ui_lock();
//     lv_obj_add_flag(ui_TabPage3, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_clear_state(ui_Button7, LV_STATE_DISABLED);
//     lv_obj_add_flag(ui_TextArea6, LV_OBJ_FLAG_CLICKABLE);
//     ui_unlock();
//     system("i2cset -y -f 1 0x26 0x11 0x00");
// }

void ax_cmm_test_task() {
    lv_obj_t ** ui_Button = &ui_Button6;
    lv_obj_t ** ui_TextArea = &ui_TextArea6;
    lv_obj_t ** ui_TabPage = &ui_TabPage5;
    // int ret;
    std::string lines;
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "test llm cmm.\n");
    ui_unlock();
    int success_count = 0;
    {
        int request_id_num = -1;
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec("/usr/local/m5stack/bin/sample_cmm ", [&](const std::string &json_body) {
            printf("json_body:%s\n", json_body.c_str());
            int true_s = json_body.find("true");
            if (true_s != std::string::npos) {
                std::string request_id = sample_json_str_get(json_body, "request_id");
                sem_post(&sem);
                uart_exec.clear_request_id(std::stoi(request_id));
            } else {
                if (request_id_num == -1) {
                    request_id_num = std::stoi(sample_json_str_get(json_body, "request_id"));
                }
                std::string delta = sample_json_str_get(json_body, "delta");
                static int flage  = 1;
                str_parse_line(delta, lines, flage, [&](const std::string &_lin) {
                    if (_lin.find("ax_mem_cmm_test_") != std::string::npos) {
                        if (_lin.find("end success") != std::string::npos) {
                            success_count++;
                        } else {
                            if (_lin.find("end.Total:3, Pass:3,") != std::string::npos &&
                                (_lin.find("test_012") == std::string::npos) &&
                                (_lin.find("test_014") == std::string::npos)) {
                                success_count++;
                            }
                        }
                        ui_lock();
                        lv_textarea_add_text(* ui_TextArea, _lin.c_str());
                        ui_unlock();
                    }
                });
            }
        });
        // sem_wait(&sem);
        struct timespec ts;
        int WaitTime = 0;
        while (1) {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
            int ret = sem_timedwait(&sem, &ts);
            if (ret == 0) {
                break;
            }
            WaitTime = ax_llm_connect_flage ? 0 : WaitTime + 1;
            if (WaitTime > 3) {
                ui_lock();
                lv_textarea_add_text(* ui_TextArea, "cmm WaitTime timeout!\n");
                lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                ui_unlock();
                printf("dev disconnect!\n");
                uart_exec.clear_request_id(request_id_num);
                break;
            }
        }
        sem_destroy(&sem);
    }

cmm_task_e:
    ui_lock();
    if (success_count < 18) {
        lv_textarea_add_text(* ui_TextArea, "cmm test false!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_textarea_add_text(* ui_TextArea, "cmm test success!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    lv_textarea_add_text(* ui_TextArea, "ax_llm cmm over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    ui_unlock();
}

// void ax_sd_delect_test_task() {
//     // int ret;
//     std::string lines;
//     ui_lock();
//     lv_textarea_add_text(ui_TextArea7, "test llm cmm.\n");
//     ui_unlock();
//     int sd_delect_num = 0;
//     {
//         sem_t sem;
//         sem_init(&sem, 0, 0);
//         uart_exec.bashexec("ls /dev/mmcblk* | grep -v \\\"mmcblk0\\\"", [&](const std::string &json_body) {
//             printf("json_body:%s\n", json_body.c_str());
//             int true_s = json_body.find("true");
//             if (true_s != std::string::npos) {
//                 std::string request_id = sample_json_str_get(json_body, "request_id");
//                 sem_post(&sem);
//                 uart_exec.clear_request_id(std::stoi(request_id));
//             } else {
//                 std::string delta = sample_json_str_get(json_body, "delta");
//                 static int flage  = 1;
//                 str_parse_line(delta, lines, flage, [&sd_delect_num](const std::string &_lin) {
//                     if (_lin.find("mmcblk") != std::string::npos) {
//                         sd_delect_num++;
//                     }
//                     ui_lock();
//                     lv_textarea_add_text(ui_TextArea7, _lin.c_str());
//                     ui_unlock();
//                 });
//             }
//         });
//         sem_wait(&sem);
//         sem_destroy(&sem);
//     }

// sd_delect_task_e:
//     ui_lock();
//     if (sd_delect_num == 0) {
//         lv_obj_set_style_bg_color(ui_Button10, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//     } else {
//         lv_obj_set_style_bg_color(ui_Button10, lv_color_hex(0xFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
//     }
//     lv_textarea_add_text(ui_TextArea7, "ax_llm cmm over!\n");
//     lv_obj_add_flag(ui_TabPage7, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_clear_state(ui_Button10, LV_STATE_DISABLED);
//     lv_obj_add_flag(ui_TextArea7, LV_OBJ_FLAG_CLICKABLE);
//     ui_unlock();
// }

void ax_play_test_task() {
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    int ret;
    std::string lines;
    lv_textarea_add_text(* ui_TextArea, "ax_llm start play logo.wav!\n");
    {
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec(". /etc/profile ; tinyplay -D0 -d1 /usr/local/m5stack/logo.wav",
                           [&](const std::string &json_body) {
                               printf("json_body:%s\n", json_body.c_str());
                               int true_s = json_body.find("true");
                               if (true_s != std::string::npos) {
                                   std::string request_id = sample_json_str_get(json_body, "request_id");
                                   sem_post(&sem);
                                   uart_exec.clear_request_id(std::stoi(request_id));
                               }
                           });
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 10;
        sem_timedwait(&sem, &ts);
        // sem_wait(&sem);
        sem_destroy(&sem);
    }

play_task_e:
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "ax_llm play over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* c_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* p_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* t_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(* t_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_unlock();
}
void ax_playcap_test_task() {
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    int ret;
    std::string lines;
    lv_textarea_add_text(* ui_TextArea, "ax_llm start play cap:\n");
    {
        sem_t sem;
        sem_init(&sem, 0, 0);
        uart_exec.bashexec(". /etc/profile ; [ -f \\\"/tmp/cap_audio.wav\\\" ] && tinyplay -D0 -d1 /tmp/cap_audio.wav ",
                           [&](const std::string &json_body) {
                               printf("json_body:%s\n", json_body.c_str());
                               int true_s = json_body.find("true");
                               if (true_s != std::string::npos) {
                                   std::string request_id = sample_json_str_get(json_body, "request_id");
                                   sem_post(&sem);
                                   uart_exec.clear_request_id(std::stoi(request_id));
                               }
                           });
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 10;
        sem_timedwait(&sem, &ts);
        sem_destroy(&sem);
    }
playcap_task_e:
    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "ax_llm play cap over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* c_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* p_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* t_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(* p_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_unlock();
}

void cap_time() {
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    int hours   = 0;
    int minutes = 0;
    int seconds = 0;
    int msss    = 0;
    while (cap_flage) {
        usleep(100 * 1000);
        msss++;
        if (msss == 10) {
            msss = 0;
            seconds++;
        }
        if (seconds == 60) {
            seconds = 0;
            minutes++;
        }
        if (minutes == 60) {
            hours++;
        }
        // char buff[32];
        // sprintf(buff, "%02d:%02d:%02d", hours, minutes, seconds);
        ui_lock();
        // lv_label_set_text(ui_Label18, buff);
        lv_textarea_add_text(* ui_TextArea, ".");
        ui_unlock();
    }
}

void ax_cap_test_task() {
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    int ret;
    std::string lines;
    {
        uart_exec.bashexec(". /etc/profile ; sample_audio ai -D 0 -d 0 -w 1 -o /tmp/cap_audio.wav > /dev/null 2>&1 &",
                           [&](const std::string &json_body) {
                               uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
                           });
    }

    lv_textarea_add_text(* ui_TextArea, "ax_llm start play cap:\n");
    {
        std::thread t(cap_time);
        t.detach();
    }

    while (cap_flage) {
        usleep(10 * 1000);
    }
    {
        uart_exec.bashexec("kill -SIGINT $(pgrep -f \\\"sample_audio\\\")", [&](const std::string &json_body) {
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
    }
    sleep(1);

playcap_task_e:

    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "ax_llm play cap over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* c_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* p_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_state(* t_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_unlock();
}



void ax_touch_test_task() {
    lv_obj_t ** ui_Button = &ui_Button14;
    lv_obj_t ** ui_TextArea = &ui_TextArea10;
    lv_obj_t ** ui_TabPage = &ui_TabPage11;
    int ret;
    std::string lines;
    {
        uart_exec.bashexec(". /etc/profile ; evtest /dev/input/event0",
                           [&](const std::string &json_body) {
                                int true_s = json_body.find("true");
                                if (true_s != std::string::npos) {
                                    uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
                                    return ;
                                }
                                lines = sample_unescapeString(sample_json_str_get(json_body, "delta"));
                                ui_lock();
                                lv_textarea_add_text(* ui_TextArea, lines.c_str());
                                ui_unlock();
                           });
    }

    lv_textarea_add_text(* ui_TextArea, "ax_llm start evtest /dev/input/event0:\n");
    {
        std::thread t(cap_time);
        t.detach();
    }

    while (cap_flage) {
        usleep(10 * 1000);
    }
    {
        uart_exec.bashexec("kill -SIGINT $(pgrep -f \\\"evtest\\\")", [&](const std::string &json_body) {
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
    }
    sleep(1);

    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "ax_llm evtest over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_unlock();
}


void ax_csi_dsi_test_task() {
    lv_obj_t ** ui_Button = &ui_Button15;
    lv_obj_t ** ui_TextArea = &ui_TextArea11;
    lv_obj_t ** ui_TabPage = &ui_TabPage12;
    int ret;
    std::string lines;
    {
        uart_exec.bashexec(". /etc/profile ; cd /mnt/mmcblk1p6/dist ; ./test_kit_demo3 > /dev/null 2>&1 &",
                           [&](const std::string &json_body) {
                                int true_s = json_body.find("true");
                                if (true_s != std::string::npos) {
                                    uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
                                    return ;
                                }
                                lines = sample_unescapeString(sample_json_str_get(json_body, "delta"));
                                ui_lock();
                                lv_textarea_add_text(* ui_TextArea, lines.c_str());
                                ui_unlock();
                           });
    }

    lv_textarea_add_text(* ui_TextArea, "ax_llm start test_kit_demo3:\n");
    {
        std::thread t(cap_time);
        t.detach();
    }

    while (cap_flage) {
        usleep(10 * 1000);
    }
    {
        uart_exec.bashexec("kill -SIGINT $(pgrep -f \\\"test_kit_demo3\\\")", [&](const std::string &json_body) {
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
    }
    sleep(1);

    ui_lock();
    lv_textarea_add_text(* ui_TextArea, "ax_llm test_kit_demo3 over!\n");
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_clear_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_add_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_unlock();
}

void ax_load_test_task() {
    lv_obj_t ** ui_Button = &ui_Button11;
    lv_obj_t ** ui_TabPage = &ui_TabPage8;
    int ret;
    std::string lines;
    {
        uart_exec.bashexec(
            "sysbench cpu --time=2592000 --threads=3 run > /dev/null 2>&1 & /opt/bin/sample_npu_yolov5s_s -m "
            "/opt/data/npu/models/yolov5s.axmodel -i /opt/data/npu/images/cat.jpg -r 2592000 > /dev/null 2>&1 &",
            [](const std::string &json_body) {
                uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
            });
    }
    int WaitTime = 0;
    while (load_flage) {
        WaitTime = ax_llm_connect_flage ? 0 : WaitTime + 1;
        if (WaitTime > 5) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (ax_llm_connect_flage)
        uart_exec.bashexec(
            "kill -SIGINT $(pgrep -f \\\"sysbench\\\");kill -SIGINT $(pgrep -f \\\"sample_npu_yolov5s_s\\\")",
            [](const std::string &json_body) {
                uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
            });
load_task_e:
    ui_lock();
    lv_obj_add_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    ui_unlock();
}

void ping_loop() {
    while (1) {
        if (uart_exec.ping_sync() == 0) {
            ax_llm_connect_flage = 1;
        } else {
            ax_llm_connect_flage = 0;
            uart_exec.reset_com();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int hw_info_exit_flage = 1;
void hw_info() {
    sem_t sem;
    sem_init(&sem, 0, 0);
    struct timespec ts;
    int commit;
    while (!hw_info_exit_flage) {
        commit = uart_exec.sys_hwinfo([&sem](const std::string &json_body) {
            char str_buf[256];
            // printf("sys_hwinfo:%s\n", json_body.c_str());
            ui_lock();
            try {
                nlohmann::json req_body = nlohmann::json::parse(json_body);
                sprintf(str_buf, "%i%%", (int)req_body["data"]["cpu_loadavg"]);
                lv_label_set_text(ui_Label30, str_buf);
                sprintf(str_buf, "%i%%", (int)req_body["data"]["mem"]);
                lv_label_set_text(ui_Label31, str_buf);
                sprintf(str_buf, "%iC", (int)(((int)req_body["data"]["temperature"]) / 1000));
                lv_label_set_text(ui_Label32, str_buf);
                lv_label_set_text(ui_Label33, "eth0 not connect!");
                if (req_body["data"].contains("eth_info")) {
                    for (const auto& device : req_body["data"]["eth_info"]) {
                        std::string name = device.value("name", "Unknown");
                        std::string ip = device.value("ip", "Unknown");
                        std::string speed = device.value("speed", "Unknown");
                        // std::cout << "Name: " << name << ", IP: " << ip << ", Speed: " << speed << std::endl;
                        if(name == "eth0")
                        {
                            sprintf(str_buf, "eth0:%s:%s", ip.c_str(), speed.c_str());
                            lv_label_set_text(ui_Label33, str_buf);
                            if(speed == "1000")
                            {
                                lv_obj_set_style_bg_color(ui_Container3, lv_color_hex(0x2CF70B), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                            else
                            {
                                lv_obj_set_style_bg_color(ui_Container3, lv_color_hex(0xF30F0F), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                        }
                    }
                }
            } catch (...) {
            }
            ui_unlock();
            sem_post(&sem);
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;
        int ret = sem_timedwait(&sem, &ts);
        if (ret != 0) {
            uart_exec.clear_request_id(commit);
        }
    }
    sem_destroy(&sem);
}

extern "C" {

void cpp_init() {
    std::thread t(ping_loop);
    t.detach();
}

// // 黄色 lv_color_hex(0xF8BE4E)

void start_hw_info() {
    if (hw_info_exit_flage) {
        hw_info_exit_flage = 0;
        std::thread tt(hw_info);
        tt.detach();
    }
}
void stop_hw_info() {
    hw_info_exit_flage = 1;
    lv_label_set_text(ui_Label33, "eth0 not connect!");
}
void kit_emmc_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button2;
    lv_obj_t ** ui_TextArea = &ui_TextArea1;
    lv_obj_t ** ui_Chart = &ui_Chart1;

    lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(*ui_TextArea, "");
    if (lv_obj_has_flag(*ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(*ui_TextArea, LV_OBJ_FLAG_HIDDEN);
    if (!lv_obj_has_flag(*ui_Chart, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(*ui_Chart, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(*ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(*ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(ui_TabPage1, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(*ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(*ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_cmmc_test_task);
    t.detach();
}
void kit_sd_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button3;
    lv_obj_t ** ui_TextArea = &ui_TextArea3;
    lv_obj_t ** ui_Chart = &ui_Chart3;
    lv_obj_t ** ui_TabPage = &ui_TabPage2;

    lv_obj_t ** R_ui_Label = &ui_Label14;
    lv_obj_t ** W_ui_Label = &ui_Label15;

    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
    if (!lv_obj_has_flag(* ui_Chart, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(* ui_Chart, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_sd_test_task);
    t.detach();
}
void kit_otg_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button4;
    lv_obj_t ** ui_TextArea = &ui_TextArea4;
    lv_obj_t ** ui_TabPage = &ui_TabPage3;

    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_otg_test_task);
    t.detach();
}
void kit_i2c_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button5;
    lv_obj_t ** ui_TextArea = &ui_TextArea5;
    lv_obj_t ** ui_TabPage = &ui_TabPage4;

    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_i2c_test_task);
    t.detach();
}
void kit_io_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button13;
    lv_obj_t ** ui_TextArea = &ui_TextArea9;
    lv_obj_t ** ui_TabPage = &ui_TabPage10;
    static bool io_on = false;
    if(io_on)
    {
        uart_exec.bashexec(
        "start-stop-daemon --stop --pidfile /var/run/test_io_demo.pid ; "
        "echo 0  > /sys/class/gpio/gpio511/value ; ",
        [](const std::string &json_body) {
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
        lv_obj_set_style_bg_color(ui_Button12, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        io_on = false;
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        uart_exec.bashexec(
        "[ -d \\\"/sys/class/gpio/gpio511\\\" ] || { echo 511 > /sys/class/gpio/export ; echo out > /sys/class/gpio/gpio511/direction ; } ; "
        "echo \\\"if [[ \\\\$(( \\\\$(i2cget -y -f 1 0x43 0x0f) & 0x8 )) -eq 8 ]]; then echo 255 > /sys/class/leds/sys_led/brightness ; else echo 0 > /sys/class/leds/sys_led/brightness ; fi \\\" > /tmp/sys_led_blak.sh ; "
        "start-stop-daemon --start --quiet --background --make-pidfile --pidfile /var/run/test_io_demo.pid  "
        "--exec /bin/bash -- -c \\\""
        "i2cset -y -f 1 0x43 0x0D 0x8 ; "
        "while true ; do "
            "echo 0  > /sys/class/gpio/gpio511/value ; "
            "bash /tmp/sys_led_blak.sh ; "
            "sleep 0.5; "
            "echo 1  > /sys/class/gpio/gpio511/value ; "
            "bash /tmp/sys_led_blak.sh ; "
            "sleep 0.5 ; "
        "done \\\" ; ",
        [](const std::string &json_body) {
            uart_exec.clear_request_id(std::stoi(sample_json_str_get(json_body, "request_id")));
        });
        io_on = true;
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}


// evtest /dev/input/event0
void kit_touch_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button14;
    lv_obj_t ** ui_TextArea = &ui_TextArea10;
    lv_obj_t ** ui_TabPage = &ui_TabPage11;
    if (cap_flage) {
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        cap_flage = 0;
    } else {
        cap_flage = 1;
        // lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_textarea_set_text(* ui_TextArea, "");
        // if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
        // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

        if (ax_llm_connect_flage == 0) {
            lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
            lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            return;
        }
        // page_lock = 1;
        lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
        lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
        std::thread t(ax_touch_test_task);
        t.detach();
    }
}
void kit_csi_dsi_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button15;
    lv_obj_t ** ui_TextArea = &ui_TextArea11;
    lv_obj_t ** ui_TabPage = &ui_TabPage12;
    if (cap_flage) {
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        cap_flage = 0;
    } else {
        cap_flage = 1;
        // lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_textarea_set_text(* ui_TextArea, "");
        // if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
        // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

        if (ax_llm_connect_flage == 0) {
            lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
            lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            return;
        }
        // page_lock = 1;
        lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
        lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
        std::thread t(ax_csi_dsi_test_task);
        t.detach();
    }
}
void kit_cmm_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button6;
    lv_obj_t ** ui_TextArea = &ui_TextArea6;
    lv_obj_t ** ui_TabPage = &ui_TabPage5;
    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
    // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_cmm_test_task);
    t.detach();
}
void kit_wifi_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button7;
    lv_obj_t ** ui_TextArea = &ui_TextArea7;
    lv_obj_t ** ui_TabPage = &ui_TabPage6;

    lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_wifi_test_task);
    t.detach();
}
void kit_play_cap_test_call(lv_event_t *e)
{
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    // lv_obj_set_style_bg_color(* p_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(* p_ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_text(* ui_TextArea, "");
    // if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
    // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* p_ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* c_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_state(* p_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_state(* t_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_playcap_test_task);
    t.detach();
}
void kit_cap_test_call(lv_event_t *e)
{
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    if (cap_flage) {
        lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        cap_flage = 0;
    } else {
        cap_flage = 1;
        // lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_textarea_set_text(* ui_TextArea, "");
        // if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
        // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

        if (ax_llm_connect_flage == 0) {
            lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
            lv_obj_set_style_bg_color(* c_ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            return;
        }
        // page_lock = 1;
        lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
        // lv_obj_add_state(ui_Button4, LV_STATE_DISABLED);
        lv_obj_add_state(* p_ui_Button, LV_STATE_DISABLED);
        lv_obj_add_state(* t_ui_Button, LV_STATE_DISABLED);
        lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
        std::thread t(ax_cap_test_task);
        t.detach();
    }
}
void kit_play_logo_test_call(lv_event_t *e)
{
    lv_obj_t ** c_ui_Button = &ui_Button9;
    lv_obj_t ** p_ui_Button = &ui_Button8;
    lv_obj_t ** t_ui_Button = &ui_Button10;
    lv_obj_t ** ui_TextArea = &ui_TextArea8;
    lv_obj_t ** ui_TabPage = &ui_TabPage7;
    // lv_color_t hasasd = lv_obj_get_style_bg_color(* t_ui_Button, LV_PART_MAIN | LV_STATE_DEFAULT);
    // printf("lv_obj_set_style_bg_color----------------:%x \n", hasasd.full);
    // lv_obj_set_style_bg_color(* t_ui_Button, lv_color_hex(0x2095F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(* t_ui_Button, lv_color_hex(0xE6B73C), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_textarea_set_text(* ui_TextArea, "");
    // if (lv_obj_has_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_HIDDEN);
    // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

    if (ax_llm_connect_flage == 0) {
        lv_textarea_add_text(* ui_TextArea, "ax_llm not't connect!\n");
        lv_obj_set_style_bg_color(* t_ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }
    // page_lock = 1;
    lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_state(* c_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_state(* p_ui_Button, LV_STATE_DISABLED);
    lv_obj_add_state(* t_ui_Button, LV_STATE_DISABLED);
    lv_obj_clear_flag(* ui_TextArea, LV_OBJ_FLAG_CLICKABLE);
    std::thread t(ax_play_test_task);
    t.detach();
}
void kit_load_test_call(lv_event_t *e)
{
    lv_obj_t ** ui_Button = &ui_Button11;
    lv_obj_t ** ui_TabPage = &ui_TabPage8;
    if (load_flage) {
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
        load_flage = 0;
    } else {
        load_flage = 1;
        lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xF8BE4E), LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_textarea_set_text(ui_TextArea5, "");
        // if (lv_obj_has_flag(ui_TextArea5, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(ui_TextArea5, LV_OBJ_FLAG_HIDDEN);
        // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

        if (ax_llm_connect_flage == 0) {
            // lv_textarea_add_text(ui_TextArea5, "ax_llm not't connect!\n");
            lv_obj_set_style_bg_color(* ui_Button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            return;
        }
        // page_lock = 1;
        lv_obj_clear_flag(* ui_TabPage, LV_OBJ_FLAG_SCROLL_CHAIN);
        // lv_obj_add_state(ui_Button4, LV_STATE_DISABLED);
        // lv_obj_add_state(ui_Button6, LV_STATE_DISABLED);
        // lv_obj_add_state(ui_Button9, LV_STATE_DISABLED);
        // lv_obj_clear_flag(ui_TextArea5, LV_OBJ_FLAG_CLICKABLE);
        std::thread t(ax_load_test_task);
        t.detach();
    }
}
void kit_power_test_call(lv_event_t *e)
{
    static bool power_on = true;
    if(power_on)
    {
        lv_obj_set_style_bg_color(ui_Button12, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        power_on = false;
    }
    else
    {
        lv_obj_set_style_bg_color(ui_Button12, lv_color_hex(0x0AEF1A), LV_PART_MAIN | LV_STATE_DEFAULT);
        power_on = true;
    }
}


// void ax_otg_test(lv_event_t *e) {
//     lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_textarea_set_text(ui_TextArea6, "");
//     if (lv_obj_has_flag(ui_TextArea6, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(ui_TextArea6, LV_OBJ_FLAG_HIDDEN);
//     // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

//     if (ax_llm_connect_flage == 0) {
//         lv_textarea_add_text(ui_TextArea6, "ax_llm not't connect!\n");
//         lv_obj_set_style_bg_color(ui_Button7, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         return;
//     }
//     // page_lock = 1;
//     lv_obj_clear_flag(ui_TabPage3, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_add_state(ui_Button7, LV_STATE_DISABLED);
//     lv_obj_clear_flag(ui_TextArea6, LV_OBJ_FLAG_CLICKABLE);
//     std::thread t(ax_otg_test_task);
//     t.detach();
// }


// void ax_cmm_test(lv_event_t *e) {
//     lv_obj_set_style_bg_color(ui_Button3, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_textarea_set_text(ui_TextArea4, "");
//     if (lv_obj_has_flag(ui_TextArea4, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(ui_TextArea4, LV_OBJ_FLAG_HIDDEN);
//     // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

//     if (ax_llm_connect_flage == 0) {
//         lv_textarea_add_text(ui_TextArea4, "ax_llm not't connect!\n");
//         lv_obj_set_style_bg_color(ui_Button3, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         return;
//     }
//     // page_lock = 1;
//     lv_obj_clear_flag(ui_TabPage4, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_add_state(ui_Button3, LV_STATE_DISABLED);
//     lv_obj_clear_flag(ui_TextArea4, LV_OBJ_FLAG_CLICKABLE);
//     std::thread t(ax_cmm_test_task);
//     t.detach();
// }

// void ax_sd_delect_test(lv_event_t *e) {
//     lv_obj_set_style_bg_color(ui_Button10, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_textarea_set_text(ui_TextArea7, "");
//     if (lv_obj_has_flag(ui_TextArea7, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(ui_TextArea7, LV_OBJ_FLAG_HIDDEN);
//     // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

//     if (ax_llm_connect_flage == 0) {
//         lv_textarea_add_text(ui_TextArea7, "ax_llm not't connect!\n");
//         lv_obj_set_style_bg_color(ui_Button10, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//         return;
//     }
//     // page_lock = 1;
//     lv_obj_clear_flag(ui_TabPage7, LV_OBJ_FLAG_SCROLL_CHAIN);
//     lv_obj_add_state(ui_Button10, LV_STATE_DISABLED);
//     lv_obj_clear_flag(ui_TextArea7, LV_OBJ_FLAG_CLICKABLE);
//     std::thread t(ax_sd_delect_test_task);
//     t.detach();
// }

// void ax_load_test(lv_event_t *e) {
//     if (load_flage) {
//         lv_obj_set_style_bg_color(ui_Button11, lv_color_hex(0x79B4F2), LV_PART_MAIN | LV_STATE_DEFAULT);
//         load_flage = 0;
//     } else {
//         load_flage = 1;
//         lv_obj_set_style_bg_color(ui_Button11, lv_color_hex(0xF8BE4E), LV_PART_MAIN | LV_STATE_DEFAULT);
//         // lv_textarea_set_text(ui_TextArea5, "");
//         // if (lv_obj_has_flag(ui_TextArea5, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(ui_TextArea5, LV_OBJ_FLAG_HIDDEN);
//         // if (!lv_obj_has_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(ui_Chart7, LV_OBJ_FLAG_HIDDEN);

//         if (ax_llm_connect_flage == 0) {
//             // lv_textarea_add_text(ui_TextArea5, "ax_llm not't connect!\n");
//             lv_obj_set_style_bg_color(ui_Button11, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//             return;
//         }
//         // page_lock = 1;
//         lv_obj_clear_flag(ui_TabPage8, LV_OBJ_FLAG_SCROLL_CHAIN);
//         // lv_obj_add_state(ui_Button4, LV_STATE_DISABLED);
//         // lv_obj_add_state(ui_Button6, LV_STATE_DISABLED);
//         // lv_obj_add_state(ui_Button9, LV_STATE_DISABLED);
//         // lv_obj_clear_flag(ui_TextArea5, LV_OBJ_FLAG_CLICKABLE);
//         std::thread t(ax_load_test_task);
//         t.detach();
//     }
// }

// void llm_power_switch(lv_event_t *e) {
//     static int power_status = 1;
//     if (power_status) {
//         uart_exec.close_led();
//         usleep(100000);
//         power_status = 0;
//         system(
//             "[ -d /sys/class/gpio/PC13 ] || echo 45 > /sys/class/gpio/export ; [ \"$(cat "
//             "/sys/class/gpio/PC13/direction)\" "
//             "= \"out\" ] || echo out > /sys/class/gpio/PC13/direction ; echo 0 > /sys/class/gpio/PC13/value ; ");
//         lv_obj_set_style_bg_color(ui_Button2, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
//     } else {
//         power_status = 1;
//         system(
//             "[ -d /sys/class/gpio/PC13 ] || echo 45 > /sys/class/gpio/export ; [ \"$(cat "
//             "/sys/class/gpio/PC13/direction)\" "
//             "= \"out\" ] || echo out > /sys/class/gpio/PC13/direction ; echo 1 > /sys/class/gpio/PC13/value ; ");
//         lv_obj_set_style_bg_color(ui_Button2, lv_color_hex(0x0AEF1A), LV_PART_MAIN | LV_STATE_DEFAULT);
//     }
// }
};