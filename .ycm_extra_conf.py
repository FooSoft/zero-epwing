def FlagsForFile(filename, **kwargs):
    flags = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-std=gnu11',
        '-I/mnt/projects/zero-epwing/eb',
        '-I/mnt/projects/zero-epwing/jansson/include',
        '-x',
        'c',
    ]

    return {
        'flags': flags,
        'do_cache': True
    }
