
import subprocess
import sys
import os
import datetime

def modtime(filepath):
  try:
    return os.path.getmtime( filepath)
  except OSError:
    return 0


def main():
  units = ['lua_unhandled']
  
  result_file_source = '#pragma once \nnamespace gentab {\n\n'
  need_to_update = False
  
  for unit in units:
    
    updated_file = '../generated/%s' % unit
    source_file = '%s.sql' % unit
    
    updated_file_ts = modtime(updated_file + '.h')
    source_file_ts = modtime(source_file)
    
    #print('src:%s\nupdated:%s' % (source_file_ts, updated_file_ts))
    # check update time
    # generate new file if updated file is older
    if updated_file_ts < source_file_ts:
       subprocess.Popen(' '.join(['\"%s\"' % sys.executable,  
                                 '\"%s\\ddl2cpp.py\"' % os.getcwd(), 
                                 source_file, 
                                 updated_file, 
                                 'gentab']))
       need_to_update = True
       
    with open(source_file, 'r') as f:
       ddl = f.read()
       result_file_source += 'const char* %s = R"(%s)";\n\n' % (unit, ddl)
  
  result_file_source += '}'
  
  if need_to_update:
    with open('../generated/source.hpp', 'w') as f:
      f.write(result_file_source)
    

if __name__ == '__main__':
  main()