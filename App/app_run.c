#include "app_runner.h"

#include <signal.h>
#include <assert.h>

static SerialDevices device;

static void app_runner_stop(int sig)
{
    assert(sig == SIGINT || sig == SIGTERM);
    app_router_close();
    app_task_close();
}

int app_runner_run()
{
    signal(SIGINT,app_runner_stop);
    signal(SIGTERM,app_runner_stop);

    app_task_init(5);

    app_serial_init(&device,"/dev/ttyS2");
    app_bluetooth_setConnectionType(&device);

    app_router_init();

    app_router_registerDevice((Device *)&device);

    app_task_wait();

    return 0;
}
