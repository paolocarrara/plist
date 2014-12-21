plist
=====

**Password List Generator**

Biblioteca para auxiliar na geração de listas de senhas. Pode ser utilizada principalmente para tentar quebrar senhas.

**Estruturas da biblioteca**

```
struct sets {
	int8_t *base_set;
	int8_t **sub_set;
};
```
>Armazena o conjunto de caracteres que serão usados para a criação das senhas

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
>Armazena as informações básicas sobre o conjunto de senhas que serão geradas

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
>Armazena informações adicionais para retornar de uma seção finalizada prématuramente

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
>Armazena configurações adicionais para a geração do conjunto de senhas

**Funções da biblioteca**

```
struct p_config *new_pconfig (void);
```
>retorna uma estrutura do tipo p_config.

```
struct p_config_extra *new_p_config_extra (void);
```
>retorna uma estrutura do tipo p_config_extra

```
void p_config_set_min_s (const uint8_t, struct p_config *const);
```
>Define o tamanho mínimo para as senhas que serão geradas

```
void p_config_set_max_s (const uint8_t, struct p_config *const);
```
>Define o tamanho máximo para as senhas que serão geradas

```
void p_config_set_size (const uint8_t, struct p_config *const);
```
>Define o tamanho mínimo e máximo iguais para a geração das senhas

```
```
```
```
```
```
```
