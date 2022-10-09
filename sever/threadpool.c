#include <stdlib.h>
#include "threadpool.h"

//	线程的入口函数
static void* run(void* arg)
{
	//	记录线程池
	ThreadPool* threadpool = (ThreadPool*)arg;
	for(;;)
	{
		//	消费数据
		void* data = pop_threadpool(threadpool);
		//	如果能拿到数据则执行业务逻辑函数
		threadpool->workfp(data);
		//	本次任务结束 继续重新消费数据
	}
}

//	创建线程池
ThreadPool* create_threadpool(int thread_cnt,int store_cal,workFP workfp)
{
	//	申请线程池内存
	ThreadPool* threadpool = malloc(sizeof(ThreadPool));
	//	申请存储线程ID内存
	threadpool->tids = malloc(sizeof(pthread_t*)*thread_cnt);
	//	初始化线程数
	threadpool->thread_cnt = thread_cnt;
	//	创建队列仓库
	threadpool->store_buf = create_queue(store_cal);
	//	初始化业务函数
	threadpool->workfp = workfp;
	//	初始化互斥量、条件变量
	pthread_mutex_init(&threadpool->flock,NULL);
	pthread_mutex_init(&threadpool->block,NULL);
	pthread_cond_init(&threadpool->full,NULL);
	pthread_cond_init(&threadpool->empty,NULL);

	return threadpool;
}


//	启动线程池
void start_threadpool(ThreadPool* threadpool)
{
	for(int i=0; i<threadpool->thread_cnt; i++)
	{
		pthread_create(threadpool->tids+i,NULL,run,threadpool);
	}
}


//	生产数据
void push_threadpool(ThreadPool* threadpool,void* data)
{
	//	队尾加锁
	pthread_mutex_lock(&threadpool->block);

	//	满仓 则休眠
	while(full_queue(threadpool->store_buf))
	{
		//	唤醒消费者线程
		pthread_cond_signal(&threadpool->empty);
		//	休眠并解锁队尾
		pthread_cond_wait(&threadpool->full,&threadpool->block);
	}

	//	生产数据并存入队尾
	push_queue(threadpool->store_buf,data);
	//	唤醒消费者线程
	pthread_cond_signal(&threadpool->empty);
	//	解锁
	pthread_mutex_unlock(&threadpool->block);
}


//	消费数据
void* pop_threadpool(ThreadPool* threadpool)
{
	//	队头加锁
	pthread_mutex_lock(&threadpool->flock);

	//	空仓 则休眠
	while(empty_queue(threadpool->store_buf))
	{
		//	唤醒生产者线程
		pthread_cond_signal(&threadpool->full);
		//	休眠并解锁队头
		pthread_cond_wait(&threadpool->empty,&threadpool->flock);
	}

	//	消费数据
	void* data = front_queue(threadpool->store_buf);
	pop_queue(threadpool->store_buf);

	//	唤醒生产者线程
	pthread_cond_signal(&threadpool->full);
	//	解锁
	pthread_mutex_unlock(&threadpool->flock);
	return	data;
}


//	销毁线程池
void destory_threadpool(ThreadPool* threadpool)
{
	//	结束线程池所有消费者线程
	for(int i=0; i<threadpool->thread_cnt; i++)
	{
		pthread_cancel(threadpool->tids[i]);
	}
	pthread_mutex_destroy(&threadpool->flock);
	pthread_mutex_destroy(&threadpool->block);
	pthread_cond_destroy(&threadpool->full);
	pthread_cond_destroy(&threadpool->empty);
	destory_queue(threadpool->store_buf);
	free(threadpool->tids);
	free(threadpool);
}

