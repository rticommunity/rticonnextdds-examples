set OPENSSL_CONF=..\..\..\resources\certAuthority\dsa01\ca\dsa01RootCa.cnf
mkdir signed
call openssl smime -sign -in governance.xml -text -out .\signed\governance.p7s -signer ..\..\..\resources\certAuthority\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\certAuthority\dsa01\ca\private\dsa01RootCaKey.pem
call openssl smime -sign -in permissionsPeer.xml -text -out .\signed\permissionsPeer.p7s -signer ..\..\..\resources\certAuthority\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\certAuthority\dsa01\ca\private\dsa01RootCaKey.pem
call openssl smime -sign -in permissionsRoutingService.xml -text -out .\signed\permissionsRoutingService.p7s -signer ..\..\..\resources\certAuthority\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\certAuthority\dsa01\ca\private\dsa01RootCaKey.pem
