plist
=====

**Password List Generator**

Biblioteca para auxiliar na geração de listas de senhas. Pode ser utilizada, especialmente, para a quebra de senhas.

**Exemplos de uso**

>Nesse exemplo as senhas serão mostradas na saída padrão, que, na maioria dos casos, é o terminal em que 
>o programa foi executado

```
int main (int argc, char **argv) {

	struct p_config *config = new_p_config ();
	struct p_config_extra extra = new_p_config_extra ();
	
	/*
	 * Faz com que as senhas que serão geradas tenham 6 caracteres de comprimento
	*/
	p_config_set_size (6, config);
	
	/*
	 * O conjunto base para a geração das senhas é o alfabeto.
	 * Dessa forma as senhas irão variar de aaaaaa, baaaaa, ... , zzzzzz
	*/
	p_config_base_set ("abcdefghijklmnopqrstuvxywz", config);
	
	/*
	 * Define o número de threads que será usado para a geração das senhas. Neste caso, irá usar quatro threads.
	 */
	p_config_set_num_threads (4, extra);

	return 0;
}
```

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
void p_config_set_buffer_size (const uint16_t, struct p_config_extra *const);
```
>Define o tamanho em bytes, que será usado para manter as senhas antes de serem guardadas em um arquivo ou mostradas na tela

```
uint16_t p_config_get_buffer_size (const struct p_config_extra *const);
```
>Pega o tamanho do buffer que será utilizado na geração das senhas

```
void p_config_open_out_file (const char *const, struct p_config_extra *const);
```
>Abre o arquivo no qual as senhas serão armazenadas

```
void p_config_set_interval (const uint8_t, struct p_config_extra *const);
```
>Define o intervalo de tempo entre cada ponto de checagem

```
void p_config_set_save (const char *const, struct p_config_extra *const);
```
>Estabelece que a geração das senhas será acompanhada de pontos de checagem

```
void p_config_set_num_threads (const uint8_t, struct p_config_extra *const);
```
>Define o número de threads que serão utilizadas na geração das senhas

```
void p_config_restore (struct p_config *const, struct p_config_extra *const, const char *const);
```
>Recupera de uma seção inacabada

```
#if __LP64__
uint64_t p_config_get_total             (const struct p_config *const);
#else
uint32_t p_config_get_total             (const struct p_config *const);
#endif
```
>Pega o total de senhas que serão geradas

```
void p_config_base_set (char *const, struct p_config *const);
```
>Define o conjunto base de caractéres que serão utilizados na geração das senhas

```
void p_config_sub_chars_from (char *const, const uint8_t, struct p_config *const);
```
>Na posição escolhida substitui os caractéres base por um novo conjunto

```
void p_config_rm_sub_set (const uint8_t, struct p_config *const);
```
>Remove e volta para os caractéres base na posição escolhida

```
void generate (struct p_config *, struct p_config_extra *);
```
>Inicia a geração das senhas utilizando as configurações passadas

```
void free_p_config (struct p_config *const);
```
>Libera a estrutura p_config

```
void free_p_config_extra (struct p_config_extra *const);
```
>Libera a estrutura p_config_extra

```
void print_p_config (const struct p_config *);
```
>Mostra as configurações que serão utilizadas na geração das senhas
