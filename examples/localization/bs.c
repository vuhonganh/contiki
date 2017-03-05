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
  PROCESS(bs_config1_broadcast, "Broadcast bs1");
  AUTOSTART_PROCESSES(&bs_config1_broadcast);
  /*---------------------------------------------------------------------------*/

  // buffer to help register and remember which anchor is set (in configuration 1)
  int anchor_ids[NB_ANCHOR] = { 0 };
  int rssi_vals[NB_ANCHOR];
  bool anchor_is_set[NB_ANCHOR] = { 0 };  // at the beginning no anchor is set
  
  // time stamp
  int counter = 1;
  
  static void
  broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
  {
    // rssi is negative because Power < 1W,
    // hence need to wrap around because packetbuf_attr is encoded as 16 bit unsigned int
    int rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI) - 65535;
    
    // in configuration 1: base station receives id of anchor then registers and remembers it
    int id_received = atoi(((char *)packetbuf_dataptr()));
    
    // REGISTERING
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
    if (all_set)  // print out only get a complete rssi from 3 anchors
    {
      printf("%d,%d,%d,%d,#\n", counter, rssi_vals[0], rssi_vals[1], rssi_vals[2]);
      for (int i = 0; i < NB_ANCHOR; ++i)
      {
        // reset to obtain next data
        anchor_is_set[i] = false;
      }
      counter++; // increase time stamp after each complete data
    }

  }
  static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
  static struct broadcast_conn broadcast;
  /*---------------------------------------------------------------------------*/
  PROCESS_THREAD(bs_config1_broadcast, ev, data)
  {

    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

    PROCESS_BEGIN();

    broadcast_open(&broadcast, 140, &broadcast_call);

    while(1) {

      /* Delay 0.5s */
      etimer_set(&et, CLOCK_SECOND/2);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      
      // base station blink all LEDs
      leds_on(LEDS_ALL);
      etimer_set(&et, CLOCK_SECOND/32);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
      leds_off(LEDS_ALL);      
    }
    PROCESS_END();
  }
