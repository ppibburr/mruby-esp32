#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/string.h"

#include "example_mrb.h"

#define TAG "mruby_task"

void mruby_task(void *pvParameter)
{
  mrb_state *mrb = mrb_open();
  mrbc_context *context = mrbc_context_new(mrb);
  int ai = mrb_gc_arena_save(mrb);
  ESP_LOGI(TAG, "%s", "Loading binary...");
  mrb_load_irep_cxt(mrb, example_mrb, context);
  
  if (mrb->exc) {
    ESP_LOGE(TAG, "Exception occurred: %s", mrb_str_to_cstr(mrb, mrb_inspect(mrb, mrb_obj_value(mrb->exc))));
    mrb->exc = 0;
  } else {
    ESP_LOGI(TAG, "%s", "Success");
  }
  
  mrb_gc_arena_restore(mrb, ai);
  mrbc_context_free(mrb, context);
  mrb_close(mrb);

  // This task should never end, even if the
  // script ends.
  while (1) {
  }
}

void app_main()
{
  nvs_flash_init();
  nvs_handle nvs;
  nvs_open("mruby-esp32", NVS_READWRITE, &nvs);
  nvs_set_str(nvs, "code", "p 5");
  xTaskCreatePinnedToCore(mruby_task, "mruby_task", (1024*16), NULL, 1, NULL,0);
  vTaskDelete(xTaskGetCurrentTaskHandle());
}
