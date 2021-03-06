/**************************************************************************//**
 * @file retargettextdisplay.h
 * @brief Retarget stdout to text display module.
 * @version 4.1.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/



#ifndef _RETARGETTEXTDISPLAY_H_
#define _RETARGETTEXTDISPLAY_H_

#include "emstatus.h"

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RetargetIo
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CROSSWORKS_ARM)
int __putchar(int ch);
int __getchar(void);
#endif

/*******************************************************************************
 **************************    FUNCTION PROTOTYPES    **************************
 ******************************************************************************/

EMSTATUS RETARGET_TextDisplayInit(void);
EMSTATUS RETARGET_WriteString(char*   str);

#ifdef __cplusplus
}
#endif

/** @} (end group RetargetIo) */
/** @} (end group Drivers) */

#endif /* _RETARGETTEXTDISPLAY_H_ */
