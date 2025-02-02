#include "wt_sil.h"
#include <stdio.h>


int main(int argc, char * argv[])
{
    wt_sil_state_t state = {};
    wt_sil_init(&state);
    wt_sil_repl(&state);
    wt_sil_close(&state);
    return 0;
}
