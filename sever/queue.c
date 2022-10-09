#include <stdlib.h>
#include "queue.h"

//	创建队列
Queue* create_queue(size_t cal)
{
	Queue* queue = malloc(sizeof(Queue));
	queue->arr = malloc(sizeof(void*)*(cal+1));
	queue->cal = cal+1;
	queue->front = 0;
	queue->back = 0;
	return queue;
}

//	销毁队列
void destory_queue(Queue* queue)
{
	free(queue->arr);
	free(queue);
}

//	队空
bool empty_queue(Queue* queue)
{
	return queue->front == queue->back;
}

//	队满
bool full_queue(Queue* queue)
{
	return queue->front == (queue->back+1)%queue->cal;
}

//	入队
void push_queue(Queue* queue,void* arg)
{
	queue->arr[queue->back++] = arg;
	queue->back %= queue->cal;
}

//	出队
void pop_queue(Queue* queue)
{
	queue->front = (queue->front+1)%queue->cal;
}

//	队头
void* front_queue(Queue* queue)
{
	return queue->arr[queue->front];
}

//	队尾
void* back_queue(Queue* queue)
{
	return queue->arr[(queue->back-1+queue->cal)%queue->cal];
}


