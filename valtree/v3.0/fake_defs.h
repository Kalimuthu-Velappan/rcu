/*
 * "Fake" declarations to scaffold a Linux-kernel SMP environment.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * Author: Michalis Kokologiannakis <mixaskok@gmail.com>
 */

#ifndef __FAKE_DEFS_H
#define __FAKE_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

/* Definitions taken from the Linux Kernel (v.3.0) */

#define __force
#define __kernel
#define notrace

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({				\
			const __typeof__( ((type *)0)->member ) *__mptr = (ptr); \
			(type *)( (char *)__mptr - offsetof(type,member) );})

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

/*
 * Getting something that works in C and CPP for an arg that may or may
 * not be defined is tricky.  Here, if we have "#define CONFIG_BOOGER 1"
 * we match on the placeholder define, insert the "0," for arg1 and generate
 * the triplet (0, 1, 0).  Then the last step cherry picks the 2nd arg (a one).
 * When CONFIG_BOOGER is not defined, we generate a (... 1, 0) pair, and when
 * the last step cherry picks the 2nd arg, we get a zero.
 */
#define __ARG_PLACEHOLDER_1 0,
#define config_enabled(cfg) _config_enabled(cfg)
#define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
#define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
#define ___config_enabled(__ignored, val, ...) val

/*
 * IS_ENABLED(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y' or 'm',
 * 0 otherwise.
 *
 */
#define IS_ENABLED(option)						\
	(config_enabled(option) || config_enabled(option##_MODULE))

#ifndef __maybe_unused
# define __maybe_unused         /* unimplemented */
#endif

#define ACCESS_ONCE(x) (*(volatile __typeof__(x) *)&(x))
#define ATOMIC_INIT(i)  { (i) }

/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ volatile("": : :"memory")

/* Other barriers -- x86 config */
#define mb()    __asm__ volatile("mfence":::"memory")
#define rmb()   __asm__ volatile("lfence":::"memory")
#define wmb()   __asm__ volatile("sfence" ::: "memory")

#define dma_rmb()       barrier()
#define dma_wmb()       barrier()

#define smp_mb()        mb()
#define smp_rmb()       dma_rmb()
#define smp_wmb()       barrier()

#define read_barrier_depends()          do { } while (0)
#define smp_read_barrier_depends()      do { } while (0)

#define smp_store_release(p, v)			\
	do {					\
		barrier();			\
		ACCESS_ONCE(*p) = (v);		\
	} while (0)

#define smp_load_acquire(p)				\
	({						\
		__typeof__(*p) ___p1 = ACCESS_ONCE(*p);	\
							\
		barrier();				\
		___p1;					\
	})

#define smp_mb__before_atomic() barrier()
#define smp_mb__after_atomic()  barrier()
#define smp_mb__before_atomic_inc() barrier()
#define smp_mb__after_atomic_inc() barrier()

#define smp_mb__after_unlock_lock()     do { } while (0)

/* Atomic data types */
typedef struct {
	int counter;
} atomic_t;

typedef struct {
	long counter;
} atomic_long_t;

/* Boolean data types */
typedef _Bool bool;

enum {
	false	= 0,
	true	= 1
};

/* Integer types */
typedef unsigned long ulong;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define USHRT_MAX	((u16)(~0U))
#define SHRT_MAX	((s16)(USHRT_MAX>>1))
#define SHRT_MIN	((s16)(-SHRT_MAX - 1))
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL>>1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)
#define SIZE_MAX	(~(size_t)0)

#define U8_MAX		((u8)~0U)
#define S8_MAX		((s8)(U8_MAX>>1))
#define S8_MIN		((s8)(-S8_MAX - 1))
#define U16_MAX		((u16)~0U)
#define S16_MAX		((s16)(U16_MAX>>1))
#define S16_MIN		((s16)(-S16_MAX - 1))
#define U32_MAX		((u32)~0U)
#define S32_MAX		((s32)(U32_MAX>>1))
#define S32_MIN		((s32)(-S32_MAX - 1))
#define U64_MAX		((u64)~0ULL)
#define S64_MAX		((s64)(U64_MAX>>1))
#define S64_MIN		((s64)(-S64_MAX - 1))

/* Integer division that rounds up */
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */
#define __stringify_1(x...)     #x
#define __stringify(x...)       __stringify_1(x)

/* List data types definitions and functions */
struct list_head {
	struct list_head *next, *prev;
};
 
#define LIST_HEAD_INIT(name) { &(name), &(name) }
 
#define LIST_HEAD(name)					\
	struct list_head name = LIST_HEAD_INIT(name)
 
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
        next->prev = new;
        new->next = next;
        new->prev = prev;
        prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
        __list_add(new, head, head->next);
}

#define list_entry(ptr, type, member) \
        container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
        list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
        list_entry((pos)->member.next, __typeof__(*(pos)), member)

#define list_for_each_entry(pos, head, member)                          \
	for (pos = list_first_entry(head, __typeof__(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/* Rate limit definitions */
struct ratelimit_state {
        pthread_mutex_t lock;           /* protect the state */

        int             interval;
        int             burst;
        int             printed;
        int             missed;
        unsigned long   begin;
};

#define RATELIMIT_STATE_INIT(name, interval_init, burst_init) {         \
                .lock           = __RAW_SPIN_LOCK_UNLOCKED(name.lock),  \
                .interval       = interval_init,                        \
                .burst          = burst_init,                           \
        }

#define RATELIMIT_STATE_INIT_DISABLED                                   \
        RATELIMIT_STATE_INIT(ratelimit_state, 0, DEFAULT_RATELIMIT_BURST)

#define DEFINE_RATELIMIT_STATE(name, interval_init, burst_init)         \
                                                                        \
        struct ratelimit_state name =                                   \
                RATELIMIT_STATE_INIT(name, interval_init, burst_init)   \


#define WARN_ON_RATELIMIT(condition, state)                     \
        WARN_ON(condition)

/* Notifier definitions */
#define NOTIFY_DONE             0x0000          /* Don't care */
#define NOTIFY_OK               0x0001          /* Suits me */
#define NOTIFY_STOP_MASK        0x8000          /* Don't call further */
#define NOTIFY_BAD              (NOTIFY_STOP_MASK|0x0002) /* Bad/Veto action */

#define atomic_notifier_chain_register(x, y) do { } while(0)

/* Generic CPU definitions */
#define CPU_ONLINE              0x0002 /* CPU (unsigned)v is up */
#define CPU_UP_PREPARE          0x0003 /* CPU (unsigned)v coming up */
#define CPU_UP_CANCELED         0x0004 /* CPU (unsigned)v NOT coming up */
#define CPU_DOWN_PREPARE        0x0005 /* CPU (unsigned)v going down */
#define CPU_DOWN_FAILED         0x0006 /* CPU (unsigned)v NOT going down */
#define CPU_DEAD                0x0007 /* CPU (unsigned)v dead */
#define CPU_DYING               0x0008 /* CPU (unsigned)v not running any task,
                                        * not handling interrupts, soon dead.
                                        * Called on the dying cpu, interrupts
                                        * are already disabled. Must not
                                        * sleep, must not fail */
#define CPU_POST_DEAD           0x0009 /* CPU (unsigned)v dead, cpu_hotplug
                                        * lock is dropped */
#define CPU_STARTING            0x000A /* CPU (unsigned)v soon running.
                                        * Called on the new cpu, just before
                                        * enabling interrupts. Must not sleep,
                                        * must not fail */

/* Used for CPU hotplug events occurring while tasks are frozen due to a suspend
 * operation in progress
 */
#define CPU_TASKS_FROZEN        0x0010

#define CPU_ONLINE_FROZEN       (CPU_ONLINE | CPU_TASKS_FROZEN)
#define CPU_UP_PREPARE_FROZEN   (CPU_UP_PREPARE | CPU_TASKS_FROZEN)
#define CPU_UP_CANCELED_FROZEN  (CPU_UP_CANCELED | CPU_TASKS_FROZEN)
#define CPU_DOWN_PREPARE_FROZEN (CPU_DOWN_PREPARE | CPU_TASKS_FROZEN)
#define CPU_DOWN_FAILED_FROZEN  (CPU_DOWN_FAILED | CPU_TASKS_FROZEN)
#define CPU_DEAD_FROZEN         (CPU_DEAD | CPU_TASKS_FROZEN)
#define CPU_DYING_FROZEN        (CPU_DYING | CPU_TASKS_FROZEN)
#define CPU_STARTING_FROZEN     (CPU_STARTING | CPU_TASKS_FROZEN)

/* Hibernation and suspend events */
#define PM_HIBERNATION_PREPARE  0x0001 /* Going to hibernate */
#define PM_POST_HIBERNATION     0x0002 /* Hibernation finished */
#define PM_SUSPEND_PREPARE      0x0003 /* Going to suspend the system */
#define PM_POST_SUSPEND         0x0004 /* Suspend finished */
#define PM_RESTORE_PREPARE      0x0005 /* Going to restore a saved image */
#define PM_POST_RESTORE         0x0006 /* Restore failed */


/* "Cheater" definitions based on restricted Kconfig choices. */

#define CONFIG_TREE_RCU
#define CONFIG_NO_HZ
#define CONFIG_SMP

#ifndef CONFIG_RCU_FANOUT
# define CONFIG_RCU_FANOUT 32
#endif

#ifndef CONFIG_RCU_FANOUT_LEAF
# define CONFIG_RCU_FANOUT_LEAF 16
#endif

#define CONFIG_RCU_STALL_COMMON
#define CONFIG_RCU_CPU_STALL_TIMEOUT 21

#ifndef CONFIG_NR_CPUS
# ifdef FORCE_FAILURE_6
#  define CONFIG_NR_CPUS (CONFIG_RCU_FANOUT_LEAF + 1)
# else
#  define CONFIG_NR_CPUS 2
# endif
#endif

#define NR_CPUS CONFIG_NR_CPUS
#define nr_cpu_ids NR_CPUS
#define HZ 100

#undef __CHECKER__
#undef CONFIG_PREEMPT_RCU
#undef CONFIG_RCU_FANOUT_EXACT
#undef CONFIG_RCU_FAST_NO_HZ
#undef CONFIG_RCU_BOOST
#undef CONFIG_RCU_NOCB_CPU
#undef CONFIG_RCU_NOCB_CPU_ALL
#undef CONFIG_RCU_CPU_STALL_INFO
#undef CONFIG_HOTPLUG_CPU
#undef CONFIG_NO_HZ_FULL_SYSIDLE
#undef CONFIG_RCU_TRACE
#undef CONFIG_GENERIC_LOCKBREAK
#undef CONFIG_DEBUG_OBJECTS_RCU_HEAD
#undef CONFIG_TRACING
#undef CONFIG_DEBUG_LOCK_ALLOC
#undef CONFIG_DEBUG_SPINLOCK
#undef CONFIG_DEBUG_MUTEXES
#undef CONFIG_RCU_USER_QS
#undef CONFIG_MODULES
#undef CONFIG_PROVE_RCU
#undef CONFIG_TASKS_RCU
#undef CONFIG_PREEMPT_COUNT

/* Some definitions based on CONFIG_NO_HZ_FULL=n option */
#define tick_nohz_full_enabled() 0
#define is_housekeeping_cpu(cpu) 1
#define housekeeping_affine(cpu) do { } while (0)

/* Stub some compiler directives */
#define ____cacheline_internodealigned_in_smp
#define __percpu
#define __rcu
#define __init
#define __cpuinit
#define __cpuinitdata
#define __jiffy_data
#define __read_mostly
#define __noreturn

#define __acquires(x)
#define __releases(x)
#define __release(RCU)
#define __acquire(RCU)

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

struct lock_class_key { };


/* "Cheater" definitions for percpu variables -- arrays are used instead */
#define DECLARE_PER_CPU(type, name) extern __typeof__(type) name[NR_CPUS]
#define DEFINE_PER_CPU(type, name)  __typeof__(type) name[NR_CPUS]
#define DEFINE_PER_CPU_SHARED_ALIGNED(type, name) DEFINE_PER_CPU(type, name)

/* Disable CONFIG_RCU_TRACE */
#define tracepoint_string(x) ""
#define trace_rcu_utilization(x) do { } while (0)
#define trace_rcu_grace_period(rcuname, gpnum, gpevent) do { } while (0)
#define trace_rcu_grace_period_init(rcuname, gpnum, level, grplo, grphi, \
                                    qsmask) do { } while (0)
#define trace_rcu_future_grace_period(rcuname, gpnum, completed, c,	\
                                      level, grplo, grphi, event)	\
	do { } while (0)
#define trace_rcu_nocb_wake(rcuname, cpu, reason) do { } while (0)
#define trace_rcu_preempt_task(rcuname, pid, gpnum) do { } while (0)
#define trace_rcu_unlock_preempted_task(rcuname, gpnum, pid) do { } while (0)
#define trace_rcu_quiescent_state_report(rcuname, gpnum, mask, qsmask, level, \
                                         grplo, grphi, gp_tasks) do { } \
        while (0)
#define trace_rcu_fqs(rcuname, gpnum, cpu, qsevent) do { } while (0)
#define trace_rcu_dyntick(polarity, oldnesting, newnesting) do { } while (0)
#define trace_rcu_prep_idle(reason) do { } while (0)
#define trace_rcu_callback(rcuname, rhp, qlen_lazy, qlen) do { } while (0)
#define trace_rcu_kfree_callback(rcuname, rhp, offset, qlen_lazy, qlen) \
        do { } while (0)
#define trace_rcu_batch_start(rcuname, qlen_lazy, qlen, blimit) \
        do { } while (0)
#define trace_rcu_invoke_callback(rcuname, rhp) do { } while (0)
#define trace_rcu_invoke_kfree_callback(rcuname, rhp, offset) do { } while (0)
#define trace_rcu_batch_end(rcuname, callbacks_invoked, cb, nr, iit, risk) \
        do { } while (0)
#define trace_rcu_torture_read(rcutorturename, rhp, secs, c_old, c)	\
        do { } while (0)
#define trace_rcu_barrier(name, s, cpu, cnt, done) do { } while (0)

/* Module macros */
#define MODULE_ALIAS(x)
#define module_param(name, type, perm)
#define EXPORT_SYMBOL_GPL(sym)

/* Logging macros */
#define KERN_INFO
#define KERN_ERR
#define printk(args...) fprintf(stderr, args)
#define pr_info(args...) fprintf(stderr, args)
#define pr_err(args...) fprintf(stderr, args)
#define pr_cont(args...) fprintf(stderr, args)
#define ftrace_dump(x) do { } while (0)
#define dump_cpu_task(x) do { } while (0)

#define lockdep_set_class_and_name(lock, class, name) do { } while (0)

/* Stub some rcu_expedited stuff */
typedef int cpumask_var_t;

int rcu_expedited;

#define zalloc_cpumask_var(cm, GFP) 0
#define cpumask_copy(cm, mask) do { } while (0)
#define cpumask_clear_cpu(cpu_id, cm) do { } while (0)
#define cpumask_weight(cm) 0
#define try_stop_cpus(exp, fun, arg) 0
#define EAGAIN 0
#define free_cpumask_var(cm) do { } while (0)
#define udelay(time) do { } while (0)

/* Do not keep track of the process' state */
#define set_current_state(STATE) 
#define __set_current_state(STATE)

/* Nidhugg will take care of the scheduling for us */
#define schedule()

/* is_idle_task should NOT be a statically defined macro. 
 * However, due to the fact that we are verifying only a portion of Tree RCU's
 * source code, we CAN set it equal to 1. That way, no warning is triggered
 * and there should not be any behavioural change for RCU.
 * idle_task just returns a null pointer.
 * signal_pending(x) always returns false.
 */
#define is_idle_task(current) 1
#define idle_task(x) NULL
#define signal_pending(x) 0

/* Our definition for task_struct */
struct task_struct {
	int pid;
	pthread_t tid;
	unsigned long state;
	char comm[20];
};
struct task_struct __thread *current;

/* CPU iterators based on CONFIG_HOTPLUG_CPU=n */
#define smp_processor_id() get_cpu()
#define for_each_possible_cpu(cpu) for ((cpu) = 0; (cpu) < NR_CPUS; (cpu)++)
#define for_each_online_cpu(cpu) for_each_possible_cpu(cpu)
#define for_each_cpu(cpu, cm) for_each_possible_cpu(cpu)

/* 
 * on_each_cpu() is stubbed because we don't really care about it. This 
 * function is only used in rcu_barrier(), and for the purposes of the
 * test we can stub it.
 */
#define on_each_cpu(func, info, wait) do { } while (0)

/* Softirq definitions and data types */
#define open_softirq(x, y) do { } while (0)
int need_softirq[NR_CPUS];
#define raise_softirq(x) do { need_softirq[get_cpu()] = 1; } while (0)
#define in_softirq() 0

struct softirq_action {
};

/* Notifier data types -- not of much interest */
struct notifier_block;

typedef int (*notifier_fn_t)(struct notifier_block *nb,
			     unsigned long action, void *data);

struct notifier_block {
	notifier_fn_t notifier_call;
	struct notifier_block __rcu *next;
	int priority;
};

/* Warning statements which trigger assertions */
int noassert;
#ifdef ORDERING_BUG
#define SET_NOASSERT() do { noassert = 1; } while (0)
#define CK_NOASSERT() noassert
#define BUG_ON(condition) assert(noassert || !(condition))
#define BUILD_BUG_ON(condition) assert(noassert || !(condition))
#define WARN_ON(condition) assert(noassert || !(condition))
#define WARN_ONCE(condition, format...) assert(noassert || !(condition))
#define WARN_ON_ONCE(condition)	 ({		\
	int __ret_warn_on = !!(condition);	\
	assert(noassert || !(condition));	\
	__ret_warn_on;				\
})
#else
#define SET_NOASSERT() do { noassert = 1; smp_mb(); } while (0)
#define CK_NOASSERT() ({ smp_mb(); noassert; })
#define BUG_ON(condition) assert(!(condition) || CK_NOASSERT())
#define BUILD_BUG_ON(condition) assert(!(condition) || CK_NOASSERT())
#define WARN_ON(condition) assert(!(condition) || CK_NOASSERT())
#define WARN_ONCE(condition, format...) assert(!(condition) || CK_NOASSERT())
#define WARN_ON_ONCE(condition) ({		\
	int __ret_warn_on = !!(condition);	\
	assert(!(condition) || CK_NOASSERT());	\
	__ret_warn_on;				\
})
#endif

#define panic(msg) { perror(msg); assert(0); }
#define IS_ERR(x) 0

/* 
 * Atomic operations based on cheater definitions and gcc language extensions.
 * These language extensions are also supported by the clang compiler.
 * Note that these operations are supported under SC, TSO and PSO in Nidhugg, 
 * but only for the model __ATOMIC_SEQ_CST, even if otherwise specified.
 */
#define atomic_add(i, v) __atomic_add_fetch(&(v)->counter, i, __ATOMIC_RELAXED)
#define atomic_add_return(i, v) atomic_add(i, v)
#define atomic_sub(i, v) __atomic_sub_fetch(&(v)->counter, i, __ATOMIC_RELAXED)
#define atomic_inc(v) atomic_add(1, v)
#define atomic_inc_return(v) atomic_inc(v)
#define atomic_dec(v) atomic_sub(1, v)
#define atomic_dec_and_test(v) !atomic_dec(v)
#define atomic_set(v, i) (v)->counter = i
#define atomic_read(v) ACCESS_ONCE((v)->counter)
#define atomic_cmpxchg(v, old, new)					\
	__atomic_compare_exchange(&(v)->counter, &old, &new, 0,		\
				  __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define atomic_long_add(i, v) atomic_add(i, v)
#define atomic_long_add_return(i, v) atomic_add_return(i, v)
#define atomic_long_sub(i, v) atomic_sub(i, v)
#define atomic_long_inc(v) atomic_inc(v)
#define atomic_long_inc_return(v) atomic_inc_return(v)
#define atomic_long_dec(v) atomic_dec(v)
#define atomic_long_dec_and_test(v) atomic_dec_and_test(v)
#define atomic_long_read(v) atomic_read(v)
#define atomic_long_cmpxchg(v, old, new) atomic_cmpxchg(v, old, new)

/* Preempt and bh definitions */
#define preempt_enable() barrier()
#define preempt_disable() barrier()
#define preempt_disable_notrace() barrier()
#define preempt_enable_notrace() barrier()
#define local_bh_disable() do { } while (0)
#define local_bh_enable() do { } while (0)

#define prefetch(next) do { } while (0)

/* More CPU-relevant definitions, CONFIG_HOTPLUG_CPU=n  */
unsigned long volatile __jiffy_data jiffies;

#define cpu_is_offline(cpu) 0
#define cpu_is_online(cpu) 1
#define cpu_online(cpu) 1
#define need_resched() 1
#define nr_context_switches() 0
#define idle_cpu(cpu) 0
#define hardirq_count() 0
#define HARDIRQ_SHIFT 0
#define trigger_all_cpu_backtrace() do { } while (0)

/* Exclude CPU hotplug operations */
#define try_get_online_cpus() 1
#define num_online_cpus() nr_cpu_ids
#define get_online_cpus() do { } while (0)
#define put_online_cpus() do { } while (0)
#define cpu_notifier(fn, pri) do { (void)(fn); } while (0)
#define pm_notifier(fn, pri)  do { (void)(fn); } while (0)
#define register_cpu_notifier(notifier) do { } while (0)
#define hotcpu_notifier(n, a) do { } while (0)

#define smp_call_function_single(cpu, fun, arg, wait) do { } while (0)

/* Functions designated to run in early_initcalls must be called explicitly */
#define early_initcall(fn) 


/* Declarations to emulate CPU, interrupts, and scheduling.  */
void __VERIFIER_assume(int);

int get_cpu(void);
void set_cpu(int);

int cond_resched(void);
void smp_send_reschedule(int);
void set_need_resched(void);
void fake_acquire_cpu(int);
void fake_release_cpu(int);
#define might_sleep() do { } while (0)
void local_irq_save(unsigned long flags);
void local_irq_restore(unsigned long flags);
void local_irq_enable(void);
void local_irq_disable(void);
int irqs_disabled_flags(unsigned long flags);
int in_interrupt(void);
#define in_irq() in_interrupt()
void do_IRQ(void);

#endif /* __FAKE_DEFS_H */
