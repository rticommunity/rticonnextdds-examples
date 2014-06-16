===========================================
      How to install ODBC connector 
===========================================

How to install ODBC connector 
=============================
    In this section we explain how to install the MySQL relational database
with the ODBC connector which will be used by this example. MySQL is used in the
'Durable Writer History', 'Durable Reader State' and 'Persistence Service'
scenarios.

MySQL Installation
------------------

** Windows-Based Systems

1) First, verify that MySQL 5.1 and ODBC Connector 5.1 are installed and running
on your system. The installation of MySQL is beyond the scope of this document.
Please refer to the MySQL Reference Manual for the process to install and 
configure MySQL. However, take into account the following details:
    - Install the correct version according to your system (32 bits vs 64 bits).
    - Make sure that the daemon is running (mysqld.exe).

2) RTI Persistence Service requires the installation of the MySQL ODBC 5.1 
driver. The driver is not bundled with the MySQL server and must be 
installed separately.

The ODBC connector can be downloaded from the following web address:
http://dev.mysql.com/downloads/connector/odbc/

Make sure you install the ODBC connector corresponding to your architecture. If 
you are compiling using Visual Studio Express, you will need a 32-bit connector.

Create a MySQL Account
----------------------
Before you run this example using MySQL, you need to obtain a MySQL user account
from your database administrator. If you are acting as your own database 
administrator, start MySQL from the command prompt to connect to the 
MySQL server as the MySQL root user:
> mysql -uroot
If you have assigned a password to the root account, you will also need to 
provide a -p option.

For example, to create a new MySQL account with a user name of "test" and 
a password of "test", enter the following:
> mysql -uroot
mysql> GRANT ALL PRIVILEGES ON *.* TO 'test'@'localhost' IDENTIFIED
BY 'test' WITH GRANT OPTION;
mysql> exit;

The remaining sections of this document assume that a MySQL user named "test"
with the password "test" has an account on the local host.

Creating a working DSN
----------------------
1) [Only for external databases] Create a DSN for the persistence service. To
      add a data source, follow these steps:

      a) Open the ODBC Data Source Administrator:
         To run ODBC Data Source Administrator 32-bits, in a 64-bits environment
         you have to run "%SystemRoot%\SysWOW64\odbcad32.exe".

         Windows 2000 systems: 
             Select Start, Control Panel, Performance and Maintenance, 
             Administrative Tools, Data Sources (ODBC).

         Windows XP and Windows Server 2003 systems: 
             Select Start, Control Panel, Administrative Tools, Data Sources 
             (ODBC).

         Windows Vista systems: 
             Select Start, Control Panel, System and Maintenance, Administrative
             Tools, Data Sources (ODBC).

         Windows 7 and Windows Server 2008 R2: 
             Select Start, Control Panel, System and Security, Administrative 
             Tools, Data Sources (ODBC).

         Windows 8 and Windows Server 2012 R2:
             Select Start, Control Panel, System and Security, Administrative 
             Tools, Data Source (ODBC).

      b) Select the User DSN tab.
      
      c) Click Add; the Create New Data Source dialog appears.

      
    For TimesTen:
      - Select TimesTen Data Manager 11.2.1 from the list of drivers.
      - Click Finish; the TimesTen ODBC Setup dialog appears.
      - Fill out the fields in the dialog.
      - Enter "test_dsn" as the Data Source Name (DSN).
      - Enter a Data Store Path; the path must exist, so you may want to create
        a new directory first.

        The data store path name uniquely identifies the physical data store. It
        is the full path name of the data store (e.g., C:\data\test_dsn). Note 
        that this name is not a file name - the actual data store file names 
        have suffixes (e.g., C:\data\test_dsn.ds0, C:\data\test_dsn.log0).
      
      - Use the Database Character Set that you have configured while TimesTen 
        was installing. In this case we will use 'AL32UTF8'.
      - Verify that the Temporary checkbox is not checked.
      - All other fields can be left empty.
      - Click OK.

    For MySQL:
      - Select the 'MySQL ODBC 5.1 Driver' from the list of drivers.
      - Click the Finish button; the MySQL ODBC Driver Configuration dialog 
        appears.
      - Fill out the fields in the dialog.
      - Enter "test_dsn" as the Data Source Name (DSN).
      - Enter "test" as the User and "test" as the Password.
      - Select "test" as the Database.
      - All other fields can be left empty.
      - Click the OK button.
