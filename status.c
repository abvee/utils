#include <stdio.h>
#include <unistd.h>

void printJSON(int battery, char *batcolour, char *date);
int batpercent(FILE *fp);
void getcolour(char batcolour[], FILE *statfile);
char *fdate(); // get current date

// global variables
FILE *batfile, *colourfile;

int main(int argc, char *argv[]) {
	batfile = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	colourfile = fopen("/sys/class/power_supply/BAT0/status", "r");

	char batcolour[7]; // battery panel colour

	while (sleep(5) == 0) {
		getcolour(batcolour, colourfile);
		printJSON(batpercent(batfile), batcolour, fdate());
	}
}

void printJSON(int battery, char *batcolour, char *date) {
	printf("\
	[\
		{\
			\"full_text\": \"%d%%\",\
			\"background\": \"#%s\",\
			\"align\": \"center\",\
			\"color\": \"#000000\"\
		},\
		{\
			\"full_text\": \"%s\",\
			\"background\": \"#6c71c6\",\
			\"color\": \"#000000\",\
			\"align\": \"center\"\
		}\
	],\
	", battery, batcolour, date);
}

int batpercent(FILE *fp) {
	int bat = 0;
	char c;

	for (; (c = getc(fp)) >= '0' && c <= '9'; bat *= 10)
		bat += c - '0';

	return bat/10;
}

void stcopy(char *from, char *to) {
	while (*to++ = *from++)
		;
}
void getcolour(char batcolour[7], FILE *statfile) {
	if (getc(statfile) == 'D')
		stcopy("bf616a", batcolour);
	else
		stcopy("a3be8c", batcolour);
}

// Return day of the week
char *day(int x) {
	switch (x) {
		case 0:
			return "Sun";
		case 1:
			return "Mon";
		case 2:
			return "Tue";
		case 3:
			return "Wed";
		case 4:
			return "Thu";
		case 5:
			return "Fri";
		case 6:
			return "Sat";
	}
}

#include <time.h>
char *fdate() {

	// MAGIC
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	// MAGIC END

	static char date[100];
	sprintf(date, "%02d/%02d %s %02d:%02d", tm.tm_mday, tm.tm_mon + 1, day(tm.tm_wday), tm.tm_hour, tm.tm_min);
	return date;
}
