import def_make_make

make_release_str = ""
make_debug_str = ""
make_analyze_str = ""

flags = """
EXE = messenger
EXE_ARGS =
EXE_LOG = 2>log
"""

include = "\nINCLUDE = -Isrc/include/ -Icutils/include/\n"

cpp_files = ""
cpp_objects = ""

с_files = "src/source/main.c cutils/source/swap.c cutils/source/convert_endian.c cutils/source/check_endian.c cutils/source/logging.c cutils/source/file_wraper.c cutils/source/my_assert.c"
c_objects = "src/build/main.o src/build/swap.o src/build/convert_endian.o src/build/check_endian.o src/build/logging.o src/build/file_wraper.o src/build/my_assert.o"

debug_target = """debug: all
all: {} {}
\t@$(CXX) {} {} $(STDCPP_VERSION) -o $(EXE) $(LINK_FLAGS_DEBUG)\n""".format(cpp_objects, c_objects, cpp_objects, c_objects)

release_target = """release: all
all: {} {}
\t@$(CXX) {} {} $(STDCPP_VERSION) -o $(EXE) $(LINK_FLAGS_RELEASE)""".format(cpp_objects, c_objects, cpp_objects, c_objects)

analyze_target = """analyze: all
all:
\t@clang-tidy {} {} --config-file=.clang-tidy
\t#@clang-tidy --dump-config -checks=bugprone-*,google-*,readabillity-*,clang-analyzer-*,-clang-analyzer-cplusplus-*
#--config-file=.clang-tidy 
#--dump-config -checks=bugprone-*,google-*,readabillity-*,clang-analyzer-*,-clang-analyzer-cplusplus-*
""".format(cpp_files, с_files)
# ------------------------------------------------------------------------------

# cpp objects for debug: -------------------------------------------------------

cpp_obj_debug       = ''
cpp_sources_splited = cpp_files.split()
cpp_objects_splited = cpp_objects.split()

cpp_ns_files = len(cpp_sources_splited)
cpp_no_files = len(cpp_objects_splited)

if (cpp_no_files != cpp_ns_files):
    print("Warning number of .o files doenst match number of .cpp files")
    exit(0)

for i in range(cpp_no_files):
    cpp_obj_debug += "{}: {}\n\t@$(CXX) $(STDCPP_VERSION) -c $^ $(DEBUG_FLAGS) $(INCLUDE) -o $@\n\n".format(cpp_objects_splited[i], cpp_sources_splited[i])

# cpp objects for release: -----------------------------------------------------

cpp_obj_release = ''
cpp_sources_splited = cpp_files.split()
cpp_objects_splited = cpp_objects.split()

cpp_ns_files = len(cpp_sources_splited)
cpp_no_files = len(cpp_objects_splited)

if (cpp_no_files != cpp_ns_files):
    print("Warning number of .o files doenst match number of .cpp files")
    exit(0)

for i in range(cpp_no_files):
    cpp_obj_release += "{}: {}\n\t@$(CXX) $(STDCPP_VERSION) -c $^ $(RELEASE_FLAGS) $(INCLUDE) -o $@\n\n".format(cpp_objects_splited[i], cpp_sources_splited[i])

# c objects for debug: ---------------------------------------------------------

c_obj_debug       = ''
c_sources_splited = с_files.split()
c_objects_splited = c_objects.split()

c_ns_files = len(c_sources_splited)
c_no_files = len(c_objects_splited)

if (c_no_files != c_ns_files):
    print("Warning number of .o files doenst match number of .c files")
    exit(0)

for i in range(c_no_files):
    c_obj_debug += "{}: {}\n\t@$(CC) $(STDC_VERSION) -c $^ $(DEBUG_FLAGS) $(INCLUDE) -o $@\n\n".format(c_objects_splited[i], c_sources_splited[i])

# c objects for release: -------------------------------------------------------

c_obj_release = ''
c_sources_splited = с_files.split()
c_objects_splited = c_objects.split()

c_ns_files = len(c_sources_splited)
c_no_files = len(c_objects_splited)

if (c_no_files != c_ns_files):
    print("Warning number of .o files doenst match number of .c files")
    exit(0)

for i in range(c_no_files):
    c_obj_release += "{}: {}\n\t@$(CC) $(STDC_VERSION) -c $^ $(RELEASE_FLAGS) $(INCLUDE) -o $@\n\n".format(c_objects_splited[i], c_sources_splited[i])

make_release_str += flags + def_make_make.def_frags + include + def_make_make.sep + release_target + def_make_make.sep + cpp_obj_release + c_obj_release
make_debug_str += flags + def_make_make.def_frags + include + def_make_make.sep + debug_target + def_make_make.sep + cpp_obj_debug + c_obj_debug
make_analyze_str += analyze_target 
