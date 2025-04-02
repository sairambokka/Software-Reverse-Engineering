#include <stdint.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
typedef int BOOL;
#define FALSE 0
#define TRUE 1
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>

// Opaque predicates
#define OPAQUE_PRED_1(x) ((x * x) >= 0)
#define OPAQUE_PRED_2(x) ((x * x + 1) > x)
#define BOGUS_JUMP goto fake_label; fake_label:

// Global variables for integrity checking
volatile unsigned int g_global_checksum = 0;
volatile int g_anti_debug_trigger = 0;
struct timeval g_time_check_start, g_time_check_end;

// Forward declarations
void timing_integrity_handler(int sig);
BOOL doCheck(char user[], unsigned char* key);


/// Memory integrity checking using text segment
BOOL verify_code_integrity() {
    volatile unsigned int checksum = 0;
    unsigned char* code_start = (unsigned char*)&doCheck;  // Using doCheck instead of main
    
    // Calculate checksum of code section
    for(int i = 0; i < 1000; i++) {
        checksum = (checksum * 31) + code_start[i];
    }
    
    return checksum == g_global_checksum;
}

void timing_integrity_handler(int sig) {
    g_anti_debug_trigger = 1;
}

BOOL check_debugger() {
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        return TRUE;  // Debugger detected
    }
    ptrace(PTRACE_DETACH, 0, 1, 0);
    return FALSE;
}

BOOL check_timing_integrity() {
    volatile int counter = 0;
    gettimeofday(&g_time_check_start, NULL);
    
    for(int i = 0; i < 1000; i++) {
        counter = (counter * 7) + (i * 13);
        counter %= 1009;
    }
    
    gettimeofday(&g_time_check_end, NULL);
    long elapsed = (g_time_check_end.tv_sec - g_time_check_start.tv_sec) * 1000000 +
                  (g_time_check_end.tv_usec - g_time_check_start.tv_usec);
                  
    return elapsed > 100000;
}

// Split the hash calculation into parts
BOOL calculate_hash_part1(EVP_MD_CTX* mdctx, char user[]) {
    volatile int y = 42;
    if((y * y - 1764) == 0) {
        return EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL) &&
               EVP_DigestUpdate(mdctx, user, strlen(user));
    }
    return FALSE;
}

BOOL calculate_hash_part2(EVP_MD_CTX* mdctx, BYTE* sha1Data) {
    volatile int z = 100;
    while(z > 90) z = (z * 17 + 5) % 89;
    
    return EVP_DigestFinal_ex(mdctx, sha1Data, NULL);
}

WORD calculate_checksum_part1(BYTE* data, int len) {
    WORD result = 0;
    volatile int t = 1;
    
    for(int i = 0; i < len; i++) {
        result = (result << 4) | (result >> 12);
        t = (t * 31337 + 1) % 1009;
        if(t % 2) result ^= t;
        result = ((result * 31) + data[i]) & 0xFFFF;
    }
    
    return result;
}

WORD calculate_checksum_part2(unsigned char* key) {
    WORD result = 0;
    volatile int x = 1;
    
    for(int i = 0; i < 16; i++) {
        if(OPAQUE_PRED_1(x)) {
            result = ((result * 127) + key[i]) & 0xFFFF;
            x = (x * 31337 + 1) % 1009;
        } else {
            return 0; // Never reached
        }
    }
    
    return result;
}

BOOL doCheck(char user[], unsigned char* key) {
    // Set up timing check
    signal(SIGALRM, timing_integrity_handler);
    alarm(1);  // Set maximum execution time
    
    // Anti-debugging checks
    if (check_debugger() || check_timing_integrity() || g_anti_debug_trigger) {
        return FALSE;
    }
    
    // Code integrity verification
    if (!verify_code_integrity()) {
        return FALSE;
    }
    volatile int x = 1;
    WORD checkSHA1 = 0;
    WORD checkKey = 0;
    int state = 0;
    BYTE sha1Data[20] = {0};
    EVP_MD_CTX* mdctx = NULL;
    BOOL result = FALSE;
    
    if(!OPAQUE_PRED_1(x)) {
        return FALSE;
    }

    x = (x * 31337 + 1) % 1009;
    BOGUS_JUMP;

    mdctx = EVP_MD_CTX_create();
    if(!mdctx) {
        return FALSE;
    }

    while(state != 5) {
        switch(state ^ (x % 3)) {
            case 0:
                if(calculate_hash_part1(mdctx, user)) {
                    state = 2;
                } else {
                    goto cleanup;
                }
                break;
            case 1:
                if(calculate_hash_part2(mdctx, sha1Data)) {
                    state = 3;
                } else {
                    goto cleanup;
                }
                break;
            case 2:
                checkSHA1 = calculate_checksum_part1(sha1Data, sizeof(sha1Data));
                state = 1;
                break;
            case 3:
                checkKey = calculate_checksum_part2(key);
                state = 4;
                break;
            case 4:
                state = 5;
                if((checkSHA1 ^ 0xFFFF) == (checkKey ^ 0xFFFF)) {
                    result = TRUE;
                }
                break;
        }
    }

cleanup:
    if(mdctx) {
        EVP_MD_CTX_destroy(mdctx);
    }
    return result;
}

BOOL doCheckConvert(char user[], char keychars[]) {
    if (strlen(keychars) != 32) {
        return FALSE;
    }

    unsigned char key[16];
    char temp[3] = { 0 };
    char* check;
    
    volatile int t = 1;
    for (int i = 0; i < 16; i++) {
        if(OPAQUE_PRED_1(t)) {
            memcpy(temp, &keychars[2 * i], 2);
            key[i] = strtol(temp, &check, 16);
            t = (t * 31337 + 1) % 1009;
            
            if (check != &temp[2]) {
                return FALSE;
            }
        } else {
            return FALSE;
        }
    }

    return doCheck(user, key);
}

int main(int argc, char* argv[]) {
    // Initialize anti-debugging measures
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) >= 0) {
        ptrace(PTRACE_DETACH, 0, 1, 0);
    } else {
        return -1;  // Already being traced
    }
    
    // Set up signal handlers
    signal(SIGTRAP, SIG_IGN);
    signal(SIGILL, SIG_IGN);
    
    // Calculate initial code checksum
    unsigned char* code_start = (unsigned char*)&doCheck;
    g_global_checksum = 0;
    for(int i = 0; i < 1000; i++) {
        g_global_checksum = (g_global_checksum * 31) + code_start[i];
    }

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    
    // Removed deprecated OPENSSL_config call

#ifdef _DEBUG
    if (argc == 2) {
        unsigned char key[16];
        srand(time(NULL));
        for (int i = 0; i < 16; i++) {
            key[i] = rand();
        }
        while (1) {
            printf("Key: ");
            for (int i = 0; i < 16; i++) {
                printf("%02hhx", key[i]);
            }
            printf(": ");
            if (doCheck(argv[1], key)) {
                break;
            }
            for (int i = 15; i >= 0; i--) {
                key[i]++;
                if (key[i] != 0) break;
            }
        }
        printf("Found key: ");
        for (int i = 0; i < 16; i++) {
            printf("%02hhx", key[i]);
        }
        printf("\n");
        return 0;
    }
#endif

    if (argc != 3) {
        fprintf(stderr, "Error: Please provide a username and key\n");
        return -1;
    }

    if (doCheckConvert(argv[1], argv[2])) {
        printf("You're winner!\n");
    } else {
        printf("You lose\n");
    }

    return 0;
}