from operator import itemgetter
import os.path


# Simplify path that contain useless ../
def simplify_path(path):
    simplified_path = []
    for index, sub_path in enumerate(path.split('/')):
        if sub_path == '..' and index > 0:
            simplified_path.pop()
        else:
            simplified_path.append(sub_path + '/')

    return ''.join(simplified_path).rstrip('/')

def cpp_to_o(path):
    return path.rstrip('cpp').replace('/', '') + 'o'

def search_header_file(file_name, current_dir, cpp_header_tuple, header_list):
    file_name = current_dir + file_name
    file_name = simplify_path(file_name)

    # Make sure this file not appeared yet.
    if file_name in header_list:
        return
    header_list.append(file_name)

    # Check the cpp file that corresponding this header file.
    cpp_name = file_name.rstrip('h') + 'cpp'
    if os.path.isfile(cpp_name):
        search_cpp_file(cpp_name, current_dir, cpp_header_tuple)


    with open(file_name, 'r') as file:
        for line in file:
            index = line.find('#include "')
            if index != -1:
                line = line.replace('#include ', '', 1)
                line = line.strip('"\n')
                path = line.rsplit('/', 1)

                path_len = len(path)
                if path_len > 1:
                    new_dir = current_dir + path[0] + '/'
                else:
                    new_dir = current_dir

                search_header_file(
                    path[path_len - 1], new_dir, cpp_header_tuple, header_list)

def search_cpp_file(file_name, current_dir, cpp_header_tuple):
    file_name = simplify_path(file_name)

    # Make sure this cpp file not appeared yet.
    if file_name in [x[0] for x in cpp_header_tuple]:
        return

    header_list = []
    output = cpp_to_o(file_name)
    cpp_header_tuple.append((file_name, header_list, output))

    with open(file_name, 'r') as file:
        for line in file:
            index = line.find('#include "')
            if index != -1:
                line = line.replace('#include ', '', 1)
                line = line.strip('"\n')
                path = line.rsplit('/', 1)

                path_len = len(path)
                if path_len > 1:
                    new_dir = current_dir + path[0] + '/'
                else:
                    new_dir = current_dir

                search_header_file(
                    path[path_len - 1], new_dir, cpp_header_tuple, header_list)

    header_list.sort()

def divid_intersection(a, b):
    intersection = set((x[0] for x in a)).intersection((x[0] for x in b))
    divided_a = []
    divided_b = []
    divided_intersection = []
    for data in a:
        if data[0] in intersection:
            divided_intersection.append(data)
        else:
            divided_a.append(data)

    for data in b:
        if data[0] not in intersection:
            divided_b.append(data)

    return divided_a, divided_b, divided_intersection

def write_o(file, target_name, o_list):
    file.write(target_name + ' =')
    for output in [x[2] for x in o_list]:
        file.write(' \\\nobj/' + output)
    file.write('\n\n')


def main():
    file_network = []
    search_cpp_file('networkmain.cpp', '', file_network)
    file_network.sort(key=itemgetter(0))

    file_local = []
    search_cpp_file('gomokumain.cpp', '', file_local)
    file_local.sort(key=itemgetter(0))

    network, local, obj = divid_intersection(file_network, file_local)

    with open('Makefile', 'w') as file:
        write_o(file, 'OBJ', obj)
        write_o(file, 'OBJ_NET', network)
        write_o(file, 'OBJ_LOCAL', local)

        file.write('GENERAL_FLAG = -std=c++11\n\n')

        file.write('EM : mkdir $(OBJ) $(OBJ_NET)\n'
                   '\tg++ -o EM $(OBJ) $(OBJ_NET) '
                   '$(GENERAL_FLAG) -pthread -O3\n'
                   'EM : OPT_FLAG = -O3\n\n')

        file.write('Debug : mkdir $(OBJ) $(OBJ_NET)\n'
                   '\tg++ -o EM $(OBJ) $(OBJ_NET) '
                   '$(GENERAL_FLAG) -pthread -O0 -g\n'
                   'Debug : OPT_FLAG = -O0 -g\n\n')

        file.write('Local : mkdir $(OBJ) $(OBJ_LOCAL)\n'
                   '\tg++ -o EM $(OBJ) $(OBJ_LOCAL) '
                   '$(GENERAL_FLAG) -pthread -O3\n'
                   'Local : OPT_FLAG = -O3\n\n')

        file.write('mkdir :\n'
                   '\tif ! [ -d "obj" ]; then mkdir obj; fi\n\n')
        file.write('clean :\n'
                   '\tif [ -d "obj" ]; then rm -r obj; fi\n'
                   '\tif [ -e "EM" ]; then rm EM; fi\n\n')

        objects = obj + network + local
        objects.sort(key=itemgetter(0))

        for cpp_header_o in objects:
            cpp = cpp_header_o[0]
            output = cpp_header_o[2]
            file.write('obj/' + output + ': ' + cpp)
            for header in cpp_header_o[1]:
                file.write(' \\\n ' + header)
            file.write('\n\tg++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@\n\n')


if __name__ == '__main__':
    main()