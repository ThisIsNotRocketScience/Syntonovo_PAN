/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"

#include "usb_device_descriptor.h"

/*******************************************************************************
* Variables
******************************************************************************/

/* Define endpoint for data class */
usb_device_endpoint_struct_t g_UsbDeviceMidiStreamingEndpoints[USB_CDC_VCOM_ENDPOINT_DIC_COUNT] = {
    {
        USB_CDC_VCOM_BULK_IN_ENDPOINT | (USB_IN << 7U), USB_ENDPOINT_BULK, FS_CDC_VCOM_BULK_IN_PACKET_SIZE,
    },
    {
        USB_CDC_VCOM_BULK_OUT_ENDPOINT | (USB_OUT << 7U), USB_ENDPOINT_BULK, FS_CDC_VCOM_BULK_OUT_PACKET_SIZE,
    }};

usb_device_interface_struct_t g_UsbDeviceMidiStreamingInterface[] = {
    {0,
     {
         USB_CDC_VCOM_ENDPOINT_DIC_COUNT, g_UsbDeviceMidiStreamingEndpoints,
     },
     NULL}};

#define USB_MIDISTREAMING_INTERFACE_COUNT (1)

usb_device_interfaces_struct_t g_UsbDeviceMidiStreamingInterfaces[USB_MIDISTREAMING_INTERFACE_COUNT] = {
    {USB_CDC_VCOM_DIC_CLASS, USB_CDC_VCOM_DIC_SUBCLASS, USB_CDC_VCOM_DIC_PROTOCOL, 0,
     g_UsbDeviceMidiStreamingInterface, sizeof(g_UsbDeviceMidiStreamingInterface) / sizeof(usb_device_interfaces_struct_t)},
};

/* Define configurations for virtual com */
usb_device_interface_list_t g_UsbDeviceMidiStreamingInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
    		USB_MIDISTREAMING_INTERFACE_COUNT, g_UsbDeviceMidiStreamingInterfaces,
    },
};

/* Define class information for virtual com */
usb_device_class_struct_t g_UsbDeviceMidiStreamingConfig = {
		g_UsbDeviceMidiStreamingInterfaceList, kUSB_DeviceClassTypeAudio, USB_DEVICE_CONFIGURATION_COUNT,
};

/* Define device descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceDescriptor[USB_DESCRIPTOR_LENGTH_DEVICE] = {
    /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_LENGTH_DEVICE,
    /* DEVICE Descriptor Type */
    USB_DESCRIPTOR_TYPE_DEVICE,
    /* USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION), USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION),
    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_CLASS,
    /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,
    /* Protocol code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,
    /* Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid) */
    USB_CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID (assigned by the USB-IF) */
    0xC0U, 0x16U,
    /* Product ID (assigned by the manufacturer) */
    0xE4U, 0x05U,
    /* Device release number in binary-coded decimal */
    USB_SHORT_GET_LOW(1), USB_SHORT_GET_HIGH(1),
    /* Index of string descriptor describing manufacturer */
    0x01,
    /* Index of string descriptor describing product */
    0x02,
    /* Index of string descriptor describing the device's serial number */
    0x00,
    /* Number of possible configurations */
    USB_DEVICE_CONFIGURATION_COUNT,
};

#define DEVICE_CONFIG_DESCRIPTOR_SIZE (USB_DESCRIPTOR_LENGTH_CONFIGURE + USB_DESCRIPTOR_LENGTH_INTERFACE + 9 + USB_DESCRIPTOR_LENGTH_INTERFACE + 7 + 1*(6+6+9+9)+9+5+9+5)
#define AUDIO_CLASS (1)
#define AUDIO_CONTROL_SUBCLASS (1)
#define IFACE_ONLY_DESC_SIZE              (9)
#define USB_CS_INTERFACE          (0x24)
#define USB_IFACE_DESCRIPTOR      (4)
#define MIDISTREAMING_SUBCLASS                 (0x03)
#define USB_CS_ENDPOINT           (0x25)

#define  USB_BULK_PIPE                       (0x02)
#define USB_ENDPOINT_DESCRIPTOR   (5)

/* MS Class-Specfic Interface Descriptor Subtypes */
#define MS_DESCRIPTOR_UNDEFINED                 (0x00)
#define MS_HEADER                                               (0x01)
#define MIDI_IN_JACK                                    (0x02)
#define MIDI_OUT_JACK                                   (0x03)
#define ELEMENT                                                 (0x04)

/* MS Class-Specific Endpoint Descriptor Subtypes */
#define DESCRIPTOR_UNDEFINED                    (0x00)
#define MS_GENERAL                                              (0x01)

/* MS MIDI IN and OUT Jack types */
#define JACK_TYPE_UNDEFINED                             (0x00)
#define EMBEDDED                                                (0x01)
#define EXTERNAL                                                (0x02)

/* Define configuration descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceConfigurationDescriptor[DEVICE_CONFIG_DESCRIPTOR_SIZE] = {
    /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_LENGTH_CONFIGURE,
    /* CONFIGURATION Descriptor Type */
    USB_DESCRIPTOR_TYPE_CONFIGURE,
    /* Total length of data returned for this configuration. */
    USB_SHORT_GET_LOW(DEVICE_CONFIG_DESCRIPTOR_SIZE),
    USB_SHORT_GET_HIGH(DEVICE_CONFIG_DESCRIPTOR_SIZE),
    /* Number of interfaces supported by this configuration */
    2,
    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
    1,
    /* Index of string descriptor describing this configuration */
    0,
    /* Configuration characteristics D7: Reserved (set to one) D6: Self-powered D5: Remote Wakeup D4...0: Reserved
       (reset to zero) */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK) |
        (USB_DEVICE_CONFIG_SELF_POWER << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT),
    /* Maximum power consumption of the USB * device from the bus in this specific * configuration when the device is
       fully * operational. Expressed in 2 mA units *  (i.e., 50 = 100 mA).  */
    USB_DEVICE_MAX_POWER,

    /* AUDIOCONTROL Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE, USB_DESCRIPTOR_TYPE_INTERFACE,
	0, // interface number
	0x00, // alternate setting
	0, // no endpoints
	AUDIO_CLASS,
	AUDIO_CONTROL_SUBCLASS,
	0x00, // interface protocol, unused
	0x00, // interface, unused

	/* AudioControl class-specific descriptor */
	IFACE_ONLY_DESC_SIZE, // size of functional descriptor
	USB_CS_INTERFACE,
	0x01, // HEADER subtype
	0x00, 0x01, // AC spec release number
	0x09, 0x00, // total size of class-specific descriptors
	0x01, // number of streaming interfaces
	0x01, // MIDIStreaming interface 1 belongs to this AC interface

	/* MIDISTREAMING INTERFACE DESCRIPTOR */
	        IFACE_ONLY_DESC_SIZE,
	        USB_IFACE_DESCRIPTOR,
	        (uint8_t)(0x01), /* bInterfaceNumber */
	        0x00, /* bAlternateSetting */
	        0x02, /* 2 endpoints */
	        AUDIO_CLASS,
	        MIDISTREAMING_SUBCLASS,
	        0x00, /* Interface Protocol, unused */
	        0x00, /* Interface, unused */

	        /* MIDIStreaming Class-Specific descriptor */
	        0x07,             /* size of Functional Desc in bytes */
	        USB_CS_INTERFACE,  /* descriptor type*/
	        0x01, /* HEADER subtype */
	        0x00, 0x01,  /* USB Class Definitions for AC spec release number in BCD */
	        7+6+6+9+9, 0x00, /* Total size of class-specific descriptors */

	        /* MIDI adapter MIDI IN Jack Descriptor (Embedded) */
	        0x06,             /* size of Functional Desc in bytes */
	        USB_CS_INTERFACE,  /* descriptor type*/
	        MIDI_IN_JACK,  /* descriptor type*/
	        EMBEDDED,       /* jack type */
	        0x01, /* jack ID */
	        0x06, /* unused */

	        /* MIDI adapter MIDI IN Jack Descriptor (External) */
	        0x06,             /* size of Functional Desc in bytes */
	        USB_CS_INTERFACE,  /* descriptor type*/
	        MIDI_IN_JACK,  /* descriptor type*/
	        EXTERNAL,       /* jack type */
	        0x02, /* jack ID */
	        0x06, /* unused */

	        /* MIDI adapter MIDI OUT Jack Descriptor (Embedded) */
	        0x09,             /* size of Functional Desc in bytes */
	        USB_CS_INTERFACE,  /* descriptor type*/
	        MIDI_OUT_JACK,  /* descriptor type*/
	        EMBEDDED,       /* jack type */
	        0x05, /* jack ID */
	        0x01, /* number of input pins */
	        0x01, /* BaSourceID(1) */
	        0x01, /* BaSourcePin(1) */
	        0x04, /* unused */

	        /* MIDI adapter MIDI OUT Jack Descriptor (External) */
	        0x09,             /* size of Functional Desc in bytes */
	        USB_CS_INTERFACE,  /* descriptor type*/
	        MIDI_OUT_JACK,  /* descriptor type*/
	        EXTERNAL,       /* jack type */
	        0x06, /* jack ID */
	        0x01, /* number of input pins */
	        0x01, /* BaSourceID(1) */
	        0x01, /* BaSourcePin(1) */
	        0x04, /* unused */

	        /* MIDI Adapter Standard Bulk OUT Endpoint Descriptor */
	        0x09,
	        USB_ENDPOINT_DESCRIPTOR,
	        0x02, /* OUT endpoint 1 */
	        USB_BULK_PIPE,
	        64, 0x00, /* max packet size */
	        0x00, /* interval */
	        0x00, /* refresh */
	        0x00, /* synch address */

	        /* Class-specific MS Bulk OUT Endpoint Descriptor */
	        0x05,
	        USB_CS_ENDPOINT,
	        MS_GENERAL,
	        0x01, /* Number of embedded MIDI IN jacks */
	        0x01, /* ID of the Embedded MIDI IN jack */

	        /* MIDI Adapter Standard Bulk IN Endpoint Descriptor */
	        0x09,
	        USB_ENDPOINT_DESCRIPTOR,
	        0x81, /* IN endpoint 2 */
	        USB_BULK_PIPE,
	        64, 0x00, /* max packet size */
	        0x00, /* interval */
	        0x00, /* refresh */
	        0x00, /* synch address */

	        /* Class-specific MS Bulk IN Endpoint Descriptor */
	        0x05,
	        USB_CS_ENDPOINT,
	        MS_GENERAL,
	        0x01, /* Number of embedded MIDI OUT jacks */
	        0x05, /* ID of the Embedded MIDI OUT jack */
};

/* Define string descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString0[] = {2U + 2U, USB_DESCRIPTOR_TYPE_STRING, 0x09, 0x04};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString1[] = {
    2U + 2U * 5U, USB_DESCRIPTOR_TYPE_STRING,
    'T',           0x00U,
    'I',           0x00U,
    'N',           0x00U,
    'R',           0x00U,
    'S',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString2[] = {2U + 2U * 6U, USB_DESCRIPTOR_TYPE_STRING,
                                'F',           0,
                                'e',           0,
                                'n',           0,
                                'i',           0,
                                'x',           0,
                                '4',           0,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString3[] = {2U + 2U * 1U, USB_DESCRIPTOR_TYPE_STRING,
                                '0',           0,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString4[] = {2U + 2U * 7U, USB_DESCRIPTOR_TYPE_STRING,
                                'U',           0,
                                'S',           0,
                                'B',           0,
                                ' ',           0,
                                'O',           0,
                                'u',           0,
                                't',           0,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString5[] = {2U + 2U * 12U, USB_DESCRIPTOR_TYPE_STRING,
                                'E',           0,
                                'x',           0,
                                't',           0,
                                ' ',           0,
                                'M',           0,
                                'I',           0,
                                'D',           0,
                                'I',           0,
                                ' ',           0,
                                'O',           0,
                                'u',           0,
                                't',           0,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString6[] = {2U + 2U * 6U, USB_DESCRIPTOR_TYPE_STRING,
                                'U',           0,
                                'S',           0,
                                'B',           0,
                                ' ',           0,
                                'I',           0,
                                'n',           0,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString7[] = {2U + 2U * 11U, USB_DESCRIPTOR_TYPE_STRING,
                                'E',           0,
                                'x',           0,
                                't',           0,
                                ' ',           0,
                                'M',           0,
                                'I',           0,
                                'D',           0,
                                'I',           0,
                                ' ',           0,
                                'I',           0,
                                'n',           0,
};

uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT] = {g_UsbDeviceString0,
																	  g_UsbDeviceString1,
																	  g_UsbDeviceString2,
																	  g_UsbDeviceString3,
																	  g_UsbDeviceString4,
																	  g_UsbDeviceString5,
																	  g_UsbDeviceString6,
                                                                      g_UsbDeviceString7};

/* Define string descriptor size */
uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0), sizeof(g_UsbDeviceString1), sizeof(g_UsbDeviceString2),
	sizeof(g_UsbDeviceString3), sizeof(g_UsbDeviceString4), sizeof(g_UsbDeviceString5),
	sizeof(g_UsbDeviceString6), sizeof(g_UsbDeviceString7), };
usb_language_t g_UsbDeviceLanguage[USB_DEVICE_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray, g_UsbDeviceStringDescriptorLength, (uint16_t)0x0409,
}};

usb_language_list_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0, sizeof(g_UsbDeviceString0), g_UsbDeviceLanguage, USB_DEVICE_LANGUAGE_COUNT,
};

/*******************************************************************************
* Code
******************************************************************************/
/*!
 * @brief USB device get device descriptor function.
 *
 * This function gets the device descriptor of the USB device.
 *
 * @param handle The USB device handle.
 * @param deviceDescriptor The pointer to the device descriptor structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor)
{
    deviceDescriptor->buffer = g_UsbDeviceDescriptor;
    deviceDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE;
    return kStatus_USB_Success;
}

/*!
 * @brief USB device get configuration descriptor function.
 *
 * This function gets the configuration descriptor of the USB device.
 *
 * @param handle The USB device handle.
 * @param configurationDescriptor The pointer to the configuration descriptor structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor)
{
    if (USB_CDC_VCOM_CONFIGURE_INDEX > configurationDescriptor->configuration)
    {
        configurationDescriptor->buffer = g_UsbDeviceConfigurationDescriptor;
        configurationDescriptor->length = USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        return kStatus_USB_Success;
    }
    return kStatus_USB_InvalidRequest;
}

/*!
 * @brief USB device get string descriptor function.
 *
 * This function gets the string descriptor of the USB device.
 *
 * @param handle The USB device handle.
 * @param stringDescriptor Pointer to the string descriptor structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor)
{
    if (stringDescriptor->stringIndex == 0U)
    {
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageString;
        stringDescriptor->length = g_UsbDeviceLanguageList.stringLength;
    }
    else
    {
        uint8_t languageId = 0U;
        uint8_t languageIndex = USB_DEVICE_STRING_COUNT;

        for (; languageId < USB_DEVICE_LANGUAGE_COUNT; languageId++)
        {
            if (stringDescriptor->languageId == g_UsbDeviceLanguageList.languageList[languageId].languageId)
            {
                if (stringDescriptor->stringIndex < USB_DEVICE_STRING_COUNT)
                {
                    languageIndex = stringDescriptor->stringIndex;
                }
                break;
            }
        }

        if (USB_DEVICE_STRING_COUNT == languageIndex)
        {
            return kStatus_USB_InvalidRequest;
        }
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageList[languageId].string[languageIndex];
        stringDescriptor->length = g_UsbDeviceLanguageList.languageList[languageId].length[languageIndex];
    }
    return kStatus_USB_Success;
}

/*!
 * @brief USB device set speed function.
 *
 * This function sets the speed of the USB device.
 *
 * Due to the difference of HS and FS descriptors, the device descriptors and configurations need to be updated to match
 * current speed.
 * As the default, the device descriptors and configurations are configured by using FS parameters for both EHCI and
 * KHCI.
 * When the EHCI is enabled, the application needs to call this fucntion to update device by using current speed.
 * The updated information includes endpoint max packet size, endpoint interval, etc.
 *
 * @param handle The USB device handle.
 * @param speed Speed type. USB_SPEED_HIGH/USB_SPEED_FULL/USB_SPEED_LOW.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed)
{
    usb_descriptor_union_t *ptr1;
    usb_descriptor_union_t *ptr2;

    ptr1 = (usb_descriptor_union_t *)(&g_UsbDeviceConfigurationDescriptor[0]);
    ptr2 = (usb_descriptor_union_t *)(&g_UsbDeviceConfigurationDescriptor[USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1]);

    while (ptr1 < ptr2)
    {
        if (ptr1->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            if (USB_SPEED_HIGH == speed)
            {
                /*if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                     (USB_CDC_VCOM_INTERRUPT_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    ptr1->endpoint.bInterval = HS_CDC_VCOM_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE,
                                                       ptr1->endpoint.wMaxPacketSize);
                }
                else*/ if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_CDC_VCOM_BULK_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_BULK_IN_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                         (USB_CDC_VCOM_BULK_OUT_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_BULK_OUT_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else
                {
                }
            }
            else
            {
                /*if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                      (USB_CDC_VCOM_INTERRUPT_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    ptr1->endpoint.bInterval = FS_CDC_VCOM_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE,
                                                       ptr1->endpoint.wMaxPacketSize);
                }
                else*/ if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                      (USB_CDC_VCOM_BULK_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_BULK_IN_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else if (((ptr1->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                    (USB_CDC_VCOM_BULK_OUT_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_BULK_OUT_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else
                {
                }
            }
        }
        ptr1 = (usb_descriptor_union_t *)((uint8_t *)ptr1 + ptr1->common.bLength);
    }

    /*for (int i = 0; i < USB_CDC_VCOM_ENDPOINT_CIC_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            g_UsbDeviceCdcVcomCicEndpoints[i].maxPacketSize = HS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE;
        }
        else
        {
            g_UsbDeviceCdcVcomCicEndpoints[i].maxPacketSize = FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE;
        }
    }
    for (int i = 0; i < USB_CDC_VCOM_ENDPOINT_DIC_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            if (g_UsbDeviceMidiStreamingEndpoints[i].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_UsbDeviceMidiStreamingEndpoints[i].maxPacketSize = HS_CDC_VCOM_BULK_IN_PACKET_SIZE;
            }
            else
            {
                g_UsbDeviceMidiStreamingEndpoints[i].maxPacketSize = HS_CDC_VCOM_BULK_OUT_PACKET_SIZE;
            }
        }
        else
        {
            if (g_UsbDeviceMidiStreamingEndpoints[i].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_UsbDeviceMidiStreamingEndpoints[i].maxPacketSize = FS_CDC_VCOM_BULK_IN_PACKET_SIZE;
            }
            else
            {
                g_UsbDeviceMidiStreamingEndpoints[i].maxPacketSize = FS_CDC_VCOM_BULK_OUT_PACKET_SIZE;
            }
        }
    }*/

    return kStatus_USB_Success;
}
