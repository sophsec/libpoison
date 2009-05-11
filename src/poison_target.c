
/* allocate a new target */
/* error checking done by caller! */
poison_target_t *poison_new_target(void)
{
	poison_target_t *node;

	node = malloc(sizeof(poison_target_t));

	if (node)
	{
		memset((char *)node, 0, sizeof(node));
	}

	return node;
}

/* free target list */
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
