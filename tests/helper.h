#ifndef ENGINE_TEST_HELPERS
#define ENGINE_TEST_HELPERS
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TEST_INIT int TEST_COUNT = 1;
#define DUMP_D "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n"
#define DUMP_U "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"

extern int TEST_COUNT;

void RTMSG(const char* format, ...){
	char buf[1024];
	va_list args;
    
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

	printf("\t%s\n", buf);

    return;
}

int TEST(int (*testRoutine)(void*), const char* name, void* args){
	int result = 0;
	printf("#%d %s - Running\n", TEST_COUNT++, name);
	result = testRoutine(args);

	if(result){
		printf("\t%s - \033[1;31mFAILED\033[0m with %d\n\n", name, result);
	}
	else{
		printf("\t%s - \033[0;32mPASSED\033[0m with %d\n\n", name, result);
	}

	return result;
}

void HEX_DUMP(void *addr, int len) {
    int i;
    char buff[17] = {0};
    unsigned char *pc = (unsigned char*)addr;
	unsigned char desc[1024] = {0};


    printf(DUMP_D);
    sprintf(buff, "SIZE %d\n", (int)len);
    printf(buff, strlen(buff));

    // Output description if given.
    if (desc != NULL)
        printf("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("\t%s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    printf("\n");
    printf(DUMP_U);
}
#endif