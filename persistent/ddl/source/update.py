
import subprocess
import sys
import os
import datetime

def modtime(filepath):
  try:
    return os.path.getmtime( filepath)
  except OSError:
    return 0

#function checks if the single one file has changes
def needToUpdate(listOfFiles, dirPath):

    for unit in listOfFiles:
        updated_file = '%s/../generated/%s.h' % (dirPath, unit)
        source_file = '%s/%s.sql' % (dirPath, unit)

        updated_file_ts = modtime(updated_file)
        source_file_ts = modtime(source_file)
        
        #print (updated_file_ts, "\t", source_file_ts)
        # check update time
        # generate new file if updated file is older
        if updated_file_ts < source_file_ts:
            return True

    return False


def main():
  units = ['unhandled', 'scenario']

  dirPath = 'ddl/source' #os.path.dirname(os.path.realpath(__file__))

  if not needToUpdate(units, dirPath):
      return
  
  result_file_source = '#pragma once \nnamespace gentab {\n\n'
  
  for unit in units:
    
    updated_file = '%s/../generated/%s' % (dirPath, unit)
    source_file = '%s/%s.sql' % (dirPath, unit)
   
    #generate schemes for a single one file
    subprocess.Popen(' '.join(['\"%s\"' % sys.executable,  
                              '\"%s\\ddl2cpp.py\"' % dirPath, 
                              source_file, 
                              updated_file, 
                              'gentab']))
    
    # copy source of the file into string for raw execution   
    with open(source_file, 'r') as f:
       ddl = f.read()
       result_file_source += 'const char* %s = R"(%s)";\n\n' % (unit, ddl)

  result_file_source += 'const char* queries[] = {\n'
  for unit in units:    
      result_file_source += '          %s,\n' %unit 
  result_file_source = result_file_source[:-2]
  result_file_source += '\n};'
  result_file_source += '\n\n}'

  with open('%s/../generated/source.hpp' % dirPath, 'w') as f:
    f.write(result_file_source)
    

if __name__ == '__main__':
  main()