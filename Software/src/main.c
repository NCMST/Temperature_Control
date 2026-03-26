#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* Înregistrăm modulul de logging pentru acest fișier */
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    uint32_t counter = 0;

    LOG_INF("Sistemul Zephyr a pornit pe ESP32!");
    LOG_INF("Incepem numaratoarea cu delay de 1 secunda...");

    while (1) {
        /* printk este varianta simplă, LOG_INF este varianta cu timestamp și culori */
        printk("Hello World din Zephyr! Iteratia: %u\n", counter++);

        /* k_sleep suspendă thread-ul curent, permițând procesorului să intre în idle */
        k_sleep(K_SECONDS(1));
    }

    return 0;
}