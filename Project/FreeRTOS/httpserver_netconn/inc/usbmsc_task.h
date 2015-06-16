#ifndef __usbmsc_task__
#define __usbmsc_task__

#include  <stdint.h>
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;

void USB_MSC_Init(void);

#endif

