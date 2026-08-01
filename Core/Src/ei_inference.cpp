#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include <stdio.h>
#include <string.h>

#define NUM_SENSOR_FEATURES 15

#if (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE % NUM_SENSOR_FEATURES) != 0
#error "EI input frame size is not divisible by 15. Check Edge Impulse features/window."
#endif

static float ei_feature_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static size_t ei_feature_index = 0;

static int get_signal_data(size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr, ei_feature_buffer + offset, length * sizeof(float));
    return 0;
}

static int run_ei_classifier_now(void)
{
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

    if (res != EI_IMPULSE_OK) {
        printf("EI run_classifier failed: %d\r\n", res);
        return -1;
    }

    float best_score = 0.0f;
    const char *best_label = "unknown";

    printf("\r\n--- Edge Impulse Prediction ---\r\n");

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        printf("%s: %.3f\r\n",
               result.classification[ix].label,
               result.classification[ix].value);

        if (result.classification[ix].value > best_score) {
            best_score = result.classification[ix].value;
            best_label = result.classification[ix].label;
        }
    }

    printf("FINAL CLASS: %s (%.3f)\r\n", best_label, best_score);
    printf("-------------------------------\r\n\r\n");

    return 0;
}

extern "C" int ei_add_sensor_reading(
    float co2,
    float temp,
    float humidity,
    float pressure,
    float gas_resistance,
    float pm1_0,
    float pm2_5,
    float pm4_0,
    float pm10_0,
    float pnc_0_5,
    float pnc_1_0,
    float pnc_2_5,
    float pnc_4_0,
    float pnc_10_0,
    float tps
)
{
    if ((ei_feature_index + NUM_SENSOR_FEATURES) > EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_feature_index = 0;
    }

    ei_feature_buffer[ei_feature_index++] = co2;
    ei_feature_buffer[ei_feature_index++] = temp;
    ei_feature_buffer[ei_feature_index++] = humidity;
    ei_feature_buffer[ei_feature_index++] = pressure;
    ei_feature_buffer[ei_feature_index++] = gas_resistance;
    ei_feature_buffer[ei_feature_index++] = pm1_0;
    ei_feature_buffer[ei_feature_index++] = pm2_5;
    ei_feature_buffer[ei_feature_index++] = pm4_0;
    ei_feature_buffer[ei_feature_index++] = pm10_0;
    ei_feature_buffer[ei_feature_index++] = pnc_0_5;
    ei_feature_buffer[ei_feature_index++] = pnc_1_0;
    ei_feature_buffer[ei_feature_index++] = pnc_2_5;
    ei_feature_buffer[ei_feature_index++] = pnc_4_0;
    ei_feature_buffer[ei_feature_index++] = pnc_10_0;
    ei_feature_buffer[ei_feature_index++] = tps;

    printf("EI buffer: %lu / %lu\r\n",
           (unsigned long)ei_feature_index,
           (unsigned long)EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);

    if (ei_feature_index >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        int status = run_ei_classifier_now();

        ei_feature_index = 0;

        return status;
    }

    return 1;
}
