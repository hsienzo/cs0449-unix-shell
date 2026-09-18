#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// ------------------------------------------------------------------------------------------------
// Helpers

void err_msg(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

void get_line(char* buffer, int size) {
	fgets(buffer, size, stdin);
	int len = strlen(buffer);

	if(len != 0 && buffer[len - 1] == '\n')
		buffer[len - 1] = '\0';
}

int split_words(char* string_to_parse, char *tokens[]) {
	// will split on spaces or colons
	const char* delim = " \t\n";
	char* state;
	char* token = strtok_r(string_to_parse, delim, &state);
	int i;
	for(i = 0; token != NULL; i++) {
		tokens[i] = token;
		token = strtok_r(NULL, delim, &state);
	}

	// make sure to NULL out the last slot in the array
	tokens[i] = NULL;
	return i;
}

void handle_redirection(char *tokens[], int num_tokens) {
	int input_counter = 0;
	int output_counter = 0;
	int input_index = -1;
	int output_index = -1;
	char *input_file = NULL;
	char *output_file = NULL;
	
	for (int i = num_tokens - 1; i >= 0; i--) {
		if (tokens[i] == NULL) {
			continue;
		}	
		if (strcmp(tokens[i], "<") == 0) {
			input_counter++;
			
			if (input_counter > 1) {
				err_msg("Error: Too many redirections\n");
			}
			if (tokens[i+1] == NULL) {
				err_msg("Error: Missing input file\n");
			}
			
			input_file = tokens[i+1];
			input_index = i;			
		} else if (strcmp(tokens[i], ">") == 0) {
			output_counter++;
			
			if (output_counter > 1) {
				err_msg("Error: Too many redirections\n");
			}
			
			if (tokens[i+1] == NULL) {
				err_msg("Error: Missing output file\n");
			}
			
			output_file = tokens[i+1];
			output_index = i;
		}
	}
	
	if (input_file != NULL) {
		if (freopen(input_file, "r", stdin) == NULL) {
			err_msg("Error: Could not open file.\n");
		}
		tokens[input_index] = NULL;
	}
	
	if (output_file != NULL) {
		if (freopen(output_file, "w", stdout) == NULL) {
			err_msg("Error: Could not open file.\n");
		}
		tokens[output_index] = NULL;
	}
}

// ------------------------------------------------------------------------------------------------
// Shell's regular program!!!!!

void run_regular_progs(char *tokens[], int num_tokens) {
	int child_pid = fork();
	if (child_pid == 0) {
		signal(SIGINT, SIG_DFL);
		
		handle_redirection(tokens, num_tokens);
		
		execvp(tokens[0], tokens);
		
		perror("couldn't execute program");
		exit(1);
	} else {
		int child_status;

		if (waitpid(child_pid, &child_status, 0) == -1) {
			return;
		}
		
		if(!WIFEXITED(child_status)) {
			if(WIFSIGNALED(child_status)) {
				printf("Child exited due to signal '%s'.\n", strsignal(WTERMSIG(child_status)));
			} else {
				printf("Child terminated abnormally.\n");
			}
		}	
	}
}

// ------------------------------------------------------------------------------------------------
// Main's Functions!!!!!

void print_prompt() {
	printf("myshell> ");
	fflush(stdout);
}

int get_input_and_tokenize(char *input_buffer, int size, char *tokens[]) {
	get_line(input_buffer, size);	
	return split_words(input_buffer, tokens);
}

void check_and_run_commands(char *tokens[], int num_tokens) {
	if (strcmp(tokens[0], "exit") == 0) {
		int code = 0;
		if (tokens[1] != NULL) {
			sscanf(tokens[1], "%d", &code);
		}
		
		exit(code);
	} else if (strcmp(tokens[0], "cd") == 0) {
		chdir(tokens[1]);
	} else {
		run_regular_progs(tokens, num_tokens);
	} 
}

// ------------------------------------------------------------------------------------------------
// Main

int main() {
	signal(SIGINT, SIG_IGN);
	
	char input_buffer[300];
	char *tokens[151];
		
	while(1) {
		print_prompt();
		int num_tokens = get_input_and_tokenize(input_buffer, 300, tokens);

		if(num_tokens != 0) {
			check_and_run_commands(tokens, num_tokens);
		}
	}
	return 0;
}