./ddl2cpp log.sql             ../generated/log              table
./ddl2cpp existing_tables.sql ../generated/existing_tables  table
./ddl2cpp sources.sql         ../generated/sources          table

echo "#pragma once" > ddl.hpp
echo "namespace scheme {" >> ddl.hpp
echo "static char const* log  = R\"($(cat log.sql))\";" >> ddl.hpp

echo "static char const* create_log_index = \"CREATE INDEX is_sent_partial ON log(is_sent) WHERE is_sent = 0\";" >> ddl.hpp
echo "static char const* existing_tables = R\"($(cat existing_tables.sql))\";" >> ddl.hpp
echo "static char const* sources = R\"($(cat sources.sql))\";" >> ddl.hpp
echo "}" >> ddl.hpp
