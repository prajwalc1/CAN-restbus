/*
 * can_config.c
 *
 *  Created on: Sep 13, 2013
 *      Author: benny
 */

#include <can_netlink.h>
#include <libsocketcan.h>

int initCan(/*podboxconfig_t *sysCfg*/)
{
  int retVal;
  struct can_ctrlmode cm;

  memset(&cm, 0, sizeof(cm));

  can_do_stop("can0");    // error does not matter

  retVal = can_set_bitrate("can0", 500000);

  cm.mask = CAN_CTRLMODE_BERR_REPORTING | CAN_CTRLMODE_ONE_SHOT | CAN_CTRLMODE_LOOPBACK | CAN_CTRLMODE_LISTENONLY | CAN_CTRLMODE_3_SAMPLES;
  cm.flags =  CAN_CTRLMODE_3_SAMPLES;

  if (can_set_ctrlmode("can0", &cm) < 0)
    return 1;

  if(can_do_start("can0") < 0)
    return 1;

  can_do_stop("can1");    // error does not matter

  retVal = can_set_bitrate("can1", 500000);

  if (can_set_ctrlmode("can1", &cm) < 0)
    return 1;

  if(can_do_start("can1") < 0)
    return 1;

  return 0;
}
