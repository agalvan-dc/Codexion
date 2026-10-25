/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:35:51 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 18:12:30 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODERS_H
# define CODERS_H

# include <pthread.h>
# include <sys/time.h>
# include <stdint.h>
# include <stdbool.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <limits.h>
# include <stdlib.h>

# include "algo/heap.h"

static inline void	heap_swap_items(t_heap_item *a, t_heap_item *b)
{
	t_heap_item	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

# define LOG_LINE_MAX	64
# define LOG_BATCH		256
# define NO_DONGLE		-1

typedef struct	s_log
{
	char	text[LOG_LINE_MAX];
}	t_log;

typedef enum e_state
{
	ST_WAIT,
	ST_COMPILE,
	ST_DEBUG,
	ST_REFACTOR,
	ST_BURNT,
	ST_DONE
}	t_state;


typedef struct	s_dongle
{
	pthread_mutex_t	mutex;
	bool			in_use;
	uint64_t		available_at;
}	t_dongle;

typedef struct	s_coder
{
	int				id;
	int				num_com;
	uint64_t		last_com;
	uint64_t		last_comp_start;
	uint64_t		deadline;
	uint64_t		wait_since;
	uint64_t		request_time;
	t_state			state;

	pthread_t		thread;
	pthread_cond_t	cond;
	int				left_dongle;
	int				right_dongle;
	bool			has_pair;

	struct	s_table	*table;
	struct	s_coder	*next;
}	t_coder;

typedef	struct	s_table
{
	int				nb_cod;
	int				nb_com;
	uint64_t		ttb;
	uint64_t		ttc;
	uint64_t		ttd;
	uint64_t		ttr;
	uint64_t		dc;
	t_scheduler		scheduler;

	bool			simulation_end;
	bool			burned_out;
	pthread_mutex_t	pool_mutex;

	t_coder			*coders;
	t_dongle		*dongles;
	t_heap			*wait_heap;
	pthread_mutex_t	heap_mutex;

	t_log			*logs;
	size_t			log_cap;
	size_t			log_count;
	size_t			log_head;
	pthread_mutex_t	log_mutex;
	pthread_cond_t	log_cond;

	pthread_t		printer;
	bool			printer_started;
	bool			printer_stop;
	pthread_t		*threads;
	int				thread_count;

	uint64_t		start_ms;
	int				done_count;
	int				burned_coder_id;
}	t_table;

/*	parse/parse.c */
t_table		*parse_arg(int argc, char **argv);
bool		in_range(int value, int min, int max);
bool		is_valid_num(char **argv);
bool		is_valid_time(char **argv);
bool		is_valid_scheduler(char *arg);

/*	parse/parsing_utils.c */
int			str_to_int(char *str);
bool		fill_table(t_table *tab, char **argv);

/*	parse/display.c */
void		show_usage(void);

/*	init/init_coders.c*/
t_coder		*init_coders(t_table *table);

/* init/init_dongles.c	*/
t_dongle	*init_dongles(t_table *table);

/*	init/init_threads.c */
bool		init_logs(t_table *table);
bool		init_waiters(t_table *table);
bool		init_threads(t_table *table);
bool		start_threads(t_table *table);
void		join_threads(t_table *table);

/*	utils/free.c */
void		*free_prev(void *coder, void *aux);
void		pthread_destroy(t_table *table);
bool		pthread_coder_destroy(t_coder *coder);

/*	core/coders.c */
uint64_t	now_ms(void);
void		log_add(t_table *table, int id, const char *msg);
void		*printer_routine(void *arg);
void		*coder_routine(void *arg);
void		end_simulation(t_table *table);
bool		run_simulation(t_table *table);
void		request_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);
bool		can_take_pair(t_table *table, int left_idx, int right_idx, uint64_t now);
bool		is_burned_out(t_coder *coder, uint64_t now);



/* heap/heap.c	*/

t_heap	*heap_create(t_scheduler scheduler, int capacity);
bool	heap_empty(t_heap *heap);
void	heap_destroy(t_heap *heap);
t_coder	*heap_pop(t_heap *heap);
t_coder	*heap_top(t_heap *heap);

/* heap/heap_ops.c	*/
void	heap_push(t_heap *heap, t_coder *coder, uint64_t request_time);
void	heap_bubble_up(t_heap *heap, int idx);
void	heap_bubble_down(t_heap *heap, int idx);

#endif
