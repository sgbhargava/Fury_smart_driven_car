#ifndef HANDLERS_HPP_
#define HANDLERS_HPP_
#include "can.h"
#ifdef __cplusplus
extern "C" {
#endif
extern can_std_id_t can_test1;
extern can_std_id_t can_test2;
extern can_fullcan_msg_t *can_ptr;
extern can_fullcan_msg_t *can_test1_ptr;
 bool can_custom_init();
//can_std_grp_id_t
#ifdef __cplusplus
}
#endif
#endif
