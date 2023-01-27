#pragma once

// mode numbers must start from 0 and be consecutive
#define CONTROL_STRATEGIES(X) \
    /** Contactor shorted. */ \
    X(control_shorted, 0) \
    /** Control function written in lisp. */ \
    X(control_lisp, 1)

#define X_ENUM(name, value) name = value,
typedef enum {
    CONTROL_STRATEGIES(X_ENUM)
} control_strategy_t;
#undef X_ENUM

extern const char * control_strategies[];


void control_init();
void control_init_lisp();

void control_new_state();

void control_set_strategy(control_strategy_t s);
control_strategy_t control_get_strategy();
