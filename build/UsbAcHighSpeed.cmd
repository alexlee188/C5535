MEMORY
{
CROM (RX) : origin = 0xfe0000, length = 0x000600 /* 1536B */
}

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
	.onchip_code	:	> SARAM
    .bss    :  > SARAM
/*
    .ping_pong_i2sRxLeftBuf   : block(0x8000)    {} > SARAM
    .ping_pong_i2sRxRightBuf   : block(0x10000)    {} > SARAM
    .ping_pong_i2sTxLeftBuf   : block(0x20000)    {} > SARAM
    .ping_pong_i2sTxRightBuf   : block(0x40000)    {} > SARAM
*/

	.charrom : > CROM
}


/* HWAFFT Routines ROM Addresses */

_hwafft_br = 0x00fefe9c;

_hwafft_8pts = 0x00fefeb0;

_hwafft_16pts = 0x00feff9f;

_hwafft_32pts = 0x00ff00f5;

_hwafft_64pts = 0x00ff03fe;

_hwafft_128pts = 0x00ff0593;

_hwafft_256pts = 0x00ff07a4;

_hwafft_512pts = 0x00ff09a2;

_hwafft_1024pts = 0x00ff0c1c;

