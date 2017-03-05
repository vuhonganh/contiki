#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(anchor_config2_broadcast, "Broadcast anchor2");
AUTOSTART_PROCESSES(&anchor_config2_broadcast);
/*---------------------------------------------------------------------------*/

int rssi = 0; // to store rssi comming from base station

static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  // In network configuration 2: 
  // if coming message is from base station, anchor captures rssi value to send back
  char * msg_recv = (char *)packetbuf_dataptr();
  printf("anchor received msg %s\n", msg_recv);
  if (strcmp(msg_recv, "bs") == 0)
  {
    // rssi is negative because Power < 1W,
    // hence need to wrap around because packetbuf_attr is encoded as 16 bit unsigned int
    rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI) - 65535;  
  }
  
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(anchor_config2_broadcast, ev, data)
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

    // anchor blinks YELLOW led before broadcast
    leds_on(LEDS_YELLOW);
    etimer_set(&et, CLOCK_SECOND/64);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
    leds_off(LEDS_YELLOW);

    
    // anchor embeds rssi to packet to send back
    char str_rssi[8];
    sprintf(str_rssi, "%d", rssi);
    
    // anchor denodes its name (which can be rime address like in configuration 1
    // however we chose to do this in order to quickly test the RSSI behavior firstly)
    
    // each anchor has a name
    // char anchor_str[8] = "A";
    // char anchor_str[8] = "B";
    char anchor_str[8] = "C";
    
    // concatenate and send as string message
    strcat(anchor_str, str_rssi);

    packetbuf_copyfrom(anchor_str, 6);
    broadcast_send(&broadcast);
    
    // printing command for debugging and testing purpose
    printf("msg sent is: %s\n", anchor_str);    
  }

  PROCESS_END();
}
