#!/usr/bin/env python

import glob

def get_file_strings(fname):
    result = []
    with open(fname, "r") as f:
        text = f.read()
    i = 0
    start = None
    current_string = None
    while i < len(text):
        if (start is None) and (text[i:i+2] == '//'):
            while text[i] != '\n':
                i += 1
        elif (start is None) and (text[i] == '"'):
            if (text[i-1] == 'R'):
                i += 1
                while text[i] != '"': i += 1
            else:
                start = i
        elif (start is None) and (text[i] in ' \t\n'):
            pass
        elif (start is None) and (text[i:i+3] == "'\"'"):
            i += 2  # skip over character literals that might otherwise confuse us
        elif (start is None):
            if current_string:
                result += [current_string]
                # print(current_string)
                # print("---------------------")
            current_string = ''
        elif (start is not None) and (text[i] == '\\'):
            i += 1
        elif (start is not None) and (text[i] == '"'):
            current_string += text[start:i+1]
            start = None
        else:
            pass  # just a plain old string character
        i += 1
    if current_string:
        result += [current_string]
    return result

def get_program_strings():
    result = []
    for fname in glob.glob("*.cpp"):
        if not fname.startswith("language-"):
            result += get_file_strings(fname)
    return result

if __name__ == '__main__':
    program_strings = set(get_program_strings())
    print('const char *program_strings[] = {')
    for s in program_strings:
        print('    %s,' % s)
    print('};')
