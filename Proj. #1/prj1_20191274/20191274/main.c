/*
---.
Here are Assumptions.

(1). All inputs are from standard input (STDIN).
(2). All inputs and outputs are lower cases.
(3). All the types used in the program are integer.
(4). Use hash_int() as hash function for hash table, 
	which is already given library (hash.c의 hash_int()를 사용할
것. 그래도 hast_int_2는 구현하셔야 합니다).
(5). Use true or false when the return type is Boolean.
(6). The number of list, 
	hash table and bitmap is less than or equal to 10.
(7). You can use any function in given source codes 
	and you can implement your own code if it is needed.
---.
*/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# include "list.h"
# include "bitmap.h"
# include "hash.h"
# include "round.h"

# define MAX_LIST_CNT 10
# define MAX_HASHMAP_CNT 10
# define MAX_BITMAP_CNT 10

# define HASH_FIND_ERROR -20191274

char input[128];
char words[8][32];

struct list* lists[MAX_LIST_CNT];

struct bitmap* bitmaps[MAX_BITMAP_CNT];

struct hash* hashmaps[MAX_HASHMAP_CNT];

/* ---. */
/*
This signal() func() is Called When dynamicMemoryAllocation is failed.
*/
void signal(void) {
	printf("MEMORY ALLOCATION ERROR! in main.c\n");
	exit(0);
}

void resetInput(char* input) {
	memset(input, 0, sizeof(char) * 128);
}

void resetWordsArray(char words[8][32]) {
	memset(words, 0, sizeof(char) * 8 * 32);
}

void parsing(char* input, char words[8][32]) {
	const int inputLen = strlen(input);
	int wordSize = 0;
	int wordCnt = 0;

	for (int idx = 0; idx < inputLen; idx++) {
		if (input[idx] == ' ' || input[idx] == '\n') {
			words[wordCnt++][wordSize] = '\0';
			wordSize = 0;

			continue;
		}

		words[wordCnt][wordSize++] = input[idx]; 
	}
}
/* ---. */

/* --- list start. ---.*/

// This is about compareFunc().
const bool less
(const struct list_elem* elem1, const struct list_elem* elem2, void* aux) {
	const int elem1Data = list_entry(elem1, struct list_item, elem)->data;
	const int elem2Data = list_entry(elem2, struct list_item, elem)->data;

	return elem1Data < elem2Data;
}

// (ex. create list list0 && etc.).
void createL(char* name) {
	int idx = atoi(name + 4);
	/*

	(ex).
	name = "list0"; ==>> name + 4 = "0";

	*/

	struct list* tmp = malloc(sizeof(struct list) * 1);
	if (tmp == NULL) {
		signal();
	}
	lists[idx] = tmp; tmp = NULL;

	list_init(lists[idx]);
}

// (ex. delete list0)
void deleteL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	// If list is empty.
	if (list_empty(lists[idx])) {
		/*
		free(lists[idx]->head);
		lists[idx]->head = NULL;

		free(lists[idx]->tail);
		lists[idx]->tail = NULL;
		*/

		free(lists[idx]);
		lists[idx] = NULL;

		return;
	}

	// If list is not empty.
	struct list_elem* iter = list_begin(lists[idx]);
	for (; iter != list_end(lists[idx]); iter = list_remove(iter));

	free(lists[idx]);
	lists[idx] = NULL;
}

void dumpdataL(char* name) {
	int idx = atoi(name + 4); // Since "list"'s len = 4.

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	struct list_elem* iter = list_begin(lists[idx]);
	for (; iter != list_end(lists[idx]); iter = list_next(iter)) {
		printf("%d ", list_entry(iter, struct list_item, elem)->data);
	}
	printf("\n");
}

struct list_item* makeListItem(void) {
	struct list_item* temp = malloc(sizeof(struct list_item) * 1);
	if (temp == NULL) {
		signal();
	}
	
	temp->elem.prev = NULL;
	temp->elem.next = NULL;
	temp->data = 0;

	return temp;
}

/*
if option == 0, then list_push_back() Call.
if option == 1, then list_push_front() Call.
*/
void pushL(char* name, int data, int option) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	struct list_item* temp = makeListItem();
	temp->data = data;

	if (option == 0) {
		list_push_back(lists[idx], &temp->elem);
	}
	else if (option == 1) {
		list_push_front(lists[idx], &temp->elem);
	}
}

/*
if option == 0, then list_pop_back() Call.
if option == 1, then list_pop_front() Call.
*/
void popL(char* name, int option) {
	int idx = atoi(name + 4);

	// Q. 0 <= idx && idx < MAX_LIST_CNT ?

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	if (option == 0) {
		list_pop_back(lists[idx]);
	}
	else if (option == 1) {
		list_pop_front(lists[idx]);
	}
}

// (ex. list_front list0 && etc.).
void frontL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	int data = list_entry(list_front(lists[idx]), struct list_item, elem)->data;

	printf("%d\n", data);
}

// (ex. list_back list0 && etc.).
void backL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	int data = list_entry(list_back(lists[idx]), struct list_item, elem)->data;

	printf("%d\n", data);
}

// (ex. list_insert list0 1 4 && etc.).
void insertL(char* name, int insertIdx, int data) {
	int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_LIST_CNT) {
		return;
	}

	if (lists[idx] == NULL) {
		return;
	}

	struct list_item* temp = makeListItem();
	temp->data = data;

	struct list_elem* iter = list_begin(lists[idx]);
	for (int cnt = 0; cnt < insertIdx; cnt++) {
		iter = list_next(iter);
	}

	list_insert(iter, &temp->elem);
}

// (ex. list_insert_ordered list0 5 && etc.).
void insertOrderedL(char* name, int data) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	struct list_item* temp = makeListItem();
	temp->data = data;

	list_insert_ordered(lists[idx], &temp->elem, less, NULL);
}

// (ex. list_remove list0 2 && etc.).
// 2 means removeIdx.
void removeL(char* name, int removeIdx) {
	int idx = atoi(name + 4);

	// Is it needed in Proj. #1 Testcases ?
	if (idx < 0 || idx >= MAX_LIST_CNT) {
		return;
	}

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	struct list_elem* iter = list_begin(lists[idx]);
	for (int cnt = 0; cnt < removeIdx; cnt++) {
		iter = list_next(iter);
	}

	list_remove(iter);
}

/*
Dont forget followed func().

void
list_splice (struct list_elem *before,
			struct list_elem *first, struct list_elem *last)
;

*/
// (ex. list_splice list0 2 list1 1 4 && etc.).
// 즉, list0의 idx = 2자리 Before에, list1의 idx = 1 ~~ idx < 4 추출하여, Inserting.
void spliceL (char* destName, int destIdx,
	char* sourceName, int sourceIdx1, int sourceIdx2) {
	int destListIdx = atoi(destName + 4);
	int sourceListIdx = atoi(sourceName + 4);

	if (lists[destListIdx] == NULL || lists[sourceListIdx] == NULL) {
		return;
	}

	struct list_elem* sourceListElemFirst = list_begin(lists[sourceListIdx]);
	for (int cnt = 0; cnt < sourceIdx1; cnt++) {
		sourceListElemFirst = list_next(sourceListElemFirst);
	}
	struct list_elem* sourceListElemLast = list_begin(lists[sourceListIdx]);
	for (int cnt = 0; cnt < sourceIdx2; cnt++) {
		sourceListElemLast = list_next(sourceListElemLast);
	}

	struct list_elem* destListElemBefore = list_begin(lists[destListIdx]);
	for (int cnt = 0; cnt < destIdx; cnt++) {
		destListElemBefore = list_next(destListElemBefore);
	}

	list_splice(destListElemBefore, sourceListElemFirst, sourceListElemLast);
}

const size_t maxL(char* name, bool* successFlag) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_LIST_CNT) {
		*successFlag = false;

		return (size_t)0;
	}

	if (lists[idx] == NULL) {
		*successFlag = false;

		return (size_t)0;
	}

	if (list_empty(lists[idx])) {
		*successFlag = false;

		return (size_t)0;
	}

	/*
	If this func() can here, 
	the list is not empty.
	*/

	*successFlag = true;

	struct list_elem* temp = list_max(lists[idx], less, NULL);
	return list_entry(temp, struct list_item, elem)->data;
}

const size_t minL(char* name, bool* successFlag) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_LIST_CNT) {
		*successFlag = false;

		return (size_t)0;
	}

	if (lists[idx] == NULL) {
		*successFlag = false;

		return (size_t)0;
	}

	if (list_empty(lists[idx])) {
		*successFlag = false;

		return (size_t)0;
	}

	/*
	If this func() can here,
	the list is not empty.
	*/

	*successFlag = true;

	struct list_elem* temp = list_min(lists[idx], less, NULL);
	return list_entry(temp, struct list_item, elem)->data;
}

const bool emptyL(char* name) {
	int idx = atoi(name + 4);

	/*
	if (idx < 0 || idx >= MAX_LIST_CNT) // Umm...
	// Wait a minute...
	*/

	/*
	if (lists[idx] == NULL) {
		// Umm...
	}
	*/

	return list_empty(lists[idx]);
}

const size_t sizeL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return (size_t)0;
	}

	return list_size(lists[idx]);
}

void shuffleL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	list_shuffle(lists[idx]);
}

void sortL(char* name) {
	int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	list_sort(lists[idx], less, NULL);
}

void swapL(const char* name, const int idx1, const int idx2) {
	/*
	First of all, do exceptionHandling.
	Wait a minute...
	*/

	int idx = atoi(name + 4);

	// If list is empty ?
	if (list_empty(lists[idx])) {
		return;
	}

	int sizeOfList = list_size(lists[idx]);

	// check idx1.
	if (idx1 < 0 || idx1 >= sizeOfList) {
		return;
	}

	// check idx2.
	if (idx2 < 0 || idx2 >= sizeOfList) {
		return;
	}

	if (lists[idx] == NULL) {
		return;
	}

	struct list_elem* iter1;
	struct list_elem* iter2;

	// about idx1.
	iter1 = list_begin(lists[idx]);
	for (int cnt = 0; cnt < idx1; cnt++) {
		iter1 = list_next(iter1);
	}

	// about idx2.
	iter2 = list_begin(lists[idx]);
	for (int cnt = 0; cnt < idx2; cnt++) {
		iter2 = list_next(iter2);
	}

	list_swap(iter1, iter2);
}

// name1's list's duplicated element ==>> name2's list's에 Inserted.
void uniqueL(char* name1, char* name2) {
	int idx1 = atoi(name1 + 4);
	int idx2 = atoi(name2 + 4);

	if (lists[idx1] == NULL || lists[idx2] == NULL) {
		return;
	}
	
	list_unique(lists[idx1], lists[idx2], less, NULL);
}

// (ex. list_reverse list0 ).
void reverseL(char* name) {
	const int idx = atoi(name + 4);

	if (lists[idx] == NULL) {
		return;
	}

	if (list_empty(lists[idx])) {
		return;
	}

	list_reverse(lists[idx]);
}

// --- list end. ---.

// --- bitmap start. ---.

const bool fromStrToBool(char* str) {
	if (strcmp(str, "true") == 0) {
		return true;
	}
	else {
		return false;
	}
}

void createB(char* name, size_t size) {
	int idx = atoi(name + 2);

	// If bitmaps[idx] already exist.
	if (bitmaps[idx]) {
		return;
	}

	bitmaps[idx] = bitmap_create(size);
}

void dumpdataB(char* name) {
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}

	/* bitmap_dump(bitmaps[idx]);
	*/
	for (int i = 0; i < bitmap_size(bitmaps[idx]); i++) {
		const bool temp = bitmap_test(bitmaps[idx], i);
		if (temp) {
			printf("1");
		}
		else {
			printf("0");
		}
	}
	printf("\n");
}

void markB(char* name, int bitIdx) {
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}

	bitmap_mark(bitmaps[idx], bitIdx);
}

void expandB(char* name, size_t expandedSize) {
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}

	bitmaps[idx] = bitmap_expand(bitmaps[idx], expandedSize);
}

void set_allB(char* name, char* val) {
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}

	bool booleanValue;
	if (strcmp(val, "true") == 0) {
		booleanValue = true;
	}
	else if (strcmp(val, "false") == 0) {
		booleanValue = false;
	}

	bitmap_set_all(bitmaps[idx], booleanValue);
}

void deleteB(char* name) {
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}

	bitmap_destroy(bitmaps[idx]);
}

// (ex. bitmap_all bm0 0 16 ).
bool allB(char* name, size_t start, size_t cnt) {
	int idx = atoi(name + 2);

	return bitmap_all(bitmaps[idx], start, cnt);
}

// (ex. bitmap_any bm0 0 16 ).
bool anyB(char* name, size_t start, size_t cnt) {
	int idx = atoi(name + 2);

	/*if (bitmaps[idx] == NULL) {
		return false;
	}*/ // Is it needed in SP Proj. #1 ?

	return bitmap_any(bitmaps[idx], start, cnt);
}

const bool containsB(char* name, size_t start, size_t cnt, bool value) {
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	
	return bitmap_contains(bitmaps[idx], start, cnt, value);
}

const size_t countB(char* name, size_t start, size_t cnt, bool value) {
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	
	return bitmap_count(bitmaps[idx], start, cnt, value);
}

void dumpB(char* name) {
	// ---.
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	bitmap_dump(bitmaps[idx]);
}

void flipB(char* name, size_t flipIdx) {
	// ---.
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	bitmap_flip(bitmaps[idx], flipIdx);
}

const bool noneB(char* name, size_t start, size_t cnt) {
	// ---.
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	return bitmap_none(bitmaps[idx], start, cnt);
}

void resetB(char* name, size_t resetIdx) {
	// ---.
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	bitmap_reset(bitmaps[idx], resetIdx);
}

size_t scanB(char* name, size_t start, size_t cnt, bool value) {
	// ---.
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	return bitmap_scan(bitmaps[idx], start, cnt, value);
}

const size_t scan_and_flipB(char* name, size_t start, size_t cnt, bool value) {
	/*
	const size_t idx = scanB(name, start, cnt, value);
	printf("%zu\n", idx);

	if (idx == BITMAP_ERROR) {
		return;
	}

	flipB(name, idx);
	*/
	// ==>> Just use existing func()...

	int idx = atoi(name + 2);

	return bitmap_scan_and_flip(bitmaps[idx], start, cnt, value);
}

void setB(char* name, size_t setIdx, bool value) {
	// ---.
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	bitmap_set(bitmaps[idx], setIdx, value);
}

void set_multipleB(char* name, size_t start, size_t cnt, bool value) {
	// ---.
	int idx = atoi(name + 2);

	if (bitmaps[idx] == NULL) {
		return;
	}
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	bitmap_set_multiple(bitmaps[idx], start, cnt, value);
}

const size_t sizeB(char* name) {
	// ---.
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	return bitmap_size(bitmaps[idx]);
}

const bool testB(char* name, size_t testIdx) {
	// ---.
	int idx = atoi(name + 2);

	/*
	if (bitmaps[idx] == NULL) {
		return;
	}
	*/
	// ---.
	// this part is duplicated many times. ==>> So, modularization ?

	return bitmap_test(bitmaps[idx], testIdx);
}

// --- bitmap end. ---.

// --- hashmap start. ---.

unsigned int hashFuncH(const struct hash_elem* elem, /*const */void* aux) {
	return hash_int(elem->value);
}

_Bool lessH(const struct hash_elem* elem1, const struct hash_elem* elem2, void* aux) {
	return elem1->value < elem2->value;
}

void printH(struct hash_elem* elem, void* aux) {
	printf("%d ", elem->value);
}

void squareH(struct hash_elem* elem, void* aux) {
	elem->value = elem->value * elem->value;
}

void tripleH(struct hash_elem* elem, void* aux) {
	elem->value = elem->value * elem->value * elem->value;
}

void createH(char* name) {
	const int idx = atoi(name + 4); // (ex. "hash0", and etc.).

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] != NULL) {
		return;
	}

	hashmaps[idx] = malloc(sizeof(struct hash) * 1);
	if (hashmaps[idx] == NULL) {
		signal();
	}

	hash_init(hashmaps[idx], hashFuncH, lessH, NULL);

	// Wait a minute...
}

void dumpdataH(char* name) {
	const int idx = atoi(name + 4); // (ex. "hash0", and etc.).

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] == NULL) {
		return;
	}

	if (hash_empty(hashmaps[idx])) {
		return;
	}

	hash_apply(hashmaps[idx], printH);
	printf("\n");

	// Wait a minute...
}

void insertH(char* name, int key) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] == NULL) {
		return;
	}

	struct hash_elem* newElem = malloc(sizeof(struct hash_elem) * 1);
	newElem->value = key; // In Proj. #1, We assume that key == value.

	hash_insert(hashmaps[idx], newElem);
}

void applyH(char* name, char* func_str) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] == NULL) {
		return;
	}

	if (strcmp(func_str, "square") == 0) {
		hash_apply(hashmaps[idx], squareH);
	}
	else if (strcmp(func_str, "triple") == 0) {
		hash_apply(hashmaps[idx], tripleH);
	}
}

void freeH(struct hash_elem* elem, void* aux) {
	if (elem == NULL) {
		return;
	}

	free(elem);
	// elem = NULL;
}

void deleteH(char* name) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] == NULL) {
		return;
	}

	hash_destroy(hashmaps[idx], freeH);
}

void hashElemDeleteH(char* name, int key) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;
	}

	if (hashmaps[idx] == NULL) {
		return;
	}

	struct hash_elem* targetElem = malloc(sizeof(struct hash_elem) * 1);
	targetElem->value = key;
	/* 
	* Assume that key and value are same.
	*/

	hash_delete(hashmaps[idx], targetElem);
}

/* const */ bool emptyH(char* name) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return false; // ?...
	}

	if (hashmaps[idx] == NULL) {
		return false; // ?...
	}

	return hash_empty(hashmaps[idx]);
}

const size_t sizeH(char* name) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		/*
		return;
		*/
		return -1;
	}

	if (hashmaps[idx] == NULL) {
		return -1;
	}

	return hash_size(hashmaps[idx]);
}

void clearH(char* name) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;

		/*
		return -1;
		*/
	}

	if (hashmaps[idx] == NULL) {
		return;

		/*
		return -1;
		*/
	}

	hash_clear(hashmaps[idx], freeH);
}

const int findH(char* name, int key) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		/*
		return;
		*/

		/*
		return -1;
		*/

		exit(0); // ...
	}

	if (hashmaps[idx] == NULL) {
		/*
		return;
		*/

		/*
		return -1;
		*/

		exit(0); // ...
	}

	struct hash_elem* targetElem = malloc(sizeof(struct hash_elem) * 1);
	targetElem->value = key;

	struct hash_elem* result = hash_find(hashmaps[idx], targetElem);
	
	// If findH() ERROR.
	if (result == NULL) {
		return HASH_FIND_ERROR;
	}

	return result->value;
}

void replaceH(char* name, int newKey) {
	const int idx = atoi(name + 4);

	if (idx < 0 || idx >= MAX_HASHMAP_CNT) {
		return;

		/*
		return -1;
		*/
	}

	if (hashmaps[idx] == NULL) {
		return;

		/*
		return -1;
		*/
	}

	struct hash_elem* newElem = malloc(sizeof(struct hash_elem) * 1);
	newElem->value = newKey;

	hash_replace(hashmaps[idx], newElem);
}

// --- hashmap end. ---.

int main(void) {
	srand(time(NULL)); // for randomization.
	// list_shuffle() func()의 호출 간격이 짧다면, 동일한 seedNumber가 인자로 들어가는 것 같습니다. 따라서, Random성이 떨어지는 것 같습니다.
	// list_shuffle() func()에서, srand(time(NULL)); Remove...

	while (fgets(input, 128, stdin) != NULL) {
		parsing(input, words);

		if (strcmp(words[0], "quit") == 0) {
			break;
		}
		// (ex. create list list0 ).
		else if (strcmp(words[0], "create") == 0) {
			if (strcmp(words[1], "list") == 0) {
				createL(words[2]);
			}
			else if (strcmp(words[1], "bitmap") == 0) {
				createB(words[2], (size_t)atoi(words[3]));
			}
			else if (strcmp(words[1], "hashtable") == 0) {
				createH(words[2]);
			}
		}
		// (ex. dumpdata list0 ).
		else if (strcmp(words[0], "dumpdata") == 0) {
			char type[4 + 1];
			strncpy(type, words[1], strlen(words[1]) - 1);
			type[strlen(words[1]) - 1] = '\0';

			if (strcmp(type, "list") == 0) {
				dumpdataL(words[1]);
			}
			else if (strcmp(type, "bm") == 0) {
				/* printf("%s \n", type); */
				dumpdataB(words[1]);
			}
			else if (strcmp(type, "hash") == 0) {
				dumpdataH(words[1]);
			}
		}
		// (ex. delete list0 ).
		else if (strcmp(words[0], "delete") == 0) {
			char type[4 + 1];
			strncpy(type, words[1], strlen(words[1]) - 1);
			type[strlen(words[1]) - 1] = '\0';

			if (strcmp(type, "list") == 0) {
				deleteL(words[1]);
			}
			else if (strcmp(type, "bm") == 0) {
				deleteB(words[1]);
			}
			else if (strcmp(type, "hash") == 0) {
				deleteH(words[1]);
			}
		}
		// (ex. list_splice list0 2 list1 1 4 ).
		else if ((strcmp(words[0], "list_splice") == 0)) {
			spliceL(words[1], atoi(words[2]), words[3], atoi(words[4]), atoi(words[5]));
		}
		// (ex. list_push_back list0 1 ). 
		else if (strcmp(words[0], "list_push_front") == 0) {
			pushL(words[1], atoi(words[2]), 1);
		}
		else if (strcmp(words[0], "list_push_back") == 0) {
			pushL(words[1], atoi(words[2]), 0);
		}
		else if (strcmp(words[0], "list_pop_front") == 0) {
			popL(words[1], 1);
		}
		else if (strcmp(words[0], "list_pop_back") == 0) {
			popL(words[1], 0);
		}
		else if (strcmp(words[0], "list_front") == 0) {
			frontL(words[1]);
		}
		else if (strcmp(words[0], "list_back") == 0) {
			backL(words[1]);
		}
		else if (strcmp(words[0], "list_insert") == 0) {
			insertL(words[1], atoi(words[2]), atoi(words[3]));
		}
		else if (strcmp(words[0], "list_insert_ordered") == 0) {
			insertOrderedL(words[1], atoi(words[2]));
		}
		else if (strcmp(words[0], "list_remove") == 0) {
			removeL(words[1], atoi(words[2]));
		}
		else if (strcmp(words[0], "list_max") == 0) {
			bool successFlag;
			const size_t Max = maxL(words[1], &successFlag);

			if (successFlag == true) {
				printf("%zu\n", Max);
			}
		}
		else if (strcmp(words[0], "list_min") == 0) {
			bool successFlag;
			const size_t Min = minL(words[1], &successFlag);

			if (successFlag == true) {
				printf("%zu\n", Min);
			}
		}
		else if (strcmp(words[0], "list_empty") == 0) {
			const bool IsListEmpty
				= emptyL(words[1]);

			if (IsListEmpty) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "list_size") == 0) {
			const size_t sizeOfList = sizeL(words[1]);

			printf("%zu\n", sizeOfList);
		}
		else if (strcmp(words[0], "list_shuffle") == 0) {
			shuffleL(words[1]);
		}
		else if (strcmp(words[0], "list_sort") == 0) {
			sortL(words[1]);
		}
		else if (strcmp(words[0], "list_swap") == 0) {
			swapL(words[1], atoi(words[2]), atoi(words[3]));
		}
		else if (strcmp(words[0], "list_unique") == 0) {
			uniqueL(words[1], words[2]);
		}
		else if (strcmp(words[0], "list_reverse") == 0) {
			reverseL(words[1]);
		}
		else if (strcmp(words[0], "bitmap_mark") == 0) {
			markB(words[1], (size_t)atoi(words[2]));
		}
		else if (strcmp(words[0], "bitmap_expand") == 0) {
			expandB(words[1], (size_t)atoi(words[2]));
		}
		else if (strcmp(words[0], "bitmap_set_all") == 0) {
			set_allB(words[1], words[2]);
		}
		else if (strcmp(words[0], "bitmap_all") == 0) {
			const bool temp = allB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]));

			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "bitmap_any") == 0) {
			const bool temp = anyB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]));

			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "bitmap_contains") == 0) {
			const bool temp = containsB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]), fromStrToBool(words[4]));

			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "bitmap_count") == 0) {
			const size_t temp = countB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]), fromStrToBool(words[4]));

			printf("%zu\n", temp);
		}
		else if (strcmp(words[0], "bitmap_dump") == 0) {
			dumpB(words[1]);
		}
		else if (strcmp(words[0], "bitmap_flip") == 0) {
			flipB(words[1], (size_t)atoi(words[2]));
		}
		else if (strcmp(words[0], "bitmap_none") == 0) {
			const bool temp = noneB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]));

			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "bitmap_reset") == 0) {
			resetB(words[1], (size_t)atoi(words[2]));
		}
		else if (strcmp(words[0], "bitmap_scan") == 0) {
			const size_t temp = scanB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]), fromStrToBool(words[4]));

			printf("%zu\n", temp);
		}
		else if (strcmp(words[0], "bitmap_scan_and_flip") == 0) {
			printf("%zu\n", scan_and_flipB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]), fromStrToBool(words[4])));
		}
		else if (strcmp(words[0], "bitmap_set") == 0) {
			setB(words[1], (size_t)atoi(words[2]), fromStrToBool(words[3]));
		}
		else if (strcmp(words[0], "bitmap_set_multiple") == 0) {
			set_multipleB(words[1], (size_t)atoi(words[2]), (size_t)atoi(words[3]), fromStrToBool(words[4]));
		}
		else if (strcmp(words[0], "bitmap_size") == 0) {
			const size_t temp = sizeB(words[1]);

			printf("%zu\n", temp);
		}
		else if (strcmp(words[0], "bitmap_test") == 0) {
			const bool temp = testB(words[1], (size_t)atoi(words[2]));
			
			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "hash_insert") == 0) {
			insertH(words[1], atoi(words[2]));
			}
		else if (strcmp(words[0], "hash_apply") == 0) {
				applyH(words[1], words[2]);
		}
		else if (strcmp(words[0], "hash_delete") == 0) {
			hashElemDeleteH(words[1], atoi(words[2]));
		}
		else if (strcmp(words[0], "hash_empty") == 0) {
			const bool temp = emptyH(words[1]);

			if (temp) {
				printf("true\n");
			}
			else {
				printf("false\n");
			}
		}
		else if (strcmp(words[0], "hash_size") == 0) {
			const size_t temp = sizeH(words[1]);
			if (temp == -1) {
				printf(" Exception is occured in sizeH()... \n");
				exit(0);
			}

			printf("%zu\n", temp);
		}
		else if (strcmp(words[0], "hash_clear") == 0) {
			clearH(words[1]);
		}
		else if (strcmp(words[0], "hash_find") == 0) {
			const int temp = findH(words[1], atoi(words[2]));

			if (temp == HASH_FIND_ERROR) {
				continue;
			}

			printf("%d\n", temp);
		}
		else if (strcmp(words[0], "hash_replace") == 0) {
			replaceH(words[1], atoi(words[2]));
			// replaceH() : 없으면, 그냥 Add하는 func().
		}
		else {
			printf(" Finished... Thank you... \n ");
		}

		/* reset input && words. */
		resetInput(input);
		resetWordsArray(words);
	}

	return 0;
}