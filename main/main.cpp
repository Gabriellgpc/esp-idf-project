#include <stdio.h>
#include <ctype.h> //toupper

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SPP_TAG "SPP_GABRIEL_TEST"

bool initBluetooth(const char* deviceName, esp_spp_cb_t *esp_spp_cb);
static void esp_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

static uint32_t _spp_client = 0;

extern "C" void
app_main(){
    printf("Vamos começar...\n");

    if(!initBluetooth("ESP32_DE_GABRIEL", esp_spp_callback)){
      printf("Ja deu errado!");
    }
    while(true)
    {
      vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

bool initBluetooth(const char* deviceName, esp_spp_cb_t *esp_spp_cb)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  if ((ret = esp_bluedroid_init()) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  if ((ret = esp_bluedroid_enable()) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  if ((ret = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK) {
      ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
      return false;
  }

  esp_bt_dev_set_device_name(deviceName);

  return true;
}

static void esp_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "Evento: ESP_SPP_INIT_EVT");
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, "ESP32_SPP_SERVER");
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        printf("Evento: ESP_SPP_DISCOVERY_COMP_EVT\n");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        printf("Evento: ESP_SPP_OPEN_EVT\n");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        printf("Evento: ESP_SPP_CLOSE_EVT\n");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        printf("Evento: ESP_SPP_START_EVT\n");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        printf("Evento: ESP_SPP_CL_INIT_EVT\n");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT");
        printf("Tamanho: %d da mensagem: %s\n", param->data_ind.len, param->data_ind.data);
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        printf("Evento: ESP_SPP_CONG_EVT\n");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        printf("Evento: ESP_SPP_WRITE_EVT\n");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        _spp_client = param->open.handle;
        printf("Evento: ESP_SPP_SRV_OPEN_EVT\n");
        break;
    default:
        break;
    }
}
