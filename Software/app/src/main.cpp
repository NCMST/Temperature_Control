#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <math.h>

#define SLEEP_TIME_MS       1000
#define ADC_NODE            DT_NODELABEL(adc1)

/* Preluarea pinilor în mod nativ din Device Tree (nodul zephyr,user) */
static const struct gpio_dt_spec moc_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), moc_gpios);
static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define ADC_CHANNEL         3
#define ADC_RESOLUTION      12
#define ADC_GAIN            ADC_GAIN_1_4
#define ADC_REFERENCE       ADC_REF_INTERNAL

/* Constante NTC */
#define NTC_A               0.001129148
#define NTC_B               0.000234125
#define NTC_C               0.0000000876741
#define NTC_R2              9990.0  /* Rezistenta fixa de 10k */

static const struct adc_channel_cfg channel_cfg = {
    .gain             = ADC_GAIN,
    .reference        = ADC_REFERENCE,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = ADC_CHANNEL,
    .differential     = 0
};

double read_ntc_temp(void) {
    int16_t buf = 0;
    
    struct adc_sequence sequence = {
        .channels    = BIT(ADC_CHANNEL),
        .buffer      = &buf,
        .buffer_size = sizeof(buf),
        .resolution  = ADC_RESOLUTION,
    };

    int err = adc_read(adc_dev, &sequence);
    if (err < 0) {
        LOG_ERR("ADC read failed: %d", err);
        return -273.15;
    }

    double adc_val = static_cast<double>(buf);
    
    /* Protectie impotriva divizarii la 0 sau valori aberante */
    if (adc_val < 10.0 || adc_val > 4085.0) {
        return -50.0; 
    }

    /* Calcul rezistenta direct din rapoarte brute ADC, eliminand erorile de VCC */
    double Rth = NTC_R2 * (adc_val / (4095.0 - adc_val));

    double logRth = log(Rth);
    double temperature_k = 1.0 / (NTC_A + (NTC_B * logRth) + (NTC_C * pow(logRth, 3.0)));
    
    return temperature_k - 273.15;
}

int main(void)
{
    /* Verificare conformitate structura GPIO */
    if (!gpio_is_ready_dt(&moc_gpio)) {
        LOG_ERR("Error: MOC GPIO device not ready");
        return -1;
    }

    if (!device_is_ready(adc_dev)) {
        LOG_ERR("ADC device not ready");
        return -1;
    }

    /* Configurare pin prin structura sa nativa */
    gpio_pin_configure_dt(&moc_gpio, GPIO_OUTPUT_INACTIVE);

    adc_channel_setup(adc_dev, &channel_cfg);

    LOG_INF("Zephyr system started on ESP32!");

    while (1)
    {
        double temp_c = read_ntc_temp();
        LOG_INF("Current temperature: %.2f *C", temp_c);

        /* Toggle pin utilizand structura de date spec */
        gpio_pin_toggle_dt(&moc_gpio);

        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}