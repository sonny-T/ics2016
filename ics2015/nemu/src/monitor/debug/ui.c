#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{"si", "Step to execute", cmd_si},
	{"info","Display information reg",cmd_info},
	{"x","x 10 0xffffffff scan memory",cmd_x}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
static int cmd_x(char *args)
{
	int i = 0;
	int j = 0;
	int n = 0;

    int memory = 0 ;
	swaddr_t expr_n;

	char * _x = strtok(args," ");
	char *expr = strtok(NULL," ");
	if(_x==NULL || expr==NULL)
	{
		printf("Invalid input number requst\n");
	}
	else
	{
		for(i=0;i<strlen(_x);i++)
		{
			n = n*10+(*(_x+i)-'0');
		}
		expr_n = strtol(expr,NULL,16);
		for(j=0;j<n;j++)
		{
			//memory = *(int *)expr_n;
			memory = swaddr_read(expr_n,4);
			printf("0x%08x   0x%08x\n",expr_n,memory);
			expr_n  = expr_n + 1;
			
		}
	}

	return 0;
}
static int cmd_info(char *args)
{ 
	swaddr_t memory_value;
	char *info_r = strtok(NULL," ");
	char *r = "r";
	if(info_r == NULL)
	{
		printf("Invalid input information requst\n");
	}
	else
	{
		if((strcmp(info_r,r)) == 0)
		{
			memory_value = swaddr_read(cpu.eax,4);
			printf("eax  0x%08x  0x%08x\n",cpu.eax,memory_value);
			memory_value = swaddr_read(cpu.gpr[0]._16,2);
			printf("ea  0x%04x  0x%04x\n",cpu.gpr[0]._16,memory_value);
						
			memory_value = swaddr_read(cpu.ecx,4);
			printf("ecx  0x%08x  0x%08x\n",cpu.ecx,memory_value);
			memory_value = swaddr_read(cpu.gpr[1]._16,2);
			printf("cx  0x%04x  0x%04x\n",cpu.gpr[1]._16,memory_value);
			
			memory_value = swaddr_read(cpu.edx,4);
			printf("edx  0x%08x  0x%08x\n",cpu.edx,memory_value);
			memory_value = swaddr_read(cpu.gpr[2]._16,2);
			printf("dx  0x%04x  0x%04x\n",cpu.gpr[2]._16,memory_value);

			memory_value = swaddr_read(cpu.ebx,4);
			printf("ebx  0x%08x  0x%08x\n",cpu.ebx,memory_value);
			memory_value = swaddr_read(cpu.gpr[3]._16,2);
			printf("bx  0x%04x  0x%04x\n",cpu.gpr[3]._16,memory_value);
				
		//	memory_value = swaddr_read(cpu.esp,4);
		//	printf("esp  0x%08x  0x%08x\n",cpu.esp,memory_value);
		/*	memory_value = swaddr_read(cpu.eax,4);
			printf("sp    %p  0x%08x\n",&cpu.gpr[4]._16,cpu.gpr[4]._16);

			memory_value = swaddr_read(cpu.eax,4);
			printf("ebp   %p  0x%08x\n",&cpu.ebp,cpu.ebp);
			memory_value = swaddr_read(cpu.eax,4);
			printf("bp    %p  0x%08x\n",&cpu.gpr[5]._16,cpu.gpr[5]._16);

			memory_value = swaddr_read(cpu.eax,4);
			printf("esi   %p  0x%08x\n",&cpu.esi,cpu.esi);
			memory_value = swaddr_read(cpu.eax,4);
			printf("si    %p  0x%08x\n",&cpu.gpr[6]._16,cpu.gpr[6]._16);

			memory_value = swaddr_read(cpu.eax,4);
			printf("edi   %p  0x%08x\n",&cpu.edi,cpu.edi);
			memory_value = swaddr_read(cpu.eax,4);
			printf("di    %p  0x%08x\n",&cpu.gpr[7]._16,cpu.gpr[7]._16);*/
	
			memory_value = swaddr_read(cpu.eip,4);
			printf("eip  0x%08x  0x%08x\n",cpu.eip,memory_value);
		}
		else
		{
			printf("Failed command\n");
		}
	}
	return 0;
}
static int cmd_si(char *args)
{
	int i;
	int	n=0;
	char *cpu_exec_cmd = strtok(NULL," ");
	if(cpu_exec_cmd == NULL)
	{
		printf("Invalid input execute number\n");
	}
	else
	{
		for(i = 0;i<strlen(cpu_exec_cmd);i++)
		{
			n = n*10+(*(cpu_exec_cmd+i)-'0');
		}
	}
	cpu_exec(n);
	//printf("%s successful\n",cpu_exec_cmd);
	return 0;
}
static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s -- %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 * eg si 2 3 , at last  args = 2 3
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
