#include <stdio.h>
#include <unistd.h>

// prototypes
void printJSON(int battery, char *batcolour, char *date);
int batpercent(FILE *fp);
char* getcolour(FILE *statfile);
char* fdate(); // get current date, in specified format

// global variables
FILE *batfile, *colourfile;

int main(int argc, char *argv[]) {
	batfile = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	colourfile = fopen("/sys/class/power_supply/BAT0/status", "r");

	printf("{\"version\": 1,\"click_events\": true}\n");
	printf("[\n");
	printf("[],\n");
	do {
		printJSON(batpercent(batfile), getcolour(colourfile), fdate());

		// set file cursor back to beginning
		fseek(colourfile, 0, SEEK_SET);
		fseek(batfile, 0, SEEK_SET);
	}
	while (sleep(5) == 0);
}

// print the JSON output and flush buffer
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
	],\n", battery, batcolour, date);

	// MAGIC
	fflush(stdout);
	// MAGIC END
}

// battery percentage
int batpercent(FILE *fp) {
	int bat = 0;
	char c;

	for (; (c = getc(fp)) >= '0' && c <= '9'; bat *= 10)
		bat += c - '0';

	while (getc(fp) != EOF); // stupid housekeeping

	return bat/10;
}

// check if battery is charging or not, return green if it is, red if not
char* getcolour(FILE *statfile) {
	char c = getc(statfile);

	while (getc(statfile) != EOF); // stupid housekeeping

	if (c == 'D')
		return "bf616a";
	return "a3be8c";
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
char* fdate() {

	// MAGIC
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	// MAGIC END

	static char date[100];
	sprintf(date, "%02d/%02d %s %02d:%02d", tm.tm_mday, tm.tm_mon + 1, day(tm.tm_wday), tm.tm_hour, tm.tm_min);
	return date;
}
