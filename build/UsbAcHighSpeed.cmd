SECTIONS
{
    .text   :  > SARAM
    .switch :  > DARAM
    .cinit  :  > DARAM
    .pinit  :  > DARAM
    .printf :  > SARAM
    .const  :  > SARAM
    .data   :  > SARAM
    .cio    :  > DARAM

    .bss    :  > SARAM
/*
    .ping_pong_i2sRxLeftBuf   : block(0x8000)    {} > SARAM
    .ping_pong_i2sRxRightBuf   : block(0x10000)    {} > SARAM
    .ping_pong_i2sTxLeftBuf   : block(0x20000)    {} > SARAM
    .ping_pong_i2sTxRightBuf   : block(0x40000)    {} > SARAM
*/
}
