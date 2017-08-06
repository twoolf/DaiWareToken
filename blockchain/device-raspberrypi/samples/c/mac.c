/*******************************************************************************
* Copyright (c) 2014 IBM Corporation and other Contributors.
*
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*
* Contributors:
*   Jeffrey Dare - Initial Contribution
*******************************************************************************/

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if.h>

#define MAC_STRING_LENGTH 13

char *getmac(char *iface)
{
  char *ret = malloc(MAC_STRING_LENGTH);
  struct ifreq s;
  
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  strcpy(s.ifr_name, iface);
  if (fd >= 0 && ret && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
  {
    int i;
    for (i = 0; i < 6; ++i)
      snprintf(ret+i*2,MAC_STRING_LENGTH-i*2,"%02x",(unsigned char) s.ifr_addr.sa_data[i]);
  }
  else
  {
    perror("malloc/socket/ioctl failed");
    exit(1);
  }
  return(ret);
}
