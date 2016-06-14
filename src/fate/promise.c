#include <stdbool.h>
#include <fate/promise.h>

bool fe_promise_wait(fe_promise l) {return false;}
bool fe_promise_is_ready(fe_promise l) {return false;}
void fe_promise_set_callback(fe_promise l, fe_promise_callback func, void *userdata) {}

