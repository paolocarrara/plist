plist
=====

**Password List Generator**

Biblioteca para auxiliar na geração de listas de senhas. Pode ser utilizada principalmente para tentar quebrar senhas.

**Estruturas da biblioteca**

```
struct sets {
	int8_t *base_se;
	int8_t **sub_set;
};
```
>Armazena os conjuntos de caracteres que irão ser usados para a criação das senhas

```
struct p_config {
	uint8_t min_s;
	uint8_t max_s;
	#if __L64__
	uint64_t total;
	#else
	uint32_t total;
	#endif
	struct sets charsets;
};
```
>Armazena as informações básicas sobre o conjunto de senhas que irão ser geradas

```
struct p_config_restore {
	const char *out_name;
	const char *save_name;
	#ifdef __LP64__
	uint64_t *remaining;
	#else
	uint32_t *remaining;
	#endif
};
```
>Armazena informações adicionais para retornar de uma seção não finalizada

```
struct p_config_extra {
	uint16_t buffer_size;
	uint8_t num_threads;
	uint8_t interval;
	uint8_t out_fd;
	uint8_t save_fd;
	uint8_t save;
	uint8_t load;
	struct p_config_restore restore;
};
```
>Armazena configurações adicionais para a geração do conjunto de senhas</p>

**Funções da biblioteca**

```
struct p_config *new_pconfig (void);
```
>retorna uma estrutura do tipo p_config.

```
struct p_config_extra *new_p_config_extra (void);
```
>retorna uma estrutura do tipo p_config_extra
