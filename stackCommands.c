#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CAPACITY 20

typedef struct {
    char* items[MAX_CAPACITY];
    int size;
} Stack;

Stack* create_stack() {
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->size = 0;
    return stack;
}

void destroy_stack(Stack* stack) {
    for (int i = 0; i < stack->size; i++) {
        free(stack->items[i]);
    }
    free(stack);
}

void push(Stack* stack, char* item) {
    if (stack->size == MAX_CAPACITY) {
        free(stack->items[0]);
        for (int i = 0; i < stack->size - 1; i++) {
            stack->items[i] = stack->items[i+1];
        }
        stack->size--;
    }
    char* new_item = (char*) malloc(strlen(item) + 1);
    strcpy(new_item, item);
    stack->items[stack->size++] = new_item;
}

char* pop(Stack* stack) {
    if (stack->size == 0) {
        return NULL;
    }
    char* item = stack->items[--stack->size];
    return item;
}

char* top(Stack* stack) {
    if (stack->size == 0) {
        return NULL;
    }
    char* item = stack->items[stack->size-1];
    return item;
}

char* get_element_at(Stack* stack, int index) {
    if (index < 0 || index > stack->size)
    {
        // printf("Null Returned\n");
        return NULL;
    }
    // printf("getting element\n");
    return stack->items[(stack->size) - index];
}

int get_stack_size(Stack* stack) {
    return stack->size;
}

// int main() {
//     Stack* stack = create_stack();

//     // push(stack, "apple");
//     push(stack, "banana");
//     push(stack, "cherry");

//     push(stack, "foo");
//     push(stack, "bar");
//     push(stack, "baz");
//     push(stack, "qux");
//     push(stack, "quux");
//     push(stack, "corge");
//     push(stack, "grault");
//     push(stack, "garply");
//     push(stack, "waldo");
//     push(stack, "fred");
//     push(stack, "plugh");
//     push(stack, "xyzzy");
//     push(stack, "thud");
//     push(stack, "quuz");
//     push(stack, "zxcvb");
//     push(stack, "asdfg");
//     push(stack, "qwerty");
//     push(stack, "uiop");
//     push(stack, "hjkl");

//     printf("Top element: %s\n", top(stack));

//     printf("last element %s \n", get_element_at(stack, 19));

//     char* item;
//     while ((item = pop(stack))) {
//         printf("Popped: %s\n", item);
//         free(item);
//     }

//     destroy_stack(stack);
//     return 0;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #define STACK_SIZE 20

// typedef struct {
//     char *items[STACK_SIZE];
//     int top;
// } Stack;

// void initStack(Stack *stack) {
//     stack->top = -1;
// }

// int isFull(Stack *stack) {
//     return stack->top == STACK_SIZE - 1;
// }

// int isEmpty(Stack *stack) {
//     return stack->top == -1;
// }

// void push(Stack *stack, char *item) {
//     if (isFull(stack)) {
//         printf("Stack is full. Popping bottom item.\n");
//         free(stack->items[0]); // free memory of bottom item
//         for (int i = 1; i < STACK_SIZE; i++) { // move all items up
//             stack->items[i-1] = stack->items[i];
//         }
//         stack->top--; // decrement top
//     }
//     stack->top++;
//     stack->items[stack->top] = malloc(strlen(item)+1);
//     strcpy(stack->items[stack->top], item);
// }

// char *pop(Stack *stack) {
//     if (isEmpty(stack)) {
//         printf("Stack is empty. Cannot pop.\n");
//         return NULL;
//     }
//     char *poppedItem = stack->items[stack->top];
//     stack->top--;
//     return poppedItem;
// }

// int main() {
//     Stack myStack;
//     initStack(&myStack);

//     push(&myStack, "Hello");
//     push(&myStack, "World");
//     push(&myStack, "How");
//     push(&myStack, "Are");
//     push(&myStack, "You");
//     push(&myStack, "Today");
//     push(&myStack, "This");
//     push(&myStack, "Is");
//     push(&myStack, "A");
//     push(&myStack, "Test");
//     push(&myStack, "Of");
//     push(&myStack, "The");
//     push(&myStack, "Stack");
//     push(&myStack, "Implementation");
//     push(&myStack, "With");
//     push(&myStack, "Max");
//     push(&myStack, "Capacity");
//     push(&myStack, "Of");
//     push(&myStack, "20");
//     push(&myStack, "Items");
//     push(&myStack, "Overflow");
    
//     printf("Popped: %s\n", pop(&myStack));
//     printf("Popped: %s\n", pop(&myStack));
//     printf("Popped: %s\n", pop(&myStack));
    
//     push(&myStack, "New Item");
//     printf("Popped: %s\n", pop(&myStack));
    
//     push(&myStack, "Another New Item");
//     printf("Popped: %s\n", pop(&myStack));
    
//     return 0;
// }