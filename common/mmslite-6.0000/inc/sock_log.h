/************************************************************************/
/* SOFTWARE MODULE HEADER ***********************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*          2002 - 2003, All Rights Reserved                            */
/*									*/
/* MODULE NAME : sock_log.h 						*/
/* PRODUCT(S)  : General Sockets Interface logging			*/
/*									*/
/*	    								*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/02/10  JRB	   Del check for ctx==NULL (must be valid).	*/
/* 09/20/07  MDE    02     Modified macros to take ctx, log_disable	*/
/* 06/25/03  JRB    01     Move log macros from "gensock2.h".		*/
/************************************************************************/
#ifndef SOCK_LOG_INCLUDED
#define SOCK_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif



#define SOCK_LOG_ERR	0x0001   
#define SOCK_LOG_NERR	0x0002   
#define SOCK_LOG_FLOW	0x0004   
#define SOCK_LOG_RX	0x0008   
#define SOCK_LOG_TX	0x0010   

extern ST_UINT  sock_debug_sel;

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_ERR_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_NERR_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_FLOW_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_RX_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  SOCK_LOG_TX_TEXT;

/*********************** SOCK_LOG_ERR Macros *****************************/

#define SOCK_LOG_ERR0(ctx,a) \
	SLOG_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a)
#define SOCK_LOG_ERR1(ctx,a,b) \
	SLOG_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a,b)
#define SOCK_LOG_ERR2(ctx,a,b,c) \
	SLOG_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a,b,c)
#define SOCK_LOG_ERR3(ctx,a,b,c,d) \
	SLOG_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a,b,c,d)
#define SOCK_LOG_ERR4(ctx,a,b,c,d,e) \
	SLOG_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a,b,c,d,e)
#define SOCK_LOG_ERR5(ctx,a,b,c,d,e,f) \
	SLOG_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),SOCK_LOG_ERR_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define SOCK_LOG_ERRC0(ctx,a) \
	SLOGC_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a)
#define SOCK_LOG_ERRC1(ctx,a,b) \
	SLOGC_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b)
#define SOCK_LOG_ERRC2(ctx,a,b,c) \
	SLOGC_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b,c)
#define SOCK_LOG_ERRC3(ctx,a,b,c,d) \
	SLOGC_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b,c,d)
#define SOCK_LOG_ERRC4(ctx,a,b,c,d,e) \
	SLOGC_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b,c,d,e)
#define SOCK_LOG_ERRC5(ctx,a,b,c,d,e,f) \
	SLOGC_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b,c,d,e,f)

/* Hex log macro	*/
#define SOCK_LOG_ERRH(ctx,a,b) \
	SLOGH ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_ERR),a,b)

/*********************** SOCK_LOG_NERR Macros *****************************/

#define SOCK_LOG_NERR0(ctx,a) \
	SLOG_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a)
#define SOCK_LOG_NERR1(ctx,a,b) \
	SLOG_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a,b)
#define SOCK_LOG_NERR2(ctx,a,b,c) \
	SLOG_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a,b,c)
#define SOCK_LOG_NERR3(ctx,a,b,c,d) \
	SLOG_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a,b,c,d)
#define SOCK_LOG_NERR4(ctx,a,b,c,d,e) \
	SLOG_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a,b,c,d,e)
#define SOCK_LOG_NERR5(ctx,a,b,c,d,e,f) \
	SLOG_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),SOCK_LOG_NERR_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define SOCK_LOG_NERRC0(ctx,a) \
	SLOGC_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a)
#define SOCK_LOG_NERRC1(ctx,a,b) \
	SLOGC_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b)
#define SOCK_LOG_NERRC2(ctx,a,b,c) \
	SLOGC_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b,c)
#define SOCK_LOG_NERRC3(ctx,a,b,c,d) \
	SLOGC_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b,c,d)
#define SOCK_LOG_NERRC4(ctx,a,b,c,d,e) \
	SLOGC_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b,c,d,e)
#define SOCK_LOG_NERRC5(ctx,a,b,c,d,e,f) \
	SLOGC_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b,c,d,e,f)

/* Hex log macro	*/
#define SOCK_LOG_NERRH(ctx,a,b) \
	SLOGH ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_NERR),a,b)

/*********************** SOCK_LOG_FLOW Macros *****************************/

#define SOCK_LOG_FLOW0(ctx,a) \
	SLOG_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a)
#define SOCK_LOG_FLOW1(ctx,a,b) \
	SLOG_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a,b)
#define SOCK_LOG_FLOW2(ctx,a,b,c) \
	SLOG_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a,b,c)
#define SOCK_LOG_FLOW3(ctx,a,b,c,d) \
	SLOG_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a,b,c,d)
#define SOCK_LOG_FLOW4(ctx,a,b,c,d,e) \
	SLOG_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a,b,c,d,e)
#define SOCK_LOG_FLOW5(ctx,a,b,c,d,e,f) \
	SLOG_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),SOCK_LOG_FLOW_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define SOCK_LOG_FLOWC0(ctx,a) \
	SLOGC_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a)
#define SOCK_LOG_FLOWC1(ctx,a,b) \
	SLOGC_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b)
#define SOCK_LOG_FLOWC2(ctx,a,b,c) \
	SLOGC_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b,c)
#define SOCK_LOG_FLOWC3(ctx,a,b,c,d) \
	SLOGC_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b,c,d)
#define SOCK_LOG_FLOWC4(ctx,a,b,c,d,e) \
	SLOGC_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b,c,d,e)
#define SOCK_LOG_FLOWC5(ctx,a,b,c,d,e,f) \
	SLOGC_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b,c,d,e,f)

/* Hex log macro	*/
#define SOCK_LOG_FLOWH(ctx,a,b) \
	SLOGH ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_FLOW),a,b)

/*********************** SOCK_LOG_RX Macros *****************************/

#define SOCK_LOG_RX0(ctx,a) \
	SLOG_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a)
#define SOCK_LOG_RX1(ctx,a,b) \
	SLOG_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a,b)
#define SOCK_LOG_RX2(ctx,a,b,c) \
	SLOG_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a,b,c)
#define SOCK_LOG_RX3(ctx,a,b,c,d) \
	SLOG_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a,b,c,d)
#define SOCK_LOG_RX4(ctx,a,b,c,d,e) \
	SLOG_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a,b,c,d,e)
#define SOCK_LOG_RX5(ctx,a,b,c,d,e,f) \
	SLOG_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),SOCK_LOG_RX_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define SOCK_LOG_RXC0(ctx,a) \
	SLOGC_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a)
#define SOCK_LOG_RXC1(ctx,a,b) \
	SLOGC_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b)
#define SOCK_LOG_RXC2(ctx,a,b,c) \
	SLOGC_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b,c)
#define SOCK_LOG_RXC3(ctx,a,b,c,d) \
	SLOGC_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b,c,d)
#define SOCK_LOG_RXC4(ctx,a,b,c,d,e) \
	SLOGC_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b,c,d,e)
#define SOCK_LOG_RXC5(ctx,a,b,c,d,e,f) \
	SLOGC_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b,c,d,e,f)

/* Hex log macro	*/
#define SOCK_LOG_RXH(ctx,a,b) \
	SLOGH ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_RX),a,b)

/*********************** SOCK_LOG_TX Macros *****************************/

#define SOCK_LOG_TX0(ctx,a) \
	SLOG_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a)
#define SOCK_LOG_TX1(ctx,a,b) \
	SLOG_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a,b)
#define SOCK_LOG_TX2(ctx,a,b,c) \
	SLOG_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a,b,c)
#define SOCK_LOG_TX3(ctx,a,b,c,d) \
	SLOG_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a,b,c,d)
#define SOCK_LOG_TX4(ctx,a,b,c,d,e) \
	SLOG_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a,b,c,d,e)
#define SOCK_LOG_TX5(ctx,a,b,c,d,e,f) \
	SLOG_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),SOCK_LOG_TX_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define SOCK_LOG_TXC0(ctx,a) \
	SLOGC_0 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a)
#define SOCK_LOG_TXC1(ctx,a,b) \
	SLOGC_1 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b)
#define SOCK_LOG_TXC2(ctx,a,b,c) \
	SLOGC_2 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b,c)
#define SOCK_LOG_TXC3(ctx,a,b,c,d) \
	SLOGC_3 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b,c,d)
#define SOCK_LOG_TXC4(ctx,a,b,c,d,e) \
	SLOGC_4 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b,c,d,e)
#define SOCK_LOG_TXC5(ctx,a,b,c,d,e,f) \
	SLOGC_5 ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b,c,d,e,f)

/* Hex log macro	*/
#define SOCK_LOG_TXH(ctx,a,b) \
	SLOGH ((!(ctx)->log_disable) && (sock_debug_sel & SOCK_LOG_TX),a,b)

#ifdef __cplusplus
}
#endif

#endif /* SOCK_LOG_INCLUDED */
