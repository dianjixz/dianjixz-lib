#include "tbox/tbox.h"

int main(int argc, char** argv) {
    if (!tb_init(tb_null, tb_null)) return 0;

    tb_vector_ref_t vector = tb_vector_init(0, tb_element_str(tb_true));
    if (vector) {
        tb_vector_insert_tail(vector, "hello");
        tb_vector_insert_tail(vector, "tbox");

        tb_for_all (tb_char_t const*, cstr, vector) {
            tb_trace_i("%s", cstr);
        }
        tb_vector_exit(vector);
    }
    tb_exit();
    return 0;
}