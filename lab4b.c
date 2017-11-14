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
#include <pthread.h>


static struct option long_options[]={
        {"period",required_argument,NULL,'p'},
        {"scale",required_argument,NULL,'s'},
        {"log",required_argument,NULL,'l'},
        {0,0,0,0}
};

int period = 1;
int running = 1;
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
int thread_function()
{
    mraa_aio_context temperature;
    temperature=mraa_aio_init(1);
    time_t clock;
    struct tm* current_time;
    char time_storage[9];
    //mraa_aio_dir(temperature,MRAA_AIO_OUT);
    signal(SIGINT,do_when_interrupted);

    while(run_flag)
    {

        uint16_t temp;
        temp = mraa_aio_read(temperature);
        float R = 625.0 / temp - 1.0;
        R = R0 * R;
        float real_temp = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15; //temperature conversion
        if(tempType == 'C')
            real_temp = (real_temp - 32) * 5 / 9;
        time(&clock);
        current_time = localtime(&clock);
        strftime(time_storage, 9, "%H:%M:%S",current_time);
        if(running)
            printf("%s %.1f \n",time_storage,real_temp);
        sleep(period);
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
    pthread_t printThread;
    if (pthread_create(&printThread, NULL, (void *)thread_function, NULL) < 0){
        perror("error creating thread");
        exit(1);
    }

    //variables for read and write
    char buffer[2048];
    int count = 0;

    struct pollfd poll_list[1];
    poll_list[0].fd = STDIN_FILENO;
    poll_list[0].events = POLLIN|POLLHUP|POLLERR;


    char cr = 13;
    char lf = 10;

    int command_index = 0;
    char command[1024];

    while(true)
    {
        int retval = poll(poll_list,(unsigned long)1,0);
        if(retval < 0)
        {
            fprintf(stderr,"Polling Error\n");
        }

        if((poll_list[0].revents&POLLIN) == POLLIN)
        {
            count = read(STDIN_FILENO, buffer, 2048);
            int i;
            for(i = 0; i < count; i++)
            {
                if(buffer[i] == 3)
                {
                    exit(1);
                }
                else if (buffer[i] == 10 || buffer[i] == 13)
                {
                    command[command_index]='\0';
                    printf("RECEIVED: %s",command);
                    if (strcmp(command,"OFF")==0){
                        printf("TURNING OFF NOW!\n");
                        exit(0);
                    }
                    else if (strcmp(command,"STOP")==0){
                        running=0;
                        printf("STOP\n");
                    }
                    else if (strcmp(command,"START")==0){
                        running=1;
                        printf("START\n");
                    }
                    else if (strcmp(command,"SCALE=F")==0){
                        tempType='F';
                    }
                    else if (strcmp(command,"SCALE=C")==0){
                        tempType='C';
                    }
                    strcpy(command,"");
                    command_index=0;
                }
                else
                {
                    command[command_index]=buffer[i];
                    command_index+=1;
                }
            }
        }
    }
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
