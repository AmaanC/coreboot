/* 

LinuxBIOS interface to memory-saving variant of LZMA decoder
(C)opyright 2006 Carl-Daniel Hailfinger
Released under the GNU GPL

*/

#include "lzmadecode.c"
#include "string.h"
#include "console.h"


unsigned long ulzma(u8 *src, u8 *dst)
{
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	UInt32 outSize;
	SizeT inProcessed;
	SizeT outProcessed;
	int res;
	CLzmaDecoderState state;
	SizeT mallocneeds;
	unsigned char scratchpad[15980];

	memcpy(properties, src, LZMA_PROPERTIES_SIZE);
	outSize = *(UInt32 *)(src + LZMA_PROPERTIES_SIZE);
	if (LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
		printk(BIOS_WARNING, "Incorrect stream properties\n");
	}
	mallocneeds = (LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (mallocneeds > 15980) {
		printk(BIOS_WARNING, "Decoder scratchpad too small!\n");
	}
	state.Probs = (CProb *)scratchpad;
	res = LzmaDecode(&state, src + LZMA_PROPERTIES_SIZE + 8, (SizeT)0xffffffff, &inProcessed,
		dst, outSize, &outProcessed);
	if (res != 0) {
		printk(BIOS_WARNING, "Decoding error = %d\n", res);
	}
	return outSize;
}
