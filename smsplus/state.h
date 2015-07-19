
#ifndef _STATE_H_
#define _STATE_H_



#define STATE_VERSION   0x0101      /* Version 1.1 (BCD) */
#define STATE_HEADER    "SST\0"     /* State file header */

/* Function prototypes */
void system_save_state(void *fd);
void system_load_state(void *fd);

//>>>davex
int save_state_to_mem(void *stor);
int get_save_state_size(void);
int load_state_from_mem(void *stor);
//<<<

#endif /* _STATE_H_ */
