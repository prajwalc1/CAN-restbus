/*
 * restbus.c
 *
 *  Created on: Sep 12, 2013
 *      Author: Prajwal
 */

/*
 * can_receive.c
 *
 *  Created on: May 2, 2013
 *      Author: Prajwal
 */

#include "can_config.h"

#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/fcntl.h>

#include <time.h>

#define VERSION "0.1"
#define BUF_SIZ 255

static void send_data(struct can_frame);
static void CAN_MSG_BCM_GeneralStatus(void);
static void CAN_MSG_CLUSTER_GeneralStatus(void);
static void CAN_MSG_ECM_GeneralStatus(void);
static void CAN_MSG_ADAS_SetUpInfo3(void);
static void CAN_MSG_ADAS_SetUpInfo1(void);
static void CAN_MSG_ADAS_ControlStatus_SR(void);
static void CAN_MSG_CLUSTER_BasicInfo_2(void);
static void CAN_MSG_BrakeSystemtoADAS2(void);
static void CAN_MSG_VehicleDynamic02(void);
static void CAN_MSG_Rear_Wheel_Speed_Frame(void);
static void CAN_MSG_Front_Wheel_Speed_Frame(void);
static void CAN_MSG_ADAS_Vehicle_Status(void);
static void CAN_MSG_LWSOUT_M2(void);

static int socketCAN0 = -1, socketCAN1 = -1;
static int running = 1;

struct itimerval old;
struct itimerval new;

struct can_frame BCM_GeneralStatus, CLUSTER_GeneralStatus, ECM_GeneralStatus,
    ADAS_SetUpInfo3, ADAS_SetUpInfo1, ADAS_ControlStatus_SR,
    CLUSTER_BasicInfo_2, BrakeSystemtoADAS2, VehicleDynamic02,
    Rear_Wheel_Speed_Frame, Front_Wheel_Speed_Frame, ADAS_Vehicle_Status,
    LWSOUT_M2;

void catch_alarm(int sig)
{
  static int counter = 0;

  /* every 10 ms */
  if (counter % 1 == 0)
  {
    send_data(LWSOUT_M2);
  }
  /* every 20ms */
  if (counter % 2 == 0)
  {
    send_data(ADAS_ControlStatus_SR);
    send_data(BrakeSystemtoADAS2);
    send_data(VehicleDynamic02);
    send_data(Rear_Wheel_Speed_Frame);
    send_data(Front_Wheel_Speed_Frame);
    send_data(ADAS_Vehicle_Status);
  }

  /* every 40ms */
  if (counter % 4 == 0)
  {
    send_data(CLUSTER_BasicInfo_2);
  }

  /* every 100 ms */
  if (counter % 10 == 0)
  {
    send_data(BCM_GeneralStatus);
    send_data(CLUSTER_GeneralStatus);
    send_data(ECM_GeneralStatus);
    send_data(ADAS_SetUpInfo3);
    send_data(ADAS_SetUpInfo1);
  }

  counter++;

  // signal(sig, catch_alarm);

}

int main(int argc, char **argv)
{
  struct ifreq ifr;
  struct sockaddr_can addr;
  char *interface = "can0";
  int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
  int error = 10;
  FILE *out = stdout;
  struct sockaddr_can addr1;
  struct sigaction sa;

  char *interface1 = "can1";

  // printf("interface = %s, family = %d, type = %d, proto = %d\n", interface, family, type, proto);

  //error = initCan();
  //fprintf(out, "%d\n", error);

  /* CAN0 */
  if ((socketCAN0 = socket(family, type, proto)) < 0)
  {
    perror("socket");
    return 1;
  }

  addr.can_family = family;
  strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
  if (ioctl(socketCAN0, SIOCGIFINDEX, &ifr))
  {
    perror("ioctl");
    return 1;
  }
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(socketCAN0, (struct sockaddr *) &addr, sizeof(addr)) < 0)
  {
    perror("bind");
    return 1;
  }

  /* CAN1 */
  if ((socketCAN1 = socket(family, type, proto)) < 0)
  {
    perror("socket");
    return 1;
  }

  addr1.can_family = family;
  strcpy(ifr.ifr_name, interface1);
  if (ioctl(socketCAN1, SIOCGIFINDEX, &ifr))
  {
    perror("ioctl");
    return 1;
  }
  addr1.can_ifindex = ifr.ifr_ifindex;

  if (bind(socketCAN1, (struct sockaddr *) &addr1, sizeof(addr)) < 0)
  {
    perror("bind");
    return 1;
  }

  CAN_MSG_BCM_GeneralStatus();
  CAN_MSG_CLUSTER_GeneralStatus();
  CAN_MSG_ECM_GeneralStatus();
  CAN_MSG_ADAS_SetUpInfo3();
  CAN_MSG_ADAS_SetUpInfo1();
  CAN_MSG_ADAS_ControlStatus_SR();
  CAN_MSG_CLUSTER_BasicInfo_2();
  CAN_MSG_BrakeSystemtoADAS2();
  CAN_MSG_VehicleDynamic02();
  CAN_MSG_Rear_Wheel_Speed_Frame();
  CAN_MSG_Front_Wheel_Speed_Frame();
  CAN_MSG_ADAS_Vehicle_Status();
  CAN_MSG_LWSOUT_M2();

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &catch_alarm;
  sigaction(SIGALRM, &sa, NULL );


  //signal(SIGALRM, catch_alarm);
  new.it_interval.tv_sec = 0;
  new.it_interval.tv_usec = 10000;
  new.it_value.tv_sec = 0;
  new.it_value.tv_usec = 10000;

  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;

  if (setitimer(ITIMER_REAL, &new, NULL ) < 0)
    printf("timer init failed\n");
  else
    printf("timer init succeeded\n");
  /*  while(1) sleep(2);
   return EXIT_SUCCESS;*/

  while (running)
  {

  }

  exit(EXIT_SUCCESS);
}

static void send_data(struct can_frame frame)
{
  int n = 0, err;
  int i;
  char buf[BUF_SIZ];
  FILE *out = stdout;
  int ret;

  if (frame.can_id & CAN_EFF_FLAG)
    n = snprintf(buf, BUF_SIZ, "<0x%08x> ", frame.can_id & CAN_EFF_MASK);
  else
    n = snprintf(buf, BUF_SIZ, "<0x%03X>  ", frame.can_id & CAN_SFF_MASK);

  n += snprintf(buf + n, BUF_SIZ - n, "[%d] ", frame.can_dlc);
  for (i = 0; i < frame.can_dlc; i++)
  {
    n += snprintf(buf + n, BUF_SIZ - n, "%02X ", frame.data[i]);
  }
  if (frame.can_id & CAN_RTR_FLAG)
    n += snprintf(buf + n, BUF_SIZ - n, "remote request");

  // fprintf(out, "%s\n", buf);

  retry: ret = write(socketCAN0, &frame, sizeof(frame));

  if (ret < 0)
  {
    if (errno == ENOBUFS)
    {
      sched_yield();
      goto retry;
    }
    else
    {
      perror("write failed");
    }
  }
  do
  {
    err = fflush(out);
    if (err == -1 && errno == EPIPE)
    {
      err = -EPIPE;
      fclose(out);
    }
  }
  while (err == -EPIPE);
}

static void CAN_MSG_BCM_GeneralStatus()
{
  BCM_GeneralStatus.can_id = 0x60D;
  BCM_GeneralStatus.can_dlc = 8;
  memset(BCM_GeneralStatus.data, 0, 8);
}

static void CAN_MSG_CLUSTER_GeneralStatus()
{
  CLUSTER_GeneralStatus.can_id = 0x5C5;
  CLUSTER_GeneralStatus.can_dlc = 8;
  memset(BCM_GeneralStatus.data, 0, 8);
  BCM_GeneralStatus.data[2] = 0x03;
  BCM_GeneralStatus.data[3] = 0xE8;
}

static void CAN_MSG_ECM_GeneralStatus()
{
  ECM_GeneralStatus.can_id = 0x551;
  ECM_GeneralStatus.can_dlc = 8;
  memset(ECM_GeneralStatus.data, 0, 8);
  ECM_GeneralStatus.data[3] = 0x80;
}

static void CAN_MSG_ADAS_SetUpInfo3()
{
  ADAS_SetUpInfo3.can_id = 0x4EC;
  ADAS_SetUpInfo3.can_dlc = 8;
  ADAS_SetUpInfo3.data[0] = 0xC1;
  ADAS_SetUpInfo3.data[1] = 0xB5;
  ADAS_SetUpInfo3.data[2] = 0x9E;
  ADAS_SetUpInfo3.data[3] = 0x3C;
  ADAS_SetUpInfo3.data[4] = 0x24;
  ADAS_SetUpInfo3.data[5] = 0x18;
  ADAS_SetUpInfo3.data[6] = 0x41;
  ADAS_SetUpInfo3.data[7] = 0x33;
}

static void CAN_MSG_ADAS_SetUpInfo1()
{
  ADAS_SetUpInfo1.can_id = 0x4EA;
  ADAS_SetUpInfo1.can_dlc = 8;
  ADAS_SetUpInfo1.data[0] = 0x00;
  ADAS_SetUpInfo1.data[1] = 0x0E;
  ADAS_SetUpInfo1.data[2] = 0x20;
  ADAS_SetUpInfo1.data[3] = 0x26;
  ADAS_SetUpInfo1.data[4] = 0x22;
  ADAS_SetUpInfo1.data[5] = 0x75;
  ADAS_SetUpInfo1.data[6] = 0x31;
  ADAS_SetUpInfo1.data[7] = 0xFC;
}

static void CAN_MSG_ADAS_ControlStatus_SR()
{
  ADAS_ControlStatus_SR.can_id = 0x3D0;
  ADAS_ControlStatus_SR.can_dlc = 6;
  memset(ADAS_ControlStatus_SR.data, 0, 6);
  ADAS_ControlStatus_SR.data[2] = 0x40;
}

static void CAN_MSG_CLUSTER_BasicInfo_2()
{
  CLUSTER_BasicInfo_2.can_id = 0x355;
  CLUSTER_BasicInfo_2.can_dlc = 8;
  memset(CLUSTER_BasicInfo_2.data, 0, 8);
  CLUSTER_BasicInfo_2.data[6] = 0x60;
}

static void CAN_MSG_BrakeSystemtoADAS2()
{
  BrakeSystemtoADAS2.can_id = 0x2AB;
  BrakeSystemtoADAS2.can_dlc = 8;
  memset(BrakeSystemtoADAS2.data, 0, 8);
  BrakeSystemtoADAS2.data[6] = 0x80;
}

static void CAN_MSG_VehicleDynamic02()
{
  VehicleDynamic02.can_id = 0x292;
  VehicleDynamic02.can_dlc = 8;
  memset(VehicleDynamic02.data, 0, 8);
  VehicleDynamic02.data[3] = 0x7F;
  VehicleDynamic02.data[4] = 0x70;
}

static void CAN_MSG_Rear_Wheel_Speed_Frame()
{
  Rear_Wheel_Speed_Frame.can_id = 0x285;
  Rear_Wheel_Speed_Frame.can_dlc = 8;
  Rear_Wheel_Speed_Frame.data[0] = 0x09;
  Rear_Wheel_Speed_Frame.data[1] = 0x60;
  Rear_Wheel_Speed_Frame.data[2] = 0x09;
  Rear_Wheel_Speed_Frame.data[3] = 0x60;
  Rear_Wheel_Speed_Frame.data[4] = 0x00;
  Rear_Wheel_Speed_Frame.data[5] = 0x00;
  Rear_Wheel_Speed_Frame.data[6] = 0xB2;
  Rear_Wheel_Speed_Frame.data[7] = 0x39;
}

static void CAN_MSG_Front_Wheel_Speed_Frame()
{
  Front_Wheel_Speed_Frame.can_id = 0x284;
  Front_Wheel_Speed_Frame.can_dlc = 8;
  Front_Wheel_Speed_Frame.data[0] = 0x09;
  Front_Wheel_Speed_Frame.data[1] = 0x60;
  Front_Wheel_Speed_Frame.data[2] = 0x09;
  Front_Wheel_Speed_Frame.data[3] = 0x60;
  Front_Wheel_Speed_Frame.data[4] = 0x00;
  Front_Wheel_Speed_Frame.data[5] = 0x00;
  Front_Wheel_Speed_Frame.data[6] = 0xB2;
  Front_Wheel_Speed_Frame.data[7] = 0x38;
}

static void CAN_MSG_ADAS_Vehicle_Status()
{
  ADAS_Vehicle_Status.can_id = 0x27A;
  ADAS_Vehicle_Status.can_dlc = 7;
  ADAS_Vehicle_Status.data[0] = 0x00;
  ADAS_Vehicle_Status.data[1] = 0x00;
  ADAS_Vehicle_Status.data[2] = 0x55;
  ADAS_Vehicle_Status.data[3] = 0x00;
  ADAS_Vehicle_Status.data[4] = 0x00;
  ADAS_Vehicle_Status.data[5] = 0x01;
  ADAS_Vehicle_Status.data[6] = 0xD2;
}

static void CAN_MSG_LWSOUT_M2()
{
  LWSOUT_M2.can_id = 0x002;
  LWSOUT_M2.can_dlc = 8;
  memset(LWSOUT_M2.data, 0, 8);
  LWSOUT_M2.data[3] = 0x07;
  LWSOUT_M2.data[4] = 0x61;
}
