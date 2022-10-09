#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct Queue
{
	void** arr;	//	存放内存的首地址
	size_t cal;	//	容量
	int front;	//	队头位置
	int back;	//	队尾位置
}Queue;

//	创建队列
Queue* create_queue(size_t cal);
//	销毁队列
void destory_queue(Queue* queue);
//	队空
bool empty_queue(Queue* queue);
//	队满
bool full_queue(Queue* queue);
//	入队
void push_queue(Queue* queue,void* arg);
//	出队
void pop_queue(Queue* queue);
//	队头
void* front_queue(Queue* queue);
//	队尾
void* back_queue(Queue* queue);

#endif//QUEUE_H
