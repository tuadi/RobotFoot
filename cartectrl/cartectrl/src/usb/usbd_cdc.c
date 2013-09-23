/**
  ******************************************************************************
  * @file    usbd_cdc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"
#include <string.h>

//------------------------------------------------------------------------------

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#undef min
#undef max

#define min(x,y)            ((x) < (y) ? (x) : (y))
#define max(x,y)            ((x) > (y) ? (x) : (y))

// Constraint x to be between lo and hi
#define clip(x, lo, hi)     (max(min(x, hi), lo))


/* Private variables ---------------------------------------------------------*/

/* These are external variables imported from CDC core to be used for IN
   transfer management. */
// Note: The variable names are a bit misleading, that is, it is from the HOST
// viewpoint, so the Rx buffer contains data that the HOST will RECEIVE.
extern uint8_t  APP_Rx_Buffer [];

extern volatile uint32_t APP_Rx_ptr_in;
extern volatile uint32_t APP_Rx_ptr_out;

static const uint8_t *APP_Rx_Buffer_end = APP_Rx_Buffer + APP_RX_DATA_SIZE;
static const uint32_t APP_Rx_wrap_mask = (APP_RX_DATA_SIZE - 1);

// MUST BE A POWER OF TWO!
#define READ_BUFFER_SIZE    APP_RX_DATA_SIZE
static uint8_t  read_buffer[READ_BUFFER_SIZE];
static uint32_t read_buffer_wrap_mask = (READ_BUFFER_SIZE - 1);

static struct pipe
{
    uint8_t *const buffer;
    uint32_t ptr_in;
    uint32_t ptr_out;
}
rx =
{
    read_buffer
};

/* Private function prototypes -----------------------------------------------*/

static uint16_t cdc_Init   (void);
static uint16_t cdc_DeInit (void);
static uint16_t cdc_Ctrl   (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t cdc_DataTx (uint8_t* Buf, uint32_t Len);
static uint16_t cdc_DataRx (uint8_t* Buf, uint32_t Len);


CDC_IF_Prop_TypeDef cdc_fops =
{
    cdc_Init,
    cdc_DeInit,
    cdc_Ctrl,
    cdc_DataTx,
    cdc_DataRx
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  in_APP_Rx_buffer
  *         Number of elements in buffer
  * @param  None
  * @return Number of elements in buffer
  * @note   Remember that RX is for IN transaction, so from the device point of à
  *         view, it's rather TX.
  */
static inline uint32_t in_APP_Rx_buffer(void)
{
    return (APP_Rx_ptr_in - APP_Rx_ptr_out) & APP_Rx_wrap_mask;
}

/**
  * @brief  left_APP_Rx_buffer
  *         Number of elements left in buffer
  * @param  None
  * @return Number of elements left in buffer
  * @note   Remember that RX is for IN transaction, so from the device point of à
  *         view, it's rather TX.
  */
static inline uint32_t left_APP_Rx_buffer(void)
{
    return APP_RX_DATA_SIZE - in_APP_Rx_buffer() - 1;
}

/**
  * @brief  rx_in_buffer
  *         Number of elements in a pipe
  * @param  None
  * @return Number of elements in a pipe
  */
static inline uint32_t rx_in_buffer(struct pipe *p)
{
    return (p->ptr_in - p->ptr_out) & read_buffer_wrap_mask;
}

/**
  * @brief  rx_left_buffer
  *         Number of elements left in a pipe
  * @param  None
  * @return Number of elements left in a pipe
  */
static inline uint32_t rx_left_buffer(struct pipe *p)
{
    return READ_BUFFER_SIZE - rx_in_buffer(p) - 1;
}

/**
  * @brief  cdc_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @return Result of the operation (USBD_OK in all cases)
  */
static uint16_t cdc_Init(void)
{
    return USBD_OK;
}

/**
  * @brief  cdc_DeInit
  *         "Deinitializes" the Media on the STM32
  * @param  None
  * @return Result of the operation (USBD_OK in all cases)
  */
static uint16_t cdc_DeInit(void)
{
    /* It does not get called at all! */
    return USBD_OK;
}


/**
  * @brief  cdc_Ctrl
  *         Manage the CDC class requests
  *
  *         @note
  *         This function runs in privilegied priority (from USB interrupt).
  *
  * @param  Cmd Command code
  * @param  Buf Buffer containing command data (request parameters)
  * @param  Len Number of data to be sent (in bytes)
  * @return Result of the operation (USBD_OK in all cases)
  */
static uint16_t cdc_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:

    break;

  case GET_LINE_CODING:

    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;

  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  cdc_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in
  *         this function.
  *
  *         @note
  *         This function runs in privilegied priority (from USB interrupt).
  *
  *         @note
  *         This function, whilst being visible in the USB OTG library, is never
  *         actually used: one should play with the APP_Rx_buffer above with the
  *         APP_Rx_ptr_in to actually notify the USB driver to send some data.
  *
  * @param  Buf Buffer of data to be sent
  * @param  Len Number of data to be sent (in bytes)
  * @return Result of the operation: USBD_OK
  */
static uint16_t cdc_DataTx (uint8_t* Buf, uint32_t Len)
{

	uint32_t i;
	//loop through buffer

	for( i = 0; i < Len; i++ )
	{
		//push data into transfer buffer
		APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[i] ;
		//increase pointer value
		APP_Rx_ptr_in++;
		/* To avoid buffer overflow */
		if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_in = 0;
		}
	}

	return USBD_OK;
}

/**
  * @brief  cdc_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  *         @note
  *         This function runs in privilegied priority (from USB interrupt).
  *
  * @param  Buf Buffer of data to be received
  * @param  Len Number of data received (in bytes)
  * @return Result of the operation: USBD_OK, or USBD_BUSY if buffer full
  */
static uint16_t cdc_DataRx (uint8_t* Buf, uint32_t Len)
{
    for (; Len; --Len)
    {
        rx.buffer[rx.ptr_in] = *Buf++;
        rx.ptr_in = (rx.ptr_in + 1) & read_buffer_wrap_mask;
        if (rx.ptr_in == rx.ptr_out)
        {
            // "Discard" old data
            // This should never happen anymore!
            rx.ptr_out = (rx.ptr_out + 1) & read_buffer_wrap_mask;
        }
    }
    usb_resume_out_xfer();
	return USBD_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  usb_tx
  *         Send requested data
  *
  * @param  Buf Buffer of data to be transmitted
  * @param  Len Number of data in buffer (in bytes)
  * @return Number of bytes actually put in buffer.
  */
size_t usb_tx(const uint8_t *buf, size_t len)
{
    const size_t left_in_buffer = left_APP_Rx_buffer();

    // Limit transfer to what the buffer can hold at this time.
    len = min(left_in_buffer, len);
    const size_t result = len;
    uint8_t *const ptr_in = APP_Rx_Buffer + APP_Rx_ptr_in;

    // Number of bytes before wrapping down.
    const size_t upper_buffer = APP_Rx_Buffer_end - ptr_in;
    const size_t direct_xfer  = min(len, upper_buffer);


    memcpy(ptr_in, buf, direct_xfer);
    len -= direct_xfer;
    if (len)
    {
        // Some data has to be put in the beginning of the buffer.
        buf += direct_xfer;
        memcpy(APP_Rx_Buffer, buf, len);
    }

    // Update ptr_in to notify driver of the new data
    // Safe to update normally because the driver doesn't modify it.
    APP_Rx_ptr_in = (APP_Rx_ptr_in + result) & APP_Rx_wrap_mask;

    return result;
}

/**
  * @brief  usb_rx
  *         Get received data.
  *
  * @param  Buf Buffer of data to be received
  * @param  Len Capacity of the buffer (in bytes)
  * @return Number of bytes actually read
  */
size_t usb_rx(uint8_t *buf, size_t len)
{
    const size_t in_buffer = rx_in_buffer(&rx);
    // What is in the read buffer is what can be put in the supplied buffer
    len = min(len, in_buffer);
    const size_t result = len;
    const uint8_t *const ptr_out = rx.buffer + rx.ptr_out;

    uint32_t direct = min(READ_BUFFER_SIZE - rx.ptr_out, len);

    memcpy(buf, ptr_out, direct);
    len -= direct;
    if (len)
    {
        buf += direct;
        memcpy(buf, rx.buffer, len);
    }
    rx.ptr_out = (rx.ptr_out + result) & read_buffer_wrap_mask;

    return result;
}

// PUT DOXYGEN COMMENT HERE
void usb_resume_out_xfer(void)
{
    size_t const left = rx_left_buffer(&rx);
    if (left >= USB_OTG_FS_MAX_PACKET_SIZE)
    {

        extern USB_OTG_CORE_HANDLE USB_OTG_dev;
        extern uint8_t USB_Rx_Buffer[CDC_DATA_MAX_PACKET_SIZE];
        USB_OTG_CORE_HANDLE *pdev = &USB_OTG_dev;

        if (DCD_GetEPStatus(&USB_OTG_dev, CDC_OUT_EP) != USB_OTG_EP_TX_VALID)
        {
            DCD_EP_PrepareRx(&USB_OTG_dev,
                            CDC_OUT_EP,
                            (uint8_t*)(USB_Rx_Buffer),
                            CDC_DATA_OUT_PACKET_SIZE);
        }
    }
}
