--  1. DROP old version databases
drop database if exists ONE;
drop database if exists TWO;
drop database if exists THREE;


--  2. CREATE databases
create database ONE;
create database TWO;
create database THREE;


--  3. CREATE tables
use ONE;
create table STOCKAGE (
    ID      int not null auto_increment,    --  ID
    REF     longtext not null,             --  reference
    VAL     longtext not null,             --  valeur
    constraint DEFAULT_PK primary key (ID)
);
use TWO;
create table STOCKAGE (
    ID      int not null auto_increment,    --  ID
    REF     longtext not null,             --  reference
    VAL     longtext not null,             --  valeur
    constraint DEFAULT_PK primary key (ID)
);
use THREE;
create table STOCKAGE (
    ID      int not null auto_increment,    --  ID
    REF     longtext not null,             --  reference
    VAL     longtext not null,             --  valeur
    constraint DEFAULT_PK primary key (ID)
);


--  4. DROP old version users
drop user 'USER_ONE'@'localhost';
drop user 'USER_TWO'@'localhost';
drop user 'USER_THREE'@'localhost';


--  5. CREATE users
create user 'USER_ONE'@'localhost' identified by 'AZERTY';
create user 'USER_TWO'@'localhost' identified by 'AZERTY';
create user 'USER_THREE'@'localhost' identified by 'AZERTY';


--  6. GRANT users
grant SELECT, DELETE, INSERT on ONE.STOCKAGE to 'USER_ONE'@'localhost' identified by 'AZERTY';
grant SELECT, DELETE, INSERT on TWO.STOCKAGE to 'USER_TWO'@'localhost' identified by 'AZERTY';
grant SELECT, DELETE, INSERT on THREE.STOCKAGE to 'USER_THREE'@'localhost' identified by 'AZERTY';
