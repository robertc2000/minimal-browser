/* COMAN ROBERT - 313CB*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "utils.h"

int get_command_code(const char* command,
					 char functions[NR_COMMANDS][MAX_CMD_LEN])
{
	for(int i = 0; i < NR_COMMANDS; i++)
		if(!strcmp(command, functions[i]))
			return i;
	return -1;
}

int main(int argc, char*argv[])
{	
	if(argc != 3)
	{
		printf("Wrong number of arguments\n");
		exit(1);
	}
	char* input = argv[1];
	char* output = argv[2];
	FILE* fin = fopen(input, "r");
	if(!fin)
		exit(1);
	FILE* fout = fopen(output, "w");
	if(!fout)
		exit(1);
	char command[MAX_CMD_LEN];
	char functions[NR_COMMANDS][MAX_CMD_LEN] = {"set_band", "newtab",
	"deltab", "change_tab", "print_open_tabs", "goto", "back", "forward", 
	"history", "del_history", "list_dl", "downloads", "download", "wait"};
	
	long bandwidth = KILO; //default
	T_TabList L = NULL;
	T_Tab* current_tab = NULL;
	current_tab = newtab(&L); // primul tab
	AQ history = InitQ(sizeof(TWebPage));
	AQ downloads = InitQ(sizeof(Resource));
	T_DList completed = NULL;
	if(!history || !downloads || !current_tab)
		exit(1);
	fscanf(fin, "%s", command);
	while(!feof(fin))
	{
		int code = get_command_code(command, functions);
		switch(code)
		{
			case 0: //set_band
			{
				fscanf(fin, "%ld", &bandwidth);
				break;
			}
			case 1: //newtab
			{
				current_tab = newtab(&L);
				break;
			}
			case 2: //deltab;
			{
				if(L != NULL)
					current_tab = deltab(&L, current_tab);
				break;
			}
			case 3: //change_tab
			{
				int index;
				fscanf(fin, "%d", &index);
				current_tab = change_tab(L, index);
				break;
			}
			case 4: //print_tabs
			{
				print_open_tabs(L, fout);
				break;
			}
			case 5: //goto
			{
				go_to(current_tab, history, fin);
				wait(1, bandwidth, downloads, &completed);
				break;
			}
			case 6: //back
			{
				back(current_tab, fout);
				break;
			}
			case 7: //forward
			{
				forward(current_tab, fout);
				break;
			}
			case 8: //history
			{
				show_history(history, fout);
				break;
			}
			case 9: //del_history
			{
				int nr_entries;
				fscanf(fin, "%d", &nr_entries);
				del_history(history, nr_entries);
				break;
			}
			case 10: //list_dl
			{
				list_dl(current_tab->current_page, fout);
				break;
			}
			case 11: //downloads
			{
				print_downloads(downloads, completed, fout);
				break;
			}
			case 12: //download
			{
				int index;
				fscanf(fin, "%d", &index);
				add_downloads_to_queue(downloads, current_tab, index);
				break;
			}
			case 13: //wait
			{
				int seconds;
				fscanf(fin, "%d", &seconds);
				wait(seconds, bandwidth, downloads, &completed);
			}
		}
		fscanf(fin, "%s", command);
	}
	fclose(fin);
	fclose(fout);
	deltabs_all(&L);
	DistrQ(history, free);
	DistrQ(downloads, free);
	free_completed(&completed);
}