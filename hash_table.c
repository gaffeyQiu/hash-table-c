#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

// ht_new_item 分配一个 ht_item 结构体大小的内存块，并保存传入的 k 和 v
// static 标记因为只能哈希表内部代码使用 
static ht_item *
ht_new_item(const char *k, const char * v) {
	ht_item *i = malloc(sizeof(ht_item));
	i->key = strdup(k);
	i->value = strdup(v);
	return i;
}

// ht_new 初始化 hashtable，size 定义了 ht_item 的数量， 目前固定为53个
// 我们将在调整大小一节调整他
// 使用 calloc 初始化 item， 它会用 NULL 填充分配的内存，NULL 表示item为空
ht_hash_table *
ht_new() {
	ht_hash_table *ht = malloc(sizeof(ht_hash_table	));
	ht->size = 53;
	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));

	return ht;
}

static void
ht_del_item(ht_item *i) {
	free(i->key);
	free(i->value);
	free(i);
}

void
ht_del_hash_table(ht_hash_table *ht) {
	for (int i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL) {
			ht_del_item(item);
		}
	}
	free(ht->items);
	free(ht);
}

// ht_hash 输入一个字符串，返回一个 0～m 之间的数字
// 1. 将输入的字符串转换为一个大整数
// 2. 对 m 进行取模，从而使整数减小到 0~m 之间
// 3. 变量 a 应该是一个大于 ASCII 字母表的质数
static int
ht_hash(const char *s, const int a, const int m) {
	long hash = 0;
	const int len_s = strlen(s);
	for (int i = 0; i < len_s; i++) {
		hash += (long) pow(a, len_s - (i+1)) * s[i];
		hash = hash % m;
	}
}

static int
ht_get_hash(const char *s, const int num_buckets, const int attempt) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void
ht_insert(ht_hash_table* ht, const char *key, const char* value) {
	ht_item *item = ht_new_item(key, value);
	int index = ht_get_hash(item->key, ht->size, 0);
	ht_item *cur_item = ht->items[index];
	
	int i = 1;
	while(cur_item != NULL) {
		index = ht_get_hash(item->key, item->value, i);
		cur_item = ht->items[index];
		i++;
	}

	ht->items[index] = item;
	ht->count++;
}

char *
ht_search(ht_hash_table *ht, const char *key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item *item = ht->items[index];

	int i = 1;
	while (item != NULL)
	{
		if (strcmp(item->key, key) == 0) {
			return item->value;
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}

	return NULL;
}

static ht_item HT_DELETE_ITEM = {NULL, NULL};

void
ht_delete(ht_hash_table *ht, const char *key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item *item = ht->items[index];

	int i = 0;
	while (item != NULL)
	{
		if (item != &HT_DELETE_ITEM) {
			if (strcmp(item->key, key) == 0) {
				ht_del_item(item);
				ht->items[index] = &HT_DELETE_ITEM;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}

	ht->count--;
}