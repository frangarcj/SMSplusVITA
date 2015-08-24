#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/moduleinfo.h>

#include "psplib/pl_snd.h"
#include "psplib/video.h"
#include "psplib/pl_psp.h"
#include "psplib/ctrl.h"
#include <vita2d.h>

#include "menu.h"
#include "revitalize.h"

PSP2_MODULE_INFO(0,1,PSP_APP_NAME);
//PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

void show_splash()
{
	vita2d_start_drawing();
	vita2d_clear_screen();

	vita2d_texture *splash = vita2d_create_empty_texture(960, 544);

	splash = vita2d_load_PNG_buffer(revitalize);

	vita2d_draw_texture(splash, 0, 0);

	vita2d_end_drawing();
	vita2d_swap_buffers();

	sceKernelDelayThread(5000000); // Delay 5 seconds

	vita2d_free_texture(splash);
}

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

  show_splash();

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
