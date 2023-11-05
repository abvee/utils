#include <stdio.h>
#include <unistd.h>

// prototypes
void printJSON(int battery, char *batcolour, char *date, float temp, float power);
unsigned long atoi(FILE *fp);
char* getcolour(FILE *statfile);
char* fdate(); // get current date, in specified format

int main(int argc, char *argv[]) {
	FILE *batfile, *colourfile, *tempfile, *volfile, *currfile;

	batfile = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	colourfile = fopen("/sys/class/power_supply/BAT0/status", "r");
	tempfile = fopen("/sys/devices/virtual/thermal/thermal_zone9/temp", "r");
	volfile = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
	currfile = fopen("/sys/class/power_supply/BAT0/current_now", "r");

	printf("{\"version\": 1,\"click_events\": true}\n");
	printf("[\n");
	printf("[],\n");
	do {
		printJSON(atoi(batfile), getcolour(colourfile), fdate(), atoi(tempfile)/1000.0, (atoi(volfile) * atoi(currfile))/1000000000000.0);

		// set file cursor back to beginning
		fseek(colourfile, 0, SEEK_SET);
		fseek(batfile, 0, SEEK_SET);
		fseek(tempfile, 0, SEEK_SET);
		fseek(volfile, 0, SEEK_SET);
		fseek(currfile, 0, SEEK_SET);
	}
	while (sleep(5) == 0);
}

// print the JSON output and flush buffer
void printJSON(int battery, char *batcolour, char *date, float temp, float power) {
	printf("\
	[\
		{\
			\"full_text\": \"%ld%%\",\
			\"background\": \"#%s\",\
			\"align\": \"center\",\
			\"color\": \"#000000\"\
		},\
		{\
			\"full_text\": \"%.1fW\",\
			\"background\": \"#eee8d5\",\
			\"align\": \"center\",\
			\"color\": \"#000000\"\
		},\
		{\
			\"full_text\": \"%.1fC\",\
			\"background\": \"#1c78be\",\
			\"color\": \"#000000\",\
			\"align\": \"center\"\
		},\
		{\
			\"full_text\": \"%s\",\
			\"background\": \"#6c71c6\",\
			\"color\": \"#000000\",\
			\"align\": \"center\"\
		}\
	],\n", battery, batcolour, power, temp, date);

	// MAGIC
	fflush(stdout);
	// MAGIC END
}

// convert string to int. use for battery percentage and temp
unsigned long atoi(FILE *fp) {
	unsigned int x = 0;
	char c;

	for (; (c = getc(fp)) >= '0' && c <= '9'; x *= 10)
		x += c - '0';

	fseek(fp, 0, SEEK_END); // stupid housekeeping

	return x/10;
}

// check if battery is charging or not, return green if it is, red if not
char* getcolour(FILE *statfile) {
	char c = getc(statfile);

	fseek(statfile, 0, SEEK_END); // stupid housekeeping

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
