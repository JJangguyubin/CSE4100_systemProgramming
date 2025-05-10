#include "bitmap.h"
#include <assert.h>	
#include "limits.h"	// 		#include <limits.h>
#include "round.h"	// 		#include <round.h>
#include <stdio.h>
#include <stdlib.h>	


#include "hex_dump.h"	
#define ASSERT(CONDITION) assert(CONDITION)	

/* Element type.

   This must be an unsigned integer type at least as wide as int.

   Each bit represents one bit in the bitmap.
   If bit 0 in an element represents bit K in the bitmap,
   then bit 1 in the element represents bit K+1 in the bitmap,
   and so on. */
/*

I think bit 0 in an element here? ==>> '0'00.... .... .....000
But, I'm not sure right now...

If It is right, then we can say that
bit 0 in an element ==>> bit K in an bitmap.
bit 1 in an element ==>> bit K + 1 in an bitmap.
and so on...
.... .... .....

*/

typedef unsigned long elem_type;
/*
As you know, unsigned long is 4Bytes ==>> 32Bits.
*/

/* Number of bits in an element. */
#define ELEM_BITS (sizeof (elem_type) * CHAR_BIT)
/*
ELEM_BITS = 32; ==>> ,that is, each elem_type can store 32 bits...
*/

/* From the outside, a bitmap is an array of bits.  From the
   inside, it's an array of elem_type (defined above) that
   simulates an array of bits. */
struct bitmap
  {
    size_t bit_cnt;     /* Number of bits. */
    elem_type *bits;    /* Elements that represent bits. */
    // Maybe... I think that by using realloc() or malloc(), we can expand it... 
    // But, I'm not sure at now...
  };

/* Returns the index of the element that contains the bit
   numbered BIT_IDX. */
static inline size_t
elem_idx (size_t bit_idx) 
{
  return bit_idx / ELEM_BITS;
}
/*
elem_idx() : bit Idx가 속한 elem을 찾아서 elem Idx로 return.
*/
/*

Let's draw it...

[ 0 | 1 | 0 | 1 |  elem 0 | 1 | 0 | 1 | 0 ][ elem 1 ][ elem 2 ][ elem 3 ], and so on...
&&
each elem Idx has 32 bits...

*/

/* Returns an elem_type where only the bit corresponding to
   BIT_IDX is turned on. */
static inline elem_type
bit_mask (size_t bit_idx) 
{
  return (elem_type) 1 << (bit_idx % ELEM_BITS);
}
/*
,that is,
If bit_idx == 0, then 000.... .... .....001 .
If bit_idx == 1, then 000.... .... .....010 .
and so on...

So, we can say that bit 0 in element : 000.... .... .....00'0' <<== here... .

finally(?), we can say that bit_mask(size_t bit_idx) : 해당 bit_idx가 0 or 1인지 find하기 위해 사용될 것입니다.
*/

/* Returns the number of elements required for BIT_CNT bits. */
static inline size_t
elem_cnt (size_t bit_cnt)
{
  return DIV_ROUND_UP (bit_cnt, ELEM_BITS);
}
/*
, that is, 
elem_cnt() : Parameter로 들어온 bit_cnt 개의 bit들을 저장 및 사용하기 위해 필요한 elem의 갯수인 elem_cnt를 return해줌.
*/

/* Returns the number of bytes required for BIT_CNT bits. */
static inline size_t
byte_cnt (size_t bit_cnt)
{
  return sizeof (elem_type) * elem_cnt (bit_cnt);
}
/*
, that is,
byte_cnt() : Parameter로 들어온 bit_cnt 개의 bit들을 저장 및 사용하기 위해 필요한 byte들의 갯수인 byte_cnt를 return해줌.
*/

/* Returns a bit mask / in which / the bits actually used in the last
   element of B's bits are set to 1 and the rest are set to 0. */
static inline elem_type
last_mask (const struct bitmap *b) 
{
  int last_bits = b->bit_cnt % ELEM_BITS; /* ELEM_BITS = 32; */
  return last_bits ? ((elem_type) 1 << last_bits) - 1 : (elem_type) -1;
}
/*

(ex. If b->bit_cnt == 37, Then
last_bits = 5;
, Then By doing ((elem_type) 1 << last_bits)
000.... .... .....100000
, Then By doing -1 
000.... .... .....011111

and so on...

So, I think that last bit Idx의 값만 0을 가지고, 해당 elem에서 Idx - 1 ~ 0 부분은 값을 1로 가지게 해서,
and so on...
last bit의 Value가 0 인지 1 인지 알아내기 위함인가...

*/

/* Creation and destruction. */

/* Initializes B to be a bitmap of BIT_CNT bits
   and sets all of its bits to false.
   Returns true if success, false if memory allocation
   failed. */
struct bitmap *
bitmap_create (size_t bit_cnt) 
{
  struct bitmap *b = malloc (sizeof *b);
  if (b != NULL)
    {
      b->bit_cnt = bit_cnt;
      b->bits = malloc (byte_cnt (bit_cnt)); /* byte_cnt(bit_cnt) : 해당 bit_cnt 개의 bit들을 저장 및 운영하기 위해 필요한 byte_cnt를 return. */
      if (b->bits != NULL || bit_cnt == 0)
        {
          bitmap_set_all (b, false);
          return b;
        }
      free (b);
    }
  return NULL;
}
/* 
즉, bit_cnt를 저장 및 운영하려고 하는 Initial bitmap을 Creation한 뒤에, 모든 value를 false(or 0)으로 reset...
*/

/* Creates and returns a bitmap with BIT_CNT bits in the
   BLOCK_SIZE bytes of storage preallocated at BLOCK.
   BLOCK_SIZE must be at least bitmap_needed_bytes(BIT_CNT). */
struct bitmap *
bitmap_create_in_buf (size_t bit_cnt, void *block, size_t block_size )
	// Remove KERNEL MACRO 'UNUSED')
{
  struct bitmap *b = block;
  
  ASSERT (block_size >= bitmap_buf_size (bit_cnt));

  b->bit_cnt = bit_cnt;
  b->bits = (elem_type *) (b + 1);
  bitmap_set_all (b, false);
  return b;
}
/*

즉, 해당 func()은 Parameter로 들어온 block_size의 block 안에 bit_cnt 개를 운용하는 bitmap을 Creation...

(block) [[bit_cnt][*bits][||||||||||||.... Content... .... .....]]의 구조인가...
, that is, 
*bits ~~>> Content .

*/

/* Returns the number of bytes required to accomodate a bitmap
   with BIT_CNT bits (for use with bitmap_create_in_buf()). */
size_t
bitmap_buf_size (size_t bit_cnt) 
{
  return sizeof (struct bitmap) + byte_cnt (bit_cnt);
} /* bitmap_buf_size() : bit_cnt 개의 bit들을 운영 및 관리하기 위한 bitmap을 buf에서 운용하고자 할 때, 그 buf의 size를 return. */

/* Destroys bitmap B, freeing its storage.
   Not for use on bitmaps created by
   bitmap_create_preallocated(). */
void
bitmap_destroy (struct bitmap *b) 
{
  if (b != NULL) 
    {
      free (b->bits);
      free (b);
    }
} /* bitmap_destory() : 이건 그냥 memoryDeallocation할 때, 쓰일 듯 합니다. */

/* Bitmap size. */

/* Returns the number of bits in B. */
size_t
bitmap_size (const struct bitmap *b)
{
  return b->bit_cnt;
}

/* Setting and testing single bits. */

/* Atomically sets the bit numbered IDX in B to VALUE. */
void
bitmap_set (struct bitmap *b, size_t idx, bool value) 
{
  ASSERT (b != NULL);
  ASSERT (idx < b->bit_cnt);
  if (value)
    bitmap_mark (b, idx);
  else
    bitmap_reset (b, idx);
}

/* Atomically sets the bit numbered BIT_IDX in B to true. */
void
bitmap_mark (struct bitmap *b, size_t bit_idx)  /* ,that is, bit_idx position의 value를 1로 turn on...  */
{
  size_t idx = elem_idx (bit_idx);
  elem_type mask = bit_mask (bit_idx);

  /* This is equivalent to `b->bits[idx] |= mask' except that it
     is guaranteed to be atomic on a uniprocessor machine.  See
     the description of the OR instruction in [IA32-v2b]. */
  asm ("orl %k1, %k0" : "=m" (b->bits[idx]) : "r" (mask) : "cc");
}

/* Atomically sets the bit numbered BIT_IDX in B to false. */
void
bitmap_reset (struct bitmap *b, size_t bit_idx) /* bitmap[bit_idx] : 1 ==>> 0. */
{
  size_t idx = elem_idx (bit_idx);
  elem_type mask = bit_mask (bit_idx);

  /* This is equivalent to `b->bits[idx] &= ~mask' except that it
     is guaranteed to be atomic on a uniprocessor machine.  See
     the description of the AND instruction in [IA32-v2a]. */
  asm ("andl %k1, %k0" : "=m" (b->bits[idx]) : "r" (~mask) : "cc");
}

/* Atomically toggles the bit numbered IDX in B;
   that is, if it is true, makes it false,
   and if it is false, makes it true. */
void
bitmap_flip (struct bitmap *b, size_t bit_idx) /* bitmap_flip() : 0 ==>> 1 || 1 ==>> 0. */
{
  size_t idx = elem_idx (bit_idx);
  elem_type mask = bit_mask (bit_idx);

  /* This is equivalent to `b->bits[idx] ^= mask' except that it
     is guaranteed to be atomic on a uniprocessor machine.  See
     the description of the XOR instruction in [IA32-v2b]. */
  asm ("xorl %k1, %k0" : "=m" (b->bits[idx]) : "r" (mask) : "cc");
}

/* Returns the value of the bit numbered IDX in B. */
bool
bitmap_test (const struct bitmap *b, size_t idx) /* return bitmap[idx]; */
{
  ASSERT (b != NULL);
  ASSERT (idx < b->bit_cnt);
  return (b->bits[elem_idx (idx)] & bit_mask (idx)) != 0;
}

/* Setting and testing multiple bits. */

/* Sets all bits in B to VALUE. */
void
bitmap_set_all (struct bitmap *b, bool value) 
{
  ASSERT (b != NULL);

  bitmap_set_multiple (b, 0, bitmap_size (b), value);
}

/* Sets the CNT bits starting at START in B to VALUE. */
void /* ,that is, start ~~ start + cnt - 1의 Value를 value로 Set. */
bitmap_set_multiple (struct bitmap *b, size_t start, size_t cnt, bool value) 
{
  size_t i;
  
  ASSERT (b != NULL);
  ASSERT (start <= b->bit_cnt);
  ASSERT (start + cnt <= b->bit_cnt);

  for (i = 0; i < cnt; i++)
    bitmap_set (b, start + i, value);
}

/* Returns the number of bits in B between START and START + CNT,
   exclusive, that are set to VALUE. */
size_t
bitmap_count (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
  size_t i, value_cnt;

  ASSERT (b != NULL);
  ASSERT (start <= b->bit_cnt);
  ASSERT (start + cnt <= b->bit_cnt);

  value_cnt = 0;
  for (i = 0; i < cnt; i++)
    if (bitmap_test (b, start + i) == value)
      value_cnt++;
  return value_cnt;
}

/* Returns true if any bits in B between START and START + CNT,
   exclusive, are set to VALUE, and false otherwise. */
bool
bitmap_contains (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
  size_t i;
  
  ASSERT (b != NULL);
  ASSERT (start <= b->bit_cnt);
  ASSERT (start + cnt <= b->bit_cnt);

  for (i = 0; i < cnt; i++)
    if (bitmap_test (b, start + i) == value)
      return true;
  return false;
}
/*
,that is, start ~~ start + cnt - 1의 range에서 value의 값을 가지는 bit가 있는가?...
*/

/* Returns true if any bits in B between START and START + CNT,
   exclusive, are set to true, and false otherwise.*/
bool
bitmap_any (const struct bitmap *b, size_t start, size_t cnt) 
{
  return bitmap_contains (b, start, cnt, true);
} /* ,that is, start ~~ start + cnt -1의 range에서 true로 켜진 bit가 1 개라도 있는가?... */

/* Returns true if no bits in B between START and START + CNT,
   exclusive, are set to true, and false otherwise.*/
bool
bitmap_none (const struct bitmap *b, size_t start, size_t cnt) 
{
  return !bitmap_contains (b, start, cnt, true);
}
/* ,that is, start ~~ start + cnt - 1의 range에서 true로 켜진 bit가 1 개도 없는가?... */

/* Returns true if every bit in B between START and START + CNT,
   exclusive, is set to true, and false otherwise. */
bool
bitmap_all (const struct bitmap *b, size_t start, size_t cnt) 
{
  return !bitmap_contains (b, start, cnt, false);
} /* 해당 range에의 bit들의 value가 모두 true인가?... */

/* Finding set or unset bits. */

/* Finds and returns the starting index of the first group of CNT
   consecutive bits in B at or after START that are all set to
   VALUE.
   If there is no such group, returns BITMAP_ERROR. */
size_t
bitmap_scan (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
  ASSERT (b != NULL);
  ASSERT (start <= b->bit_cnt);

  if (cnt <= b->bit_cnt) 
    {
      size_t last = b->bit_cnt - cnt;
      size_t i;
      for (i = start; i <= last; i++)
        if (!bitmap_contains (b, i, cnt, !value))
          return i; 
    }
  return BITMAP_ERROR;
}

/* Finds the first group of CNT consecutive bits in B at or after
   START that are all set to VALUE, flips them all to !VALUE,
   and returns the index of the first bit in the group.
   If there is no such group, returns BITMAP_ERROR.
   If CNT is zero, returns 0.
   Bits are set atomically, but testing bits is not atomic with
   setting them. */
size_t
bitmap_scan_and_flip (struct bitmap *b, size_t start, size_t cnt, bool value)
{
  size_t idx = bitmap_scan (b, start, cnt, value);
  if (idx != BITMAP_ERROR) 
    bitmap_set_multiple (b, idx, cnt, !value);
  return idx;
}

/* Returns the number of bytes needed to store B in a file. */
size_t
bitmap_file_size (const struct bitmap *b) 
{
  return byte_cnt (b->bit_cnt);
}

/* Debugging. */

/* Dumps the contents of B to the console as hexadecimal. */
void
bitmap_dump (const struct bitmap *b) 
{
  hex_dump (0, b->bits, byte_cnt (b->bit_cnt)/2, false);
}

// ---.

struct bitmap* bitmap_expand(struct bitmap* bitmap, int expandedSize) {
/****************************************************************
- Functionality : Expand the given bitmap to the size (backward expansion).
- Parameter     : Pointer of bitmap that you want to expand and the required size of it.
- Return value  : Pointer of expanded bitmap if succeed, NULL if fail.
/***************************************************************/
    
    // NullPointerException.
    if (bitmap == NULL) {
        return NULL;
    }

    const size_t beforeSize = bitmap_size(bitmap);
    const size_t afterSize = beforeSize + expandedSize;

    // Since this is about Shrinking.
    if (bitmap_size(bitmap) > afterSize) {
        return NULL;
    }

    // In this case, we don't have to expandProcessing.
    if (bitmap_size(bitmap) == afterSize) {
        return bitmap;
    }

    struct bitmap* temp = bitmap_create(afterSize);
    if (temp == NULL) {
        return NULL;
    }

    bitmap_set_multiple(temp, beforeSize, expandedSize, false);
    for (int idx = 0; idx < beforeSize; idx++) {
        const bool isTrue = bitmap_contains(bitmap, idx, 1, true);
        if (isTrue) {
            bitmap_set(temp, idx, true);
        }
        else {
            bitmap_set(temp, idx, false);
        }
    }

    bitmap_destroy(bitmap);
    
    return temp;
} 
/* 
이런 식으로 짜면, 
내부 구조를 잘 몰라도 되지 않을까?... ==>> Encapsulation ?
*/

// ---.