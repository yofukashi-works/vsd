// $Id$

#ifndef SCI_H
#define SCI_H

//#include <stddef.h>
//#include <itron.h>

#include "dds.h"

typedef int				VP_INT;
typedef unsigned char	UB;
typedef int				size_t;

/* �o�b�t�@���I�[�o���������Ƃ��̓���͕ۏ؂���܂��� */
#define RX_BUFFER_LENGTH	32		/* ��M�o�b�t�@�̒��� */
#define TX_BUFFER_LENGTH	48		/* ���M�o�b�t�@�̒��� */

void sci_init(VP_INT exinf);			/* ������ */
void sci_int_handler(VP_INT exinf);
void sci_flush_rx(void);				/* ��M�o�b�t�@�N���A */
void sci_flush_tx(void);				/* ���M�o�b�t�@�N���A */
size_t sci_read(UB* data, size_t n);	/* ��M�o�b�t�@����ǂݍ��� */
	/* data��0���w�肷��ƁA��M�o�b�t�@�ɂ��܂��Ă���f�[�^�̃o�C�g����Ԃ� */
size_t sci_write(UB* data, size_t n);	/* ���M�o�b�t�@�ɏ������� */
	/* data��0���w�肷��ƁA���M�o�b�t�@�ɂ��܂��Ă���f�[�^�̃o�C�g����Ԃ� */

#endif
