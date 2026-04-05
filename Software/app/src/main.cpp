#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <math.h>

#define SLEEP_TIME_MS       1000

#define MOC_PIN             23
#define ADC_NODE            DT_NODELABEL(adc1)

const static struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define ADC_CHANNEL         3
#define ADC_RESOLUTION      12
#define ADC_GAIN            ADC_GAIN_1_4
#define ADC_REFERENCE       ADC_REF_INTERNAL
// #define ADC_ACQ_TIME        ADC_ACQ_TIME_DEFAULT

/* Constant table for NTC thermistor */
#define NTC_A               0.001129148
#define NTC_B               0.000234125
#define NTC_C               0.0000000876741
#define NTC_R2              9990.0
#define VCC                 3.3

static const struct adc_channel_cfg channel_cfg = {
    .gain             = ADC_GAIN,
    .reference        = ADC_REFERENCE,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = ADC_CHANNEL,
    .differential     = 0
};

double read_ntc_temp(void) {
    int16_t buf = 0; // Inițializăm cu 0
    
    struct adc_sequence sequence = {
        .channels    = BIT(ADC_CHANNEL),
        .buffer      = &buf,
        .buffer_size = sizeof(buf),
        .resolution  = ADC_RESOLUTION,
    };

    // EFECTUĂM CITIREA REALĂ
    int err = adc_read(adc_dev, &sequence);
    
    if (err < 0) {
        LOG_ERR("ADC read failed: %d", err);
        return -273.15;
    }

    // ACUM LOGĂM VALOAREA REALĂ
    LOG_INF("Raw ADC Real: %d", buf);

    double adc_val = static_cast<double>(buf);
    
    // Protecție împotriva valorilor extreme (0 sau 4095)
    if (adc_val < 10) return -50.0; 

    // Calcul tensiune (Folosim 3.1V ca referință practică pentru ESP32 cu Gain 1/4)
    double Vout = (adc_val * 3.1) / 4095.0;

    // Calcul rezistență NTC (Presupunem: VCC -> R_fix (10k) -> Pin 39 -> NTC -> GND)
    // Dacă NTC e la VCC, formula se inversează!
    double Rth = NTC_R2 * (Vout / (VCC - Vout));
    
    if (Rth <= 0) return -99.0;

    double logRth = log(Rth);
    double temperature_k = 1.0 / (NTC_A + (NTC_B * logRth) + (NTC_C * pow(logRth, 3.0)));
    
    return temperature_k - 273.15;
}


int main(void)
{
    if (!device_is_ready(gpio_dev)) {
        LOG_ERR("Error: GPIO device not ready");
        return -1;
    }

    if (!device_is_ready(adc_dev)) {
        LOG_ERR("ADC device not ready");
        return -1;
    }

    /* Configure pin 23 as output and low */
    gpio_pin_configure(gpio_dev, MOC_PIN, GPIO_OUTPUT_INACTIVE);

    adc_channel_setup(adc_dev, &channel_cfg);

    LOG_INF("Zephyr system started on ESP32!");
    LOG_INF("Logging every second...");

    while (1)
    {
        /* LOG_INF prints the variable with a timestamp */
        double temp_F = read_ntc_temp();

        auto temp_c = (temp_F - 32) * (5.0 / 9.0); // Convertim în Celsius

        LOG_INF("Current temperature: %.2f °C", temp_c);

        gpio_pin_toggle(gpio_dev, MOC_PIN); /* Toggle pin to see the change on the oscilloscope */

        /* k_sleep suspends the current thread, allowing the processor to enter idle */
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
