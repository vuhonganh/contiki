/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
int reads[10];
int cnt = 0;
int avg = 0;
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  /* original comment
  printf("broadcast message received from %d.%d: '%s'\n",
	 from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
  */

  //packetbuf_attr_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  // int rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI) - 65535;
  // printf("%d\n", rssi);
  // printf("broadcast message received from %d.%d: '%s', %d  \n",
  //         from->u8[0], from->u8[1], (char *)packetbuf_dataptr(), 
  //         rssi);


}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 140, &broadcast_call);
  // printf("At start: Im anchor. My rime addr is %d \n", linkaddr_node_addr);
  // leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);
  // leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);
  // leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);
  while(1) 
  {
    // leds_on(CC26XX_DEMO_LEDS_REBOOT);
    // leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);

    /* Delay 2-4 seconds */  //below is not true 
    //etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
    
    /* Delay 1s */ //below is not true
    // etimer_set(&et, CLOCK_SECOND + random_rand() % (2 * CLOCK_SECOND));

    // delay 0.5 - 1.5 second
    etimer_set(&et, (CLOCK_SECOND + (random_rand() % 3) * CLOCK_SECOND)/2);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_on(LEDS_YELLOW);
    etimer_set(&et, CLOCK_SECOND/64);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
    leds_off(LEDS_YELLOW);

    //int anchor_addr;
    // anchor_addr = (int) linkaddr_node_addr;
    
    // packetbuf_copyfrom("anchor", 6);
    char str[15];
    sprintf(str, "%d", linkaddr_node_addr);

    packetbuf_copyfrom(str, 6);
    broadcast_send(&broadcast);
    //printf("Im anchor. My rime addr is %d \n", linkaddr_node_addr);
    //printf("broadcast message sent\n");
    //printf("my node id is: %d\n", node_id);  //only use for cooja
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
