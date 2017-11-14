//
// Created by Dharma Naidu on 11/13/17.
//

#include <stdio.h>
#include <mraa.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <getopt.h>
#include <poll.h>
#include <stdint.h>
#include <stdlib.h>


static struct option long_options[]={
        {"period",required_argument,NULL,'p'},
        {"scale",required_argument,NULL,'s'},
        {"log",required_argument,NULL,'l'},
        {0,0,0,0}
};

int period = 1;
char tempType = 'F';
char *logfile = "";
const int R0 = 100000;
const int B=4275;
sig_atomic_t volatile run_flag=1;
void do_when_interrupted(int sig)
{
    if (sig==SIGINT)
        run_flag=0;
}
int test_temp(){
    mraa_aio_context temperature;
    temperature=mraa_aio_init(1);

    //mraa_aio_dir(temperature,MRAA_AIO_OUT);
    signal(SIGINT,do_when_interrupted);
    struct pollfd poll_array[1];
    poll_array[0].fd = STDIN_FILENO;
    poll_array[0].events = POLLIN | POLLHUP | POLLERR;

    while(run_flag){
        int r = poll(poll_array, 1, 0);
        if (r < 0)
            exit(1);
        if (poll_array[0].revents & POLLIN) //input from temp sensor
        {
            printf("input from stdin ");
            sleep(5);
        }

        /*uint16_t temp;
        temp = mraa_aio_read(temperature);
        float R = 625.0 / temp - 1.0;
        R = R0 * R;
        double log_thing = log(R / R0);
        float real_temp = 1.0 / (log_thing / B + 1 / 298.15) - 273.15; //temperature conversion

        printf("temperature:%f \n", real_temp);
        sleep(1);*/
    }
    mraa_aio_close(temperature);
    return 0;
}
int main(int argc, char *argv[])
{
    int *opt_index = NULL;
    int opt = getopt_long(argc, argv, "", long_options, opt_index);

    while (opt != -1) {
        switch (opt) {
            case 'p' :
                period = atoi(optarg);
                break;
            case 'l':
                logfile = optarg;
                break;
            case 's' :
                tempType = optarg[0];
                break;
            case '?':
                fprintf(stderr, "Unrecognized argument, correct usage is \n");
                fprintf(stderr, "--period=INTERVAL --log=LOGFILE");
                exit(1);
        }
        opt = getopt_long(argc, argv, "", long_options, opt_index);
    }
    test_temp();
    /*
    mraa_gpio_context buzzer;
    buzzer = mraa_gpio_init(62);
    mraa_gpio_dir(buzzer, MRAA_GPIO_OUT);
    signal(SIGINT, do_when_interrupted);
    while(run_flag){
        mraa_gpio_write(buzzer, 1);
        sleep(1);
        mraa_gpio_write(buzzer, 0);
        sleep(1);
    }
    mraa_gpio_write(buzzer, 0);
    mraa_gpio_close(buzzer);
    */
    printf("MRAA version: %s\n",mraa_get_version());
    return 0;
}
