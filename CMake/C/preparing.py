header_file = open('index.h', 'w')

text_for_file = "void printPythonLine(){std::cout<<\"Python Header!\" << std::endl;}"
header_file.write(text_for_file)
header_file.close()