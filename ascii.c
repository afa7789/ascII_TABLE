#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__linux__) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <iconv.h>
#endif

// Cross-platform character conversion function
void convert_char_cross_platform(unsigned char val) {
#ifdef __APPLE__
    // macOS-specific conversion using CoreFoundation
    char buffer[2] = {val, 0};
    CFStringRef cfStr = CFStringCreateWithCString(
        kCFAllocatorDefault,
        buffer,
        kCFStringEncodingMacRoman
    );
    
    if (cfStr) {
        char utf8Buffer[16];
        if (CFStringGetCString(cfStr, utf8Buffer, sizeof(utf8Buffer), kCFStringEncodingUTF8)) {
            printf("%s", utf8Buffer);
        } else {
            printf("�");
        }
        CFRelease(cfStr);
    } else {
        printf("�");
    }
#elif defined(_WIN32)
    // Windows-specific conversion
    char buffer[2] = {val, 0};
    wchar_t wide[2];
    char utf8[8];
    
    MultiByteToWideChar(CP_ACP, 0, buffer, 1, wide, 2);
    WideCharToMultiByte(CP_UTF8, 0, wide, 1, utf8, 8, NULL, NULL);
    printf("%s", utf8);
#elif defined(__linux__)
    // Linux-specific conversion using iconv
    static int initialized = 0;
    static iconv_t cd = (iconv_t)-1;
    
    if (!initialized) {
        cd = iconv_open("UTF-8", "ISO-8859-1");
        initialized = 1;
        if (cd == (iconv_t)-1) {
            // Fallback if iconv initialization fails
            if (val < 128) {
                printf("%c", val);
            } else {
                printf("�");
            }
            return;
        }
    }
    
    char input[2] = {val, 0};
    char output[8] = {0};
    char *inbuf = input;
    char *outbuf = output;
    size_t inbytesleft = 1;
    size_t outbytesleft = sizeof(output) - 1;
    
    iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    printf("%s", output);
#else
    // Generic fallback for other platforms
    if (val < 128) {
        printf("%c", val);
    } else {
        printf("�");
    }
#endif
}

// Prints characters, handling control codes and extended characters
void print_display_char(int val) {
    if ((val >= 0 && val <= 31) || val == 127) {
        // Convert control characters to ^X format
        printf("^%c", val + 64);
    } else if (val >= 32 && val <= 126) {
        // Print normal printable ASCII characters
        printf("%c", val);
    } else if (val >= 128 && val <= 255) {
        // Handle extended ASCII characters using platform-specific conversion
        convert_char_cross_platform(val);
    }
}

int main() {
    // Set locale to support UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    // First loop: 0 to 31 with increments of 32
    for (int i = 0; i <= 31; i++) {
        int i2 = i + 32;
        int i3 = i2 + 32;
        int i4 = i3 + 32;
        printf("%3d %#4x ", i, i);
        print_display_char(i);
        printf("  %3d %#4x ", i2, i2);
        print_display_char(i2);
        printf("  %3d %#4x ", i3, i3);
        print_display_char(i3);
        printf("  %3d %#4x ", i4, i4);
        print_display_char(i4);
        printf("\n");
    }
    printf("\n");
    // Second loop: 128 to 159 with increments of 32
    for (int i = 128; i <= 159; i++) {
        int i2 = i + 32;
        int i3 = i2 + 32;
        int i4 = i3 + 32;
        printf("%3d %#4x ", i, i);
        print_display_char(i);
        printf("  %3d %#4x ", i2, i2);
        print_display_char(i2);
        printf("  %3d %#4x ", i3, i3);
        print_display_char(i3);
        printf("  %3d %#4x ", i4, i4);
        print_display_char(i4);
        printf("\n");
    }
    return 0;
}
