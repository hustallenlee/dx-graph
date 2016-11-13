#ifndef _CONTROLLER_
#define _CONTROLLER_
#include <bits/stdc++.h>
#include "utils/dispatcher.h"
#include "utils/log_wrapper.h"
#include "utils/buffer.h"

class controller{
    private:
        dx_lib::buffer *edge_buffer;
        
    public:
        controller(){}
        ~controller(){
            delete edge_buffer;
        }
};
#endif
