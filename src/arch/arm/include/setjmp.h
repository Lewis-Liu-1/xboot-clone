#ifndef __ARM_SETJMP_H__
#define __ARM_SETJMP_H__

struct __jmp_buf {
	unsigned int regs[10];
};

typedef struct __jmp_buf jmp_buf[1];


int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#endif /* __ARM_SETJMP_H__ */
