#include <stdio.h>
#include <string.h>
#include "history.h"
#include "errors.h"
 
/* ------------------------------------------------------------------ */
/* Helpers de test                                                     */
/* ------------------------------------------------------------------ */
 
static int	g_passed = 0;
static int	g_failed = 0;
 
static void	check(const char *label, int condition)
{
	if (condition)
	{
		printf("  \033[32m[PASS]\033[0m %s\n", label);
		g_passed++;
	}
	else
	{
		printf("  \033[31m[FAIL]\033[0m %s\n", label);
		g_failed++;
	}
}
 
/* ------------------------------------------------------------------ */
/* Tests — history                                                     */
/* ------------------------------------------------------------------ */
 
static void	test_history_basic(void)
{
	t_history	h;
 
	printf("\n=== history : ajout et lecture basique ===\n");
	history_init(&h);
 
	history_add(&h, "ls -la");
	history_add(&h, "cd /tmp");
	history_add(&h, "echo hello");
 
	check("count == 3 apres 3 ajouts", h.count == 3);
	check("history_get(1) == 'ls -la'",
		strcmp(history_get(&h, 1), "ls -la") == 0);
	check("history_get(2) == 'cd /tmp'",
		strcmp(history_get(&h, 2), "cd /tmp") == 0);
	check("history_get(3) == 'echo hello'",
		strcmp(history_get(&h, 3), "echo hello") == 0);
	check("history_get(0) == NULL (hors limites)",
		history_get(&h, 0) == NULL);
	check("history_get(99) == NULL (hors limites)",
		history_get(&h, 99) == NULL);
 
	printf("\n--- Affichage de l'historique (doit afficher 3 lignes) ---\n");
	history_print(&h);
 
	history_free(&h);
	check("entries[0] == NULL apres free", h.entries[0] == NULL);
	check("count == 0 apres free", h.count == 0);
}
 
static void	test_history_empty_input(void)
{
	t_history	h;
 
	printf("\n=== history : entrees invalides ignorees ===\n");
	history_init(&h);
 
	history_add(&h, "");
	history_add(&h, NULL);
	history_add(&h, "ls");
 
	check("commande vide ignoree", h.count == 1);
	check("NULL ignore", h.count == 1);
	check("seule 'ls' enregistree",
		strcmp(history_get(&h, 1), "ls") == 0);
 
	history_free(&h);
}
 
static void	test_history_circular_buffer(void)
{
	t_history	h;
	int			i;
	char		buf[32];
 
	printf("\n=== history : buffer circulaire (101 entrees) ===\n");
	history_init(&h);
 
	i = 0;
	while (i < 101)
	{
		snprintf(buf, sizeof(buf), "cmd_%d", i);
		history_add(&h, buf);
		i++;
	}
 
	check("count == 101 apres 101 ajouts", h.count == 101);
	check("history_get(1) pointe sur cmd_1 (oldest visible)",
		strcmp(history_get(&h, 1), "cmd_1") == 0);
	check("history_get(100) pointe sur cmd_100",
		strcmp(history_get(&h, 100), "cmd_100") == 0);
 
	printf("\n--- Affichage des 5 dernieres entrees ---\n");
	i = 97;
	while (i <= 101)
	{
		printf("  history_get(%d) = %s\n", i, history_get(&h, i));
		i++;
	}
 
	history_free(&h);
}
 
static void	test_history_navigation(void)
{
	t_history	h;
 
	printf("\n=== history : navigation haut/bas ===\n");
	history_init(&h);
 
	history_add(&h, "cmd_A");
	history_add(&h, "cmd_B");
	history_add(&h, "cmd_C");
 
	check("current_index == count apres ajout", h.current_index == 3);
	check("history_current() == NULL quand curseur en fin",
		history_current(&h) == NULL);
 
	history_prev(&h);
	check("apres 1 x UP : current == 'cmd_C'",
		strcmp(history_current(&h), "cmd_C") == 0);
 
	history_prev(&h);
	check("apres 2 x UP : current == 'cmd_B'",
		strcmp(history_current(&h), "cmd_B") == 0);
 
	history_prev(&h);
	check("apres 3 x UP : current == 'cmd_A'",
		strcmp(history_current(&h), "cmd_A") == 0);
 
	history_prev(&h);
	check("UP en bout : reste sur cmd_A",
		strcmp(history_current(&h), "cmd_A") == 0);
 
	history_next(&h);
	check("apres 1 x DOWN depuis debut : current == 'cmd_B'",
		strcmp(history_current(&h), "cmd_B") == 0);
 
	history_next(&h);
	history_next(&h);
	check("DOWN en bout : history_current() == NULL (ligne vide)",
		history_current(&h) == NULL);
 
	history_free(&h);
}
 
/* ------------------------------------------------------------------ */
/* Tests — errors                                                      */
/* ------------------------------------------------------------------ */
 
static void	test_exit_codes(void)
{
	printf("\n=== errors : codes de sortie ===\n");
	check("ERR_CMD_NOT_FOUND -> 127", get_exit_code(ERR_CMD_NOT_FOUND) == 127);
	check("ERR_PERMISSION    -> 126", get_exit_code(ERR_PERMISSION) == 126);
	check("ERR_SYNTAX        -> 2",   get_exit_code(ERR_SYNTAX) == 2);
	check("ERR_NO_SUCH_FILE  -> 1",   get_exit_code(ERR_NO_SUCH_FILE) == 1);
	check("ERR_PIPE_FAILED   -> 1",   get_exit_code(ERR_PIPE_FAILED) == 1);
	check("ERR_FORK_FAILED   -> 1",   get_exit_code(ERR_FORK_FAILED) == 1);
	check("ERR_MALLOC        -> 1",   get_exit_code(ERR_MALLOC) == 1);
	check("ERR_NUMERIC_ARG   -> 255", get_exit_code(ERR_NUMERIC_ARG) == 255);
	check("ERR_NONE          -> 0",   get_exit_code(ERR_NONE) == 0);
}
 
static void	test_error_messages(void)
{
	printf("\n=== errors : messages stderr (a verifier visuellement) ===\n");
	printf("--- Sortie attendue ---\n");
	printf("minishell: foobar: command not found\n");
	printf("minishell: /etc/shadow: Permission denied\n");
	printf("minishell: fichier.txt: No such file or directory\n");
	printf("minishell: syntax error near unexpected token `|'\n");
	printf("minishell: pipe: failed to create pipe\n");
	printf("minishell: exit: abc: numeric argument required\n");
	printf("--- Sortie reelle ---\n");
	print_error(ERR_CMD_NOT_FOUND, "foobar");
	print_error(ERR_PERMISSION, "/etc/shadow");
	print_error(ERR_NO_SUCH_FILE, "fichier.txt");
	print_error(ERR_SYNTAX, "|");
	print_error(ERR_PIPE_FAILED, NULL);
	print_error(ERR_NUMERIC_ARG, "abc");
}
 
static void	test_last_exit_code(void)
{
	printf("\n=== errors : get/set last exit code ($?) ===\n");
 
	set_last_exit_code(0);
	check("set(0)   -> get() == 0",   get_last_exit_code() == 0);
 
	set_last_exit_code(127);
	check("set(127) -> get() == 127", get_last_exit_code() == 127);
 
	set_last_exit_code(1);
	check("set(1)   -> get() == 1",   get_last_exit_code() == 1);
 
	print_error(ERR_CMD_NOT_FOUND, "test");
	check("print_error(CMD_NOT_FOUND) met a jour g_last_exit_code a 127",
		get_last_exit_code() == 127);
 
	print_error(ERR_SYNTAX, "|");
	check("print_error(SYNTAX) met a jour g_last_exit_code a 2",
		get_last_exit_code() == 2);
}
 
/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */
 
int	main(void)
{
	printf("\n╔══════════════════════════════════════╗\n");
	printf("║     Tests P3 — history & errors      ║\n");
	printf("╚══════════════════════════════════════╝\n");
 
	test_history_basic();
	test_history_empty_input();
	test_history_circular_buffer();
	test_history_navigation();
	test_exit_codes();
	test_error_messages();
	test_last_exit_code();
 
	printf("\n══════════════════════════════════════\n");
	printf("  Resultats : \033[32m%d passes\033[0m | \033[31m%d echoues\033[0m\n",
		g_passed, g_failed);
	printf("══════════════════════════════════════\n\n");
 
	if (g_failed > 0)
		return (1);
	return (0);
}