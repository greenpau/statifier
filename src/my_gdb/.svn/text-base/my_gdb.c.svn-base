/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/reg.h>

#include "my_gdb.h"
#include "breakpoints.h"
#include "dump.h"
#include "environment.h"
#include "my_ptrace.h"
#include "registers.h"
#include "processor.h"
#include "thread_local_storage_syscall.h"
#include "dump_tls_data.c"

#include <sys/types.h>
#include <sys/wait.h>

void print_status(FILE *file, int status, pid_t pid)
{
	int sig_num;
	if (WIFEXITED(status)) {
		fprintf(
			file,
			"%s pid %lu exited with status %d\n",
			pgm_name,
			(unsigned long)pid,
			WEXITSTATUS(status)
		);
		exit(1);
	}
	if (WIFSIGNALED(status)) {
		sig_num = WTERMSIG(status);
		fprintf(
			file,
			"%s pid %lu was terminated by signal %d (%s)\n",
			pgm_name,
			(unsigned long)pid,
			sig_num,
			strsignal(sig_num)
		);
		exit(1);
	}
	#if 0
	if (WIFSTOPPED(status)) {
		sig_num = WSTOPSIG(status);
		fprintf(
			file,
			"%s pid %lu was stopped by signal %d (%s)\n",
			pgm_name,
			(unsigned long)pid,
			sig_num,
			strsignal(sig_num)
		);
		return;
	}
	#endif
}

const char *pgm_name;
const char *program;
const char *misc_src;
const char *regs_from_kernel;
const char *init_maps;
const char *registers;
const char *maps;
const char *set_thread_area;
const char *output_dir;
unsigned long breakpoint_start;
unsigned long interpreter_file_entry;

unsigned long val_offset;
char *dir_name;

void init_dir_name()
{
	size_t length;
	size_t i;
	dir_name = strdup(pgm_name);
	length = strlen(dir_name);
	for (i = 0; i < length; i++) {
		if (dir_name[length - 1 - i] == (char)'/') {
			break;
		} else {
			dir_name[length - 1 - i] = (char)0;
		}
	}
	
}

unsigned long my_strtoul(const char *string)
{
	unsigned long result;
	char *endptr;
	errno = 0;
	if (string[0] == (char)0) {
		fprintf(
			stderr,
			"%s: string is empty\n",
			pgm_name
		);
		exit(1);
	}
	result = strtoul(string, &endptr, 0);
	if (errno != 0) { /* Something wrong */
		fprintf(
			stderr,
			"%s: can't convert '%s'. Errno=%d (%s)\n",
			pgm_name,
			string,
			errno,
			strerror(errno)
		);
		exit(1);
	}
	if (*endptr != (char)0) {
		fprintf(
			stderr,
			"%s: there is illegal symbols in '%s'.\n",
			pgm_name,
			string
		);
		exit(1);
	}
	return result;
}

void process_args(int argc, char *argv[])
{
	size_t i = 0;
	pgm_name = argv[i++];
	if (argc != 11) {
		fprintf(
			stderr,
			"Usage: %s <program> <breakpoint_start> <interpreter_file_entry> <misc_src> <regs_from_kernel> <init_maps> <registers> <maps> <set_thread_area> <output_dir>\n",
			pgm_name
		);
		exit(1);
	}
	program                = argv[i++];
	breakpoint_start       = my_strtoul(argv[i++]);
	interpreter_file_entry = my_strtoul(argv[i++]);
	misc_src               = argv[i++];
	regs_from_kernel       = argv[i++];
	init_maps              = argv[i++];
	registers              = argv[i++];
	maps                   = argv[i++];
	set_thread_area        = argv[i++];
	output_dir             = argv[i++];
}

void execute_command(const char *command)
{
	int status;
	status = system(command);
	if (status == -1) {
		fprintf(
			stderr,
			"%s: can't run '%s' - '%s' (errno=%d)\n",
			pgm_name,
			command,
			strerror(errno),
			errno
		);
		exit(1);
	}
	status = WEXITSTATUS(status);
	if (status) {
		fprintf(
			stderr,
			"%s: program '%s' status=%d\n",
			pgm_name,
			command,
			status
		);
		exit(1);
	}
}

void cp_proc_maps(pid_t pid, const char *filename, int output_all_mappings)
{
	char command[10000];
	snprintf(
		command,
		sizeof(command), 
		"%s/maps.sh %lu %s %d", 
		dir_name,
		(unsigned long)pid,
		filename,
		output_all_mappings
	);
	execute_command(command);
}

void dump_regs_name(const char *filename, struct user_regs_struct *user, int hit_breakpoint)
{
	FILE *f;

	f = fopen(filename, "w");
	dump_regs(f, user, hit_breakpoint);
	fclose(f);
}

void dump_tls_data_name(pid_t pid, const char *filename)
{
	FILE *f;

	f = fopen(filename, "w");
	dump_tls_data(pid, f);
	fclose(f);
}

void dump_misc_src(pid_t pid, const char *filename)
{
	unsigned long pc;
	unsigned long sp;
	FILE *f;

	pc = get_pc(pid);
	sp = get_sp(pid);

	val_offset = pc - interpreter_file_entry;
	if (val_offset  % PAGE_SIZE) {
		fprintf(
			stderr,
			"%s: pc=0x%lx, interpreter_file_entry=0x%lx, val_offset=0x%lx - it's not modulo %lx\n",
			pgm_name,
			pc,
			interpreter_file_entry,
			val_offset,
			PAGE_SIZE
		);
		exit(1);
	}
	
	f = fopen(filename, "w");	
		fprintf(f, "val_stack_pointer=0x%lx\n", sp);
		fprintf(f, "val_offset=0x%lx\n", val_offset);
	fclose(f);
}

void parent(pid_t pid)
{
	int status;
	struct user_regs_struct user;
	int tls_data_dumped = 0;

	init_dir_name();

	/* Wait for PTRACE_ME finish */
	wait(&status);
	print_status(stderr, status, pid);
	dump_misc_src(pid, misc_src);
	breakpoint_start += val_offset;
	
	user = get_registers(pid);
	dump_regs_name(regs_from_kernel, &user, 0);
	cp_proc_maps(pid, init_maps, 1);

	breakpoint_add(pid, breakpoint_start);
	while(1) {
		MY_PTRACE(PTRACE_SYSCALL, pid, NULL, NULL, 1, 1);
		wait(&status);
		print_status(stderr, status, pid);
		user = get_registers(pid);
		if (get_register(pid, SYSCALL_REG) == TLS_SYSCALL) {
			if (! tls_data_dumped) {
				tls_data_dumped = 1;
				dump_tls_data_name(pid, set_thread_area);
			}
			continue;
		}
		if (get_pc(pid) == (breakpoint_start + 1) ) {
			dump_regs_name(registers, &user, 1);
			cp_proc_maps(pid, maps, 0);
			//breakpoint_do(pid, 0);
			breakpoint_deactivate(pid, 0);
			dumps(pid, maps, output_dir);
			break;
		}
	}
	ptrace(PTRACE_KILL, pid, NULL, NULL);
}
int main(int argc, char *argv[], char *envp[])
{
	pid_t pid;
	char *new_argv[2];
	process_args(argc, argv);

	pid = fork();
	switch(pid) {
		long pt_res;
		case -1:
			fprintf(
				stderr,
				"%s - can't fork - '%s' (errno=%d)\n",
				pgm_name,
				strerror(errno),
				errno
			);
		exit(1);

		case 0: /* child */
			update_environment();
			pt_res = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			if (pt_res == -1) {
				fprintf(
					stderr,
					pgm_name,
					"%s [CHILD]  - can't PTRACE_ME - '%s' (errno=%d)\n",
					strerror(errno),
					errno
				);
				exit(1);
			}
			new_argv[0] = (char *)program;
			new_argv[1] = NULL;
			execv(program, new_argv);
			fprintf(
				stderr,
				"%s [CHILD]  - can't execve '%s' - '%s' (errno=%d)\n",
				pgm_name,
				argv[1],
				strerror(errno),
				errno
			);
		exit(1);

		default: /* parent */
			parent(pid);
		exit(0);
	}
}

