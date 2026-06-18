#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/sensor.h>       // Pentru MAX6675
#include <zephyr/display/cfb.h>          // Pentru Ecran text SSD1306
#include <math.h>
#include <stdio.h>                       // Pentru snprintf
#include <stdlib.h>                      // Pentru strtod

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Preluare periferice hardware direct din nodurile Overlay-ului */
static const struct gpio_dt_spec moc_gpio = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), moc_gpios);
static const struct device *adc_dev       = DEVICE_DT_GET(DT_NODELABEL(adc1));
static const struct device *max6675_dev   = DEVICE_DT_GET(DT_NODELABEL(max6675));
static const struct device *display_dev   = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

static const struct adc_channel_cfg channel_cfg = {
    .gain             = ADC_GAIN_1_4,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = CONFIG_THERMAL_ADC_CHANNEL,
    .differential     = 0
};

// Citire NTC folosind constantele din Kconfig convertite explicit la double
double read_ntc_temp(void) {
    int16_t buf = 0;
    struct adc_sequence sequence = {
        .channels    = BIT(CONFIG_THERMAL_ADC_CHANNEL),
        .buffer      = &buf,
        .buffer_size = sizeof(buf),
        .resolution  = 12,
    };

    if (adc_read(adc_dev, &sequence) < 0) return -273.15;

    double adc_val = static_cast<double>(buf);
    if (adc_val < 10.0 || adc_val > 4085.0) return -50.0;

    // Conversie string text din Kconfig -> double numeric
    double ntc_r2 = strtod(CONFIG_THERMAL_NTC_RESISTOR_FIXED, NULL);
    double ntc_a  = strtod(CONFIG_THERMAL_NTC_COEFF_A, NULL);
    double ntc_b  = strtod(CONFIG_THERMAL_NTC_COEFF_B, NULL);
    double ntc_c  = strtod(CONFIG_THERMAL_NTC_COEFF_C, NULL);

    double Rth = ntc_r2 * (adc_val / (4095.0 - adc_val));
    double logRth = log(Rth);
    double temperature_k = 1.0 / (ntc_a + (ntc_b * logRth) + (ntc_c * pow(logRth, 3.0)));
    
    return temperature_k - 273.15;
}

int main(void)
{
    // 1. Verificare dispozitive hardware
    if (!gpio_is_ready_dt(&moc_gpio) || !device_is_ready(adc_dev) || 
        !device_is_ready(max6675_dev) || !device_is_ready(display_dev)) {
        LOG_ERR("Hardware initialization failed!");
        return -1;
    }

    // 2. Inițializare periferice
    gpio_pin_configure_dt(&moc_gpio, GPIO_OUTPUT_INACTIVE);
    adc_channel_setup(adc_dev, &channel_cfg);

    // 3. Inițializare Manager Ecran
    cfb_framebuffer_init(display_dev);
    cfb_framebuffer_clear(display_dev, true);
    
    // În Zephyr v4.2.0 API-ul folosește set_font
    cfb_framebuffer_set_font(display_dev, 0); 

    LOG_INF("All drivers initialized successfully!");

    while (1)
    {
        // --- CITIRE MAX6675 (Canal corect Zephyr v4: SENSOR_CHAN_AMBIENT_TEMP) ---
        struct sensor_value chamber_temp;
        sensor_sample_fetch(max6675_dev);
        sensor_channel_get(max6675_dev, SENSOR_CHAN_AMBIENT_TEMP, &chamber_temp);
        double t_chamber = sensor_value_to_double(&chamber_temp);

        // --- CITIRE NTC ---
        double t_ambient = read_ntc_temp();

        // --- AFIȘARE PE LOGS ---
        LOG_INF("Chamber: %.2f *C | Ambient: %.2f *C", t_chamber, t_ambient);

        // --- AFIȘARE PE OLED ---
        char buffer[32];
        cfb_framebuffer_clear(display_dev, false);
        
        snprintf(buffer, sizeof(buffer), "Chamber: %.1f C", t_chamber);
        cfb_print(display_dev, buffer, 0, 0);
        
        snprintf(buffer, sizeof(buffer), "Ambient: %.1f C", t_ambient);
        cfb_print(display_dev, buffer, 0, 16);
        
        cfb_framebuffer_finalize(display_dev);

        // Controlul de test pe osciloscop
        gpio_pin_toggle_dt(&moc_gpio);
        k_msleep(1000);
    }
    return 0;
}