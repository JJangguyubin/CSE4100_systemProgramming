/* Hash table.

See hash.h for basic information. */

#include "hash.h"
#include <assert.h>	
#include <stdlib.h>	

#define ASSERT(CONDITION) assert(CONDITION)

#define list_elem_to_hash_elem(LIST_ELEM)                       \
        list_entry(LIST_ELEM, struct hash_elem, list_elem)
/* 
Convert list_elem to hash_elem... 
*/

static struct list *find_bucket (struct hash *, struct hash_elem *);
static struct hash_elem *find_elem (struct hash *, struct list *,
                                    struct hash_elem *);
static void insert_elem (struct hash *, struct list *, struct hash_elem *);
static void remove_elem (struct hash *, struct hash_elem *);
static void rehash (struct hash *);

/* Initializes hash table H to compute hash values using HASH and
   compare hash elements using LESS, given auxiliary data AUX. */
bool
hash_init (struct hash *h,
           hash_hash_func *hash, hash_less_func *less, void *aux) 
/*
hash_hash_func() : key ~~>> hash .
*/
{
  h->elem_cnt = 0; // elements# ?...
  h->bucket_cnt = 4; // 초기 bucket_cnt는 4로 설정... [0][1][2][3]...
  h->buckets = malloc (sizeof *h->buckets * h->bucket_cnt);
  h->hash = hash; // hash : hashfunc().
  h->less = less; // less : compareFunc().
  h->aux = aux; // don't care...

  if (h->buckets != NULL) 
    {
      hash_clear (h, NULL); // each bucket = NULL;...
      return true;
    }
  else
    return false;
}

/* Removes all the elements from H.
   
   If DESTRUCTOR is non-null, then it is called for each element
   in the hash.  DESTRUCTOR may, if appropriate, deallocate the
   memory used by the hash element.  However, modifying hash
   table H while hash_clear() is running, using any of the
   functions hash_clear(), hash_destroy(), hash_insert(),
   hash_replace(), or hash_delete(), yields undefined behavior,
   whether done in DESTRUCTOR or elsewhere. */

/*
즉, hash_clear() func()를 Call하는 도중에, 
다른 hash관련 func()들을 Call하면 안된다고 합니다.
*/

void
hash_clear (struct hash *h, hash_action_func *destructor) 
{
  size_t i;

  for (i = 0; i < h->bucket_cnt; i++) 
    {
      struct list *bucket = &h->buckets[i];

      if (destructor != NULL) 
        while (!list_empty (bucket)) 
          {
            struct list_elem *list_elem = list_pop_front (bucket);
            struct hash_elem *hash_elem = list_elem_to_hash_elem (list_elem);
            destructor (hash_elem, h->aux);
          }

      list_init (bucket); 
    }    

  h->elem_cnt = 0;
}

/* Destroys hash table H.

   If DESTRUCTOR is non-null, then it is first called for each
   element in the hash.  DESTRUCTOR may, if appropriate,
   deallocate the memory used by the hash element.  However,
   modifying hash table H while hash_clear() is running, using
   any of the functions hash_clear(), hash_destroy(),
   hash_insert(), hash_replace(), or hash_delete(), yields
   undefined behavior, whether done in DESTRUCTOR or
   elsewhere. */
void
hash_destroy (struct hash *h, hash_action_func *destructor) 
{
  if (destructor != NULL)
    hash_clear (h, destructor);
  free (h->buckets);
}
/*
, that is, if you want destroy hashtable, then call hash_destroy() func().
*/

/* Inserts NEW into hash table H and returns a null pointer, if
   no equal element is already in the table.
   If an equal element is already in the table, returns it
   without inserting NEW. */   
struct hash_elem *
hash_insert (struct hash *h, struct hash_elem *new)
{
  struct list *bucket = find_bucket (h, new);
  struct hash_elem *old = find_elem (h, bucket, new);
  /*
  && duplication is not allowed...
  */

  if (old == NULL) 
    insert_elem (h, bucket, new);

  rehash (h);
  // If bucket이 부족할 경우, 새로운 bucket을 add...

  return old; 
}
/*
즉, Parameter로 넘어온 struct hash_elem* new를 hashtable에 Inserting...
*/

/* Inserts NEW into hash table H, replacing any equal element
   already in the table, which is returned. */
struct hash_elem *
hash_replace (struct hash *h, struct hash_elem *new) 
{
  struct list *bucket = find_bucket (h, new);
  struct hash_elem *old = find_elem (h, bucket, new);

  if (old != NULL)
    remove_elem (h, old);
  insert_elem (h, bucket, new);
  /* 
  oldElem이 없었다면, 그냥 newElem 추가하는 것일듯... 
  */

  rehash (h);

  return old;
}
/*
즉,
기존에 hash_table[key] = oldValue가 존재한다고 가정했을 때,
hash_table[key] = newValue로 교체하고 싶을 때,
이용하는 것 같습니다...       
*/

/* Finds and returns an element equal to E in hash table H, or a
   null pointer if no equal element exists in the table. */
struct hash_elem *
hash_find (struct hash *h, struct hash_elem *e) 
{
  return find_elem (h, find_bucket (h, e), e);
}

/* Finds, removes, and returns an element equal to E in hash
   table H.  Returns a null pointer if no equal element existed
   in the table.

   If the elements of the hash table are dynamically allocated,
   or own resources that are, then it is the caller's
   responsibility to deallocate them. */
struct hash_elem *
hash_delete (struct hash *h, struct hash_elem *e)
{
  struct hash_elem *found = find_elem (h, find_bucket (h, e), e);
  if (found != NULL) 
    {
      remove_elem (h, found);
      rehash (h); 
    }
  return found;
}
/*
즉,
hashtable[key] = value; 로 이미 Mapping이 되어있을 때,
hashtable[key] = unknown(or NULL);로 다시 Reset?...
*/

/* Calls ACTION for each element in hash table H in arbitrary
   order. 
   Modifying hash table H while hash_apply() is running, using
   any of the functions hash_clear(), hash_destroy(),
   hash_insert(), hash_replace(), or hash_delete(), yields
   undefined behavior, whether done from ACTION or elsewhere. */
void
hash_apply (struct hash *h, hash_action_func *action) 
{
  size_t i;
  
  ASSERT (action != NULL);

  for (i = 0; i < h->bucket_cnt; i++) 
    {
      struct list *bucket = &h->buckets[i];
      struct list_elem *elem, *next;

      for (elem = list_begin (bucket); elem != list_end (bucket); elem = next) 
        {
          next = list_next (elem);
          action (list_elem_to_hash_elem (elem), h->aux);
        }
    }
}
/*
즉, Parameter로 들어온 hashtable이 있을 때,
printf() or doSomething() func()을 Parameter로 넘겨주면,
해당 func()을 각 elem에 apply하여, 
action을 취하도록 하는 func()인 것 같습니다...
즉, hash_apply() : action(eachElem) ==>> doSomething ==>> output will be created...
*/

/* Initializes I for iterating hash table H.

   Iteration idiom:

      struct hash_iterator i; // C++의 iterator와 동일한 Role을 수행.

      hash_first (&i, h); // iterator가 firstElem(or headElem)을 가리키도록 수정.
      or 각 bucket을 순회하는 iterator... ==>> 걍, hash_next()에서, 알아서 처리를 해줍니다.
      while (hash_next (&i))
        {
          struct foo *f = hash_entry (hash_cur (&i), struct foo, elem);
          ...do something with f...
        }

   Modifying hash table H during iteration, using any of the
   functions hash_clear(), hash_destroy(), hash_insert(),
   hash_replace(), or hash_delete(), invalidates all
   iterators. */
void
hash_first (struct hash_iterator *i, struct hash *h) 
{
  ASSERT (i != NULL);
  ASSERT (h != NULL);

  i->hash = h;
  i->bucket = i->hash->buckets; // bucket means eachBucket?...
  i->elem = list_elem_to_hash_elem (list_head (i->bucket));
}

/* Advances I to the next element in the hash table and returns
   it.  Returns a null pointer if no elements are left.  Elements
   are returned in arbitrary order.

   Modifying a hash table H during iteration, using any of the
   functions hash_clear(), hash_destroy(), hash_insert(),
   hash_replace(), or hash_delete(), invalidates all
   iterators. */
struct hash_elem *
hash_next (struct hash_iterator *i)
{
  ASSERT (i != NULL);

  i->elem = list_elem_to_hash_elem (list_next (&i->elem->list_elem));
  while (i->elem == list_elem_to_hash_elem (list_end (i->bucket))) // eachBucket에 end에 도달했다면, 다음 bucket으로 넘어가는 듯...
    {
      // If iterator reach end?, then i->elem = NULL;로 설정해주고, NULL을 return하도록 처리...
      if (++i->bucket >= i->hash->buckets + i->hash->bucket_cnt)
        {
          i->elem = NULL;
          break;
        }
      i->elem = list_elem_to_hash_elem (list_begin (i->bucket));
    }
  
  return i->elem;
}

/* Returns the current element in the hash table iteration, or a
   null pointer at the end of the table.  Undefined behavior
   after calling hash_first() but before hash_next(). */
struct hash_elem *
hash_cur (struct hash_iterator *i) 
{
  return i->elem;
  // 현재 iterator가 가리키고 있는 hash_elem(list.h에서는 list_item이라고 부르는 듯?...)을 뱉어냄...
  // 이 returnValue를 이용하여, value를 추출해내면 될 듯?...
}

/* Returns the number of elements in H. */
size_t
hash_size (struct hash *h) 
{
  return h->elem_cnt;
}
/*
이건 그냥, hashtable에 등록된 Elem의 Cnt...
*/

/* Returns true if H contains no elements, false otherwise. */
bool
hash_empty (struct hash *h) 
{
  return h->elem_cnt == 0;
} /* Is hashtable empty?...*/

/* Fowler-Noll-Vo hash constants, for 32-bit word sizes. */
#define FNV_32_PRIME 16777619u
#define FNV_32_BASIS 2166136261u

/* Returns a hash of the SIZE bytes in BUF. */
unsigned
hash_bytes (const void *buf_, size_t size)
{
  /* Fowler-Noll-Vo 32-bit hash, for bytes. */
  const unsigned char *buf = buf_;
  unsigned hash;

  ASSERT (buf != NULL);

  hash = FNV_32_BASIS;
  while (size-- > 0)
    hash = (hash * FNV_32_PRIME) ^ *buf++;

  return hash;
} 
// Maybe..., 
// inputBuf with Size = size ~~>> hash_bytes() is executed ~~>> hashValue...

/* Returns a hash of string S. */
unsigned
hash_string (const char *s_) 
{
  const unsigned char *s = (const unsigned char *) s_;
  unsigned hash;

  ASSERT (s != NULL);

  hash = FNV_32_BASIS;
  while (*s != '\0')
    hash = (hash * FNV_32_PRIME) ^ *s++;

  return hash;
}
/*
즉, inputString ~~>> hash_string() ~~>> hashValue...
*/

/* Returns a hash of integer I. */
unsigned
hash_int (int i) 
{
  return hash_bytes (&i, sizeof i);
}
/*
이건, 그냥 int ~~>> hash_int() is executed... ~~>> hashValue... 
*/

// ---.

unsigned int hash_int_2(int i) {
    /******************************************************************
    - Functionality : Implement this in your own way and describe it in the document.
    - Paramter      : Integer that will be hashed.
    - Return Value  : Hash value of integer i.
    *******************************************************************/

    unsigned int hashValue = (unsigned int)i; 
    /* hashValue == bucketID. */
    hashValue ^= (hashValue << 20);
    hashValue ^= (hashValue >> 19);
    hashValue ^= (hashValue << 12);
    hashValue ^= (hashValue >> 7);

    return hashValue;
}
/*
Q. What is hash_func() ?
A. "key ~~>> bucketID(or hashValue)" ~~>> value.
*/

// ---.

/* Returns the bucket in H that E belongs in. */
static struct list *
find_bucket (struct hash *h, struct hash_elem *e) 
{
  size_t bucket_idx = h->hash (e, h->aux) & (h->bucket_cnt - 1);
  return &h->buckets[bucket_idx];
}
/*
즉, struct hash_elem* e가 속한 bucket을 뱉어냄...
*/

/* Searches BUCKET in H for a hash element equal to E.  Returns
   it if found or a null pointer otherwise. */
static struct hash_elem *
find_elem (struct hash *h, struct list *bucket, struct hash_elem *e) 
{
  struct list_elem *i;

  for (i = list_begin (bucket); i != list_end (bucket); i = list_next (i)) 
    {
      struct hash_elem *hi = list_elem_to_hash_elem (i);
      if (!h->less (hi, e, h->aux) && !h->less (e, hi, h->aux))
        return hi; 
    }
  return NULL;
}
/*
Parameter로 들어온 bucket을 탐색하여, Parameter로 들어온 struct hash_elem* e가 존재하는 지 Checking...
*/

/* Returns X with its lowest-order bit set to 1 turned off. */
static inline size_t
turn_off_least_1bit (size_t x) 
{
  return x & (x - 1);
}

/* Returns true if X is a power of 2, otherwise false. */
static inline size_t
is_power_of_2 (size_t x) 
{
  return x != 0 && turn_off_least_1bit (x) == 0;
}

/* Element per bucket ratios. */
#define MIN_ELEMS_PER_BUCKET  1 /* Elems/bucket < 1: reduce # of buckets. */
#define BEST_ELEMS_PER_BUCKET 2 /* Ideal elems/bucket. */
#define MAX_ELEMS_PER_BUCKET  4 /* Elems/bucket > 4: increase # of buckets. */

/* Changes the number of buckets in hash table H to match the
   ideal(이상적이게?...).  This function can fail because of an out-of-memory
   condition, but that'll just make hash accesses less efficient;
   we can still continue. ==>> 즉, 내부구현은 don't care... */
static void
rehash (struct hash *h) 
{
  size_t old_bucket_cnt, new_bucket_cnt;
  struct list *new_buckets, *old_buckets;
  size_t i;

  ASSERT (h != NULL);

  /* Save old bucket info for later use. */
  old_buckets = h->buckets;
  old_bucket_cnt = h->bucket_cnt;

  /* Calculate the number of buckets to use now.
     We want one bucket for about every BEST_ELEMS_PER_BUCKET.
     We must have at least four buckets, and the number of
     buckets must be a power of 2. */
  new_bucket_cnt = h->elem_cnt / BEST_ELEMS_PER_BUCKET;
  if (new_bucket_cnt < 4)
    new_bucket_cnt = 4;
  while (!is_power_of_2 (new_bucket_cnt))
    new_bucket_cnt = turn_off_least_1bit (new_bucket_cnt);

  /* Don't do anything if the bucket count wouldn't change. */
  if (new_bucket_cnt == old_bucket_cnt)
    return;

  /* Allocate new buckets and initialize them as empty. */
  new_buckets = malloc (sizeof *new_buckets * new_bucket_cnt);
  if (new_buckets == NULL) 
    {
      /* Allocation failed.  This means that use of the hash table will
         be less efficient.  However, it is still usable, so
         there's no reason for it to be an error. */
      return;
    }
  for (i = 0; i < new_bucket_cnt; i++) 
    list_init (&new_buckets[i]);

  /* Install new bucket info. */
  h->buckets = new_buckets;
  h->bucket_cnt = new_bucket_cnt;

  /* Move each old element into the appropriate new bucket. */
  for (i = 0; i < old_bucket_cnt; i++) 
    {
      struct list *old_bucket;
      struct list_elem *elem, *next;

      old_bucket = &old_buckets[i];
      for (elem = list_begin (old_bucket);
           elem != list_end (old_bucket); elem = next) 
        {
          struct list *new_bucket
            = find_bucket (h, list_elem_to_hash_elem (elem));
          next = list_next (elem);
          list_remove (elem);
          list_push_front (new_bucket, elem);
        }
    }

  free (old_buckets);
}

/* Inserts E into BUCKET (in hash table H). */
static void
insert_elem (struct hash *h, struct list *bucket, struct hash_elem *e) 
{
  h->elem_cnt++;
  list_push_front (bucket, &e->list_elem);
}
/*
즉, 해당 bucket에 Elem을 Inserting...
*/

/* Removes E from hash table H. */
static void
remove_elem (struct hash *h, struct hash_elem *e) 
{
  h->elem_cnt--;
  list_remove (&e->list_elem);
}
/*
즉, Parameter로 들어온 struct hash_elem* e를 Removing...
*/

// Wait a minute...
