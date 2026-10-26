/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_lock.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

void	lock_pair(t_table *table, t_coder *coder)
{
	int	first;
	int	second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first == second)
	{
		pthread_mutex_lock(&table->dongles[first].mutex);
		return ;
	}
	if (first > second)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	pthread_mutex_lock(&table->dongles[first].mutex);
	pthread_mutex_lock(&table->dongles[second].mutex);
}

void	unlock_pair(t_table *table, t_coder *coder)
{
	int	first;
	int	second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first == second)
	{
		pthread_mutex_unlock(&table->dongles[first].mutex);
		return ;
	}
	if (first > second)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	pthread_mutex_unlock(&table->dongles[second].mutex);
	pthread_mutex_unlock(&table->dongles[first].mutex);
}
