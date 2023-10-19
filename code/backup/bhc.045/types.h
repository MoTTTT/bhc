/*			Copyright Q Solutions				*/
/*	File:		types.h						*/
/*	Programmer:	MoT						*/
/*	Project:	uNode Libraries					*/
/*	Module:		Compile time type optimisations			*/
/*									*/
/*									*/

/* 		Optimise generic types for size & speed			*/
#ifndef BYTETYPE
#define BYTETYPE
typedef	unsigned char byte;		/* Unsigned 8 bits: byte	*/
#endif
#ifndef UINTTYPE
#define UINTTYPE
typedef	unsigned int uint;		/* Unsigned 16 bits: uint	*/
#endif
#ifndef ULONGTYPE
#define ULONGTYPE
typedef	unsigned long ulong;		/* Optimise 32 bits: ulong	*/
#endif
