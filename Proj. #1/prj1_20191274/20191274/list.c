#include "list.h"
#include <assert.h>	
#define ASSERT(CONDITION) assert(CONDITION)	

// ---.
/*
This header file is for list_shuffle() (func()).
*/
# include <stdlib.h>
# include <time.h>
// ---.

/* Our doubly linked lists have two header elements: the "head"
   just before the first element and the "tail" just after the
   last element.  The `prev' link of the front header is null, as
   is the `next' link of the back header.  Their other two links
   point toward each other via the interior elements of the list.

   An empty list looks like this:

                      +------+     +------+
                  <---| head |<--->| tail |--->
                      +------+     +------+

   A list with two elements in it looks like this:

        +------+     +-------+     +-------+     +------+
    <---| head |<--->|   1   |<--->|   2   |<--->| tail |<--->
        +------+     +-------+     +-------+     +------+

   The symmetry of this arrangement eliminates lots of special
   cases in list processing.  For example, take a look at
   list_remove(): it takes only two pointer assignments and no
   conditionals.  That's a lot simpler than the code would be
   without header elements.

   (Because only one of the pointers in each header element is used,
   we could in fact combine them into a single header element
   without sacrificing this simplicity.  But using two separate
   elements allows us to do a little bit of checking on some
   operations, which can be valuable.) */

static bool is_sorted (struct list_elem *a, struct list_elem *b,
                       list_less_func *less, void *aux);
                       
/* Returns true if ELEM is a head, false otherwise. */
static inline bool
is_head (struct list_elem *elem)
{
  return elem != NULL && elem->prev == NULL && elem->next != NULL;
}

/* Returns true if ELEM is an interior element,
   false otherwise. */
static inline bool
is_interior (struct list_elem *elem)
{
  return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

/* Returns true if ELEM is a tail, false otherwise. */
static inline bool
is_tail (struct list_elem *elem)
{
  return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

/* Initializes LIST as an empty list. */
void
list_init (struct list *list)
{
  ASSERT (list != NULL);
  list->head.prev = NULL;
  list->head.next = &list->tail;
  list->tail.prev = &list->head;
  list->tail.next = NULL;
}

/* Returns the beginning of LIST.  */
struct list_elem *
list_begin (struct list *list)
{
  ASSERT (list != NULL);
  return list->head.next;
}

/* Returns the element after ELEM in its list.  If ELEM is the
   last element in its list, returns the list tail.  Results are
   undefined if ELEM is itself a list tail. */
struct list_elem *
list_next (struct list_elem *elem)
{
  ASSERT (is_head (elem) || is_interior (elem));
  return elem->next;
}

/* Returns LIST's tail.

   list_end() is often used in iterating through a list from
   front to back.  See the big comment at the top of list.h for
   an example. */
struct list_elem *
list_end (struct list *list)
{
  ASSERT (list != NULL);
  return &list->tail;
}

/* Returns the LIST's reverse beginning, for iterating through
   LIST in reverse order, from back to front. */
struct list_elem *
list_rbegin (struct list *list) 
{
  ASSERT (list != NULL);
  return list->tail.prev;
}

/* Returns the element before ELEM in its list.  If ELEM is the
   first element in its list, returns the list head.  Results are
   undefined if ELEM is itself a list head. */
struct list_elem *
list_prev (struct list_elem *elem)
{
  ASSERT (is_interior (elem) || is_tail (elem));
  return elem->prev;
}

/* Returns LIST's head.

   list_rend() is often used in iterating through a list in
   reverse order, from back to front.  Here's typical usage,
   following the example from the top of list.h:

      for (e = list_rbegin (&foo_list); e != list_rend (&foo_list);
           e = list_prev (e))
        {
          struct foo *f = list_entry (e, struct foo, elem);
          ...do something with f...
        }
*/
struct list_elem *
list_rend (struct list *list) 
{
  ASSERT (list != NULL);
  return &list->head;
}

/* Return's LIST's head.

   list_head() can be used for an alternate style of iterating
   through a list, e.g.:

      e = list_head (&list);
      while ((e = list_next (e)) != list_end (&list)) 
        {
          ...
        }
*/
struct list_elem *
list_head (struct list *list) 
{
  ASSERT (list != NULL);
  return &list->head;
}

/* Return's LIST's tail. */
struct list_elem *
list_tail (struct list *list) 
{
  ASSERT (list != NULL);
  return &list->tail;
}

/* Inserts ELEM just before BEFORE, which may be either an
   interior element or a tail.  The latter case is equivalent to
   list_push_back(). */
void
list_insert (struct list_elem *before, struct list_elem *elem)
{
  ASSERT (is_interior (before) || is_tail (before));
  ASSERT (elem != NULL);

  elem->prev = before->prev;
  elem->next = before;
  before->prev->next = elem;
  before->prev = elem;
}

/* Removes elements FIRST though LAST (exclusive) from their
   current list, then inserts them just before BEFORE, which may
   be either an interior element or a tail. */
void
list_splice (struct list_elem *before,
             struct list_elem *first, struct list_elem *last)
{
  ASSERT (is_interior (before) || is_tail (before)); // struct list_elem* head의 before에 Insert가 불가능하기 때문.
  if (first == last) // last는 exclusive하기 때문에, 만약, first == last라면, do nothing.
    return;
  last = list_prev (last); // last는 exclusive하기 때문.
  // By doing this, last means Real(or real) last.

  ASSERT (is_interior (first)); 
  ASSERT (is_interior (last));
  /*
  struct list의 member인 head나 tail을 Remove하면 안되기 때문.
  */

  /* Cleanly remove FIRST...LAST from its current list. */
  first->prev->next = last->next;
  last->next->prev = first->prev;

  /* Splice FIRST...LAST into new list. */
  first->prev = before->prev;
  last->next = before;
  before->prev->next = first;
  before->prev = last;
}

/* Inserts ELEM at the beginning of LIST, so that it becomes the
   front in LIST. */
void
list_push_front (struct list *list, struct list_elem *elem)
{
  list_insert (list_begin (list), elem); //, that is, Insert elem before first element.

  // Are you remember that
  /*
  void
      list_insert(struct list_elem* before, struct list_elem* elem); ?
  */
}

/* Inserts ELEM at the end of LIST, so that it becomes the
   back in LIST. */
void
list_push_back (struct list *list, struct list_elem *elem)
{
  list_insert (list_end (list), elem);
}

/* Removes ELEM from its list and returns the element that
   followed it.  Undefined behavior if ELEM is not in a list.

   It's not safe to treat ELEM as an element in a list after
   removing it.  In particular, using list_next() or list_prev()
   on ELEM after removal yields undefined behavior.  This means
   that a naive loop to remove the elements in a list will fail:

   ** DON'T DO THIS **
   for (e = list_begin (&list); e != list_end (&list); e = list_next (e))
     {
       ...do something with e...
       list_remove (e); // ,that is, do not touch iterator?
     }
   ** DON'T DO THIS **

   Here is one correct way to iterate and remove elements from a
   list:

   for (e = list_begin (&list); e != list_end (&list); e = list_remove (e))
     {
       ...do something with e...
     }

   If you need to free() elements of the list then you need to be
   more conservative.  Here's an alternate strategy that works
   even in that case:

   while (!list_empty (&list))
     {
       struct list_elem *e = list_pop_front (&list); // , that is, use localVariable?
       ...do something with e...
     }
*/
struct list_elem *
list_remove (struct list_elem *elem)
{
  ASSERT (is_interior (elem));
  elem->prev->next = elem->next;
  elem->next->prev = elem->prev;

  // I think that this is more safe.
  // ---.
  // elem->prev = NULL;
  // elem->next = NULL;
  // ---.
  // Wait a minute...
  // Nope.

  return elem->next;
} // ,that is, Remove && Return next element?

/* Removes the front element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct list_elem *
list_pop_front (struct list *list)
{
  struct list_elem *frontL = list_front (list);
  list_remove (frontL);
  return frontL;
} //, that is, Remove front && Return front.

/* Removes the back element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct list_elem *
list_pop_back (struct list *list)
{
  struct list_elem *backL = list_back (list);
  list_remove (backL);
  return backL;
}

/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */
struct list_elem *
list_front (struct list *list)
{
  ASSERT (!list_empty (list));
  return list->head.next;
}

/* Returns the back element in LIST.
   Undefined behavior if LIST is empty. */
struct list_elem *
list_back (struct list *list)
{
  ASSERT (!list_empty (list));
  return list->tail.prev;
}

/* Returns the number of elements in LIST.
   Runs in O(n) in the number of elements. */
size_t
list_size (struct list *list)
{
  struct list_elem *e;
  size_t cnt = 0;

  for (e = list_begin (list); e != list_end (list); e = list_next (e))
    cnt++;
  return cnt;
}

/* Returns true if LIST is empty, false otherwise. */
bool
list_empty (struct list *list)
{
  return list_begin (list) == list_end (list);
}

/* Swaps the `struct list_elem *'s that A and B point to. */
static void
swap (struct list_elem **a, struct list_elem **b) 
{
  struct list_elem *t = *a;
  *a = *b;
  *b = t;
}

// ---.
/*
this func() checks If Ptr == NULL.
*/
const bool is_NULL(void* ptr) {
    return (ptr == NULL);
}
// ---.

// ---.
/*
this func() checks If Ptr1 == Ptr2.
*/
const bool is_SAME(struct list_elem* ptr1, struct list_elem* ptr2) {
    return (ptr1 == ptr2);
}
// ---.

/* Reverses the order of LIST. */
void
list_reverse (struct list *list)
{
  if (!list_empty (list)) 
    {
      struct list_elem *e;

      for (e = list_begin (list); e != list_end (list); e = e->prev)
        swap (&e->prev, &e->next);
      swap (&list->head.next, &list->tail.prev);
      swap (&list->head.next->prev, &list->tail.prev->next);
    }
}

/* Returns true only if the list elements A through B (exclusive)
   are in order according to LESS given auxiliary data AUX. */
static bool
is_sorted (struct list_elem *a, struct list_elem *b,
           list_less_func *less, void *aux)
{
  if (a != b)
    while ((a = list_next (a)) != b) 
      if (less (a, list_prev (a), aux))
        return false;
  return true;
}

/* Finds a run, starting at A and ending not after B, of list
   elements that are in nondecreasing order according to LESS
   given auxiliary data AUX.  Returns the (exclusive) end of the
   run.
   A through B (exclusive) must form a non-empty range. */
static struct list_elem *
find_end_of_run (struct list_elem *a, struct list_elem *b,
                 list_less_func *less, void *aux)
{
  ASSERT (a != NULL);
  ASSERT (b != NULL);
  ASSERT (less != NULL);
  ASSERT (a != b);
  
  do 
    {
      a = list_next (a);
    }
  while (a != b && !less (a, list_prev (a), aux));
  return a;
}

/* Merges A0 through A1B0 (exclusive) with A1B0 through B1
   (exclusive) to form a combined range also ending at B1
   (exclusive).  Both input ranges must be nonempty and sorted in
   nondecreasing order according to LESS given auxiliary data
   AUX.  The output range will be sorted the same way. */
static void
inplace_merge (struct list_elem *a0, struct list_elem *a1b0,
               struct list_elem *b1,
               list_less_func *less, void *aux)
{
  ASSERT (a0 != NULL);
  ASSERT (a1b0 != NULL);
  ASSERT (b1 != NULL);
  ASSERT (less != NULL);
  ASSERT (is_sorted (a0, a1b0, less, aux));
  ASSERT (is_sorted (a1b0, b1, less, aux));

  while (a0 != a1b0 && a1b0 != b1)
    if (!less (a1b0, a0, aux)) 
      a0 = list_next (a0);
    else 
      {
        a1b0 = list_next (a1b0);
        list_splice (a0, list_prev (a1b0), a1b0);
      }
}

/* Sorts LIST according to LESS given auxiliary data AUX, using a
   natural iterative merge sort that runs in O(n lg n) time and
   O(1) space in the number of elements in LIST. */
void
list_sort (struct list *list, list_less_func *less, void *aux)
{
  size_t output_run_cnt;        /* Number of runs output in current pass. */

  ASSERT (list != NULL);
  ASSERT (less != NULL);

  /* Pass over the list repeatedly, merging adjacent runs of
     nondecreasing elements, until only one run is left. */
  do
    {
      struct list_elem *a0;     /* Start of first run. */
      struct list_elem *a1b0;   /* End of first run, start of second. */
      struct list_elem *b1;     /* End of second run. */

      output_run_cnt = 0;
      for (a0 = list_begin (list); a0 != list_end (list); a0 = b1)
        {
          /* Each iteration produces one output run. */
          output_run_cnt++;

          /* Locate two adjacent runs of nondecreasing elements
             A0...A1B0 and A1B0...B1. */
          a1b0 = find_end_of_run (a0, list_end (list), less, aux);
          if (a1b0 == list_end (list))
            break;
          b1 = find_end_of_run (a1b0, list_end (list), less, aux);

          /* Merge the runs. */
          inplace_merge (a0, a1b0, b1, less, aux);
        }
    }
  while (output_run_cnt > 1);

  ASSERT (is_sorted (list_begin (list), list_end (list), less, aux));
}

/* Inserts ELEM in the proper position in LIST, which must be
   sorted according to LESS given auxiliary data AUX.
   Runs in O(n) average case in the number of elements in LIST. */
void
list_insert_ordered (struct list *list, struct list_elem *elem,
                     list_less_func *less, void *aux)
{
  struct list_elem *e;

  ASSERT (list != NULL);
  ASSERT (elem != NULL);
  ASSERT (less != NULL);

  for (e = list_begin (list); e != list_end (list); e = list_next (e))
    if (less (elem, e, aux))
      break;
  return list_insert (e, elem);
}

/* Iterates through LIST and removes all but the first in each
   set of adjacent elements that are equal according to LESS
   given auxiliary data AUX.  If DUPLICATES is non-null, then the
   elements from LIST are appended to DUPLICATES. */
void
list_unique (struct list *list, struct list *duplicates,
             list_less_func *less, void *aux)
{
  struct list_elem *elem, *next;

  ASSERT (list != NULL);
  ASSERT (less != NULL);
  if (list_empty (list))
    return;

  elem = list_begin (list);
  while ((next = list_next (elem)) != list_end (list))
    if (!less (elem, next, aux) && !less (next, elem, aux)) 
    // ,that is, If elem's data == next's data
    // , then remove next's data.
    // && 
      {
        list_remove (next);
        if (duplicates != NULL) // I think that this is about exceptionHandling...
          list_push_back (duplicates, next);
      }
    else
      elem = next;
}

/* Returns the element in LIST with the largest value according
   to LESS given auxiliary data AUX.  If there is more than one
   maximum, returns the one that appears earlier in the list.  If
   the list is empty, returns its tail. */
struct list_elem *
list_max (struct list *list, list_less_func *less, void *aux)
{
  struct list_elem *max = list_begin (list);
  if (max != list_end (list)) 
    {
      struct list_elem *e;
      
      for (e = list_next (max); e != list_end (list); e = list_next (e))
        if (less (max, e, aux)) // ,that is, If max < e, then max = e;
          max = e; 
    }
  return max;
}

/* Returns the element in LIST with the smallest value according
   to LESS given auxiliary data AUX.  If there is more than one
   minimum, returns the one that appears earlier in the list.  If
   the list is empty, returns its tail. */
struct list_elem *
list_min (struct list *list, list_less_func *less, void *aux)
{
  struct list_elem *min = list_begin (list);
  if (min != list_end (list)) 
    {
      struct list_elem *e;
      
      for (e = list_next (min); e != list_end (list); e = list_next (e))
        if (less (e, min, aux))
          min = e; 
    }
  return min;
}

// ---.
/********************************************************************
- Functionality : Shuffle elements of LIST in the parameter.
- Parameter     : Pointer of list that will be shuffled.
- Return value  : None.
*********************************************************************
*/
void list_shuffle(struct list* list) {
    // for accurate(?) randomization.
    static int cnt = 20191274;
    cnt++;

    // As you know, 
    // First, we should do exceptionHandling.
    /*

    (1). if list == NULL ?
    (2). if list's size <= 1 ?
    /* Warning : If you call list_size() every time you need it,
    the efficiency will be very low.
    Let's create a new local variable and assign it to it and then use it.
    */

    // (1). if list == NULL ?
    if (is_NULL(list)) {
        return;
    }

    const size_t sizeOfList = list_size(list);
    // (2). if list's size <= 1 ?
    if (sizeOfList <= (size_t)1) {
        return;
    }

    // Let's do it.
    struct list_elem** arrayAboutListElem = malloc(sizeof(struct list_elem*) * sizeOfList);
    /*

    Q. What's about arrayAboutListElem ?
    A. Parameter로 들어온 list의 struct list_elem*을 각 Elem으로 하는 Array.
    좀 더 정확히 말하면, list를 구성하는 struct list_elem의 Address를 Shuffle하기 위함임.

    */

    // Step1. puts all elem in arrayAboutListElem.
    struct list_elem* curListElem = list_begin(list);
    for (int idx = 0; idx < sizeOfList; idx++) {
        arrayAboutListElem[idx] = curListElem;

        curListElem = list_next(curListElem);
    }
    curListElem = NULL; // Since this will be not needed.

    // Step2. Shuffle arrayAboutListElem.
    /*

    Acknowledgement :
    I referenced "Fisher-Yates Shuffle & Knuth Shuffle"
    https://daheenallwhite.github.io/programming/algorithm/2019/06/27/Shuffle-Algorithm-Fisher-Yates/

    */
    // srand(time(NULL)); // set seedNumber. ==>> Nope...

    srand(time(NULL) + (time_t)cnt);

    struct list_elem* tmp;
    for (int i = sizeOfList - 1; i >= 1; i--) {
        int j = rand() % (i + 1);
        // Then exchage arr[j] and arr[i].

        // i번째 Address와 j번째 Address를 Shuffle.
        // 즉, list_swap() func()Call을 통하여, 실제 list에서의 위치를 Exchage를 해줌.
        list_swap(arrayAboutListElem[i], arrayAboutListElem[j]);
        // 그다음, arrayAboutListElem에서도 Exchage해주어야 함.
        /*
        tmp = arrayAboutListElem[i];
        arrayAboutListElem[i] = arrayAboutListElem[j];
        arrayAboutListElem[j] = tmp;
        */
        // Modularization? ==>> Readability 상승?
        swap(&arrayAboutListElem[i], &arrayAboutListElem[j]);
    }

    /* Warning: Don't forget memoryDeallocation. */
    free(arrayAboutListElem);
    arrayAboutListElem = NULL;
}
// ---.

// ---.
/*
This list_swap() func() will swap two elements in the same list.
*/
void list_swap(struct list_elem* elem1, struct list_elem* elem2) {
    /*
    First, we should do exceptionHandling.

    (1). Is elem1 or elem2 NULL ?
    (2). Is elem1 == elem2 ?
    (3). Is elem1 head or tail?
    (4). Is elem2 head or tail?

    */

    /*
    Let's do it.
    */

    // (1). Is elem1 or elem2 NULL ?
    if (is_NULL(elem1) || is_NULL(elem2)) {
        return;
    }
    // (2). Is elem1 == elem2 ?
    if (is_SAME(elem1, elem2)) {
        return;
    }
    // (3). Is elem1 head or tail?
    if (is_head(elem1) || is_tail(elem1)) {
        return;
    }
    // (4). Is elem2 head or tail?
    if (is_head(elem2) || is_tail(elem2)) {
        return;
    }

    // If this func() can here, 
    // then we can know that elem1 and elem2 satisfy is_interior() func().

    // Case #1 : If elem1 and elem2 are adjacent(1) ?
    /*
        before ~~ elem1 ~~ elem2 ~~ after.
    */
    if (elem1->next == elem2 && elem2->prev == elem1) {
        struct list_elem* before = elem1->prev;
        struct list_elem* after = elem2->next;

        elem1->next = after;
        elem2->prev = before;

        elem1->prev = elem2;
        elem2->next = elem1;

        before->next = elem2;
        after->prev = elem1;
    }
    // Case #2 : If elem1 and elem2 are adjacent(2) ?
    /*
        before ~~ elem2 ~~ elem1 ~~ after.
    */
    else if (elem2->next == elem1 && elem1->prev == elem2) {
        // By using list_swap(elem2, elem1), we can do it easily.
        swap(&elem2, &elem1);
    }
    // Case #3 : If elem1 and elem2 are not adjacent ?
    // Wait a minute...
    // I think that we can solve it by using list_splice() ?
    // But, I'm not sure...
    // Let's draw this case.
    /*

    unknown1 ~~ elem1 ~~ unknown2 && unknown3 ~~ elem2 ~~ unknown4.

    */
    else {
        struct list_elem* unknown1 = elem1->prev;
        struct list_elem* unknown2 = elem1->next;

        struct list_elem* unknown3 = elem2->prev;
        struct list_elem* unknown4 = elem2->next;

        // If
        list_splice(elem2, elem1, elem1->next); // ,
        // then
        /*

        unknown1 ~~ unknown2 && unknown3 ~~ elem1 ~~ elem2 ~~ unknown4.

        */

        // If
        list_splice(unknown2, elem2, elem2->next); // ,
        // then
        /*

        unknown1 ~~ elem2 ~~ unknown2 && unknown3 ~~ elem1 ~~ unknown4.

        */
    }
}
// ---.