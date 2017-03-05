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
  PROCESS(bs_config2_broadcast, "Broadcast bs2");
  AUTOSTART_PROCESSES(&bs_config2_broadcast);
  /*---------------------------------------------------------------------------*/

  bool anchor_is_set[NB_ANCHOR] = { 0 };
  int rssi_vals[NB_ANCHOR];   

  // time stamp
  int counter = 1;
  static void
  broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
  {    

    // get anchor name and the rssi value embedded in its message
    char * msg_recv = (char *) packetbuf_dataptr();
    printf("%s\n", msg_recv);
    
    // get anchor name
    char anchor_name = msg_recv[0];
    printf("char anchor name = %c\n", anchor_name);
    
    // get rssi value embedded
    char rssi_val[4];
    memcpy(rssi_val, &msg_recv[1], 3);
    rssi_val[3] = '\0';
    int rssi = atoi(rssi_val);

    // suppose that anchor name starts from A, B, C,....
    // registering its id and store to corresponding elem in rssi array
    int anchor_id = anchor_name - 'A';
    anchor_is_set[anchor_id] = true;
    rssi_vals[anchor_id] = rssi;

    // print out once obtain a complete data
    bool all_set = true;
    for (int i = 0; i < NB_ANCHOR; ++i)
    {
      if (anchor_is_set[i] == false) 
        {
          all_set = false; 
          break;
        }
    }
    if (all_set)
    {
      printf("%d,%d,%d,%d,#\n",counter++, rssi_vals[0], rssi_vals[1], rssi_vals[2]); // increase time stamp after each complete data
      for (int i = 0; i < NB_ANCHOR; ++i)
      {
        // reset to obtain next data
        anchor_is_set[i] = false;
      }
    }

    printf("received from anchor %c rssi = %d\n", anchor_name, rssi);

  }
  static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
  static struct broadcast_conn broadcast;
  /*---------------------------------------------------------------------------*/
  PROCESS_THREAD(bs_config2_broadcast, ev, data)
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

      // in configuration 2: 
      // base station sends message indicating that it's bs (i.e. target in our context)
      packetbuf_copyfrom("bs", 6);
      broadcast_send(&broadcast);      
    }

    PROCESS_END();
  }

