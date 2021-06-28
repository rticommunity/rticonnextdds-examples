if "%1"=="dsa" GOTO signDsa
if "%1"=="rsa" GOTO signRsa
echo "sign ecdsa"
set OPENSSL_CONF=..\..\..\resources\cert\ecdsa01\ca\ecdsa01RootCa.cnf
mkdir signed
call openssl smime -sign -in governance.xml -text -out .\signed\governance.p7s -signer ..\..\..\resources\cert\ecdsa01\ca\ecdsa01RootCaCert.pem -inkey ..\..\..\resources\cert\ecdsa01\ca\private\ecdsa01RootCaKey.pem
call openssl smime -sign -in permissionsPeer.xml -text -out .\signed\permissionsPeer.p7s -signer ..\..\..\resources\cert\ecdsa01\ca\ecdsa01RootCaCert.pem -inkey ..\..\..\resources\cert\ecdsa01\ca\private\ecdsa01RootCaKey.pem
call openssl smime -sign -in permissionsRoutingService.xml -text -out .\signed\permissionsRoutingService.p7s -signer ..\..\..\resources\cert\ecdsa01\ca\ecdsa01RootCaCert.pem -inkey ..\..\..\resources\cert\ecdsa01\ca\private\ecdsa01RootCaKey.pem
exit /b

:signDsa
@echo "sign dsa"
set OPENSSL_CONF=..\..\..\resources\cert\dsa01\ca\dsa01RootCa.cnf
mkdir signed
call openssl smime -sign -in governance.xml -text -out .\signed\governance.p7s -signer ..\..\..\resources\cert\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\cert\dsa01\ca\private\dsa01RootCaKey.pem
call openssl smime -sign -in permissionsPeer.xml -text -out .\signed\permissionsPeer.p7s -signer ..\..\..\resources\cert\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\cert\dsa01\ca\private\dsa01RootCaKey.pem
call openssl smime -sign -in permissionsRoutingService.xml -text -out .\signed\permissionsRoutingService.p7s -signer ..\..\..\resources\cert\dsa01\ca\dsa01RootCaCert.pem -inkey ..\..\..\resources\cert\dsa01\ca\private\dsa01RootCaKey.pem
exit /b

:signRsa
@echo "sign rsa"
set OPENSSL_CONF=..\..\..\resources\cert\rsa01\ca\rsa01RootCa.cnf
mkdir signed
call openssl smime -sign -in governance.xml -text -out .\signed\governance.p7s -signer ..\..\..\resources\cert\rsa01\ca\rsa01RootCaCert.pem -inkey ..\..\..\resources\cert\rsa01\ca\private\rsa01RootCaKey.pem
call openssl smime -sign -in permissionsPeer.xml -text -out .\signed\permissionsPeer.p7s -signer ..\..\..\resources\cert\rsa01\ca\rsa01RootCaCert.pem -inkey ..\..\..\resources\cert\rsa01\ca\private\rsa01RootCaKey.pem
call openssl smime -sign -in permissionsRoutingService.xml -text -out .\signed\permissionsRoutingService.p7s -signer ..\..\..\resources\cert\rsa01\ca\rsa01RootCaCert.pem -inkey ..\..\..\resources\cert\rsa01\ca\private\rsa01RootCaKey.pem
exit /b
