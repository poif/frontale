--  1. DROP old version databases
drop database if exists oklm;

--  2. CREATE databases
create database oklm;

--  3. CREATE tables
use oklm;
create table querty (
    ID      int not null auto_increment,    --  ID
    REF     longtext not null,             --  reference
    VAL     longtext not null,             --  valeur
    constraint DEFAULT_PK primary key (ID)
);

--  4. DROP old version users
drop user 'zsxedcrfvtgbyhn'@'localhost';

--  5. CREATE users
create user 'zsxedcrfvtgbyhn'@'localhost' identified by 'C0uc0U@IinNSa';

--  6. GRANT users
grant SELECT, DELETE, INSERT on oklm.querty to 'zsxedcrfvtgbyhn'@'localhost' identified by 'C0uc0U@IinNSa';
