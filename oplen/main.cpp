//
//OPLEN sample and TEST
//このプロダクトは sexyhookのサブプロダクトです。
//
//by rti
//
//new BSD ライセンス / NYSLライセンス 好きに使えばいいんぢゃなイカ ^p^
//
#include "oplen.h"	//これ



//dispe で出力したアセンブラを読み込ませてテストする
void dumpoptest(const char * filename,SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE cputype)
{
	FILE * fp = fopen(filename,"rb");
	if (!fp) return ;

	int linenumber = 0;
	char buffer[1024];
	while(! feof(fp) )
	{
		unsigned char opcode[32] = {0};

		linenumber ++;
		fgets(buffer,1023,fp);

		int opelen = 0;
		int i = 0;
		for( ;  ; i += 2 ,opelen++)
		{
			if (buffer[i] < '0' || buffer[i] > 'f') break;
			if (buffer[i+1] < '0' || buffer[i+1] > 'f') break;

			unsigned char p;
			if ( buffer[i] >= '0' && buffer[i] <= '9')		p = (buffer[i] - '0') << 4;
			else											p = (buffer[i] - 'a' + 10) << 4;
			if ( buffer[i+1] >= '0' && buffer[i+1] <= '9')	p += (buffer[i+1] - '0');
			else											p += (buffer[i+1] - 'a' + 10);

			opcode[opelen] = p;
		}
		if (opelen <= 0) continue;

		//dispe は objdumpと違って lock add を lock ; add; と2文にしてしまうため lock のみの構文をスキップする.
		if (opelen == 1 && opcode[0] == 0xf0) continue; 

		printf("%d  %s\r\n",linenumber , buffer);

		int len = 0;
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(opcode,cputype) ) == opelen);
	}
	fclose(fp);
}

//objdumpで出力したアセンブラを読み込ませてテストする
void dumpoptestObjectDump(const char * filename,SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE cputype)
{
	FILE * fp = fopen(filename,"rb");
	if (!fp) return ;

	int linenumber = 0;
	char buffer[1024];
	while(! feof(fp) )
	{
		unsigned char opcode[32] = {0};

		linenumber ++;
		fgets(buffer,1023,fp);

		int opelen = 0;
		int i = 0;
		for( ;  ; i += 2 ,opelen++)
		{
			if (buffer[i] < '0' || buffer[i] > 'f') break;
			if (buffer[i+1] < '0' || buffer[i+1] > 'f') break;

			unsigned char p;
			if ( buffer[i] >= '0' && buffer[i] <= '9')		p = (buffer[i] - '0') << 4;
			else											p = (buffer[i] - 'a' + 10) << 4;
			if ( buffer[i+1] >= '0' && buffer[i+1] <= '9')	p += (buffer[i+1] - '0');
			else											p += (buffer[i+1] - 'a' + 10);

			opcode[opelen] = p;
		}
		if (opelen <= 0) continue;
		if (opelen >= 7)
		{
			//objdumpのクソは長い命令があるとぶった切るので、補正してあげる.
			size_t nowfp = ftell(fp);
			char buffer2[1024]; 
			fgets(buffer2,1023,fp);
			//ぶった切られたオペランドかどうかチェック
			const char *bfp2 = buffer2;
			for( ; *bfp2 ; ++bfp2)
			{
				if ( *bfp2 == ' ' || *bfp2 == '\t' ) break;
			}
			if (*bfp2 == 0)
			{//ぶった切られたオペランドっぽい
				linenumber ++; //この行をスキップするので行数を増やす

				//16進数化 関数化しろよっみたいな。
				for(i = 0 ;  ; i += 2 ,opelen++)
				{
					if (buffer2[i] < '0' || buffer2[i] > 'f') break;
					if (buffer2[i+1] < '0' || buffer2[i+1] > 'f') break;

					unsigned char p;
					if ( buffer2[i] >= '0' && buffer2[i] <= '9')		p = (buffer2[i] - '0') << 4;
					else												p = (buffer2[i] - 'a' + 10) << 4;
					if ( buffer2[i+1] >= '0' && buffer2[i+1] <= '9')	p += (buffer2[i+1] - '0');
					else												p += (buffer2[i+1] - 'a' + 10);

					opcode[opelen] = p;
				}
			}
			else
			{//先読みした分を戻す.
				fseek(fp,nowfp,SEEK_SET);
			}
		}

		printf("%d  %s\r\n",linenumber , buffer);

		int len = 0;
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(opcode,cputype) ) == opelen);
	}
	fclose(fp);
}

void oplengthtest()
{
	int len;
	const SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE x86 = SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE_X86;
	const SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE x64 = SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE_X64;
	{//maskmovq mm1,mm6
		unsigned char op[] = { 0x66,0x0f,0xf7,0xce };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//movntq  [edi],mm0
		unsigned char op[] = { 0x66,0x0f,0xe7,0x07 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//punpckldq mm0,mm2
		unsigned char op[] = { 0x66,0x0f,0x62,0xc2 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//pandn   mm2,mm7
		unsigned char op[] = { 0x66,0x0f,0xdf,0xd7 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//paddw   mm7,mm0
		unsigned char op[] = { 0x66,0x0f,0xfd,0xf8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//pmovmskb di,mm0
		unsigned char op[] = { 0x66,0x0f,0xd7,0xf8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//pcmpeqd mm0,mm6
		unsigned char op[] = { 0x66,0x0f,0x76,0xc6 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//punpcklwd mm1,mm0
		unsigned char op[] = { 0x66,0x0f,0x61,0xc8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//les     eax,[ecx+075h]
		unsigned char op[] = { 0xc4,0x41,0x75 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//xlat
		unsigned char op[] = { 0xd7};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//jcxz    C00497d21
		unsigned char op[] = { 0xe3,0x26};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//wait
		unsigned char op[] = { 0x9b};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//repne   scasb 
		unsigned char op[] = { 0xf2 ,0xae};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//xor         eax,ebp  
		unsigned char op[] = { 0x33 ,0xc5};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//??
		unsigned char op[] = { 0x9b,0x97,0x66,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//??
		unsigned char op[] = { 0x9b,0xc4,0x20 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//movq  %xmm0,0xfffffffffffffff8(%rsp)
		unsigned char op[] = { 0x66,0x0f,0xd6,0x44,0x24,0xf8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 6);
	}
	{//setnp %dl
		unsigned char op[] = { 0x0f,0x9b,0xc2 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//fsave  0x250(%rsp)
		unsigned char op[] = { 0x9b,0xdd,0xb4,0x24,0x50,0x02,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//fxsave6842379(%rip)       #b3d100<stdout+0x389e58>
		unsigned char op[] = { 0x0f,0xae,0x05,0x0b,0x68,0x68,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 7);
	}
	{//lockcmpxchg%esi,6849201(%rip)       #b3b054<stdout+0x387dac>
		unsigned char op[] = { 0xf0,0x0f,0xb1,0x35,0xb1,0x82,0x68,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//cmpxchg dword [esi],dx
		unsigned char op[] = { 0x0f,0xb1,0x16 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//rdtsc
		unsigned char op[] = { 0x0f,0x31 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//bswap %edi
		unsigned char op[] = { 0x0f,0xcf };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//bswap %ecx
		unsigned char op[] = { 0x0f,0xc9 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//minsd %xmm3,%xmm1
		unsigned char op[] = { 0xf2,0x0f,0x5d,0xcb };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//andnpd%xmm3,%xmm2
		unsigned char op[] = { 0x66,0x0f,0x55,0xd3 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//pshufw$0xff,%mm1,%mm2
		unsigned char op[] = { 0x0f,0x70,0xd1,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//lockaddl$0x1,4910416(%rip)       #9ad918<__bss_start+0x12958>
		unsigned char op[] = { 0xf0,0x83,0x05,0x50,0xed,0x4a,0x00,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//orpd  %xmm1,%xmm0
		unsigned char op[] = { 0x66,0x0f,0x56,0xc1 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//andpd %xmm2,%xmm0
		unsigned char op[] = { 0x66,0x0f,0x54,0xc2 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmpneqsd%xmm1,%xmm2
		unsigned char op[] = { 0xf2,0x0f,0xc2,0xd1,0x04 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 5);
	}
	{//sqrtsd%xmm0,%xmm1
		unsigned char op[] = { 0xf2,0x0f,0x51,0xc8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//bswap %r13d
		unsigned char op[] = { 0x41,0x0f,0xcd };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//bswap %edx
		unsigned char op[] = { 0x0f,0xca };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//test  $0x7fff,%ax
		unsigned char op[] = { 0x66,0xa9,0xff,0x7f };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//repstos%rax,%es:(%rdi)
		unsigned char op[] = { 0xf3,0x48,0xab };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//nopw  %cs:0x0(%rax,%rax,1)
		unsigned char op[] = { 0x66,0x2e,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 10);
	}
	{//nopw  0x0(%rax,%rax,1)
		unsigned char op[] = { 0x66,0x0f,0x1f,0x44,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 6);
	}
	{//nopw  0x0(%rax,%rax,1)
		unsigned char op[] = { 0x66,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 9);
	}
	{//repzcmpsb%es:(%rdi),%ds:(%rsi)
		unsigned char op[] = { 0xf3,0xa6 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//add   $0x76c,%ax
		unsigned char op[] = { 0x66,0x05,0x6c,0x07 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//movapd%xmm0,%xmm1
		unsigned char op[] = { 0x66,0x0f,0x28,0xc8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmovge%eax,%r13d
		unsigned char op[] = { 0x44,0x0f,0x4d,0xe8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmovs %eax,%ebp
		unsigned char op[] = { 0x0f,0x48,0xe8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//cmovl %ebx,%r12d
		unsigned char op[] = { 0x44,0x0f,0x4c,0xe3 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//xorpd %xmm0,%xmm0
		unsigned char op[] = { 0x66,0x0f,0x57,0xc0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//subsd %xmm1,%xmm0
		unsigned char op[] = { 0xf2,0x0f,0x5c,0xc1 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//ucomisd%xmm0,%xmm1
		unsigned char op[] = { 0x66,0x0f,0x2e,0xc8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmova %rax,%rcx
		unsigned char op[] = { 0x48,0x0f,0x47,0xc8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//movsd %xmm0,5110086(%rip)       
		unsigned char op[] = { 0xf2,0x0f,0x11,0x05,0x46,0xf9,0x4d,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//addsd 5110094(%rip),%xmm0       
		unsigned char op[] = { 0xf2,0x0f,0x58,0x05,0x4e,0xf9,0x4d,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//cmovae%ecx,%eax
		unsigned char op[] = { 0x0f,0x43,0xc1 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//cmp   $0x3dc,%ax
		unsigned char op[] = { 0x66,0x3d,0xdc,0x03 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmp   $0x100,%rax
		unsigned char op[] = { 0x48,0x3d,0x00,0x01,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 6);
	}
	{//mov   $0x626d656d204c5453,%rdx
		unsigned char op[] = { 0x48,0xba,0x53,0x54,0x4c,0x20,0x6d,0x65,0x6d,0x62 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 10);
	}
	{//movaps%xmm7,0xfffffffffffffff1(%rax)
		unsigned char op[] = { 0x0f,0x29,0x78,0xf1 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//add   $0xa5c020,%rax
		unsigned char op[] = { 0x48,0x05,0x20,0xc0,0xa5,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 6);
	}
	{//cmovle%eax,%esi
		unsigned char op[] = { 0x0f,0x4e,0xf0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//mulsd %xmm1,%xmm0
		unsigned char op[] = { 0xf2,0x0f,0x59,0xc1 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmovg %eax,%edx
		unsigned char op[] = { 0x0f,0x4f,0xd0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//divsd 2579023(%rip),%xmm0       
		unsigned char op[] = { 0xf2,0x0f,0x5e,0x05,0x4f,0x5a,0x27,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 8);
	}
	{//movsd 0x38(%rsp),%xmm1
		unsigned char op[] = { 0xf2,0x0f,0x10,0x4c,0x24,0x38 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 6);
	}
	{//cvtsi2sd%eax,%xmm0
		unsigned char op[] = { 0xf2,0x0f,0x2a,0xc0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//mov   $0x68d5c0,%r8
		unsigned char op[] = { 0x49,0xc7,0xc0,0xc0,0xd5,0x68,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 7);
	}
	{//movslq%r12d,%rax
		unsigned char op[] = { 0x49,0x63,0xc4};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 3);
	}
	{//repzretq
		unsigned char op[] = { 0xf3,0xc3};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 2);
	}
	{//nopl  0x0(%rax)
		unsigned char op[] = { 0x0f,0x1f,0x40,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}
	{//cmovne0xffffffa4(%ebp),%edx
		unsigned char op[] = { 0x0f,0x45,0x55,0xa4};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x64) ) == 4);
	}

	{//pand  %mm1,%mm2
		unsigned char op[] = { 0x0f,0xdb,0xd1};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//cmove %edx,%edi
		unsigned char op[] = { 0x0f,0x44,0xfa};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//punpckhbw0xffffffe8(%ebp),%mm4
		unsigned char op[] = { 0x0f,0x68,0x65,0xe8};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//pxor  %mm7,%mm0
		unsigned char op[] = { 0x0f,0xef,0xc7};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//por   %mm1,%mm0
		unsigned char op[] = { 0x0f,0xeb,0xc1};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//psllq $0x30,%mm0
		unsigned char op[] = { 0x0f,0x73,0xf0,0x30};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//packuswb0xfffffff0(%ebp),%mm0
		unsigned char op[] = { 0x0f,0x67,0x45,0xf0};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//psrlw $0x8,%mm0
		unsigned char op[] = { 0x0f,0x71,0xd0,0x08};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//paddusw%mm2,%mm1
		unsigned char op[] = { 0x0f,0xdd,0xca};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//pmullw%mm0,%mm1
		unsigned char op[] = { 0x0f,0xd5,0xc8};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//punpcklbw0xfffffff0(%ebp),%mm0
		unsigned char op[] = { 0x0f,0x60,0x45,0xf0};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//movd  %mm0,(%esi)
		unsigned char op[] = { 0x0f,0x7e,0x06};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//movd  (%esi),%mm1
		unsigned char op[] = { 0x0f,0x6e,0x0e};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//paddusb(%ecx),%mm0
		unsigned char op[] = { 0x0f,0xdc,0x01};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//emms
		unsigned char op[] = { 0x0f,0x77};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//movq  %mm0,(%edx)
		unsigned char op[] = { 0x0f,0x7f,0x02};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//movq  (%ecx),%mm0
		unsigned char op[] = { 0x0f,0x6f,0x01};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//cpuid
		unsigned char op[] = { 0x0f,0xa2};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//setl 0xffffffd7(%ebp)
		unsigned char op[] = { 0x0f,0x9c,0x45,0xd7};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//setge0xffffffe3(%ebp)
		unsigned char op[] = { 0x0f,0x9d,0x45,0xe3};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//setle 0xfffffff3(%ebp)
		unsigned char op[] = { 0x0f,0x9e,0x45,0xf3};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//bt    %ecx,0x83ff88c(,%edx,4)
		unsigned char op[] = { 0x0f,0xa3,0x0c,0x95,0x8c,0xf8,0x3f,0x08};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 8);
	}
	{//btr   %eax,0x83ff98c(,%edx,4)
		unsigned char op[] = { 0x0f,0xb3,0x04,0x95,0x8c,0xf9,0x3f,0x08};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 8);
	}
	{//shrd  %cl,%edx,%eax
		unsigned char op[] = { 0x0f,0xad,0xd0};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{ //shld  %cl,%eax,%edx
		unsigned char op[] = { 0x0f,0xa5,0xc2};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//shrd  $0x1e,%esi,%ebx
		unsigned char op[] = { 0x0f,0xac,0xf3,0x1e};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//and   $0xfc0,%ax
		unsigned char op[] = { 0x66,0x25,0xc0,0x0f};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{ //bsr   %edx,%eax
		unsigned char op[] = { 0x0f,0xbd,0xc2};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//testw $0x8400,0xfffffedc(%ebp)
		unsigned char op[] = { 0x66,0xf7,0x85,0xdc,0xfe,0xff,0xff,0x00,0x84};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 9);
	}
	{//shll  %cl,0xffffffd8(%ebp)
		unsigned char op[] = { 0xd3,0x65,0xd8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//sar         eax,cl  
		unsigned char op[] = {0xd3, 0xf8};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//notl  0x10(%ebp)
		unsigned char op[] = { 0xf7,0x55,0x10 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//shll  0x1c(%esi)
		unsigned char op[] = { 0xd1,0x66,0x1c };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//shr     eax,1
		unsigned char op[] = { 0xd1,0xe8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//setg  0xfffffe4f(%ebp)
		unsigned char op[] = { 0x0f,0x9f,0x85,0x4f,0xfe,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov   $0x8,%di
		unsigned char op[] = { 0x66,0xbf,0x08,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//mov     ax,[L00503afc]
		unsigned char op[] = { 0x66,0xa1,0xfc,0x3a,0x50,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//movl  $0x82fab38,0x84af290
		unsigned char op[] = { 0xc7,0x05,0x90,0xf2,0x4a,0x08,0x38,0xab,0x2f,0x08 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 10);
	}
	{//mov     dword [edx],dword 00000000bh
		unsigned char op[] = { 0xc7,0x02,0x0b,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov     dword [esp+020h],dword 000000000h
		unsigned char op[] = { 0xc7,0x44,0x24,0x20,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 8);
	}
	{//mov     dword [L009c384c],dword 000000002h
		unsigned char op[] = { 0xc7,0x05,0x4c,0x38,0x9c,0x00,0x02,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 10);
	}
	{//mov     dword [ebp-004h],dword 0fffffffeh
		unsigned char op[] = { 0xc7,0x45,0xfc,0xfe,0xff,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//insw
		unsigned char op[] = { 0x6d };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//lahf
		unsigned char op[] = { 0x9f };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//bound   ecx,[esp+ebp*2+07a8c788ch]
		unsigned char op[] = { 0x62,0x8c,0x6c,0x8c,0x78,0x8c,0x7a };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//xchg    eax,esp
		unsigned char op[] = { 0x94 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//cvttps2pi mm0,[esp]
		unsigned char op[] = { 0x0f,0x2c,0x04,0x24 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//leave
		unsigned char op[] = { 0xc9 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//pop     dword [L007d57e0]
		unsigned char op[] = { 0x8f,0x05,0xe0,0x57,0x7d,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//pushf
		unsigned char op[] = { 0x9c };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//xchg    eax,ecx
		unsigned char op[] = { 0x91 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//rep
		unsigned char op[] = { 0xf3 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//repne
		unsigned char op[] = { 0xf2 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//rep stos    dword ptr es:[edi]  
		unsigned char op[] = { 0xf3, 0xab };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//movsb
		unsigned char op[] = { 0xa4 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//movsw
		unsigned char op[] = { 0x66,0xa5 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov     word [edi+000000100h],word 00000h
		unsigned char op[] = { 0x66,0xc7,0x87,0x00,0x01,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 9);
	}
	{//imul    ax,dx
		unsigned char op[] = { 0x66,0x0f,0xaf,0xc2 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//mov     al,[L007d60c0]
		unsigned char op[] = { 0xa0,0xc0,0x60,0x7d,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//test    al,byte 008h
		unsigned char op[] = { 0xa8,0x08 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov     [L007d1efd],al
		unsigned char op[] = { 0xa2,0xfd,0x1e,0x7d,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{ //movsx   esi,word [eax+010h]
		unsigned char op[] = { 0x0f,0xbf,0x70,0x10 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//fadd    dword [S007bca20]
		unsigned char op[] = { 0xd8,0x05,0x20,0xca,0x7b,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//fild    dword [edx+00ch]
		unsigned char op[] = { 0xdb,0x42,0x0c };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//imul    eax,eax,dword 0000f4240h
		unsigned char op[] = { 0x69,0xc0,0x40,0x42,0x0f,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//lea     ebx,[eax*4+000000000h]
		unsigned char op[] = { 0x8d,0x1c,0x85,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//lea     edx,[ecx*4+000000003h]
		unsigned char op[] = { 0x8d,0x14,0x8d,0x03,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//lea     eax,[edx+edx*2]
		unsigned char op[] = { 0x8d,0x04,0x52 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//lea     ecx,[edi+edi*4+002h]
		unsigned char op[] = { 0x8d,0x4c,0xbf,0x02 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//lea         ecx,[win]  
		unsigned char op[] = {0x8d,0x8d,0x1c,0xfc,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//lea     ecx,[04e8d108dh]
		unsigned char op[] = {0x8d,0x0d,0x8d,0x10,0x8d,0x4e};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//lea     ecx,[ebp+0fffffb6ch]
		unsigned char op[] = {0x8d,0x8d ,0x6c ,0xfb ,0xff ,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//lea     eax,[esp]
		unsigned char op[] = {0x8d,0x04,0x24};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//lea     eax,[esp+004h]
		unsigned char op[] = {0x8d,0x44,0x24,0x04};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//test    eax,dword 040000000h
		unsigned char op[] = { 0xa9,0x00,0x00,0x00,0x40 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//fld     dword [ecx+008h]
		unsigned char op[] = { 0xdd,0x41,0x08 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//fld     dword [L007bc938]
		unsigned char op[] = { 0xdd,0x05,0x38,0xc9,0x7b,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//fstp    st(1)
		unsigned char op[] = { 0xdd,0xd9 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov     bl,byte 001h
		unsigned char op[] = { 0xb3,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//and     eax,dword 0000001c0h
		unsigned char op[] = { 0x25,0xc0,0x01,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//setl    al
		unsigned char op[] = { 0x0f,0x9c,0xc0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov     al,byte 001h
		unsigned char op[] = { 0xb0,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//bts     dword [ecx],byte 01eh
		unsigned char op[] = { 0x0f,0xba,0x29,0x1e };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}

	{//mul     dword [ebp+008h]
		unsigned char op[] = { 0xf7,0x65,0x08 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//imul    esi,esi,byte 054h 
		unsigned char op[] = { 0x6b,0xf6,0x54 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov     esi,dword [ecx+edx*4]
		unsigned char op[] = { 0x8b,0x34,0x91 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//setnz   byte [ebp-019h]
		unsigned char op[] = { 0x0f,0x95,0x45 ,0xe7 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//setnz   al
		unsigned char op[] = { 0x0f,0x95,0xc0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//xadd    dword [eax],ecx
		unsigned char op[] = { 0x0f,0xc1,0x08 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//cmp     byte [ebp+010h],byte 000h
		unsigned char op[] = { 0x80,0x7d,0x10,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}

	{//test    byte [ebp+008h],byte 001h
		unsigned char op[] = { 0xf6,0x45,0x08,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//test    byte [ebx],byte 008h 
		unsigned char op[] = {0xf6 ,0x03 ,0x08};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//shld    edx,eax,byte 002h
		unsigned char op[] = {0x0f,0xa4,0xc2,0x02};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//test        ecx,7Bh  
		unsigned char op[] = {0xf7 ,0xc1 ,0x7b ,0x00 ,0x00 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//neg     eax
		unsigned char op[] = { 0xf7,0xd8 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//cmp     eax,dword 011e1a300h
		unsigned char op[] = { 0x3d,0x00,0xa3,0xe1,0x11 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//jnz     near C00401c8a      
		unsigned char op[] = { 0x0f,0x85,0xaf,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//jng     near C0040229e
		unsigned char op[] = { 0x0f,0x8e,0x2a,0x01,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
 
	{//setz    byte [ebp-049h]
		unsigned char op[] = { 0x0f,0x94,0x45,0xb7 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//setz    al
		unsigned char op[] = { 0x0f,0x94,0xc0 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//setz    byte [esp+013h]
		unsigned char op[] = { 0x0f,0x94,0x44,0x24,0x13 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//setz    byte [ebp+0ffffff7fh]
		unsigned char op[] = { 0x0f,0x94,0x85,0x7f,0xff,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//jz      near C0047d2f2
		unsigned char op[] = { 0x0f,0x84,0x9b,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//inc     ecx
		unsigned char op[] = { 0x41 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//mov     dl,[ecx]
		unsigned char op[] = { 0x8a,0x11 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov     byte [ebp-004h],byte 001h
		unsigned char op[] = { 0xc6,0x45,0xfc,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 4);
	}
	{//mov     byte [eax],byte 025h
		unsigned char op[] = { 0xc6,0x00,0x25 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov     byte [esi+ebx+010h],byte 000h
		unsigned char op[] = { 0xc6,0x44,0x1e,0x10,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov     byte [ebp+0ffffdf80h],byte 000h
		unsigned char op[] = { 0xc6,0x85,0x80,0xdf,0xff,0xff,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov     byte [edi+edx+001h],byte 000h
		unsigned char op[] = { 0xc6,0x44,0x17,0x01,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov     [fs:000000000h],eax
		unsigned char op[] = { 0x64,0xa3,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov     dword [fs:000000000h],ecx
		unsigned char op[] = { 0x64,0x89,0x0d,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov     eax,[fs:000000000h]
		unsigned char op[] = { 0x64,0xa1,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//push    byte -001h
		unsigned char op[] = { 0x68,0x2c,0xe3,0x4e,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//push    byte -001h
		unsigned char op[] = { 0x6a,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//ret     word 00004h
		unsigned char op[] = { 0xc2,0x04,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//call        MainWindow::BlockMessageLoop (0FC1C20h)  
		unsigned char op[] = {0xe8,0xbd,0x1b,0x00,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov         dword ptr [ebp-614h],eax   
		unsigned char op[] = {0x89,0x85,0xec,0xf9,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         eax,dword ptr [ebp-614h] 
		unsigned char op[] = {0x8b ,0x85 ,0xec ,0xf9 ,0xff ,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//jmp         __catch$_WinMain@16$0+67h (0FC00DBh)  
		unsigned char op[] = {0xeb ,0x67};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov         ecx,dword ptr [e]  
		unsigned char op[] = {0x8b, 0x8d, 0x10, 0xfc, 0xff, 0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//sar	eax,0x0a
		unsigned char op[] = {0xc1, 0xf8,0x0a};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov         dword ptr [eax],offset SexyTestRunner::`vftable' (116C6ACh)  
		unsigned char op[] = {0xc7, 0x00, 0xac, 0xc6, 0x16, 0x01};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}	
	{//mov         ebp,esp  
		unsigned char op[] = {0x8b,0xec};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//mov         edx,dword ptr [`WinMain'::`2'::__LINE__Var (0E8AA98h)]  
		unsigned char op[] = {0x8b ,0x15 ,0x98 ,0xaa ,0xe8 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov       dword ptr [edx],eax 
		unsigned char op[] = {0x89,0x02};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//push        ebp 
		unsigned char op[] = {0x55};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//sub         esp,900h
		unsigned char op[] = { 0x81 ,0xec ,0x00 ,0x09 ,0x00 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//push        esi
		unsigned char op[] = { 0x56};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//push        edi
		unsigned char op[] = { 0x57};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//lea         edi,[ebp-900h] 
		unsigned char op[] = { 0x8d ,0xbd ,0x00 ,0xf7 ,0xff ,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         ecx,240h 
		unsigned char op[] = { 0xb9 ,0x40 ,0x02 ,0x00 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov         eax,0CCCCCCCCh  
		unsigned char op[] = { 0xb8 ,0xcc ,0xcc ,0xcc ,0xcc };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov         eax,dword ptr [___security_cookie (2F9754h)]   
		unsigned char op[] = { 0xa1 ,0x54 ,0x97 ,0x2f ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov         dword ptr [ebp-4],eax 
		unsigned char op[] = { 0x89 ,0x45 ,0xfc};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//add         esp,4  
		unsigned char op[] = {0x83,0xc4,0x04};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
    {//add       eax,ecx 
		unsigned char op[] = {0x03 ,0xC1};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
    {//mov eax,1
		unsigned char op[] = {0xB8,0x01,0x00,0x00,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
    {//mov       ecx,2
		unsigned char op[] = {0xB9 ,0x02 ,0x00 ,0x00 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}


	{//mov         dword ptr [ebp-400h],0 
		unsigned char op[] = {0xc7,0x85 ,0x00 ,0xfc ,0xff ,0xff ,0x00 ,0x00 ,0x00 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 10);
	}
	{//movzx       ecx,al
		unsigned char op[] = {0x0f,0xb6,0xc8};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//sar         byte ptr [ebx],cl
		unsigned char op[] = {0xd2, 0x3b};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//test        ecx,ecx  
		unsigned char op[] = {0x85,0xc9};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//jne         WinMain+0FBh (9EFFABh)
		unsigned char op[] = {0x75,0x26};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 2);
	}
	{//lea         eax,[ebp-4C8h]  
		unsigned char op[] = {0x8d ,0x85 ,0x38 ,0xfb ,0xff ,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//push        eax    
		unsigned char op[] = {0x50};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//call        boost::this_thread::get_id (0B4CA90h) 
		unsigned char op[] = {0xe8 ,0x86 ,0xcb ,0x15 ,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//mov         dword ptr [ebp-614h],eax
		unsigned char op[] = {0x89,0x85,0xec,0xf9,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         ecx,dword ptr [ebp-614h]
		unsigned char op[] = {0x8b,0x8d,0xec,0xf9,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         dword ptr [ebp-618h],ecx
		unsigned char op[] = {0x89,0x8d,0xe8,0xf9,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         dword ptr [ebp-4],0  
		unsigned char op[] = {0xc7,0x45,0xfc,0x00,0x00,0x00,0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov         edx,dword ptr [ebp-4BCh]  
		unsigned char op[] = {0x8b,0x95,0x44,0xfb,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//or          edx,1    
		unsigned char op[] = {0x83,0xca,0x01};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov         dword ptr [ebp-4BCh],edx   
		unsigned char op[] = {0x89,0x95,0x44,0xfb,0xff,0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//lea         eax,[ebp-4D4h]  
		unsigned char op[] = {0x8d,0x85,0x2c,0xfb,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//push        eax   
		unsigned char op[] = {0x50 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}
	{//call        XLDebugUtil::GetMainThreadID (9CC1B0h)   
		unsigned char op[] = { 0xe8 ,0x6f ,0xc2 ,0xfd ,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}
	{//add         esp,4
		unsigned char op[] = { 0x83, 0xc4 ,0x04 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}
	{//mov         dword ptr [ebp-614h],eax  
		unsigned char op[] = { 0x89,0x85,0xec,0xf9,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         ecx,dword ptr [ebp-614h]  
		unsigned char op[] = { 0x8b, 0x8d, 0xec, 0xf9, 0xff, 0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         dword ptr [ebp-618h],ecx   
		unsigned char op[] = { 0x89, 0x8d, 0xe8, 0xf9, 0xff, 0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//mov         dword ptr [ebp-4],0  
		unsigned char op[] = { 0xc7,0x45,0xfc,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov         dword ptr [ebp-4],0  
		unsigned char op[] = { 0xc7,0x45,0xfc,0x00,0x00,0x00,0x00 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	{//mov         edx,dword ptr [ebp-4BCh]  
		unsigned char op[] = { 0x8b,0x95,0x44,0xfb,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}
	{//or          edx,1  
		unsigned char op[] = { 0x83,0xca,0x01 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}

	{//mov         dword ptr [ebp-4BCh],edx  
		unsigned char op[] = { 0x89,0x95,0x44,0xfb,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}

	{//lea         eax,[ebp-4D4h]  
		unsigned char op[] = { 0x8d,0x85,0x2c,0xfb,0xff,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}

	{//push        eax  
		unsigned char op[] = { 0x50 };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 1);
	}

	{//call        XLDebugUtil::GetMainThreadID (9CC1B0h)  
		unsigned char op[] = { 0xe8 ,0x6f ,0xc2 ,0xfd ,0xff };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 5);
	}

	{//add         esp,4  
		unsigned char op[] = { 0x83,0xc4,0x04  };
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 3);
	}

	{//mov         ecx,dword ptr [ebp-61Ch]  
		unsigned char op[] = { 0x8b, 0x8d, 0xe4, 0xf9, 0xff, 0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}

	{//mov         dword ptr [ebp-620h],ecx  
		unsigned char op[] = { 0x89, 0x8d, 0xe0, 0xf9, 0xff, 0xff};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 6);
	}

	{//mov         dword ptr [ebp-4],1  
		unsigned char op[] = { 0xc7, 0x45, 0xfc, 0x01, 0x00, 0x00, 0x00};
		OPLEN_ASSERT((len = SEXYHOOKFuncBase::OperandLength(op,x86) ) == 7);
	}
	dumpoptestObjectDump("test/gcc.asm",x86);
	dumpoptestObjectDump("test/gdb.asm",x86);
	dumpoptestObjectDump("test/lvm.asm",x86);
	dumpoptestObjectDump("test/vim.asm",x86);
	dumpoptestObjectDump("test/Xvfb.asm",x86);
	dumpoptestObjectDump("test/doxygen.asm",x86);
	dumpoptest("test/naichichi2.exe.asm",x86);

	dumpoptestObjectDump("test/x64.doxygen.asm",x64);
	dumpoptestObjectDump("test/x64.gdb.asm",x64);
	dumpoptestObjectDump("test/x64.httpd.asm",x64);
	dumpoptestObjectDump("test/x64.memcached.asm",x64);
	dumpoptestObjectDump("test/x64.mysql.asm",x64);
	dumpoptestObjectDump("test/x64.php.asm",x64);
	dumpoptestObjectDump("test/x64.sshd.asm",x64);
	dumpoptestObjectDump("test/x64.Xvfb.asm",x64);

	dumpoptestObjectDump("test/x64.bash.asm",x64);
	dumpoptestObjectDump("test/x64.libcabocha.a.asm",x64);
	dumpoptestObjectDump("test/x64.qemu.asm",x64);
	dumpoptestObjectDump("test/x64.wine_shell32.dll.so.asm",x64);
	dumpoptestObjectDump("test/x64.wine_wined3d.dll.so.asm",x64);
}


int main()
{
	oplengthtest();

	
	puts("ok");
	return 0;
}
