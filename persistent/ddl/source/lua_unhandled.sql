
CREATE TABLE lua_unhandled(
    id                integer  primary key autoincrement,
    function_name     varchar(36) not null,
    ts                bigint      not null,
    dump              text        default null
);
