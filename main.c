#include <stdio.h>

#include <eb/eb.h>
#include <eb/error.h>

int dump(const char path[]) {
    (void) path;

    const EB_Error_Code error_code = eb_initialize_library();
    if (error_code != EB_SUCCESS) {
        perror("error: failed to initialize eb\n");
        return 1;
    }


    eb_finalize_library();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 1;
    }

    return dump(argv[1]);
}
