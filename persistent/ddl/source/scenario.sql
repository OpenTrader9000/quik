
CREATE TABLE scenario (
    id    integer primary key autoincrement,
    name text not null -- any possible length of text
);

CREATE TABLE scenario_entry (
    id              integer     primary key autoincrement,
    type            integer     not null,
    name            varchar(36) not null,
    ts              bigint unsigned not null,
    body            text        default null,
    scenario_id     integer     not null,
    FOREIGN KEY (scenario_id) REFERENCES scenario(id)
);
