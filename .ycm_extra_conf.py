def FlagsForFile(filename, **kwargs):
    flags = [
        '-Wall', '-Wextra', '-Werror',
        '-std=c11',
        '-x', 'c',
    ]

    return {
        'flags': flags,
        'do_cache': True
    }
