#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(anchor_config1_broadcast, "Broadcast anchor1");
AUTOSTART_PROCESSES(&anchor_config1_broadcast);
/*---------------------------------------------------------------------------*/
int reads[10];
int cnt = 0;
int avg = 0;
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  // in configuration 1: anchors  do not need to do anything because there is no coming message
  // its task is only to broadcast periodically for the target to capture rssi from it.
  // we keep some printing command only for debugging and testing purpose
  char * msg_recv = (char *)packetbuf_dataptr();
  printf("anchor received msg %s\n", msg_recv);
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(anchor_config1_broadcast, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 140, &broadcast_call);
  
  while(1) 
  {
  
    // delay 0.5 - 1.5 second
    etimer_set(&et, (CLOCK_SECOND + (random_rand() % 3) * CLOCK_SECOND)/2);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    // anchor blinks YELLOW led before each broadcast
    leds_on(LEDS_YELLOW);
    etimer_set(&et, CLOCK_SECOND/64);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
    leds_off(LEDS_YELLOW);

    
    // anchor broadcast its id
    char str[15];
    sprintf(str, "%d", linkaddr_node_addr);
    packetbuf_copyfrom(str, 6);
    broadcast_send(&broadcast);
    
  }

  PROCESS_END();
}
