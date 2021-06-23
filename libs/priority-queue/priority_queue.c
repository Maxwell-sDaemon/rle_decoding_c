#include <stdio.h>
#include <stdlib.h>
// priority Node
typedef struct node {
   int data;
   int priority;
   struct node* next;
} Node;
Node* newNode(int d, int p) {
   Node* temp = (Node*)malloc(sizeof(Node));
   temp->data = d;
   temp->priority = p;
   temp->next = NULL;
   return temp;
}
int peek(Node** head) {
   return (*head)->data;
}
void pop(Node** head) {
   Node* temp = *head;
   (*head) = (*head)->next;
   free(temp);
}
void push(Node** head, int d, int p) {
   Node* start = (*head);
   Node* temp = newNode(d, p);
   if ((*head)->priority > p) {
      temp->next = *head;
      (*head) = temp;
   } else {
      while (start->next != NULL &&
      start->next->priority < p) {
         start = start->next;
      }
      // Either at the ends of the list
      // or at required position
      temp->next = start->next;
      start->next = temp;
   }
}
// Function to check the queue is empty
int isEmpty(Node** head) {
   return (*head) == NULL;
}
/*// main function
int main() {
   Node* pq = newNode(7, 1);
   push(&pq, 1, 2);
   push(&pq, 3, 3);
   push(&pq, 2, 0);
   while (!isEmpty(&pq)) {
      printf("%d ", peek(&pq));
      pop(&pq);
   }
   return 0;

}*/



















