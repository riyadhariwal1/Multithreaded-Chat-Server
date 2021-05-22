#include "list.h"
#include <stdio.h>
#include <stdio.h>
#include <assert.h>

#define HEAD_NODE_SIZE 3 * LIST_MAX_NUM_HEADS //number of header, trailer and current pointers
static Node headNodeList[HEAD_NODE_SIZE];
static Node nodeList[LIST_MAX_NUM_NODES];
static List headList[LIST_MAX_NUM_HEADS];
static bool firstProgramRun = true;

//Code for stack implementation. Adapted from previous implementation in C++ by me and from this website
//https://www.tutorialspoint.com/data_structures_algorithms/stack_program_in_c.htm
#define ERROR_CODE -1
#define NODE_TYPE -5
#define HEAD_TYPE -6
#define HEAD_NODE_TYPE -7

static int nodeStack[LIST_MAX_NUM_NODES]; //stacks stores the indexes of the free spaces in the static arrays
static int headStack[LIST_MAX_NUM_HEADS];
static int headNodeStack[HEAD_NODE_SIZE];
static int nodeTop = -1;
static int headTop = -1;
static int headNodeTop = -1;

static bool isEmpty(int top) {
	return (top == -1);
}

static bool isFull(int top, int capacity) {
	return (top == capacity);
}

static int push(int type, int data) {
	if (type == HEAD_NODE_TYPE){
		if (!isFull(headNodeTop, HEAD_NODE_SIZE - 1)){
			headNodeTop += 1;
			headNodeStack[headNodeTop] = data;
			return 0;
		} else {
			return ERROR_CODE;
		}

	} else if (type == NODE_TYPE){
		if (!isFull(nodeTop, LIST_MAX_NUM_NODES - 1)){
			nodeTop += 1;
			nodeStack[nodeTop] = data;
			return 0;
		} else {
			return ERROR_CODE;
		}

	} else {
		if (!isFull(headTop, LIST_MAX_NUM_HEADS - 1)){
			headTop += 1;
			headStack[headTop] = data;
			return 0;
		} else {
			return ERROR_CODE;
		}
	}
}

static int pop(int type) {
	if (type == HEAD_NODE_TYPE){
		if (!isEmpty(headNodeTop)){
			int data = headNodeStack[headNodeTop];
			headNodeTop -= 1;
			return data;
		} else {
			return ERROR_CODE;
		}
	} else if (type == NODE_TYPE){
		if (!isEmpty(nodeTop)){
			int data = nodeStack[nodeTop];
			nodeTop -= 1;
			return data;
		} else {
			return ERROR_CODE;
		}
	} else {
		if (!isEmpty(headTop)){
			int data = headStack[headTop];
			headTop -= 1;
			return data;
		} else {
			return ERROR_CODE;
		}
	}
}

static void fillInitialStacks() {
	if (firstProgramRun){
		for (int i = 0; i < LIST_MAX_NUM_NODES; ++i){
			push(NODE_TYPE, i);
		}
		for (int i = 0; i < LIST_MAX_NUM_HEADS; ++i){
			push(HEAD_TYPE, i);
		}
		for (int i = 0; i < HEAD_NODE_SIZE; ++i){
			push(HEAD_NODE_TYPE, i);
		}
		firstProgramRun = false;
	}
}

List* List_create() {
	fillInitialStacks();

	int index = pop(HEAD_TYPE);
	if (index == ERROR_CODE){
		return NULL;
	}

	List* pList = &headList[index];
    pList->size = 0;
    pList->index = index;

    //initializes head pointer
    index = pop(HEAD_NODE_TYPE);
    pList->header = &headNodeList[index];
    pList->header->index = index;
    pList->header->data = NULL;

    //initializes trailer pointer
    index = pop(HEAD_NODE_TYPE);
    pList->trailer = &headNodeList[index];
    pList->trailer->index = index;
    pList->trailer->prev = pList->header; 
    pList->trailer->data = NULL;                                                                                                                            
    pList->header->next = pList->trailer;
    
    //initializes current pointer
    index = pop(HEAD_NODE_TYPE);
    pList->current = &headNodeList[index];
    pList->current->index = index;
    pList->current = pList->header;

	return pList;
}

int List_count(List* pList) {
	return pList->size;
}

void* List_first(List* pList) {
	if (pList->size == 0) {
		pList->current->data = NULL;
		return NULL;
	}

	pList->current = pList->header->next;
	return pList->current->data;
}

void* List_last(List* pList) {
	if (pList->size == 0) {
		pList->current->data = NULL;
		return NULL;
	}

	pList->current = pList->trailer->prev;
	return pList->current->data;
}

static bool isCurrentOutOfBounds(List* pList) {
	return !(pList->current != pList->header && pList->current != pList->trailer);
}

void* List_next(List* pList) {
	if (pList->size == 0) {
		return NULL;
	} else if (pList->current == pList->header) {
		pList->current = pList->header->next;
		return pList->current->data;
	} else if (pList->current != pList->trailer) { //if it's not beyond end of list
		pList->current = pList->current->next;
		return pList->current->data;
	}
	pList->current = pList->current->next;
	return NULL;
}

void* List_prev(List* pList) {
	if (pList->size == 0) {
		return NULL;
	} else if (pList->current == pList->trailer) {
		pList->current = pList->trailer->prev;
		return pList->current->data;
	} else if (pList->current != pList->header) { //if it's not beyond start of list
		pList->current = pList->current->prev;
		return pList->current->data;
	}
	pList->current = pList->current->prev;
	return NULL;
}

void* List_curr(List* pList) {
	if (pList->size == 0) {
		return NULL;
	} if (!isCurrentOutOfBounds(pList)) {
		return pList->current->data;
	}
	return NULL;
}

static void increaseSize(List* pList) {
	int size = pList->size;
	pList->size = size + 1;
}

static void updateCurrent(List* pList, Node* node) {
	pList->current = node;
}

//Helper function to insert a single node inside pList after the passed in node
static int add(Node* node, void* pItem, List* pList) {
	int index = pop(NODE_TYPE);
	if (index == ERROR_CODE){
		return ERROR_CODE;
	}

	Node* new = &nodeList[index];
	new->data = pItem;
	new->prev = node;
	new->next = node->next;
	node->next->prev = new;
	node->next = new;
	new->index = index;

	increaseSize(pList);
	updateCurrent(pList, new);
	return 0;
}

int List_prepend(List* pList, void* pItem) {
	return add(pList->header, pItem, pList);
}

int List_append(List* pList, void* pItem) {
	return add(pList->trailer->prev, pItem, pList);
}

int List_add(List* pList, void* pItem) {
	if (pList->current == pList->header) {
		return List_prepend(pList, pItem);
	} else if (pList->current == pList->trailer) {
		return List_append(pList, pItem);
	} else {
		return add(pList->current, pItem, pList);
	}
}

int List_insert(List* pList, void* pItem) {
	if (pList->current == pList->header) {
		return List_prepend(pList, pItem);
	} else if (pList->current == pList->trailer) {
		return List_append(pList, pItem);
	} else {
		return add(pList->current->prev, pItem, pList);
	}
}

static void decreaseSize(List* pList) {
	int size = pList->size;
	pList->size = size - 1;
}

//Helper function to remove node inside pList
static void* removeNode(Node* node, List* pList) {
	Node* prev = node->prev;
	Node* next = node->next;
	prev->next = next;
	next->prev = prev;

	int index = node->index;
	push(NODE_TYPE, index);
	decreaseSize(pList);

	return node->data;
}

void* List_remove(List* pList) {
	if (isCurrentOutOfBounds(pList) || pList->size == 0) {
		return NULL;
	}
	void* data = removeNode(pList->current, pList);

	List_next(pList);

	return data;
}

void* List_trim(List* pList) {
	if (pList->size == 0) {
		return NULL;
	}
	void* data = removeNode(pList->trailer->prev, pList);
	updateCurrent(pList, pList->trailer->prev);
	
	return data;
}

static void increaseSizeBy(List* pList, int num) {
	int size = pList->size;
	pList->size = size + num;
}

//Deallocates memory to the head pointer
static void freeHead(List* pList) {
	int index = pList->header->index;
	push(HEAD_NODE_TYPE, index);

	index = pList->trailer->index;
	push(HEAD_NODE_TYPE, index);

	index = pList->current->index;
	push(HEAD_NODE_TYPE, index);

	index = pList->index;
	push(HEAD_TYPE, index);
}

void List_concat(List* pList1, List* pList2) {
	pList1->trailer->prev->next = pList2->header->next;
	pList2->header->next->prev = pList1->trailer->prev;
	pList1->trailer = pList2->trailer;

	increaseSizeBy(pList1, pList2->size);
	freeHead(pList2);
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
	updateCurrent(pList, pList->header->next);
	while(pList->size != 0) {
		(*pItemFreeFn)(pList->header->next);
		List_remove(pList);
	}

	freeHead(pList);
}

void List_clear(List* pList) {
	if (pList != NULL){
		updateCurrent(pList, pList->header->next);
		while(pList->size != 0) {
			List_remove(pList);
		}

		freeHead(pList);
	}
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
	while(pList->current != pList->trailer) {
		if ((*pComparator)(pList->current->data, pComparisonArg)) {
			return pList->current->data;
		}
		updateCurrent(pList, pList->current->next);
	}

	updateCurrent(pList, pList->trailer);
	return NULL;
}