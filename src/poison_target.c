
/* generic linked list free */
void poison_free_targets(poison_target_t *list)
{
	poison_target_t *cur;
	poison_target_t *next;

	cur = list;

	while (cur)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}

	return;
}
