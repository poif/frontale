--  1. DROP old version databases
drop database if exists qsdfghjklm;

--  2. CREATE databases
create database qsdfghjklm;

--  3. CREATE tables
use qsdfghjklm;
create table azertyuiop (
    ID      int not null auto_increment,    --  ID
    REF     longtext not null,             --  reference
    VAL     longtext not null,             --  valeur
    constraint DEFAULT_PK primary key (ID)
);

--  4. DROP old version users
drop user 'njibhuvgycft'@'localhost';

--  5. CREATE users
create user 'njibhuvgycft'@'localhost' identified by 'Qu4NdJ353r415Gr4ND';

--  6. GRANT users
grant SELECT, DELETE, INSERT on qsdfghjklm.azertyuiop to 'njibhuvgycft'@'localhost' identified by 'Qu4NdJ353r415Gr4ND';
