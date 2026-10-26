/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:35:51 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
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

# define LOG_LINE_MAX	128
# define CLR_RESET		"\033[0m"
# define CLR_TAKE		"\033[36m"
# define CLR_COMPILE	"\033[32m"
# define CLR_DEBUG		"\033[33m"
# define CLR_REFACTOR	"\033[34m"
# define CLR_BURN		"\033[1;31m"

typedef struct s_table	t_table;

typedef enum e_state
{
	ST_WAIT,
	ST_COMPILE,
	ST_DEBUG,
	ST_REFACTOR,
	ST_BURNT,
	ST_DONE
}	t_state;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	bool			in_use;
	uint64_t		available_at;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				num_com;
	uint64_t		last_comp_start;
	uint64_t		deadline;
	uint64_t		request_time;
	t_state			state;
	pthread_t		thread;
	pthread_cond_t	cond;
	int				left_dongle;
	int				right_dongle;
	bool			has_pair;
	bool			completed;
	t_table			*table;
}	t_coder;

struct s_table
{
	int				nb_cod;
	int				nb_com;
	uint64_t		ttb;
	uint64_t		ttc;
	uint64_t		ttd;
	uint64_t		ttr;
	uint64_t		dc;
	t_scheduler		scheduler;
	uint64_t		start_ms;
	bool			simulation_end;
	bool			burned_out;
	int				burned_coder_id;
	int				done_count;
	t_coder			*coders;
	t_dongle		*dongles;
	t_heap			*wait_heap;
	pthread_t		*threads;
	int				thread_count;
	pthread_t		monitor;
	bool			monitor_started;
	pthread_cond_t	start_cond;
	bool			start_ready;
	int				threads_ready;
	int				start_phase;
	int				start_even_total;
	int				start_even_done;
	pthread_mutex_t	pool_mutex;
	pthread_mutex_t	log_mutex;
	bool			pool_ready;
	bool			log_ready;
	int				cond_ready;
	int				dongle_ready;
};

t_table		*parse_arg(int argc, char **argv);
void		show_usage(void);
int			str_to_int(char *str);
bool		fill_table(t_table *tab, char **argv);
bool		init_sync(t_table *table);
bool		init_coders(t_table *table);
bool		init_dongles(t_table *table);
bool		init_threads(t_table *table);
bool		start_threads(t_table *table);
void		join_threads(t_table *table);
uint64_t	now_ms(void);
bool		sleep_ms(t_table *table, uint64_t ms);
void		log_add(t_table *table, int id, const char *msg, const char *color);
bool		enter_state(t_coder *coder, t_state state, const char *msg);
bool		wait_start_gate(t_coder *coder);
bool		open_start_gate(t_table *table);
void		mark_even_started(t_table *table, t_coder *coder);
bool		request_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);
void		grant_dongles(t_table *table, uint64_t now);
void		lock_pair(t_table *table, t_coder *coder);
void		unlock_pair(t_table *table, t_coder *coder);
void		*monitor_routine(void *arg);
bool		run_simulation(t_table *table);
void		end_simulation(t_table *table);
void		mark_simulation_end(t_table *table);
bool		simulation_over(t_table *table);
void		*coder_routine(void *arg);
void		pthread_destroy(t_table *table);

#endif
