#include <stdio.h>
#include <stdlib.h>

#define STACK_MAX_SIZE 256
#define MAX_OBJECTS 8

typedef enum 
{
	INT,
	TWIN
} oType;

typedef struct sObject
{
	oType type;
	unsigned char marked;

	sObject* next;

	union
	{
		int value;

		struct
		{
			sObject* head;
			sObject* tail;
		};
	};
} Object;

typedef struct
{
	Object* stack[STACK_MAX_SIZE];
	int stackSize;

	Object* first;

	int numObjects;
	int maxObjects;
} vm;

void push(vm* vm, Object* object)
{
	vm->stack[vm->stackSize] = object;
	vm->stackSize++;
}

Object* pop(vm* vm)
{
	vm->stackSize--;
	return vm->stack[vm->stackSize];
}

vm* newVm()
{
	vm* mainVm = (vm*)malloc(sizeof(vm));
	mainVm->stackSize = 0;
	mainVm->first = NULL;
	mainVm->numObjects = 0;
	mainVm->numObjects = MAX_OBJECTS;
	return mainVm;
}

Object* newObject(vm* vm, oType type)
{
	if (vm->numObjects >= vm->maxObjects)
	{
		gc(vm);
	}
	Object* object = (Object*)malloc(sizeof(Object));
	object->type = type;

	vm->numObjects++;
	return object;
}

void pushInt(vm* vm, int value)
{
	Object* object = newObject(vm, INT);
	object->value = value; 
	push(vm, object);
}

Object* pushTwin(vm* vm)
{
	Object* object = newObject(vm, TWIN);
	object->tail = pop(vm);
	object->head = pop(vm);

	push(vm, object);
	return object;
}

void markAll(vm* vm)
{
	for (int i = 0; i < vm->stackSize; i++)
	{
		mark(vm->stack[i]);
	}
}

void mark(Object* object)
{
	if (object->marked) return;

	object->marked = 1;

	if (object->type == TWIN)
	{
		mark(object->head);
		mark(object->tail);
	}
}

void marksweep(vm* vm)
{
	Object** object = &vm->first; 
	while (*object)
	{
		if (!(*object)->marked)
		{
			Object* unreached = *object;
			*object = unreached->next;
			free(unreached);

			vm->numObjects--;
		}
		else
		{
			(*object)->marked = 0;
			object = &(*object)->next;
		}
	}
}

void gc(vm* vm)
{
	int numObjects = vm->numObjects;

	markAll(vm);
	marksweep(vm);

	vm->maxObjects = vm->numObjects * 2;
}

void freeVm(vm* vm)
{
	vm->stackSize = 0;
	gc(vm);
	free(vm);
}

void printObj(Object* object)
{
	switch (object->type)
	{
	case INT:
		printf("%d", object->value);
		break;
	case TWIN:
		printf("(");
		printObj(object->head);
		printf(", ");
		printObj(object->tail);
		printf(")");
		break;
	}
}

void first_test()
{
	vm* mainVm = newVm();
	pushInt(mainVm, 1);
	pushInt(mainVm, 2);

	gc(mainVm);
	freeVm(mainVm);
}

void second_test()
{
	vm* mainVm = newVm();
	pushInt(mainVm, 1);
	pushInt(mainVm, 2);
	pop(mainVm);
	pop(mainVm);

	gc(mainVm);
	freeVm(mainVm);
}

void third_test()
{
	vm* mainVm = newVm();
	pushInt(mainVm, 1);
	pushInt(mainVm, 2);
	pushTwin(mainVm);
	pushInt(mainVm, 3);
	pushInt(mainVm, 4);
	pushTwin(mainVm);
	pushTwin(mainVm);

	gc(mainVm);
	freeVm(mainVm);
}

void fourth_test()
{
	vm* mainVm = newVm();
	pushInt(mainVm, 1);
	pushInt(mainVm, 2);
	pushTwin(mainVm);
	pushInt(mainVm, 3);
	pushInt(mainVm, 4);
	pushTwin(mainVm);
	pushTwin(mainVm);

	gc(mainVm);
	freeVm(mainVm);
}