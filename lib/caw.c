#include "caw.h"

#include <string.h>

#include "../usbd/usbd_main.h"
#include "../ll/debug_usart.h"

#define USB_RX_BUFFER 256
static char reader[USB_RX_BUFFER];
static int8_t pReader = 0;

uint8_t Caw_Init( void ) //TODO fnptr of callback )
{
    for( int i=0; i<USB_RX_BUFFER; i++ ){ reader[i] = 0; }
    USB_CDC_Init();
    // TODO set static void(*) which Caw_receive_callback() forward all info to
    // alt, we could just send a *buffer that we copy data into & check in scheduler
    return 0;
}

void Caw_send_raw( uint8_t* buf, uint32_t len )
{
    USB_tx_enqueue( buf, len );
}

// luachunk expects a \0 terminated string
void Caw_send_luachunk( char* text )
{
    uint32_t len = strlen(text);
    uint8_t s[len + 2];
    memcpy( s, text, len );
    s[len] = '\n';
    s[len+1] = '\r';

    USB_tx_enqueue( s
                  , len + 2
                  );
}

void Caw_send_luaerror( char* error_msg )
{
    uint32_t len = strlen(error_msg);
    uint8_t s[len + 3];
    s[0]     = '\\';
    memcpy( &s[1], error_msg, len );
    s[len+1] = '\n';
    s[len+2] = '\r';

    USB_tx_enqueue( s
                  , len + 3
                  );
}

void Caw_send_value( uint8_t type, float value )
{
    //
}

uint8_t Caw_try_receive( void )
{
    // TODO add scanning for 'goto_bootloader' override command. return 2
    // TODO add start_flash_chunk command handling. return 3
    // TODO add end_flash_chunk command handling. return 4
    static uint8_t* buf;
    static uint32_t len;

    if( USB_rx_dequeue( &buf, &len ) ){
        memcpy( &(reader[pReader])
              , (char*)buf
              , len
              );
        pReader += len;
        if( reader[pReader-1] == '\n'
         || reader[pReader-1] == '\r' ){ // line is finished!
            reader[pReader] = '\0';
            pReader++;
            return 1;
        }
    }
    return 0;
}

char* Caw_get_read( void )
{
    return reader;
}

uint32_t Caw_get_read_len( void )
{
    uint32_t len = pReader;
    pReader = 0;
    return len;
}
