#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/moduleinfo.h>

#include "psplib/pl_snd.h"
#include "psplib/video.h"
#include "psplib/pl_psp.h"
#include "psplib/ctrl.h"

#include "menu.h"

PSP2_MODULE_INFO(0,1,PSP_APP_NAME);
//PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static void ExitCallback(void* arg)
{
  ExitPSP = 1;
}

int main(int argc,char *argv[])
{
  /* Initialize PSP */
  pl_psp_init("cache0:/SMSPlusVITA/");
  pl_snd_init(768, 1);
  pspCtrlInit();
  pspVideoInit();

  /* Initialize callbacks */
  pl_psp_register_callback(PSP_EXIT_CALLBACK,
                           ExitCallback,
                           NULL);
  pl_psp_start_callback_thread();

  /* Start emulation */
  InitMenu();
  DisplayMenu();
  TrashMenu();

  /* Release PSP resources */
  pl_snd_shutdown();
  pspVideoShutdown();
  pl_psp_shutdown();

  return(0);
}
