#!/bin/sh

chown fr fput
chgrp root fput
mv fput /usr/bin/
chmod u+s /usr/bin/fput

chown fr fget
chgrp root fget
mv fget /usr/bin/
chmod u+s /usr/bin/fget

chown fr list
chgrp root list
mv list /usr/bin/
chmod u+s /usr/bin/list

chown fr do_exec
chgrp root do_exec
mv do_exec /usr/bin/
chmod u+s /usr/bin/do_exec

chown fr getacl
chgrp root getacl
mv getacl /usr/bin/
chmod u+s /usr/bin/getacl

chown fr setacl
chgrp root setacl
mv setacl /usr/bin/
chmod u+s /usr/bin/setacl

chown fr create_dir
chgrp root create_dir
mv create_dir /usr/bin/
chmod u+s /usr/bin/create_dir

chown fr cdir
chgrp root cdir
mv cdir /usr/bin/
chmod u+s /usr/bin/cdir
