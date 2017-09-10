
CREATE TABLE unhandled(
    id              integer   primary key autoincrement,
	type			integer		         not null,
    name			varchar(36)          not null,
    ts              bigint unsigned      not null,
    body			text	     default null
);
