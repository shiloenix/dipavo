#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PATH_MAX_SIZE 4096
#define FILES_ST_SIZE 10

typedef struct {
	char path[PATH_MAX_SIZE];
	size_t size;
} FileEntry;

FileEntry *files;
size_t current_idx = 0;
size_t total_number = 0;

int handler(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	// warning
	(void)typeflag;
	(void)ftwbuf;	

	if (current_idx >= total_number) {
		total_number = total_number * 2;
		files = realloc(files, total_number * sizeof(FileEntry));
		if (files == NULL) {
			perror("Failed to reallocate files");
		}
	}
	size_t filesize = sb->st_size;	
	strcpy(files[current_idx].path, path);	
	files[current_idx].size = filesize;	
	
	current_idx++;
	return 0;
}

int sort(const void *first, const void *second) {
	FileEntry one =  *(FileEntry *) first; 
	FileEntry two =  *(FileEntry *) second; 

	if (one.size == two.size)
		return 0;
	if (one.size > two.size)
		return -1;
	if (one.size < two.size)
		return 1;

	return 0;
}

void initFiles() {
	files = (FileEntry *) calloc(FILES_ST_SIZE, sizeof(FileEntry)); 
	if (files == NULL) {
		perror("Failed to allocate files array");
		exit(-1);
	}
	total_number = FILES_ST_SIZE;
}

int findMaxFilesize() {
	size_t current_max = 0;
	for (size_t i = 0; i < current_idx; i++) {
		if (files[i].size > current_max)
			current_max = files[i].size;
	}	
	return current_max;
}

void printFiles() {
	size_t max = findMaxFilesize();
	
	for (size_t i = 0; i < current_idx; i++) {
		if (files[i].size >= 1000000000) {
			printf("%ldGB\t%s\n", files[i].size / 1000000000, files[i].path);			
		} else if (files[i].size >= 1000000) {
			printf("%ldMB\t%s\n", files[i].size / 1000000, files[i].path);
		} else {
			printf("%ldKB\t%s\n", files[i].size / 1000, files[i].path);
	  }

		int num_blocks = 2 * log10((float)files[i].size / log10((float)max));
		for (int j = 0; j < num_blocks; j++){
			printf("█");
		}
		printf("\n");
	}	
}

int main(int argc, const char *argv[]) {
	
	if (argc != 2){
		printf ("Usage: %s <path>\n", argv[0]);
		exit(-1);
	}

	initFiles();
	nftw (argv[1], handler, 5, 0);
	
	qsort(files, current_idx, sizeof(FileEntry), sort);	

	printFiles();

	return 0;
}
