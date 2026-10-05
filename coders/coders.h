/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:35:51 by agalvan-          #+#    #+#             */
/*   Updated: 2026/09/30 18:40:51 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODERS_H
# define CODERS_H

# include <pthread.h>
# include <sys/time.h>
# include <stdint.h>

# include "algo/algo.h"
# include "parser/parser.h"


typedef enum e_scheduler
{
	FIFO,
	EDF,
}	t_scheduler;


typedef struct	s_coder
{
	int				id;
	int				num_com;
	uint64_t		last_com;
	pthread_mutex_t	coder_mutex;

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
	pthread_mutex_t	end_mutex;
	pthread_mutex_t	log_mutex;

	t_coder			*coders;
	pthread_mutex_t	*dongle_mutex;
}	t_table;

#endif
