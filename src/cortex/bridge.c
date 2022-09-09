#include "bridge.h"
#include "defaults.c"
#include <stdio.h>
#include <stdlib.h>

void bridge_init(void)
{

    defaults.prices_arrived = 1;
}

void bridge_free(void)
{
}

void bridge_buy(char* item)
{

    defaults.donation_arrived = 1;
    defaults_save();
    bridge_open("https://paypal.me/milgra");
}

void bridge_open(char* url)
{

    char newurl[100];
    snprintf(newurl, 100, "xdg-open %s", url);
    system(newurl);
}
