#!/usr/bin/env python

##
 # Copyright (c) 2013-2015, Roland Bock
 # All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without modification,
 # are permitted provided that the following conditions are met:
 #
 #  * Redistributions of source code must retain the above copyright notice,
 #    this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright notice,
 #    this list of conditions and the following disclaimer in the documentation
 #    and/or other materials provided with the distribution.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 # ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 # IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 # INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 # BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 # DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 # LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 # OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 # OF THE POSSIBILITY OF SUCH DAMAGE.
 ##

from __future__ import print_function
import sys
import re
import os


# error codes, we should refactor this later
ERROR_BAD_ARGS = 1
ERROR_DATA_TYPE = 10
ERROR_STRANGE_PARSING = 20


from pyparsing import CaselessLiteral, Literal, SkipTo, restOfLine, oneOf, ZeroOrMore, Optional, Combine, Suppress, \
    WordStart, WordEnd, Word, alphas, alphanums, nums, QuotedString, nestedExpr, MatchFirst, OneOrMore, delimitedList, \
    Or, Group, ParseException


# HELPERS

def get_include_guard_name(namespace, inputfile):
  val = re.sub("[^A-Za-z]+", "_", namespace + '_' + os.path.basename(inputfile))
  return val.upper()


def repl_func(m):
  if m.group(1) == '_':
    return m.group(2).upper()
  else:
    return m.group(1) + m.group(2).upper()


def repl_func_for_args(m):
  if m.group(1) == '-':
    return m.group(2).upper()
  else:
    return m.group(1) + m.group(2).upper()

def toClassName(s):
  return re.sub("(^|\s|[_0-9])(\S)", repl_func, s)


def toMemberName(s):
  return re.sub("(\s|_|[0-9])(\S)", repl_func, s)


def setArgumentBool(s, bool_value):
    first_lower = lambda s: s[:1].lower() + s[1:] if s else ''  # http://stackoverflow.com/a/3847369/5006740
    var_name = first_lower(re.sub("(\s|-|[0-9])(\S)", repl_func_for_args, s))
    globals()[var_name] = bool_value


def usage(optionalArgs = {}):
    print('\
    Usage: ddl2cpp <path to ddl> <path to target (without extension, e.g. /tmp/MyTable)> <namespace>\n\
           ddl2cpp -help')


def help_message():
    arg_string = '\n'
    pad = 0
    padding = 0
    for argument in list(optionalArgs.keys()):
        if len(argument) > pad:
            pad = len(argument)
    for argument in list(optionalArgs.keys()):
        if argument == '-help':
            continue
        if len(argument) < pad:
            padding = " " * (pad - len(argument))
        else:
            padding = ''
        arg_string = arg_string + ' [-[no]'+argument+']:  ' + padding + optionalArgs[argument] + '\n'
    print('Usage: ddl2cpp [-help]\n\n OPTIONAL ARGUMENTS:\n' + arg_string +' \n \
   <path to ddl> <path to target> <namespace>\n\
\n\
 <path to ddl>    path to your SQL database/table definitions (SHOW CREATE TABLE SomeTable) \n\
 <path to target> path to a generated C++ header file. Without extension (no *.h). \n\
 <namespace>      namespace you want. Usually a project/database name\n')
    sys.exit(0)

optionalArgs = {
    # if  -some-key is present, it will set variable someKey to True
    # if  -no-some-key is present, it will set variable someKey to False
    '-timestamp-warning': "show warning about mysql timestamp data type",  # timeStampWarning = True
    # '-no-time-stamp-warning'  # timeStampWarning = False
    '-fail-on-parse': "abort instead of silent genereation of unusable headers",  # failOnParse = True
    '-warn-on-parse': "warn about unusable headers, but continue",  # warnOnParse = True
    '-auto-id': "Assume column 'id' to have an automatic value as if AUTO_INCREMENT was specified (e.g. implicit for SQLite ROWID)",  # autoId = True
    '-help': "show this help"
}

if '-help' in sys.argv:
    help_message()
# ARGUMENT PARSING
if len(sys.argv) < (4):
  usage(optionalArgs)
  sys.exit(ERROR_BAD_ARGS)

firstPositional = 1
timestampWarning = True
failOnParse = False
warnOnParse = False
parseError = "Parsing error, possible reason: can't parse default value for a field"
autoId = False


if len(sys.argv) >= 4:
    for arg in sys.argv:
        noArg = arg.replace('-no-', '-')
        if arg in list(optionalArgs.keys()):
            setArgumentBool(arg, True)
            firstPositional += 1
        elif noArg in optionalArgs:
            setArgumentBool(noArg, False)
            firstPositional += 1
        else:
            pass

pathToDdl = sys.argv[firstPositional]

pathToHeader = sys.argv[firstPositional + 1] + '.h'
namespace = sys.argv[firstPositional + 2]


INCLUDE = 'sqlpp11'
NAMESPACE = 'sqlpp'



# PARSER
def ddlWord(string):
    return WordStart(alphanums + "_") + CaselessLiteral(string) + WordEnd(alphanums + "_")

# This function should be refactored if we find some database function which needs parameters
# Right now it works only for something like NOW() in MySQL default field value
def ddlFunctionWord(string):
    return CaselessLiteral(string) + OneOrMore("(") + ZeroOrMore(" ") + OneOrMore(")")

ddlString = Or([QuotedString("'"), QuotedString("\"", escQuote='""'), QuotedString("`")])
negativeSign = Literal('-')
ddlNum = Combine(Optional(negativeSign) + Word(nums + "."))
ddlTerm = Word(alphanums + "_$")
ddlName = Or([ddlTerm, ddlString])
ddlArguments = "(" + delimitedList(Or([ddlString, ddlTerm, ddlNum])) + ")"
ddlNotNull = Group(ddlWord("NOT") + ddlWord("NULL")).setResultsName("notNull")
ddlDefaultValue = ddlWord("DEFAULT").setResultsName("hasDefaultValue")
ddlAutoValue = Or([
    ddlWord("AUTO_INCREMENT"),
    ddlWord("SMALLSERIAL"),
    ddlWord("SERIAL"),
    ddlWord("BIGSERIAL"),
    ]).setResultsName("hasAutoValue")
ddlColumnComment = Group(ddlWord("COMMENT") + ddlString).setResultsName("comment")
ddlConstraint = Or([
        ddlWord("CONSTRAINT"),
        ddlWord("PRIMARY"),
        ddlWord("FOREIGN"),
        ddlWord("KEY"),
        ddlWord("INDEX"),
        ddlWord("UNIQUE"),
        ])
ddlColumn = Group(Optional(ddlConstraint).setResultsName("isConstraint") + OneOrMore(MatchFirst([ddlNotNull, ddlAutoValue, ddlDefaultValue, ddlFunctionWord("NOW"), ddlTerm, ddlNum, ddlColumnComment, ddlString, ddlArguments])))
ddlIfNotExists = Optional(Group(ddlWord("IF") + ddlWord("NOT") + ddlWord("EXISTS")).setResultsName("ifNotExists"))
createTable = Group(ddlWord("CREATE") + ddlWord("TABLE") + ddlIfNotExists + ddlName.setResultsName("tableName") + "(" + Group(delimitedList(ddlColumn)).setResultsName("columns") + ")").setResultsName("create")
#ddlString.setDebug(True) #uncomment to debug pyparsing

ddl = ZeroOrMore(Suppress(SkipTo(createTable, False)) + createTable)

ddlComment = oneOf(["--", "#"]) + restOfLine
ddl.ignore(ddlComment)

# MAP SQL TYPES
types = {
    'tinyint': 'tinyint',
    'smallint': 'smallint',
    'smallserial': 'smallint', # PostgreSQL
    'integer': 'integer',
    'int': 'integer',
    'serial': 'integer', # PostgreSQL
    'mediumint' : 'integer',
    'bigint': 'bigint',
    'bigserial': 'bigint', # PostgreSQL
    'char': 'char_',
    'varchar': 'varchar',
    'text': 'text',
    'tinyblob': 'blob',
    'blob': 'blob',
    'mediumblob': 'blob',
    'longblob': 'blob',
    'bool': 'boolean',
    'boolean': 'boolean',
    'double': 'floating_point',
    'float': 'floating_point',
    'date': 'day_point',
    'datetime': 'time_point',
    'timestamp': 'time_point',
    'enum': 'text',  # MYSQL
    'set': 'text',  # MYSQL
    }

ddlColumn = Group(Optional(ddlConstraint).setResultsName("isConstraint") + OneOrMore(MatchFirst([ddlNotNull, ddlAutoValue, ddlDefaultValue, ddlFunctionWord("NOW"), ddlTerm, ddlNum, ddlColumnComment, ddlString, ddlArguments])))
createTable = Group(ddlWord("CREATE") + ddlWord("TABLE") + ddlIfNotExists + ddlName.setResultsName("tableName") + "(" + Group(delimitedList(ddlColumn)).setResultsName("columns") + ")").setResultsName("create")
ddlComment = oneOf(["--", "#"]) + restOfLine

if failOnParse:
    ddl = OneOrMore(Suppress(SkipTo(createTable, False)) + createTable)
    ddl.ignore(ddlComment)
    try:
        tableCreations = ddl.parseFile(pathToDdl)
    except ParseException as e:
        print(parseError + '. Exiting [-no-fail-on-parse]')
        sys.exit(ERROR_STRANGE_PARSING)
else:
    ddl = ZeroOrMore(Suppress(SkipTo(createTable, False)) + createTable)
    ddl.ignore(ddlComment)
    tableCreations = ddl.parseFile(pathToDdl)

if warnOnParse:
    print(parseError + '. Continuing [-no-warn-on-parse]')

nsList = namespace.split('::')

# PROCESS DDL
tableCreations = ddl.parseFile(pathToDdl)

header = open(pathToHeader, 'w')
print('// generated by ' + ' '.join(sys.argv), file=header)
print('#ifndef '+get_include_guard_name(namespace, pathToHeader), file=header)
print('#define '+get_include_guard_name(namespace, pathToHeader), file=header)
print('', file=header)
print('#include <' + INCLUDE + '/table.h>', file=header)
print('#include <' + INCLUDE + '/data_types.h>', file=header)
print('#include <' + INCLUDE + '/char_sequence.h>', file=header)
print('', file=header)
for ns in nsList:
    print('namespace ' + ns, file=header)
    print('{', file=header)
DataTypeError = False
for create in tableCreations:
    sqlTableName = create.tableName
    tableClass = toClassName(sqlTableName)
    tableMember = toMemberName(sqlTableName)
    tableNamespace = tableClass + '_'
    tableTemplateParameters = tableClass
    print('  namespace ' + tableNamespace, file=header)
    print('  {', file=header)
    for column in create.columns:
        if column.isConstraint:
            continue
        sqlColumnName = column[0]
        columnClass = toClassName(sqlColumnName)
        tableTemplateParameters += ',\n               ' + tableNamespace + '::' + columnClass
        columnMember = toMemberName(sqlColumnName)
        sqlColumnType = column[1].lower()
        if sqlColumnType == 'timestamp' and timestampWarning:
            print("Warning: timestamp is mapped to sqlpp::time_point like datetime")
            print("Warning: You have to take care of timezones yourself")
            print("You can disable this warning using -no-timestamp-warning")
        columnCanBeNull = not column.notNull
        print('    struct ' + columnClass, file=header)
        print('    {', file=header)
        print('      struct _alias_t', file=header)
        print('      {', file=header)
        print('        static constexpr const char _literal[] =  "' + sqlColumnName + '";', file=header)
        print('        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;', file=header)
        print('        template<typename T>', file=header)
        print('        struct _member_t', file=header)
        print('          {', file=header)
        print('            T ' + columnMember + ';', file=header)
        print('            T& operator()() { return ' + columnMember + '; }', file=header)
        print('            const T& operator()() const { return ' + columnMember + '; }', file=header)
        print('          };', file=header)
        print('      };', file=header)
        try:
            traitslist = [NAMESPACE + '::' + types[sqlColumnType]]
        except KeyError as e:
            print ('Error: datatype "'  + sqlColumnType + '"" is not supported.')
            DataTypeError = True
        requireInsert = True
        column.hasAutoValue = column.hasAutoValue or (autoId and sqlColumnName == 'id')
        if column.hasAutoValue:
            traitslist.append(NAMESPACE + '::tag::must_not_insert')
            traitslist.append(NAMESPACE + '::tag::must_not_update')
            requireInsert = False
        if not column.notNull:
            traitslist.append(NAMESPACE + '::tag::can_be_null')
            requireInsert = False
        if column.hasDefaultValue:
            requireInsert = False
        if requireInsert:
            traitslist.append(NAMESPACE + '::tag::require_insert')
        print('      using _traits = ' + NAMESPACE + '::make_traits<' + ', '.join(traitslist) + '>;', file=header)
        print('    };', file=header)
    print('  }', file=header)
    print('', file=header)

    print('  struct ' + tableClass + ': ' + NAMESPACE + '::table_t<' + tableTemplateParameters + '>', file=header)
    print('  {', file=header)
    print('    struct _alias_t', file=header)
    print('    {', file=header)
    print('      static constexpr const char _literal[] =  "' + sqlTableName + '";', file=header)
    print('      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;', file=header)
    print('      template<typename T>', file=header)
    print('      struct _member_t', file=header)
    print('      {', file=header)
    print('        T ' + tableMember + ';', file=header)
    print('        T& operator()() { return ' + tableMember + '; }', file=header)
    print('        const T& operator()() const { return ' + tableMember + '; }', file=header)
    print('      };', file=header)
    print('    };', file=header)
    print('  };', file=header)

for ns in nsList:
    print('} // namespace ' + ns, file=header)
print('#endif', file=header)
if (DataTypeError):
    print("Error: unsupported datatypes." )
    print("Possible solutions:")
    print("A) Implement this datatype (examples: sqlpp11/data_types)" )
    print("B) Extend/upgrade ddl2cpp (edit types map)" )
    print("C) Raise an issue on github" )
    sys.exit(10) #return non-zero error code, we might need it for automation
