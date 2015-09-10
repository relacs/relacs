#include <linux/module.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <rtai.h>
#include <rtai_sched.h>

MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "RTAI test periodic task" );

#define TICK_PERIOD 1000000
#define TASK_PRIORITY 1
#define STACK_SIZE 10000

volatile int end;
static RT_TASK rt_task;

static void fun(long int t)
{
  int counter = 0;
  while (!end) {
  //  while (1) {
    if ( counter % 100 == 0 )
      rt_printk( "counter=%d\n", counter );
    counter++;
    rt_task_wait_period();
  }
}

int init_module(void)
{
  RTIME tick_period;
  end = 0;
  // rt_set_periodic_mode();
  rt_task_init(&rt_task, fun, 1, STACK_SIZE, TASK_PRIORITY, 1, 0);
  tick_period = start_rt_timer(nano2count(TICK_PERIOD));
  rt_task_make_periodic(&rt_task, rt_get_time() + tick_period, tick_period);
  rt_printk( "LOADED RT_PROCESS\n" );
  return 0;
}

void cleanup_module(void)
{
  end = 1;
  msleep( 100 );
  // stop_rt_timer();
  rt_task_delete(&rt_task);
  rt_printk( "CLEANED UP RT_PROCESS\n" );
  return;
}

