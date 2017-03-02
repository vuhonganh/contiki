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
  #include <stdlib.h>
  #include "node-id.h"
  
  #define NB_ANCHOR 3

  /*---------------------------------------------------------------------------*/
  PROCESS(example_broadcast_process, "Broadcast example");
  AUTOSTART_PROCESSES(&example_broadcast_process);
  /*---------------------------------------------------------------------------*/

  //int nb_anchor = 3;
  // int anchor_ids[3] = {33877, 400, 32833};
  // int anchor_ids[3] = {403, 1184, 1726};
  int anchor_ids[NB_ANCHOR] = { 0 };
  int rssi_vals[NB_ANCHOR];
  bool anchor_is_set[NB_ANCHOR] = { 0 };
  int counter = 1;
  static void
  broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
  {
    /* original comment
    printf("broadcast message received from %d.%d: '%s'\n",
  	 from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
    */

    //packetbuf_attr_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
    int rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI) - 65535;
    // printf("%d,%d,%d,%d,#\n", counter, rssi, rssi-2, rssi-4);
    // counter++;
    //return;
    // printf("broadcast message received from %d.%d: '%s', %d  \n",
    //         from->u8[0], from->u8[1], (char *)packetbuf_dataptr(), 
    //         rssi);
    int id_received = atoi(((char *)packetbuf_dataptr()));
    // printf("id received is %d \n", id_received);
    // printf("%d%d: '%s', %d  \n",
    //         from->u8[0], from->u8[1], (char *)packetbuf_dataptr(),rssi);

    // START HARD CODED PART

    // int idx = 0;
    // for (int i = 0; i < nb_anchor; ++i)
    // {
    //   if (id_received == anchor_ids[i])
    //   {
    //     idx = i;        
    //     break;
    //   }
    // }
    // rssi_vals[idx] = rssi;
    // anchor_is_set[idx] = true;

    // int nb_node_set = 0;
    // for (int i = 0; i < nb_anchor; ++i)
    // {
    //   if (anchor_is_set[i] == true)
    //   {
    //     nb_node_set++;
    //   }
    // }

    // printf("nb node set so far: %d\n", nb_node_set);
    // if (nb_node_set == 3)
    // {
    //   printf("a tuple is ready below:\n");
    //   printf("%d %d %d\n", rssi_vals[0], rssi_vals[1], rssi_vals[2]);
    //   for (int i = 0; i < nb_anchor; ++i)
    //   {
    //     anchor_is_set[i] = false;
    //   }
    // }

    // END HARD CODED PART

    for (int i = 0; i < NB_ANCHOR; ++i)
    {
      
      if (anchor_ids[i] == 0)  // first time running
      {
        bool set_to_previous = false;
        for (int j = 0; j < i; ++j)
        {
          if (anchor_ids[j] == id_received)
          {
            set_to_previous = true;
          }
        }
        if (set_to_previous == false)
        {
          anchor_ids[i] = id_received;
          rssi_vals[i] = rssi;
          anchor_is_set[i] = true;
          printf("node %d (rime addr = %d) is set\n", i, id_received);
          //printf("break\n");
          break;  
        }        
      }
      else if (anchor_ids[i] == id_received)  // update with the latest rssi value
      {
        rssi_vals[i] = rssi;
        anchor_is_set[i] = true;
      }
    }
    bool all_set = true;
    int nb_node_set = 0;
    for (int i = 0; i < NB_ANCHOR; ++i)
    {
      if (anchor_is_set[i] == false)
      {
        all_set = false;
      }
      else
      {
        nb_node_set++;
      }
    }
    // printf("nb of node set so far is %d\n", nb_node_set);
    if (all_set)
    {
      // printf("a tuple is ready below:\n");
      printf("%d,%d,%d,%d,#\n", counter, rssi_vals[0], rssi_vals[1], rssi_vals[2]);
      for (int i = 0; i < NB_ANCHOR; ++i)
      {
        anchor_is_set[i] = false;
      }
      counter++;
    }

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

    while(1) {

      /* Delay 0.5s */
      etimer_set(&et, CLOCK_SECOND/2);

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      leds_on(LEDS_ALL);
      etimer_set(&et, CLOCK_SECOND/32);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
      leds_off(LEDS_ALL);
      
      //packetbuf_copyfrom("bs", 6);
      //broadcast_send(&broadcast);
      //printf("Im bs. My rime addr is %d \n", linkaddr_node_addr);
      //printf("broadcast message sent\n");
      //printf("my node id is: %d\n", node_id);  //only use for cooja
    }

    PROCESS_END();
  }
  /*---------------------------------------------------------------------------*/
