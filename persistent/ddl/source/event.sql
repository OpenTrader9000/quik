
CREATE TABLE event (
    id                 integer   primary key autoincrement,
    session_id         integer     not null,
    name               varchar(40) not null, 
    comment            text ,
	ts                 bigint unsigned      not null,
    FOREIGN KEY (session_id) REFERENCES session(id)
);