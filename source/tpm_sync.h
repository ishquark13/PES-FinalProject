/*
 * @file tpm_sync.h - Synchronize TPM clock -- set to default state
 *
 * @brief Reset unused registers back to a static configuration after calling TPM0
 * 
 * ​@author​ ​Ishmael Pelayo, ispe3057@colorado.edu
​ ​*​ ​@date​ ​	December 02 2022
​ ​*​ ​@version​ ​1.0
 */

#ifndef _TPM_SYNC_H_
#define _TPM_SYNC_H_

#include <stdint.h>

/* @brief  Sets TPM to registers changed by DMA0
 *
 *
 * @param   none
 * @return  none
 */
void init_tpm_sync();


#endif //_TPM_SYNC_H_
