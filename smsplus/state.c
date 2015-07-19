/*
    state.c --
    Save state management.
*/

#include "shared.h"

void viewport_check();

int get_save_state_size(void){
    int ssize = 0;
    
    //davex: header is not saved in a memory state
    
    ssize = sizeof(vdp_t); // VDP context
    ssize += sizeof(sms_t); //SMS context (1/3)
    //SMS context (2/3)
    ssize += 1; //fputc(cart.fcr[0], fd); 
    ssize += 1; //fputc(cart.fcr[1], fd);
    ssize += 1; //fputc(cart.fcr[2], fd);
    ssize += 1; //fputc(cart.fcr[3], fd);
    //SMS context (3/3)
    ssize += 0x8000;
    
    //Z80 context
    ssize += sizeof(Z80_Regs);
    ssize += sizeof(int);
    
    //YM2413 context
    ssize += FM_GetContextSize();
    
    //SN76489 context
    ssize += SN76489_GetContextSize();
    
    return ssize;
}


int save_state_to_mem(void *stor){
	
	
    //davex: header is not saved in a memory state
    
    memcpy(stor, &vdp, sizeof(vdp_t)); //VDP context
    stor += sizeof(vdp_t);
    
    //SMS context (1/3)
    memcpy(stor, &sms, sizeof(sms_t) );
    stor += sizeof(sms_t); 
    
    //SMS context (2/3)
    memcpy(stor, &cart.fcr[0], 1 );
    stor += 1; 
    memcpy(stor, &cart.fcr[1], 1 );
    stor += 1; 
    memcpy(stor, &cart.fcr[2], 1 );
    stor += 1; 
    memcpy(stor, &cart.fcr[3], 1 );
    stor += 1; 
        
    //SMS context (3/3)
    memcpy(stor, &cart.sram, 0x8000);
    stor += 0x8000;
    

    // Z80 context 
    memcpy( stor, Z80_Context, sizeof(Z80_Regs) );
    stor += sizeof(Z80_Regs);
    memcpy( stor, &after_EI, sizeof(int) );
    stor += sizeof(int);
    
    
    // YM2413 context
    memcpy( stor, FM_GetContextPtr(), FM_GetContextSize() );
    stor += FM_GetContextSize();
    
    // SN76489 context
    memcpy(stor, SN76489_GetContextPtr(0),  SN76489_GetContextSize() );
    //stor += SN76489_GetContextSize();
    return 1;
}



int load_state_from_mem(void *stor)
{
    int i;

    /* Initialize everything */
    z80_reset(0);
    z80_set_irq_callback(sms_irq_callback);
    system_reset();
    if(snd.enabled)
        sound_reset();

    //davex: header is not saved in memory state
    

    // Load VDP context 
    memcpy(&vdp, stor, sizeof(vdp_t)); //VDP context
    stor += sizeof(vdp_t);
    
    
    //Load SMS context (1/3)
    memcpy(&sms, stor, sizeof(sms_t) );
    stor += sizeof(sms_t); 
    
    //Load SMS context (2/3)
    memcpy(&cart.fcr[0],stor, 1 );
    stor += 1; 
    memcpy(&cart.fcr[1],stor, 1 );
    stor += 1; 
    memcpy(&cart.fcr[2],stor, 1 );
    stor += 1; 
    memcpy(&cart.fcr[3],stor, 1 );
    stor += 1; 
        
    //Load SMS context (3/3)
    memcpy(&cart.sram, stor, 0x8000);
    stor += 0x8000;
    

    // Load Z80 context 
    memcpy(Z80_Context, stor, sizeof(Z80_Regs) );
    stor += sizeof(Z80_Regs);
    memcpy(&after_EI, stor,  sizeof(int) );
    stor += sizeof(int);
    
    
    // Load YM2413 context
    FM_SetContext(stor);
    stor += FM_GetContextSize();
    
    // Load SN76489 context
    SN76489_SetContext(0, stor);
   

    /* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    for(i = 0x00; i <= 0x2F; i++)
    {
        cpu_readmap[i]  = &cart.rom[(i & 0x1F) << 10];
        cpu_writemap[i] = dummy_write;
    }

    for(i = 0x30; i <= 0x3F; i++)
    {
        cpu_readmap[i] = &sms.wram[(i & 0x07) << 10];
        cpu_writemap[i] = &sms.wram[(i & 0x07) << 10];
    }

    sms_mapper_w(3, cart.fcr[3]);
    sms_mapper_w(2, cart.fcr[2]);
    sms_mapper_w(1, cart.fcr[1]);
    sms_mapper_w(0, cart.fcr[0]);

    /* Force full pattern cache update */
    bg_list_index = 0x200;
    for(i = 0; i < 0x200; i++)
    {
        bg_name_list[i] = i;
        bg_name_dirty[i] = -1;
    }

    /* Restore palette */
    for(i = 0; i < PALETTE_SIZE; i++)
        palette_sync(i, 1);
 
    viewport_check();

    return 1;
}

void system_save_state(void *fd)
{
    char *id = STATE_HEADER;
    uint16 version = STATE_VERSION;

    /* Write header */
    fwrite(id, sizeof(id), 1, fd);
    fwrite(&version, sizeof(version), 1, fd);

    /* Save VDP context */
    fwrite(&vdp, sizeof(vdp_t), 1, fd);

    /* Save SMS context */
    fwrite(&sms, sizeof(sms_t), 1, fd);

    fputc(cart.fcr[0], fd);
    fputc(cart.fcr[1], fd);
    fputc(cart.fcr[2], fd);
    fputc(cart.fcr[3], fd);
    fwrite(cart.sram, 0x8000, 1, fd);

    /* Save Z80 context */
    fwrite(Z80_Context, sizeof(Z80_Regs), 1, fd);
    fwrite(&after_EI, sizeof(int), 1, fd);

    /* Save YM2413 context */
    fwrite(FM_GetContextPtr(), FM_GetContextSize(), 1, fd);

    /* Save SN76489 context */
    fwrite(SN76489_GetContextPtr(0), SN76489_GetContextSize(), 1, fd);
}


void system_load_state(void *fd)
{
    int i;
    uint8 *buf;
    char id[4];
    uint16 version;

    /* Initialize everything */
    z80_reset(0);
    z80_set_irq_callback(sms_irq_callback);
    system_reset();
    if(snd.enabled)
        sound_reset();

    /* Read header */
    fread(id, 4, 1, fd);
    fread(&version, 2, 1, fd);

    /* Load VDP context */
    fread(&vdp, sizeof(vdp_t), 1, fd);

    /* Load SMS context */
    fread(&sms, sizeof(sms_t), 1, fd);

    cart.fcr[0] = fgetc(fd);
    cart.fcr[1] = fgetc(fd);
    cart.fcr[2] = fgetc(fd);
    cart.fcr[3] = fgetc(fd);
    fread(cart.sram, 0x8000, 1, fd);

    /* Load Z80 context */
    fread(Z80_Context, sizeof(Z80_Regs), 1, fd);
    fread(&after_EI, sizeof(int), 1, fd);

    /* Load YM2413 context */
    buf = malloc(FM_GetContextSize());
    fread(buf, FM_GetContextSize(), 1, fd);
    FM_SetContext(buf);
    free(buf);

    /* Load SN76489 context */
    buf = malloc(SN76489_GetContextSize());
    fread(buf, SN76489_GetContextSize(), 1, fd);
    SN76489_SetContext(0, buf);
    free(buf);

    /* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    for(i = 0x00; i <= 0x2F; i++)
    {
        cpu_readmap[i]  = &cart.rom[(i & 0x1F) << 10];
        cpu_writemap[i] = dummy_write;
    }

    for(i = 0x30; i <= 0x3F; i++)
    {
        cpu_readmap[i] = &sms.wram[(i & 0x07) << 10];
        cpu_writemap[i] = &sms.wram[(i & 0x07) << 10];
    }

    sms_mapper_w(3, cart.fcr[3]);
    sms_mapper_w(2, cart.fcr[2]);
    sms_mapper_w(1, cart.fcr[1]);
    sms_mapper_w(0, cart.fcr[0]);

    /* Force full pattern cache update */
    bg_list_index = 0x200;
    for(i = 0; i < 0x200; i++)
    {
        bg_name_list[i] = i;
        bg_name_dirty[i] = -1;
    }

    /* Restore palette */
    for(i = 0; i < PALETTE_SIZE; i++)
        palette_sync(i, 1);

    viewport_check();
}

